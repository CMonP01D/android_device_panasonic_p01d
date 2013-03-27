/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#define LOG_TAG "lights"
#define LOG_NDEBUG 0

#include <cutils/log.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

#define LED_PATH(target, property) \
  "/sys/class/leds/" # target "/" #property

#define WRITE_BLINK(color, target, value)                             \
  do {                                                                \
    int ret = write_int(LED_PATH(color, led_blink_ ##target), value); \
    if (ret < 0)                                                      \
      return ret;                                                     \
  } while(0)

#define WRITE_BLINK_ON_OFF(color, on_value, off_value)                \
  WRITE_BLINK(color, on, on_value);                                   \
  WRITE_BLINK(color, off, off_value);                                 \

#define WRITE_BLINK_FOR_RGB(on, off)                                  \
  WRITE_BLINK_ON_OFF(red, on, off)                                    \
  WRITE_BLINK_ON_OFF(green, on, off)                                  \
  WRITE_BLINK_ON_OFF(blue, on, off)                                   \

#define WRITE_BRIGHTNESS(color, value)                                \
  do {                                                                \
    int limited_value;                                                \
    int max_value;                                                    \
    int ret;                                                          \
    max_value  = get_max_brightness(LED_PATH(color, max_brightness)); \
    limited_value =  limit_brightness(value, max_value);              \
    ret = write_int(LED_PATH(color, brightness), limited_value);      \
    if (ret < 0)                                                      \
      return ret;                                                     \
  } while(0)

static void
init_globals(void)
{
    pthread_mutex_init(&g_lock, NULL);
}

static int
write_int(const char *path, int value)
{
    int fd;
    char buffer[20];
    int bytes;
    int bytes_written;
    static int already_warned = 0;

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        if (already_warned == 0) {
            LOGE("Failed to open %s due to %s.", path, strerror(errno));
            already_warned = 1;
        }
        return -errno;
    }

    bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
    bytes_written = write(fd, buffer, bytes);
    close(fd);

    if (bytes != bytes_written) {
        LOGE("Failed to write %s to %s due to %s.", buffer, path, strerror(errno));
        return -errno;
    }

    return 0;
}

static int
rgb_to_brightness(const struct light_state_t *state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int
set_backlight(struct light_device_t *dev, struct light_state_t *state)
{
    int ret = 0;
    int brightness = rgb_to_brightness(state);

    pthread_mutex_lock(&g_lock);
    ret = write_int(LED_PATH(lcd-backlight, brightness), brightness);
    pthread_mutex_unlock(&g_lock);

    return ret;
}

static int
get_max_brightness(const char *path)
{
    int fd;
    char buffer[20];
    int ret;
    int max;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOGE("Failed to open %s due to %s.", path, strerror(errno));
        return -errno;
    }

    ret = read(fd, buffer, sizeof(buffer));
    close(fd);

    if (ret < 0) {
        LOGE("Failed to read max brightness from %s due to %s.", path, strerror(errno));
        return -errno;
    }

    return strtol(buffer, NULL, 10);
}

static int
limit_brightness(int brightness, int limit)
{
    if (limit < 0)
        return brightness;
    return (limit * brightness) >> 8;
}

static int
write_led_color(struct light_state_t *state)
{
    int red, green, blue;
    unsigned int color = state->color;

    red = (color >> 16) & 0xFF;
    green = (color >> 8) & 0xFF;
    blue = color & 0xFF;

    WRITE_BRIGHTNESS(red, red);
    WRITE_BRIGHTNESS(green, green);
    WRITE_BRIGHTNESS(blue, blue);

    return 0;
}

static int
write_flash(struct light_state_t *state)
{
    int blink_on, blink_off;

    if (state->flashMode != LIGHT_FLASH_TIMED)
        return 0;

    WRITE_BLINK_FOR_RGB(blink_on, blink_off);

    return 0;
}

static int
set_led(struct light_state_t *state)
{
    int ret;

    ret = write_led_color(state);
    if (ret < 0)
        return ret;

    return write_flash(state);
}

#define DEFINE_SETTER(flavor)                                           \
static int                                                              \
set_ ## flavor(struct light_device_t *dev, struct light_state_t *state) \
{                                                                       \
    int ret = 0;                                                        \
    pthread_mutex_lock(&g_lock);                                        \
    ret = set_led(state);                                               \
    pthread_mutex_unlock(&g_lock);                                      \
    return ret;                                                         \
}

DEFINE_SETTER(battery)
DEFINE_SETTER(notification)
DEFINE_SETTER(attention)

static int
close_lights(struct light_device_t *dev)
{
    if (dev) {
        free(dev);
    }
    return 0;
}

static int
open_lights(const struct hw_module_t *module, const char *id,
            struct hw_device_t **device)
{
    struct light_device_t *dev;
    int (*set_light)(struct light_device_t *dev,
            struct light_state_t const *state);

    if (!strcmp(LIGHT_ID_BACKLIGHT, id)) {
        set_light = set_backlight;
    } else if (!strcmp(LIGHT_ID_BATTERY, id)) {
        set_light = set_battery;
    } else if (!strcmp(LIGHT_ID_NOTIFICATIONS, id)) {
        set_light = set_notification;
    } else if (!strcmp(LIGHT_ID_ATTENTION, id)) {
        set_light = set_attention;
    } else if (!strcmp(LIGHT_ID_KEYBOARD, id)) {
        return -ENODEV;
    } else if (!strcmp(LIGHT_ID_BUTTONS, id)) {
        return -ENODEV;
    } else {
        return -EINVAL;
    }

    dev = calloc(sizeof(struct light_device_t), 1);
    if (!dev) {
      return -ENOMEM;
    }

    pthread_once(&g_init, init_globals);

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;
    return 0;
}


static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights
};

const struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "Panasonic P-01D Lights Module",
    .author = "Hiroyuki Ikezoe",
    .methods = &lights_module_methods,
};
