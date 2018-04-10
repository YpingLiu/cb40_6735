LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := netdiag
include $(BUILD_MULTI_PREBUILT)
