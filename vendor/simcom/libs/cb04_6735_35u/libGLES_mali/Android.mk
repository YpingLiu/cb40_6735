LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libGLES_mali
LOCAL_SRC_FILES := libGLES_mali.so
LOCAL_SHARED_LIBRARIES := libbinder libdpframework libged libgpu_aux libgralloc_extra libhardware libmtk_drvb libstdc++ libstlport libsync libui
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl
include $(BUILD_PREBUILT)
