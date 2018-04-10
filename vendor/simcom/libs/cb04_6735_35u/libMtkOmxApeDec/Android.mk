LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libMtkOmxApeDec
LOCAL_SRC_FILES := libMtkOmxApeDec.so
LOCAL_SHARED_LIBRARIES := libbinder libstagefright_foundation libstagefright_omx libstdc++ libui
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
