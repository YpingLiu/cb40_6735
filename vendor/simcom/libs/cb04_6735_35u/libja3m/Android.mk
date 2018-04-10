LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libja3m
LOCAL_SRC_FILES := libja3m.so
LOCAL_SHARED_LIBRARIES := liba3m libandroid libstdc++ libstlport
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
