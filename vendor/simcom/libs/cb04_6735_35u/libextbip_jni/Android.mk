LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libextbip_jni
LOCAL_SRC_FILES := libextbip_jni.so
LOCAL_SHARED_LIBRARIES := libbtstd libextbip libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
