LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := bluetooth.blueangel
LOCAL_SRC_FILES := bluetooth.blueangel.so
LOCAL_SHARED_LIBRARIES := libbtcust libbtcusttable libbtsession libbtstd libcustom_prop libstdc++
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/hw
include $(BUILD_PREBUILT)
