LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libMtkOmxVorbisEnc
LOCAL_SRC_FILES := libMtkOmxVorbisEnc.so
LOCAL_SHARED_LIBRARIES := libmtk_drvb libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
