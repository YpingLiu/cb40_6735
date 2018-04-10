LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := audio.a2dp.blueangel
LOCAL_SRC_FILES := audio.a2dp.blueangel.so
LOCAL_SHARED_LIBRARIES := libbtcust libmedia libpower libsbccodec libstagefright libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/hw
include $(BUILD_PREBUILT)
