# Inherit from those products. Most specific first.
#$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)

# Set target and base project for flavor build
MTK_TARGET_PROJECT := $(subst full_,,$(TARGET_PRODUCT))
MTK_BASE_PROJECT := $(MTK_TARGET_PROJECT)
MTK_PROJECT_FOLDER := $(shell find device/* -maxdepth 1 -name $(MTK_BASE_PROJECT))
MTK_TARGET_PROJECT_FOLDER := $(shell find device/* -maxdepth 1 -name $(MTK_TARGET_PROJECT))

# This is where we'd set a backup provider if we had one
#$(call inherit-product, device/sample/products/backup_overlay.mk)
# Inherit from maguro device
$(call inherit-product, device/simcom/cb04_6735_35u/device.mk)

# set locales & aapt config.
PRODUCT_LOCALES := en_US zh_CN zh_TW es_ES pt_BR ru_RU fr_FR de_DE tr_TR vi_VN ms_MY in_ID th_TH it_IT ar_EG hi_IN bn_IN ur_PK fa_IR pt_PT nl_NL el_GR hu_HU tl_PH ro_RO cs_CZ ko_KR km_KH iw_IL my_MM pl_PL es_US bg_BG hr_HR lv_LV lt_LT sk_SK uk_UA de_AT da_DK fi_FI nb_NO sv_SE en_GB hy_AM zh_HK et_EE ja_JP kk_KZ sr_RS sl_SI ca_ES


# Set those variables here to overwrite the inherited values.
PRODUCT_MANUFACTURER := alps
PRODUCT_NAME := full_cb04_6735_35u
PRODUCT_DEVICE := cb04_6735_35u
PRODUCT_MODEL := cb04_6735_35u
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := alps

include build/buildplus/target/QRDExt_target.min

ifeq ($(TARGET_BUILD_VARIANT), eng)
ifeq ($(GZ_HG), true)
KERNEL_DEFCONFIG ?= cb04_6735_35u_gz_hg_debug_defconfig
else
KERNEL_DEFCONFIG ?= cb04_6735_35u_debug_defconfig
endif #GZ_HG eng
else #TARGET_BUILD_VARIANT
ifeq ($(GZ_HG), true)
KERNEL_DEFCONFIG ?= cb04_6735_35u_gz_hg_defconfig
else
KERNEL_DEFCONFIG ?= cb04_6735_35u_defconfig
endif #GZ_HG user
endif

PRELOADER_TARGET_PRODUCT ?= cb04_6735_35u
LK_PROJECT ?= cb04_6735_35u
