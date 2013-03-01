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


#define LOG_TAG "camera"
#define LOG_NDEBUG 0

#include <cutils/log.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>

#include <hardware/camera.h>
#include <utils/RefBase.h>
#include "CameraHardwareInterface.h"

using android::sp;
using android::CameraHardwareInterface;
using android::CameraParameters;
using android::String8;
using android::IMemory;
using android::IMemoryHeap;
using android::NO_INIT;
using android::INVALID_OPERATION;

typedef struct _local_camera_device {
    camera_device_t device;

    sp<CameraHardwareInterface> camera;

    preview_stream_ops *window;

    camera_notify_callback notify_callback;
    camera_data_callback data_callback;
    camera_data_timestamp_callback data_timestamp_callback;
    camera_request_memory request_memory;
    void *user;

    int preview_width;
    int preview_height;
} local_camera_device;

struct stock_camera_info {
    int facing;
    int mode;
    int orientation;
};

enum {
  CAMERA_MODE_2D = (1<<0),
  CAMERA_MODE_3D = (1<<1),
  CAMERA_NONZSL_MODE = (1<<2),
  CAMERA_ZSL_MODE = (1<<3),
  CAMERA_MODE_MAX = CAMERA_ZSL_MODE,
};

int (*LINK_getNumberofCameras)(void);
void (*LINK_getCameraInfo)(int cameraId, struct stock_camera_info *info);
sp<CameraHardwareInterface> (*LINK_openCameraHardware)(int id, int mode, int what);

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static bool g_initialized = false;
static void *g_libcamera = NULL;

static void *
open_libcamera(void)
{
    void *libcamera;

    libcamera = dlopen("libcamera.so", RTLD_LAZY);
    if (!libcamera) {
        ALOGV("Failed to dlopen libcamera.so due to %s", dlerror());
        return NULL;
    }
    return libcamera;
}

static bool
load_libcamera(void)
{
    void *libcamera = open_libcamera();

    if (!libcamera) {
        return false;
    }

    if (!dlsym(libcamera, "HAL_getNumberOfCameras")) {
        return false;
    }
    if (!dlsym(libcamera, "HAL_getCameraInfo")) {
        return false;
    }
    if (!dlsym(libcamera, "HAL_openCameraHardware")) {
        return false;
    }

    *(void**)&LINK_getNumberofCameras = dlsym(libcamera, "HAL_getNumberOfCameras");
    *(void**)&LINK_getCameraInfo = dlsym(libcamera, "HAL_getCameraInfo");
    *(void**)&LINK_openCameraHardware = dlsym(libcamera, "HAL_openCameraHardware");

    g_libcamera = libcamera;

    return true;
}

static void
init_globals(void)
{
    ALOGV("%s", __PRETTY_FUNCTION__);
    pthread_mutex_init(&g_lock, NULL);

    g_initialized = load_libcamera();
}

static int
get_number_of_cameras(void)
{
    ALOGV("%s", __PRETTY_FUNCTION__);
    pthread_once(&g_init, init_globals);

    if (!g_initialized) {
        return 0;
    }

    return LINK_getNumberofCameras();
}

static int
get_camera_info(int camera_id, struct camera_info *info)
{
    struct stock_camera_info stock_info;
    ALOGV("%s", __PRETTY_FUNCTION__);

    pthread_once(&g_init, init_globals);

    if (!g_initialized) {
        return 0;
    }

    LINK_getCameraInfo(camera_id, &stock_info);
    info->facing = stock_info.facing;
    info->orientation = stock_info.orientation;
    ALOGV("%s: facing = %d, orientation = %d, mode = %d",
          __FUNCTION__, stock_info.facing, stock_info.orientation, stock_info.mode);

    return 0;
}

sp<CameraHardwareInterface>
get_local_device(camera_device_t *device)
{
    if (!device) {
        return 0;
    }

    local_camera_device *local = (local_camera_device*)device;
    return local->camera;
}

