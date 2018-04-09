PREBUILT_PATH := $(call my-dir)
LOCAL_PATH := $(PREBUILT_PATH)


$(warning +++myAndroid.mk+++LOCAL_PATH:$(PREBUILT_PATH) PRODUCT_OUT:$(PRODUCT_OUT))

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libcurl-ss/curl
LOCAL_COPY_HEADERS := ../../.././target/product/cb04_6735_35u/obj/include/libcurl-ss/curl/mprintf.h
include $(BUILD_COPY_HEADERS)

$(shell mkdir -p $(PRODUCT_OUT)/obj/PRELOADER_OBJ/inc/)
$(shell cp -r $(LOCAL_PATH)/obj/PRELOADER_OBJ/inc/KEY_IMAGE_AUTH.h $(PRODUCT_OUT)/obj/PRELOADER_OBJ/inc/KEY_IMAGE_AUTH.h)
$(shell cp -r $(LOCAL_PATH)/obj/PRELOADER_OBJ/inc/KEY_SML_ENCODE.h $(PRODUCT_OUT)/obj/PRELOADER_OBJ/inc/KEY_SML_ENCODE.h)
$(shell cp -r $(LOCAL_PATH)/obj/PRELOADER_OBJ/inc/KEY_SML_AUTH.h $(PRODUCT_OUT)/obj/PRELOADER_OBJ/inc/KEY_SML_AUTH.h)

$(shell mkdir -p $(PRODUCT_OUT)/system/framework/)
#$(shell cp -r $(LOCAL_PATH)/system/framework/mediatek-common.jar $(PRODUCT_OUT)/system/framework/mediatek-common.jar)

$(shell mkdir -p $(PRODUCT_OUT)/symbols/system/bin/)
$(shell cp -r $(LOCAL_PATH)/symbols/system/bin/m4u_ut $(PRODUCT_OUT)/symbols/system/bin/m4u_ut)

