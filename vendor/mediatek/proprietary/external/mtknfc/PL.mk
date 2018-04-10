include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
     mtk_nfc_android_main.c  \
     mtk_nfc_sys.c  \
     halimpl/src/mtk_nfcstackp_test_mode.c  \

         
LOCAL_C_INCLUDES:= \
     $(LOCAL_PATH) \
     $(LOCAL_PATH)/inc  \
     $(LOCAL_PATH)/halimpl/inc \

LOCAL_MODULE := nfcstackp

LOCAL_CFLAGS += -DUSE_GCC -DSUPPORT_I2C  -DSUPPORT_SHARED_LIBRARY -DPORTING_LAYER
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libcutils libc libm libmtknfc libutils libhardware