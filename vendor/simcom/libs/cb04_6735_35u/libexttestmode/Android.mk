LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libexttestmode
LOCAL_SRC_FILES := libexttestmode.so
LOCAL_SHARED_LIBRARIES := libbtcust libbtcusttc1 libbtstd libextsys libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
