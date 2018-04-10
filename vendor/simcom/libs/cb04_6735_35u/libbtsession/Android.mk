LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libbtsession
LOCAL_SRC_FILES := libbtsession.so
LOCAL_SHARED_LIBRARIES := libbtstd libextsys libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
