LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := nvram_daemon
include $(BUILD_MULTI_PREBUILT)
