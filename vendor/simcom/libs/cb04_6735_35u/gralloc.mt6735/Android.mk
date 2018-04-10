LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := gralloc.mt6735
LOCAL_SRC_FILES := gralloc.mt6735.so
LOCAL_SHARED_LIBRARIES := libGLESv1_CM libgralloc_extra libhardware libion libion_mtk libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
include $(BUILD_PREBUILT)
