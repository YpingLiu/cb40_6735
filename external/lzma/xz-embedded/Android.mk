#
# Copyright (C) 2014 MediaTek Inc.
# Modification based on code covered by the mentioned copyright
# and/or permission notice(s).
#
# Copyright 2013 The Android Open Source Project
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libxz
LOCAL_MULTILIB := both
LOCAL_SRC_FILES := xz_crc32.c xz_dec_lzma2.c xz_dec_stream.c
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libxz_host
LOCAL_SRC_FILES := xz_crc32.c xz_dec_lzma2.c xz_dec_stream.c 
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_HOST_STATIC_LIBRARY)
