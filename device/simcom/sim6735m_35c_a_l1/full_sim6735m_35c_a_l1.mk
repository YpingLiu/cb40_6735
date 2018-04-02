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
$(call inherit-product, device/simcom/sim6735m_35c_a_l1/device.mk)

# set locales & aapt config.
PRODUCT_LOCALES := zh_CN en_US zh_TW

# Set those variables here to overwrite the inherited values.
PRODUCT_MANUFACTURER := alps
PRODUCT_NAME := full_sim6735m_35c_a_l1
PRODUCT_DEVICE := sim6735m_35c_a_l1
PRODUCT_MODEL := sim6735m_35c_a_l1
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := alps

ifeq ($(TARGET_BUILD_VARIANT), eng)
KERNEL_DEFCONFIG ?= sim6735m_35c_a_l1_debug_defconfig
else
KERNEL_DEFCONFIG ?= sim6735m_35c_a_l1_defconfig
endif
PRELOADER_TARGET_PRODUCT ?= sim6735m_35c_a_l1
LK_PROJECT ?= sim6735m_35c_a_l1
