LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_vt_service
LOCAL_SRC_FILES := libmtk_vt_service.so
LOCAL_SHARED_LIBRARIES := libandroid_runtime libbinder libgui libmedia libmtk_vt_client libmtk_vt_em libmtk_vt_swip libmtk_vt_utils libnativehelper libstdc++ libui libvtmal
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
