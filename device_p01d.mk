$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_us_supl.mk)

$(call inherit-product-if-exists, vendor/panasonic/p01d/p01d-vendor.mk)

DEVICE_PACKAGE_OVERLAYS += device/panasonic/p01d/overlay

# Init files in initramfs
PRODUCT_COPY_FILES += \
        device/panasonic/p01d/root/init.qcom.rc:root/init.qcom.rc \
        device/panasonic/p01d/root/init.qcom.sh:root/init.qcom.sh \
        device/panasonic/p01d/root/ueventd.qcom.rc:root/ueventd.qcom.rc \

PRODUCT_COPY_FILES += \
        device/panasonic/p01d/system/atmel-touchscreen.idc:system/usr/idc/atmel-touchscreen.idc \
        device/panasonic/p01d/system/init.qcom.post_boot.sh:system/etc/init.qcom.post_boot.sh \
        device/panasonic/p01d/system/init.touchpanel.sh:system/etc/init.touchpanel.sh \
        device/panasonic/p01d/system/surf_keypad.kl:system/usr/keylayout/surf_keypad.kl \
        device/panasonic/p01d/system/media_codecs.xml:system/etc/media_codecs.xml \
        device/panasonic/p01d/vold.fstab:system/etc/vold.fstab

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
        frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
        frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
        frameworks/base/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
        frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
        frameworks/base/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
        frameworks/base/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
        frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
        frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
        frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
        frameworks/base/data/etc/android.hardware.touchscreen.xml:system/etc/permissions/android.hardware.touchscreen.xml \
        frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
        frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
        frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
        frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml

PRODUCT_PACKAGES += \
        brcm_patchram_plus_p01d

PRODUCT_PACKAGES += \
        com.android.future.usb.accessory

PRODUCT_PACKAGES += \
        audio.a2dp.default \
        audio_policy.msm7x30 \
        audio.primary.msm7x30 \
        copybit.msm7x30 \
        gralloc.msm7x30 \
        hwcomposer.msm7x30 \
        lights.msm7x30 \
        libaudioalsa \
        libaudioutils \
        libgenlock \
        liboverlay \
        libQcomUI \
        libstagefrighthw \
        Torch \
        libtilerenderer

$(call inherit-product, build/target/product/full.mk)

PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=P_01D BUILD_FINGERPRINT=docomo/P_01D/pana2_1:2.3.4/GRH78/20120627.192518:user/release-keys PRIVATE_BUILD_DESC="P_01D-user 2.3.4 GRH78 20120627.192518 release-keys"
PRODUCT_NAME := full_p01d
PRODUCT_DEVICE := p01d
