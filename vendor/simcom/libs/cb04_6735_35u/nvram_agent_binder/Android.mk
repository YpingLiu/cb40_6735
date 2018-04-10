LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := nvram_agent_binder
include $(BUILD_MULTI_PREBUILT)
