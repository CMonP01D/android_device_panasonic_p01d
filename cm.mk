## Specify phone tech before including full_phone
$(call inherit-product, vendor/cm/config/gsm.mk)

# Release name
PRODUCT_RELEASE_NAME := p01d

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_mini_phone.mk)

# Inherit device configuration
$(call inherit-product, device/panasonic/p01d/device_p01d.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := p01d
PRODUCT_NAME := cm_p01d
PRODUCT_BRAND := panasonic
PRODUCT_MODEL := p01d
PRODUCT_MANUFACTURER := panasonic
