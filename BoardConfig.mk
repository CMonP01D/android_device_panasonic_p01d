USE_CAMERA_STUB := true

# inherit from the proprietary version
-include vendor/panasonic/p01d/BoardConfigVendor.mk

BOARD_SERIALNO_PATH := device/panasonic/p01d/serialno
ifeq ($(wildcard $(BOARD_SERIALNO_PATH)),)
TARGET_SERIALNO := P01D
else
TARGET_SERIALNO := $(shell cat $(BOARD_SERIALNO_PATH))
endif

TARGET_NO_BOOTLOADER := true
TARGET_BOARD_PLATFORM := msm7x30
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_ARCH_VARIANT_FPU : = neon
TARGET_DISABLE_ARM_PIE := true
TARGET_ARCH_LOWMEM := true
TARGET_CPU_VARIANT := scorpion

TARGET_BOOTLOADER_BOARD_NAME := msm7x30

TARGET_BOOTANIMATION_PRELOAD := true
TARGET_BOOTANIMATION_TEXTURE_CACHE := true

TARGET_PROVIDES_LIBLIGHTS := true

BOARD_HAVE_BLUETOOTH := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/panasonic/p01d/bluetooth

TARGET_KERNEL_CONFIG := cyanogenmod_ponyo_defconfig
TARGET_KERNEL_SOURCE := kernel/panasonic/p01d
BOARD_KERNEL_CMDLINE := androidboot.hardware=qcom androidboot.serialno=$(TARGET_SERIALNO)
BOARD_KERNEL_BASE := 0x13800000
BOARD_KERNEL_PAGESIZE := 4096

BOARD_BOOTIMAGE_PARTITION_SIZE := 0x00a00000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00f00000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 0x15e00000
BOARD_USERDATAIMAGE_PARTITION_SIZE := 0x15480000
BOARD_FLASH_BLOCK_SIZE := 262144

USE_OPENGL_RENDERER := true
BOARD_EGL_CFG := device/panasonic/p01d/egl.cfg

# for QCOM
BOARD_USES_QCOM_HARDWARE := true
BOARD_USES_QCOM_AUDIO_VOIPMUTE := true
BOARD_USES_QCOM_AUDIO_RESETALL := true
BOARD_ADRENO_DECIDE_TEXTURE_TARGET := true
TARGET_USES_C2D_COMPOSITION := true
COMMON_GLOBAL_CFLAGS += -DQCOM_HARDWARE -DQCOM_NO_SECURE_PLAYBACK -DANCIENT_GL
TARGET_NO_HW_VSYNC := true
TARGET_GRALLOC_USES_ASHMEM := true

ARCH_ARM_HAVE_TLS_REGISTER := true
BOARD_USE_LEGACY_TOUCHSCREEN := true
TARGET_RECOVERY_PIXEL_FORMAT := "RGBX_8888"
BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/panasonic/p01d/recovery/keys.c

COMMON_GLOBAL_CFLAGS += -DBINDER_COMPAT
BOARD_NEEDS_MEMORYHEAPPMEM := true
BOARD_USES_LEGACY_OVERLAY := true

WPA_SUPPLICANT_VERSION      := VER_0_8_X
BOARD_WLAN_DEVICE           := bcm4330
BOARD_WPA_SUPPLICANT_DRIVER := WEXT
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_wext
WIFI_DRIVER_MODULE_NAME     := "bcm4330"
WIFI_DRIVER_MODULE_PATH     := "/system/lib/modules/bcm4330.ko"
WIFI_DRIVER_MODULE_ARG      := "firmware_path=/etc/firmware/wlan/sdio_cit.bin nvram_path=/etc/firmware/wlan/nvram.txt"
WIFI_DRIVER_FW_PATH_STA     := "/etc/firmware/wlan/sdio.bin"
WIFI_DRIVER_FW_PATH_AP      := "/etc/firmware/wlan/sdio_apsta.bin"

TARGET_SPECIFIC_HEADER_PATH := device/panasonic/p01d/include
