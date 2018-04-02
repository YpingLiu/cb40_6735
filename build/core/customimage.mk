

# -----------------------------------------------------------------
# cip-build.prop
#
# add build time for custom.img
INSTALLED_BUILD_CIP_PROP_TARGET := $(TARGET_CUSTOM_OUT)/cip-build.prop
$(INSTALLED_BUILD_CIP_PROP_TARGET): 
	@mkdir -p $(dir $@)
	$(hide) echo "ro.cip.build.date=`date`" > $@
  ifdef OPTR_SPEC_SEG_DEF
  ifneq ($(strip $(OPTR_SPEC_SEG_DEF)),NONE)
	$(hide) echo "ro.operator.optr=$(OPTR)" >> $@
	$(hide) echo "ro.operator.spec=$(SPEC)" >> $@
	$(hide) echo "ro.operator.seg=$(SEG)" >> $@
  endif
  #VoLTE/IMS support
  ifeq ($(strip $(MTK_VOLTE_SUPPORT)), yes)
	$(hide) echo "ro.mtk_volte_support=1" >> $@
	$(hide) echo "persist.dbg.volte_avail_ovr=1" >> $@
  endif
  ifeq ($(strip $(MTK_IMS_SUPPORT)), yes)
	$(hide) echo "ro.mtk_ims_support=1" >> $@
  endif
  ifeq ($(strip $(MTK_WFC_SUPPORT)), yes)
	$(hide) echo "ro.mtk_wfc_support=1" >> $@
  endif
  ifeq ($(strip $(MTK_CLR_CODE_SUPPORT)), yes)
	$(hide) echo "ro.mtk_clr_code_support=1" >> $@
  endif
  ifeq ($(strip $(MTK_PRE_SIM_WO_BAL_SUPPORT)), yes)
	$(hide) echo "ro.mtk_pre_sim_wo_bal_support=1" >> $@
  endif
  #WiFi Offloading support
  ifeq ($(strip $(MTK_EPDG_SUPPORT)), yes)
  ifeq ($(strip $(OPTR)),OP08)
	$(hide) echo "ro.mtk_epdg_support=1" >> $@
	$(hide) echo "persist.net.wo.ssepdg=1" >> $@
	$(hide) echo "persist.net.wo.cust_pcscf_4=16384" >> $@
	$(hide) echo "persist.net.wo.cust_pcscf_6=16386" >> $@
    endif
  ifeq ($(strip $(OPTR)),OP16)
	$(hide) echo "ro.mtk_epdg_support=1" >> $@
	$(hide) echo "persist.net.wo.esp_setup_time=30" >> $@
  endif
  ifeq ($(strip $(OPTR)),OP18)
	$(hide) echo "ro.mtk_epdg_support=1" >> $@
 	$(hide) echo "persist.net.wo.cust_pcscf_4=16389" >> $@
 	$(hide) echo "persist.net.wo.cust_pcscf_6=16390" >> $@
  endif
  endif
endif
ifeq ($(TARGET_USERIMAGES_USE_EXT4),true)

customimage_intermediates := \
    $(call intermediates-dir-for,PACKAGING,custom)

## Generate an ext4 image
define build-customimage-target
    mkdir -p $(TARGET_CUSTOM_OUT)
    mkdir -p $(TARGET_CUSTOM_OUT)/lib
    mkdir -p $(TARGET_CUSTOM_OUT)/lib64
    mkdir -p $(TARGET_CUSTOM_OUT)/app
    mkdir -p $(TARGET_CUSTOM_OUT)/framework
    mkdir -p $(TARGET_CUSTOM_OUT)/plugin
    mkdir -p $(TARGET_CUSTOM_OUT)/media
    mkdir -p $(TARGET_CUSTOM_OUT)/etc
    mkuserimg.sh -s $(PRODUCT_OUT)/custom $(PRODUCT_OUT)/custom.img ext4 custom $(strip $(BOARD_CUSTOMIMAGE_PARTITION_SIZE)) $(PRODUCT_OUT)/root/file_contexts
endef

INSTALLED_CUSTOMIMAGE_TARGET := $(PRODUCT_OUT)/custom.img

INTERNAL_CUSTOMIMAGE_FILES := $(filter $(TARGET_CUSTOM_OUT)/%, $(ALL_PREBUILT), $(ALL_COPIED_HEADERS), $(ALL_GENERATED_SOURCES), $(ALL_DEFAULT_INSTALLED_MODULES), $(INSTALLED_BUILD_CIP_PROP_TARGET))

$(INSTALLED_CUSTOMIMAGE_TARGET) : $(INTERNAL_USERIMAGES_DEPS) $(INTERNAL_CUSTOMIMAGE_FILES)
	$(build-customimage-target)

ifneq ($(strip $(MTK_PROJECT_NAME)),)
-include $(TARGET_OUT_INTERMEDIATES)/PTGEN/partition_size.mk

ALL_CUSTOMIMAGE_CLEAN_FILES := \
        $(PRODUCT_OUT)/custom.img \
        $(TARGET_CUSTOM_OUT) \
        $(TARGET_OUT_COMMON_INTERMEDIATES)/JAVA_LIBRARIES/mediatek-op_intermediates \
        $(TARGET_OUT_INTERMEDIATES)/ETC/DmApnInfo.xml_intermediates \
        $(TARGET_OUT_INTERMEDIATES)/ETC/smsSelfRegConfig.xml_intermediates \
        $(TARGET_OUT_INTERMEDIATES)/ETC/CIP_MD_SBP_intermediates


clean-customimage:
	$(hide) echo $(SHOWTIME) $@ing ...
	$(hide) echo ALL_CUSTOMIMAGE_CLEAN_FILES=$(ALL_CUSTOMIMAGE_CLEAN_FILES)
	$(hide) rm -rf $(ALL_CUSTOMIMAGE_CLEAN_FILES)
	
endif

.PHONY: customimage
customimage: clean-customimage $(INTERNAL_USERIMAGES_DEPS) $(INTERNAL_CUSTOMIMAGE_FILES)
	$(build-customimage-target)

.PHONY: all_customimage
all_customimage:
	echo build all customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT)

.PHONY: op03_customimage
op03_customimage:
	echo build op3 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP03

.PHONY: op06_customimage
op06_customimage:
	echo build op06 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP06

.PHONY: op07_customimage
op07_customimage:
	echo build op07 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP07

.PHONY: op08_customimage
op08_customimage:
	echo build op08 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP08

.PHONY: op11_customimage
op11_customimage:
	echo build op11 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP11

.PHONY: op16_customimage
op16_customimage:
	echo build op16 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP16

.PHONY: op105_customimage
op105_customimage:
	echo build op105 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP105

.PHONY: op117_customimage
op117_customimage:
	echo build op117 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP117

.PHONY: op118_customimage
op118_customimage:
	echo build op118 customimage for $(MTK_PROJECT)
	perl vendor/mediatek/proprietary/operator/common/build/CIPbuild.pl -ini=vendor/mediatek/proprietary/operator/common/build/$(MTK_PROJECT).ini -p=$(MTK_PROJECT) -op=OP118

endif





	
	


