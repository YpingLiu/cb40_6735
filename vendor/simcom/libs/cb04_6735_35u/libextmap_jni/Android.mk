LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libextmap_jni
LOCAL_SRC_FILES := libextmap_jni.so
LOCAL_SHARED_LIBRARIES := libbtstd libextmap libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
