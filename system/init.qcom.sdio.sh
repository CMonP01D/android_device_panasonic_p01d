#!/system/bin/sh
echo -n "$1" > /sys/devices/platform/msm_sdcc.3/polling
exit 0
