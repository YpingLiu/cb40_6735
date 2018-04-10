LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libpalwlan_mtk
LOCAL_SRC_FILES := libpalwlan_mtk.so
LOCAL_SHARED_LIBRARIES := libnetutils libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
