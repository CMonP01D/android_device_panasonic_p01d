#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <cutils/properties.h>
#include <cutils/log.h>

#define LOG_TAG "format_bdaddr"
#define BDADDR_PATH "/data/simcom/btadd/bt_add.file"

static char formatted_bdaddr[17];
static char bdaddr[12];

static bool
format_bdaddr(void)
{
  sprintf(formatted_bdaddr,
	        "%02X:%02X:%02X:%02X:%02X:%02X",
          bdaddr[5], bdaddr[4], bdaddr[3],
          bdaddr[2], bdaddr[1], bdaddr[0]);

  return true;
}

static bool
read_bdaddr(void)
{
  int fd;
  size_t read_bytes;
  int ret = true;

  fd = open(BDADDR_PATH, O_RDONLY);
  if (fd < 0) {
    LOGE("Failed to open " BDADDR_PATH " due to %s.",
        strerror(errno));
    return false;
  }

  read_bytes = read(fd, bdaddr, sizeof(bdaddr));
  if (read_bytes != sizeof(bdaddr)) {
    LOGE("Failed to read " BDADDR_PATH " due to %s.",
         strerror(errno));
    ret = false;
  }

  close(fd);

  return ret;
}

static bool
write_formatted_bdaddr(void)
{
  int fd;
  int ret = true;
  int i;
  size_t written_bytes;

  fd = open(BOARD_BLUETOOTH_BDADDR_PATH, O_WRONLY|O_CREAT|O_TRUNC, 0660);
  if (fd < 0) {
    LOGE("Failed to open " BOARD_BLUETOOTH_BDADDR_PATH " due to %s.",
        strerror(errno));
    return false;
  }

  i = sizeof(bdaddr);
  while (i > 0) {
    write(fd, &bdaddr[i - 2], 1);
    write(fd, &bdaddr[i - 1], 1);
    i -= 2;
    if (i != 0)
      write(fd, ":", 1);
  }

  close(fd);

  return ret;
}

int
main(int argc, char *argv[])
{
  if (!read_bdaddr())
    exit(EXIT_FAILURE);

  if (!format_bdaddr())
    exit(EXIT_FAILURE);

  if (!write_formatted_bdaddr())
    exit(EXIT_FAILURE);

  LOGI("Setting ro.bt.bdaddr_path %s", BOARD_BLUETOOTH_BDADDR_PATH);

  property_set("ro.bt.bdaddr_path", BOARD_BLUETOOTH_BDADDR_PATH);

  exit(EXIT_SUCCESS);
}
/*
vi:ts=2:nowrap:ai:expandtab:sw=2
*/
