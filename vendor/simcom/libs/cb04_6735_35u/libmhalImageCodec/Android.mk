LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmhalImageCodec
LOCAL_SRC_FILES := libmhalImageCodec.so
LOCAL_SHARED_LIBRARIES := libJpgDecPipe libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