$(shell cp -r $(LOCAL_PATH)/system/bin/* $(PRODUCT_OUT)/system/bin/)

$(shell cp -r $(LOCAL_PATH)/obj/lib/ $(PRODUCT_OUT)/obj/)

$(shell mkdir -p $(PRODUCT_OUT)/../../../target/common/obj/APPS/mediatek-res_intermediates/src/com/mediatek/internal)
$(shell cp -r $(LOCAL_PATH)/../../.././target/common/obj/APPS/mediatek-res_intermediates/src/R.stamp $(PRODUCT_OUT)/../../../target/common/obj/APPS/mediatek-res_intermediates/src/R.stamp)
$(shell cp -r $(LOCAL_PATH)/../../.././target/common/obj/APPS/mediatek-res_intermediates/src/com/mediatek/*.java $(PRODUCT_OUT)/../../../target/common/obj/APPS/mediatek-res_intermediates/src/com/mediatek/)
$(shell cp -r $(LOCAL_PATH)/../../.././target/common/obj/APPS/mediatek-res_intermediates/src/com/mediatek/internal/*.java $(PRODUCT_OUT)/../../../target/common/obj/APPS/mediatek-res_intermediates/src/com/mediatek/internal/)

#$(warning copy files, wait...)
#$(shell cp -r $(LOCAL_PATH)/obj/APPS/ $(PRODUCT_OUT)/obj/)
#$(shell cp -r $(LOCAL_PATH)/../../.././target/product/cb04_6735_35u/obj/SHARED_LIBRARIES/ $(PRODUCT_OUT)/obj/)

#$(shell cp -r $(LOCAL_PATH)/../../.././frameworks/base/mediatek_public_api.txt $(PRODUCT_OUT)/../../../target/common/obj/PACKAGING/mediatek_public_api.txt)

#include $(CLEAR_VARS)
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE       := mediatek-common
#LOCAL_MODULE_CLASS := JAVA_LIBRARIES
#LOCAL_MODULE_PATH  := $(PRODUCT_OUT)/system/framework
#LOCAL_SRC_FILES    += ../../.././target/product/cb04_6735_35u/system/framework/mediatek-common.jar
#LOCAL_SRC_FILES    += ../../.././target/product/cb04_6735_35u/system/framework/IAudioProfileService.aidl \
#		      ../../.././target/product/cb04_6735_35u/system/framework/IAudioProfileListener.aidl
#include $(BUILD_JAVA_LIBRARY)
#include $(BUILD_PREBUILT)


$(warning +++myAndroid.mk+++)
include $(CLEAR_VARS)
LOCAL_MODULE        := libem_lte_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_lte_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

$(warning +++1 +++)
include $(CLEAR_VARS)
LOCAL_MODULE        := libvsim-adaptor-client
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libvsim-adaptor-client.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmtkshifter
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmtkshifter.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam3_utils
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam3_utils.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libMtkVideoSpeedEffect
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libMtkVideoSpeedEffect.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libbwc
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libbwc.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libjni_pq
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libjni_pq.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam3_hwpipeline
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam3_hwpipeline.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_wifi_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_wifi_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := librilmtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/librilmtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam3_pipeline
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam3_pipeline.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcamdrv
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcamdrv.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcameracustom
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcameracustom.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libdrmmtkwhitelist
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libdrmmtkwhitelist.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libcustom_jni
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcustom_jni.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libshowlogo
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libshowlogo.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libPQDCjni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libPQDCjni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmatv_cust
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmatv_cust.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libSonyIMX230PdafLibrary
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libSonyIMX230PdafLibrary.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libJniAtvService
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libJniAtvService.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaed
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaed.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libatvctrlservice
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libatvctrlservice.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam_mmp
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam_mmp.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libtimestretch
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libtimestretch.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam3_hwnode
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam3_hwnode.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libhotknot
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libhotknot.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libsimmelock
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libsimmelock.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcvsd_mtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcvsd_mtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libsec
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libsec.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmpodecoder
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmpodecoder.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmpo
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmpo.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libfgauge
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libfgauge.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libPQjni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libPQjni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libgifEncoder_jni
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libgifEncoder_jni.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libm4u
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libm4u.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_usb_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_usb_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmsbc_mtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmsbc_mtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmtklimiter
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmtklimiter.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmpoencoder
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmpoencoder.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libjni_lomoeffect
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libjni_lomoeffect.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_support_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_support_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libatciserv_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libatciserv_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libblisrc
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libblisrc.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libbessound_hd_mtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libbessound_hd_mtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libccci_util
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libccci_util.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libamrvt
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libamrvt.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libsbccodec
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libsbccodec.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_modem_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_modem_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libfeatureio
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libfeatureio.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam_hwutils
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam_hwutils.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libion_mtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libion_mtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libacdk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libacdk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmmsdkservice
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmmsdkservice.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmmsdkservice.feature_platform
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmmsdkservice.feature_platform.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmmsdkservice.feature
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmmsdkservice.feature.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmemorydumper
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmemorydumper.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libimageio_plat_drv
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libimageio_plat_drv.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libBnMtkCodec
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libBnMtkCodec.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libdirect-coredump
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libdirect-coredump.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcamera_client_mtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcamera_client_mtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaudiocompensationfilter
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaudiocompensationfilter.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaudiomtkdcremoval
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaudiomtkdcremoval.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam_utils
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam_utils.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libvt_custom
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libvt_custom.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libimageio
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libimageio.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libbessurround_mtk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libbessurround_mtk.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libgui_ext
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libgui_ext.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaudiocomponentengine
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaudiocomponentengine.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmtksqlite3_custom
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmtksqlite3_custom.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libMiraVision_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libMiraVision_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmtb
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmtb.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam_platform
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam_platform.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := librilmtkmd2
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/librilmtkmd2.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libpq_cust
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libpq_cust.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libfeatureiodrv
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libfeatureiodrv.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcustom_nvram
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcustom_nvram.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_bt_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_bt_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam3_app
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam3_app.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_sensor_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_sensor_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaudiocustparam
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaudiocustparam.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaudiosetting
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaudiosetting.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libmtk_mmutils
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libmtk_mmutils.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libAGifEncoder
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libAGifEncoder.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libui_ext
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libui_ext.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libaudiodcrflt
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libaudiodcrflt.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libspeech_enh_lib
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libspeech_enh_lib.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libcam1_utils
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcam1_utils.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libipod
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libipod.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libem_gpio_jni
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libem_gpio_jni.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libgralloc_extra
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libgralloc_extra.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := libblisrc32
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libblisrc32.so
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libcustom_prop
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/lib/libcustom_prop.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/lib
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := libwvm
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_MODULE_OWNER  := mediatek
#LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/vendor/lib/libwvm.so
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/vendor/lib
#LOCAL_PROPRIETARY_MODULE := true
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := CalendarImporter
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := APPS
#LOCAL_CERTIFICATE   := platform
#LOCAL_MODULE_SUFFIX := .apk
#LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/CalendarImporter
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := FileManager
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/FileManager
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := DataTransfer
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/DataTransfer
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := YGPS
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/YGPS
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := SystemUpdate
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/SystemUpdate
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := SystemUpdateAssistant
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/SystemUpdateAssistant
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := AutoDialer
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/AutoDialer
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := AtciService
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/AtciService
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := MtkFloatMenu
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/MtkFloatMenu
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := BLEManager
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/BLEManager
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := BatteryWarning
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/BatteryWarning
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := MiraVision
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/MiraVision
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := EngineerMode
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/EngineerMode
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := BSPTelephonyDevTool
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/BSPTelephonyDevTool
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := Omacp
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/Omacp
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := DrmProvider
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/DrmProvider
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := MTKAndroidSuiteDaemon
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/MTKAndroidSuiteDaemon
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := NlpService
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/NlpService
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE        := MtkBt
#LOCAL_MODULE_TAGS   := optional
#LOCAL_MODULE_CLASS  := APPS
#LOCAL_CERTIFICATE   := platform
#LOCAL_MODULE_SUFFIX := .apk
#LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/MtkBt
#LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
#include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := SchedulePowerOnOff
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/SchedulePowerOnOff
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := MTKThermalManager
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES		:= ../../.././target/product/cb04_6735_35u/system/app/MTKThermalManager
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/app/
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := ccaptest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/ccaptest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := gsm0710muxd
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/gsm0710muxd
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := acdk_camshottest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/acdk_camshottest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := fuelgauged
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/fuelgauged
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := dmlog
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/dmlog
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := batterywarning
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/batterywarning
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := autokd
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/autokd
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := ppl_agent
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/ppl_agent
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := sbchk
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/sbchk
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := audioregsetting
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/audioregsetting
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := thermald
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/thermald
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := audiocommand
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/audiocommand
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := perf_native_test
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/perf_native_test
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := matv
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/matv
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := GoogleOtaBinder
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/GoogleOtaBinder
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := boot_logo_updater
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/boot_logo_updater
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := ipod
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/ipod
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := pppd_btdun
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/pppd_btdun
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := fbsdktest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/fbsdktest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := kpoc_charger
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/kpoc_charger
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := em_svr
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/em_svr
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := test_fdnode
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/test_fdnode
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := iopipetest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/iopipetest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := MtkCodecService
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/MtkCodecService
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := resize_ext4
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/resize_ext4
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := testconverter
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/testconverter
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := tiny_swapoff
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/tiny_swapoff
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := lcdc_screen_cap
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/lcdc_screen_cap
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mtkrild
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/mtkrild
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := superumount
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/superumount
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := AudioSetParam
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/AudioSetParam
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := camshottest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/camshottest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := camtest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/camtest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mtk_setprop
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/mtk_setprop
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := pq
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/pq
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := permission_check
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/permission_check
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := sn
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/sn
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := gsm0710muxdmd2
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/gsm0710muxdmd2
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := tiny_swapon
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/tiny_swapon
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := dhcp6ctl
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/dhcp6ctl
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := pppd_via
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/pppd_via
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := camnodetest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/camnodetest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := atcid
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/atcid
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := tiny_mkswap
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/tiny_mkswap
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := dhcp6s
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/dhcp6s
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := testmetadata
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/testmetadata
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := atci_service
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/atci_service
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := memorydumper
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/memorydumper
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mtkrildmd2
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/mtkrildmd2
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := wifi2agps
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/wifi2agps
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := m4u_ut
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/m4u_ut
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := md_ctrl
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/md_ctrl
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := downloader
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/downloader
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mtkradiooptions
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/mtkradiooptions
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := fbconfig
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/fbconfig
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := guiext-server
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/guiext-server
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := camtest_imagebuffer
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/camtest_imagebuffer
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := thermal
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/thermal
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := factory
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/factory
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mmsdkhaltest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/mmsdkhaltest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := dm_agent_binder
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/dm_agent_binder
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := dhcp6c
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/dhcp6c
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := test_raw16node
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/test_raw16node
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := mmsdktest
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/mmsdktest
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := audiocmdservice_atci
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/audiocmdservice_atci
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE        := meta_tst
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := EXECUTABLES
LOCAL_SRC_FILES     := ../../.././target/product/cb04_6735_35u/system/bin/meta_tst
LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/system/bin
include $(BUILD_PREBUILT)

