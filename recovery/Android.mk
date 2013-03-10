# Copyright (C) 2012 Mozilla Foundation
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
# FxOS recovery branding

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE      := librecovery_ui_fxos
LOCAL_SRC_FILES   := recovery_ui.c
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES  := bootable/recovery

LOCAL_STATIC_LIBRARIES += libext4_utils libz
LOCAL_STATIC_LIBRARIES += libminzip libunz libmtdutils libmincrypt
LOCAL_STATIC_LIBRARIES += libminui libpixelflinger_static libpng libcutils
LOCAL_STATIC_LIBRARIES += libstdc++ libc

include $(BUILD_STATIC_LIBRARY)
