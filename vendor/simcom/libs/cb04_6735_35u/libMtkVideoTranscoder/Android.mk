LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libMtkVideoTranscoder
LOCAL_SRC_FILES := libMtkVideoTranscoder.so
LOCAL_SHARED_LIBRARIES := libbinder libdpframework libstagefright libstagefright_foundation libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
