LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libgpu_aux
LOCAL_SRC_FILES := libgpu_aux.so
LOCAL_SHARED_LIBRARIES := libdpframework libgralloc_extra libhardware libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
