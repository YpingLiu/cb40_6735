#
# Copyright (C) 2014 MediaTek Inc.
# Modification based on code covered by the mentioned copyright
# and/or permission notice(s).
#
# Copyright 2008 The Android Open Source Project

LOCAL_PATH := $(my-dir)


# the custom dex'ed emma library ready to put on a device.
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, core pregenerated)

LOCAL_MODULE := emma

LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_RESOURCE_DIRS := core/res pregenerated/res

LOCAL_SDK_VERSION := 8

include $(BUILD_STATIC_JAVA_LIBRARY)

ifeq ($(strip $(BUILD_MTK_API_DEP)), yes)
# emma API table.
# ============================================================
LOCAL_MODULE := emma-api

LOCAL_JAVA_LIBRARIES += $(LOCAL_STATIC_JAVA_LIBRARIES)
LOCAL_MODULE_CLASS := JAVA_LIBRARIES

LOCAL_DROIDDOC_OPTIONS:= \
		-api $(TARGET_OUT_COMMON_INTERMEDIATES)/PACKAGING/emma-api.txt \
		-nodocs \
		-hidden

include $(BUILD_DROIDDOC)
endif


# the custom emma library to add to an SDK project.
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, core pregenerated)

LOCAL_MODULE := emmalib

LOCAL_JAVA_RESOURCE_DIRS := core/res pregenerated/res

include $(BUILD_HOST_JAVA_LIBRARY)
