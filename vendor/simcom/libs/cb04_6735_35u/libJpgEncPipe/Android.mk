LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libJpgEncPipe
LOCAL_SRC_FILES := libJpgEncPipe.so
LOCAL_SHARED_LIBRARIES := libion libion_mtk libm4u libmtk_drvb libmtkjpeg libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
