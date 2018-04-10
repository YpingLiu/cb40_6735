LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_vt_client
LOCAL_SRC_FILES := libmtk_vt_client.so
LOCAL_SHARED_LIBRARIES := libandroid_runtime libbinder libgui libmtk_vt_em libmtk_vt_utils libnativehelper libstdc++ libui
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
