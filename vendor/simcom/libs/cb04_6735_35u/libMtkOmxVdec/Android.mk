LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libMtkOmxVdec
LOCAL_SRC_FILES := libMtkOmxVdec.so
LOCAL_SHARED_LIBRARIES := libaed libdpframework libgralloc_extra libgui_ext libhardware libion libion_mtk libmhalImageCodec libmmprofile libstagefright libstdc++ libui libvcodec_utility libvcodecdrv
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
