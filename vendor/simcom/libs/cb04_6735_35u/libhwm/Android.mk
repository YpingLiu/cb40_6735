LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libhwm
LOCAL_SRC_FILES := libhwm.so
LOCAL_SHARED_LIBRARIES := libfile_op libnvram libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
