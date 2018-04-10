LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libnvramagentclient
LOCAL_SRC_FILES := libnvramagentclient.so
LOCAL_SHARED_LIBRARIES := libbinder libnvram libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