static int
set_preview_window(camera_device_t *device, struct preview_stream_ops *window)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    if (!window) {
        ALOGD("%s: window is NULL", __FUNCTION__);
        return 0;
    }

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    int min_bufs = -1;
    int kBufferCount = 4;
    local_camera_device* dev = NULL;

    dev = (local_camera_device*) device;

    dev->window = window;

    if (window->get_min_undequeued_buffer_count(window, &min_bufs)) {
        ALOGE("%s: could not retrieve min undequeued buffer count", __FUNCTION__);
        return -1;
    }

    ALOGD("%s: bufs:%i", __FUNCTION__, min_bufs);

    if (min_bufs >= kBufferCount) {
        ALOGE("%s: min undequeued buffer count %i is too high (expecting at most %i)",
              __FUNCTION__, min_bufs, kBufferCount - 1);
    }

    ALOGD("%s: setting buffer count to %i", __FUNCTION__, kBufferCount);
    if (window->set_buffer_count(window, kBufferCount)) {
        ALOGE("%s: could not set buffer count", __FUNCTION__);
        return -1;
    }

    int preview_width;
    int preview_height;
    CameraParameters params(camera->getParameters());
    params.getPreviewSize(&preview_width, &preview_height);
    int hal_pixel_format = HAL_PIXEL_FORMAT_YCrCb_420_SP;

    const char *str_preview_format = params.getPreviewFormat();
    ALOGD("%s: preview format %s", __FUNCTION__, str_preview_format);

    if (window->set_usage(window, GRALLOC_USAGE_SW_WRITE_MASK)) {
        ALOGE("%s: could not set usage on gralloc buffer", __FUNCTION__);
        return -1;
    }

    if (window->set_buffers_geometry(window, preview_width,
                                     preview_height, hal_pixel_format)) {
        ALOGE("%s: could not set buffers geometry to %s",
             __FUNCTION__, str_preview_format);
        return -1;
    }

    dev->preview_width = preview_width;
    dev->preview_height = preview_height;

    return 0;
}

static camera_memory_t *
wrap_memory_data(local_camera_device *dev, const sp<IMemory>& dataPtr)
{
    void *data;
    size_t size;
    ssize_t offset;
    sp<IMemoryHeap> heap;
    camera_memory_t *mem;

    ALOGV("%s", __FUNCTION__);

    if (!dev->request_memory) {
        return NULL;
    }

    heap = dataPtr->getMemory(&offset, &size);
    data = (void *)((char *)(heap->base()) + offset);

    ALOGV("%s: data: 0x%p size: %i", __FUNCTION__, data, size);

    mem = dev->request_memory(-1, size, 1, dev->user);
    memcpy(mem->data, data, size);

    return mem;
}

static void
wrap_notify_callback(int32_t msg_type, int32_t ext1, int32_t ext2, void* user)
{
    local_camera_device* dev = NULL;

    ALOGV("%s: type %i", __FUNCTION__, msg_type);

    if (!user) {
        return;
    }

    dev = (local_camera_device*) user;

    if (dev->notify_callback) {
        dev->notify_callback(msg_type, ext1, ext2, dev->user);
    }
}

static void
wrap_data_callback(int32_t msg_type,
                   const sp<IMemory>& dataPtr,
                   void* user)
{
    camera_memory_t *data = NULL;
    local_camera_device* dev = NULL;

    ALOGV("%s: type %i", __FUNCTION__, msg_type);

    if (!user) {
        return;
    }

    dev = (local_camera_device*) user;

    data = wrap_memory_data(dev, dataPtr);

    if (dev->data_callback) {
        dev->data_callback(msg_type, data, 0, NULL, dev->user);
    }
}

static void
wrap_data_callback_timestamp(nsecs_t timestamp, int32_t msg_type,
                             const sp<IMemory>& dataPtr, void* user)
{
    local_camera_device* dev = NULL;

    ALOGV("%s: type %i", __FUNCTION__, msg_type);

    if (!user) {
        return;
    }

    dev = (local_camera_device*) user;
}


static void
set_callbacks(camera_device_t *device,
              camera_notify_callback notify_cb,
              camera_data_callback data_cb,
              camera_data_timestamp_callback data_cb_timestamp,
              camera_request_memory get_memory,
              void *user)
{
    ALOGE("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    local_camera_device *local = (local_camera_device*)device;

    local->notify_callback = notify_cb;
    local->data_callback = data_cb;
    local->data_timestamp_callback = data_cb_timestamp;
    local->request_memory = get_memory;
    local->user = user;

    camera->setCallbacks(wrap_notify_callback,
                         wrap_data_callback,
                         wrap_data_callback_timestamp,
                         (void *)local);
}

static void
enable_msg_type(camera_device_t *device, int32_t msg_type)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    camera->enableMsgType(msg_type);
}

static void
disable_msg_type(camera_device_t *device, int32_t msg_type)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    camera->disableMsgType(msg_type);
}

static int
msg_type_enabled(camera_device_t *device, int32_t msg_type)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->msgTypeEnabled(msg_type);
}

static int
start_preview(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->startPreview();
}

static void
stop_preview(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    camera->stopPreview();
}

static int
preview_enabled(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->previewEnabled();
}

static int
store_meta_data_in_buffers(camera_device_t *device, int enable)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return INVALID_OPERATION;
}

static int
start_recording(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->startRecording();
}

static void
stop_recording(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    camera->stopRecording();
}

static int
recording_enabled(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->recordingEnabled();
}

static void
release_recording_frame(camera_device_t *device, const void *opaque)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    camera->releaseRecordingFrame((IMemory*)opaque);
}

