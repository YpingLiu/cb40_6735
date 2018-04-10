LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libMtkOmxVenc
LOCAL_SRC_FILES := libMtkOmxVenc.so
LOCAL_SHARED_LIBRARIES := libbinder libdpframework libgralloc_extra libhardware libion libion_mtk libstdc++ libui libvcodec_utility libvcodecdrv
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
