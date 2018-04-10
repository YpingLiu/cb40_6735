LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := xlog_perf
include $(BUILD_MULTI_PREBUILT)
