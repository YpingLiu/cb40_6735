LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libnativecheck-jni
LOCAL_SRC_FILES := libnativecheck-jni.so
LOCAL_SHARED_LIBRARIES := libandroid_runtime libmtk_drvb libnativehelper libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
