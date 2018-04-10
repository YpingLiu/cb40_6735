LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libfile_op
LOCAL_SRC_FILES := libfile_op.so
LOCAL_SHARED_LIBRARIES := libcustom_nvram libnvram libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
