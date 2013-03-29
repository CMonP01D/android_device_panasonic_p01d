PRODUCT_COPY_FILES += \
  device/panasonic/p01d/vold.fstab:system/etc/vold.fstab \
  device/panasonic/p01d/root/init.qcom.rc:root/init.qcom.rc \
  device/panasonic/p01d/root/initlogo.rle:root/initlogo.rle \
  device/panasonic/p01d/root/ueventd.qcom.rc:root/ueventd.qcom.rc \
  device/panasonic/p01d/system/init.touchpanel.sh:system/etc/init.touchpanel.sh \
  device/panasonic/p01d/system/init.qcom.sdio.sh:system/etc/init.qcom.sdio.sh \
  device/panasonic/p01d/system/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
  device/panasonic/p01d/system/atmel-touchscreen.idc:system/usr/idc/atmel-touchscreen.idc

PRODUCT_PACKAGES += \
  copybit.msm7x30 \
  gralloc.msm7x30 \
  hwcomposer.msm7x30 \
  lights.msm7x30 \
  libgenlock \
  liboverlay \
  libstagefrighthw

PRODUCT_PACKAGES += \
  brcm_patchram_plus_p01d

$(call inherit-product, vendor/qcom/adreno200/device-adreno200.mk)
$(call inherit-product-if-exists, vendor/panasonic/p01d/p01d-vendor.mk)
