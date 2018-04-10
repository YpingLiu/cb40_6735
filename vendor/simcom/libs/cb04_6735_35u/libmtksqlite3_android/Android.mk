LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmtksqlite3_android
LOCAL_SRC_FILES := libmtksqlite3_android.so
LOCAL_SHARED_LIBRARIES := libmtk_drvb libmtksqlite3_custom libsqlite libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
include $(BUILD_PREBUILT)
