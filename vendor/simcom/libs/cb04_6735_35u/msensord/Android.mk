LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := msensord
include $(BUILD_MULTI_PREBUILT)
