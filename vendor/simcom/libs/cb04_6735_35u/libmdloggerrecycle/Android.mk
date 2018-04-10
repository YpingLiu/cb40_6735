LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmdloggerrecycle
LOCAL_SRC_FILES := libmdloggerrecycle.so
LOCAL_SHARED_LIBRARIES := libstdc++ libstlport
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
