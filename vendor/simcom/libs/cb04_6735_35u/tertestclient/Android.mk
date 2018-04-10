LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := tertestclient
include $(BUILD_MULTI_PREBUILT)
