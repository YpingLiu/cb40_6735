LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_vt_swip
LOCAL_SRC_FILES := libmtk_vt_swip.so
LOCAL_SHARED_LIBRARIES := libmtk_vt_em libmtk_vt_utils libstagefright libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
