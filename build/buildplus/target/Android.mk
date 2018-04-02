LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE               := com.sim.plugin.feature_query
LOCAL_MODULE_TAGS          := optional
LOCAL_SRC_FILES            := DefaultQuery.java FeatureQuery.java
include $(BUILD_JAVA_LIBRARY)