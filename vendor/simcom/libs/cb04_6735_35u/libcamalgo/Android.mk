LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libcamalgo
LOCAL_SRC_FILES := libcamalgo.so
LOCAL_SHARED_LIBRARIES := libEGL libGLESv2 libbinder libcamdrv libgui libmtk_drvb libperfservicenative libstdc++ libui
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