static int
auto_focus(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->autoFocus();
}

static int
cancel_auto_focus(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->cancelAutoFocus();
}

static int
take_picture(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->takePicture();
}

static int
cancel_picture(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->cancelPicture();
}

static int
set_parameters(camera_device_t *device, const char *parameters)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    CameraParameters param;
    param.unflatten(String8(parameters));

    int ret = camera->setParameters(param);

    return 0;
}

static char *
get_parameters(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NULL;
    }

    CameraParameters parameters;
    parameters = camera->getParameters();
    String8 string = parameters.flatten();

    return strdup(string.string());
}

static void
put_parameters(camera_device_t *device, char *parameters)
{
    ALOGV("%s", __PRETTY_FUNCTION__);
}

static int
send_command(camera_device_t *device,
             int32_t cmd, int32_t arg1, int32_t arg2)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return NO_INIT;
    }

    return camera->sendCommand(cmd, arg1, arg2);
}

static void
release(camera_device_t *device)
{
    ALOGV("%s", __PRETTY_FUNCTION__);

    sp<CameraHardwareInterface> camera = get_local_device(device);
    if (camera == 0) {
        return;
    }

    camera->release();
}

static int
dump(camera_device_t *device, int fd)
{
    ALOGV("%s", __PRETTY_FUNCTION__);
    return 0;
}

static void
free_local_camera_device(local_camera_device *device)
{
    camera_device_t *camera_device;
    if (!device) {
        return;
    }

    camera_device = (camera_device_t*)(device);
    free(camera_device->ops);
    free(device);
}

static int
close_camera(hw_device_t *dev)
{
    ALOGV("%s", __PRETTY_FUNCTION__);
    if (g_libcamera) {
        dlclose(g_libcamera);
        g_libcamera = NULL;
    }

    if (dev) {
        free_local_camera_device((local_camera_device*)dev);
    }

    return 0;
}

static int
open_camera(const hw_module_t *module,
            const char *id,
            hw_device_t **device)
{
    local_camera_device *local_device;
    camera_device_ops_t *ops;
    int mode = CAMERA_MODE_2D | CAMERA_NONZSL_MODE;

    ALOGV("%s", __PRETTY_FUNCTION__);

    pthread_once(&g_init, init_globals);

    sp<CameraHardwareInterface> camera;
    camera = LINK_openCameraHardware(atoi(id), mode, 0);
    if (camera == 0) {
        ALOGE("Failed to open camera device.");
        return -ENOMEM;
    }

    local_device = static_cast<local_camera_device*>(calloc(sizeof(local_camera_device), 1));
    if (!local_device) {
        return -ENOMEM;
    }

    ops = static_cast<camera_device_ops_t*>(calloc(sizeof(camera_device_ops_t), 1));
    if (!ops) {
        free_local_camera_device(local_device);
        return -ENOMEM;
    }

    camera_device_t *dev = (camera_device_t*)(local_device);
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t *)module;
    dev->common.close = close_camera;

    ops->set_preview_window = set_preview_window;
    ops->set_callbacks = set_callbacks;
    ops->enable_msg_type = enable_msg_type;
    ops->disable_msg_type = disable_msg_type;
    ops->msg_type_enabled = msg_type_enabled;
    ops->start_preview = start_preview;
    ops->stop_preview = stop_preview;
    ops->preview_enabled = preview_enabled;
    ops->store_meta_data_in_buffers = store_meta_data_in_buffers;
    ops->start_recording = start_recording;
    ops->stop_recording = stop_recording;
    ops->recording_enabled = recording_enabled;
    ops->release_recording_frame = release_recording_frame;
    ops->auto_focus = auto_focus;
    ops->cancel_auto_focus = cancel_auto_focus;
    ops->take_picture = take_picture;
    ops->cancel_picture = cancel_picture;
    ops->set_parameters = set_parameters;
    ops->get_parameters = get_parameters;
    ops->put_parameters = put_parameters;
    ops->send_command = send_command;
    ops->release = release;
    ops->dump = dump;

    dev->ops = ops;
    local_device->camera = camera;

    *device = &local_device->device.common;
    return 0;
}

static struct hw_module_methods_t camera_module_methods = {
    open : open_camera
};

camera_module_t HAL_MODULE_INFO_SYM = {
    common : {
        tag : HARDWARE_MODULE_TAG,
        version_major : 1,
        version_minor : 0,
        id : CAMERA_HARDWARE_MODULE_ID,
        name : "Panasonic P-01D Camera Module",
        author : "Hiroyuki Ikezoe",
        methods : &camera_module_methods,
        dso: 0,
        reserved: {},
    },
    get_number_of_cameras : get_number_of_cameras,
    get_camera_info : get_camera_info
};

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
