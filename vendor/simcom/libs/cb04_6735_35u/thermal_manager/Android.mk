LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := thermal_manager
include $(BUILD_MULTI_PREBUILT)
