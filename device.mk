PRODUCT_COPY_FILES += \
  device/panasonic/p01d/vold.fstab:system/etc/vold.fstab \
  device/panasonic/p01d/ueventd.qcom.rc:root/ueventd.qcom.rc \
  device/panasonic/p01d/system/atmel-touchscreen.idc:system/usr/idc/atmel-touchscreen.idc

PRODUCT_PACKAGES += \
  copybit.msm7x30 \
  gralloc.msm7x30 \
  hwcomposer.msm7x30 \
  libgenlock \
  liboverlay \
  libstagefrighthw

$(call inherit-product, vendor/qcom/adreno200/device-adreno200.mk)

