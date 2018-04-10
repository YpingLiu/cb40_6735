LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libvtmal
LOCAL_SRC_FILES := libvtmal.so
LOCAL_SHARED_LIBRARIES := libamr_wrap libamrvt libawb_wrap libbinder libblisrc libcamera_client libcamera_client_mtk libgui libmedia libskia libstagefright libstdc++ libui libvt_custom
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
