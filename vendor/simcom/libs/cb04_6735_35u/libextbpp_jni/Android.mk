LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libextbpp_jni
LOCAL_SRC_FILES := libextbpp_jni.so
LOCAL_SHARED_LIBRARIES := libbtstd libextbpp libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
