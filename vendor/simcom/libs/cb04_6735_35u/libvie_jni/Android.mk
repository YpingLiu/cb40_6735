LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libvie_jni
LOCAL_SRC_FILES := libvie_jni.so
LOCAL_SHARED_LIBRARIES := libandroid_runtime libbinder libnativehelper libstdc++ libui libvie
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
