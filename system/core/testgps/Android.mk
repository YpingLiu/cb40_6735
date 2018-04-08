# Copyright 2006-2014 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= main.c

LOCAL_MODULE := testgps

LOCAL_CFLAGS := -Werror

LOCAL_STATIC_LIBRARIES := \
        libcutils 


include $(BUILD_EXECUTABLE)

include $(call first-makefiles-under,$(LOCAL_PATH))
