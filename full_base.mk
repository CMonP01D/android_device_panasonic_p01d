#
# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This is a build configuration for a full-featured build of the
# Open-Source part of the tree. It's geared toward a US-centric
# build of the emulator, but all those aspects can be overridden
# in inherited configurations.

PRODUCT_PACKAGES := \
    drmserver \
    libdrmframework \
    libdrmframework_jni \
    libfwdlockengine \
    WAPPushManager

PRODUCT_PACKAGES += \
    libvideoeditor_jni \
    libvideoeditorplayer \
    libvideoeditor_core

# Additional settings used in all AOSP builds
PRODUCT_PROPERTY_OVERRIDES := \
    ro.com.android.dateformat=MM-dd-yyyy

# Put en_US first in the list, so make it default.
PRODUCT_LOCALES := ja_JP

PRODUCT_PACKAGES := \
    AlarmProvider \
    Bluetooth \
    Calculator \
    Calendar \
    CertInstaller \
    DrmProvider \
    LatinIME \
    Launcher2 \
    Provision \
    Phone \
    Settings \
    Sync \
    SystemUI \
    Updater \
    CalendarProvider \
    SyncProvider \
    bluetooth-health \
    hostapd \
    wpa_supplicant.conf

PRODUCT_PACKAGES += \
    icu.dat

PRODUCT_PACKAGES += \
    librs_jni

PRODUCT_PACKAGES += \
    audio.primary.default \
    audio_policy.default

PRODUCT_COPY_FILES := \
        system/bluetooth/data/audio.conf:system/etc/bluetooth/audio.conf \
        system/bluetooth/data/auto_pairing.conf:system/etc/bluetooth/auto_pairing.conf \
        system/bluetooth/data/blacklist.conf:system/etc/bluetooth/blacklist.conf \
        system/bluetooth/data/input.conf:system/etc/bluetooth/input.conf \
        system/bluetooth/data/network.conf:system/etc/bluetooth/network.conf \
        frameworks/base/media/libeffects/data/audio_effects.conf:system/etc/audio_effects.conf

# Get some sounds
$(call inherit-product-if-exists, frameworks/base/data/sounds/AllAudio.mk)

# Get the TTS language packs
$(call inherit-product-if-exists, external/svox/pico/lang/all_pico_languages.mk)

$(call inherit-product-if-exists, frameworks/base/data/fonts/fonts.mk)
$(call inherit-product-if-exists, external/lohit-fonts/fonts.mk)
$(call inherit-product-if-exists, frameworks/base/data/keyboards/keyboards.mk)
$(call inherit-product-if-exists, device/panasonic/p01d/core.mk)
