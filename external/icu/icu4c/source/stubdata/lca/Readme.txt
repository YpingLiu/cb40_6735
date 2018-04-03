This folder is added for slim the icu for the LCA project;

User Guide

1. Edit the *files.mk under the each folder in the resfiles,
add or remove the locale resource name in the resource list;

2. run the command icu-lca-build, and the new dat file will be
updated;

-------------------------Slim List------------------------------------
--------briktr
cjdict.txt khmerdict.txt laodict.txt thaidict.txt

--------coll
#kn.txt fa_AF.txt fr_CA.txt bs_Cyrl.txt sr_Latn.txt 

--------curr
#LCA Slim
CURR_SYNTHETIC_ALIAS
# az_AZ.txt az_Latn_AZ.txt bs_BA.txt bs_Latn_BA.txt\
# en_NH.txt en_RH.txt fil_PH.txt ha_GH.txt ha_Latn_NE.txt\
# ha_Latn_NG.txt ha_NE.txt ha_NG.txt he_IL.txt id_ID.txt\
# in_ID.txt iw_IL.txt ja_JP.txt\
# ja_JP_TRADITIONAL.txt kk_Cyrl_KZ.txt kk_KZ.txt ks_Arab_IN.txt ks_IN.txt\
# ky_Cyrl_KG.txt ky_KG.txt mn_Cyrl_MN.txt mn_MN.txt\
# ms_BN.txt ms_Latn_MY.txt ms_MY.txt ms_SG.txt nb_NO.txt\
# nn_NO.txt no_NO.txt no_NO_NY.txt pa_Guru_IN.txt\
# pa_IN.txt pa_PK.txt sh_BA.txt sh_CS.txt\
# sh_YU.txt shi_MA.txt sr_BA.txt sr_CS.txt sr_Cyrl_CS.txt\
# sr_Cyrl_RS.txt sr_Cyrl_XK.txt sr_Cyrl_YU.txt sr_Latn_CS.txt sr_Latn_YU.txt\
# sr_ME.txt sr_RS.txt sr_XK.txt sr_YU.txt th_TH.txt\
# th_TH_TRADITIONAL.txt tl_PH.txt tzm_Latn_MA.txt tzm_MA.txt\
# ug_Arab_CN.txt ug_CN.txt uz_AF.txt uz_UZ.txt vai_LR.txt\
# vai_Vaii_LR.txt zh_CN.txt zh_HK.txt zh_Hans_CN.txt zh_MO.txt\
# zh_SG.txt zh_TW.txt
CURR_SOURCE
#bs_Cyrl.txt bs_Latn.txt bs.txt ug.txt ml.txt nb.txt nd.txt
#sr.txt sr_Cyrl.txt sr_Cyrl_BA.txt sr_Latn.txt sr_Latn_BA.txt sr_Latn_ME.txt
#sr_Latn_RS.txt el.txt nl.txt ro.txt rof.txt ru.txt ee.txt el.txt
#ka.txt cy.txt az.txt az_Cyrl.txt az_Cyrl_AZ.txt az_Latn.txt gsw.txt
#te.txt nn.txt ne.txt ne_IN.txt sk.txt mk.txt gu.txt lv.txt mr.txt pa.txt
#pa_Arab.txt pa_Arab_PK.txt pa_Guru.txt fi.txt cs.txt ar.txt
#br.txt br.txt ky.txt ky_Cyrl.txt is.txt am.txt mn.txt mn_Cyrl.txt id.txt
#lo.txt brx.txt eu.txt sq.txt sq_MK.txt fil.txt ta.txt ta_LK.txt ta_MY.txt
#ta_SG.txt ks.txt ks_Arab.txt si.txt he.txt rm.txt gl.txt fa.txt fa_AF.txt
#ga.txt kn.txt af.txt af_NA.txt zu.txt uz.txt uz_Arab.txt uz_Arab_AF.txt
#uz_Cyrl.txt uz_Latn.txt uz_Latn_UZ.txt dz.txt lt.txt 
#en_AU.txt en_BB.txt en_BE.txt en_BM.txt en_BS.txt
#en_BW.txt en_BZ.txt en_CA.txt en_CC.txt en_CK.txt
#en_CM.txt en_CX.txt en_DG.txt en_DM.txt en_ER.txt
#en_FJ.txt en_FK.txt en_FM.txt en_GB.txt en_GD.txt
#en_GG.txt en_GH.txt en_GI.txt en_GM.txt en_GY.txt
#en_HK.txt en_IE.txt en_IM.txt en_IN.txt en_IO.txt
#en_JE.txt en_JM.txt en_KE.txt en_KI.txt en_KN.txt
#en_KY.txt en_LC.txt en_LR.txt en_LS.txt en_MG.txt
#en_MO.txt en_MS.txt en_MT.txt en_MU.txt en_MW.txt
#en_NA.txt en_NF.txt en_NG.txt en_NR.txt en_NU.txt
#en_NZ.txt en_PG.txt en_PH.txt en_PK.txt en_PN.txt
#en_PW.txt en_RW.txt en_SB.txt en_SC.txt en_SD.txt
#en_SG.txt en_SH.txt en_SL.txt en_SS.txt en_SX.txt
#en_SZ.txt en_TC.txt en_TK.txt en_TO.txt en_TT.txt
#en_TV.txt en_TZ.txt en_UG.txt en_VC.txt en_VG.txt
#en_VU.txt en_WS.txt en_ZA.txt en_ZM.txt en_ZW.txt
#es_CO.txt es_CR.txt es_CU.txt es_DO.txt es_EC.txt
#es_GT.txt es_HN.txt es_MX.txt es_NI.txt es_PA.txt
#es_PE.txt es_PH.txt es_PR.txt es_PY.txt es_SV.txt
#es_US.txt es_UY.txt es_VE.txt kk_Cyrl.txt
#fr_CD.txt fr_CH.txt fr_DJ.txt fr_DZ.txt fr_GN.txt
#fr_HT.txt fr_KM.txt fr_LU.txt fr_MG.txt fr_MR.txt
#fr_MU.txt fr_RW.txt fr_SC.txt fr_SY.txt fr_TN.txt fr_VU.txt
#pt_AO.txt pt_CV.txt pt_GW.txt pt_MO.txt pt_MZ.txt
#ar_DJ.txt ar_ER.txt ar_LB.txt it_CH.txt ca_FR.txt
#ar_SO.txt ar_SS.txt de_CH.txt de_LU.txt ln_AO.txt
#en_001.txt en_150.txt en_AG.txt en_AI.txt  ti_ER.txt
#es_AR.txt es_BO.txt es_CL.txt fr_BI.txt fr_CA.txt
#ha_Latn.txt ha_Latn_GH.txt hr_BA.txt bo_IN.txt

--------lang
#LCA Slim
LANG_SYNTHETIC_ALIAS
# az_AZ.txt az_Latn_AZ.txt bs_BA.txt bs_Latn_BA.txt\
# en_NH.txt en_RH.txt fil_PH.txt ha_GH.txt ha_Latn_GH.txt\
# ha_Latn_NE.txt ha_Latn_NG.txt ha_NE.txt ha_NG.txt he_IL.txt\
# id_ID.txt in_ID.txt iw_IL.txt\
# ja_JP.txt ja_JP_TRADITIONAL.txt kk_Cyrl_KZ.txt kk_KZ.txt ks_Arab_IN.txt\
# ks_IN.txt ky_Cyrl_KG.txt ky_KG.txt mn_Cyrl_MN.txt mn_MN.txt\
# ms_BN.txt ms_Latn_BN.txt ms_Latn_MY.txt ms_Latn_SG.txt\
# ms_MY.txt ms_SG.txt nb_NO.txt nn_NO.txt\
# no_NO.txt no_NO_NY.txt pa_Guru_IN.txt pa_IN.txt pa_PK.txt\
# ro_MD.txt sh_BA.txt sh_CS.txt sh_YU.txt\
# shi_MA.txt sr_BA.txt sr_CS.txt sr_Cyrl_BA.txt sr_Cyrl_CS.txt\
# sr_Cyrl_RS.txt sr_Cyrl_XK.txt sr_Cyrl_YU.txt sr_Latn_CS.txt sr_Latn_YU.txt\
# sr_ME.txt sr_RS.txt sr_XK.txt sr_YU.txt th_TH.txt\
# th_TH_TRADITIONAL.txt tl_PH.txt tzm_Latn_MA.txt tzm_MA.txt\
# ug_Arab_CN.txt ug_CN.txt uz_AF.txt uz_UZ.txt vai_LR.txt\
# vai_Vaii_LR.txt zh_CN.txt zh_HK.txt zh_Hans_CN.txt zh_MO.txt\
# zh_SG.txt zh_TW.txt
LANG_SOURCE
#ml.txt lo.txt ta.txt te.txt kn.txt ug.txt ug_Arab.txt el.txt
#mr.txt gu.txt bs.txt bs_Cyrl.txt bs_Latn.txt ar.txt cs.txt
#ka.txt sv.txt sv_FI.txt nb.txt da.txt brx.txt fi.txt pl.txt
#nl.txt nl_BE.txt he.txt or.txt ru.txt lt.txt bg.txt pt.txt
#mk.txt es.txt sr.txt hr.txt cy.txt dav.txt ro.txt 
#ks.txt ks_Arab.txt gsw.txt is.txt nn.txt br.txt rm.txt
#az.txt az_Cyrl.txt az_Cyrl_AZ.txt az_Latn.txt ne.txt dz.txt gl.txt zu.txt
#fil.txt eu.txt si.txt mt.txt af.txt pa.txt pa_Arab.txt pa_Arab_PK.txt pa_Guru.txt to.txt
#ee.txt kk.txt kk_Cyrl.txt ky.txt ky_Cyrl.txt mn.txt mn_Cyrl.txt
#uz.txt uz_Arab.txt uz_Arab_AF.txt uz_Cyrl.txt uz_Latn.txt uz_Latn_UZ.txt
#en_150.txt en_AG.txt en_AI.txt en_AU.txt en_BB.txt\
#en_BE.txt en_BM.txt en_BS.txt en_BW.txt en_BZ.txt\
#en_CC.txt en_CK.txt en_CM.txt en_CX.txt en_DG.txt\
#en_DM.txt en_ER.txt en_FJ.txt en_FK.txt en_FM.txt\
#en_GB.txt en_GD.txt en_GG.txt en_GH.txt en_GI.txt\
#en_GM.txt en_GY.txt en_HK.txt en_IE.txt en_IM.txt\
#en_IN.txt en_IO.txt en_JE.txt en_JM.txt en_KE.txt\
#en_KI.txt en_KN.txt en_KY.txt en_LC.txt en_LR.txt\
#en_LS.txt en_MG.txt en_MO.txt en_MS.txt en_MT.txt\
#en_MU.txt en_MW.txt en_NA.txt en_NF.txt en_NG.txt\
#en_NR.txt en_NU.txt en_NZ.txt en_PG.txt en_PH.txt\
#en_PK.txt en_PN.txt en_PW.txt en_RW.txt en_SB.txt\
#en_SC.txt en_SD.txt en_SG.txt en_SH.txt en_SL.txt\
#en_SS.txt en_SX.txt en_SZ.txt en_TC.txt en_TK.txt\
#en_TO.txt en_TT.txt en_TV.txt en_TZ.txt en_UG.txt\
#en_VC.txt en_VG.txt en_VU.txt en_WS.txt en_ZA.txt\
#en_ZM.txt en_ZW.txt ms_Latn.txt fa_AF.txt fr_CA.txt
#es_AR.txt es_BO.txt es_CL.txt es_CO.txt es_CR.txt
#es_CU.txt es_DO.txt es_EC.txt es_GT.txt es_HN.txt
#es_MX.txt es_NI.txt es_PA.txt es_PE.txt es_PR.txt
#es_PY.txt es_SV.txt es_US.txt es_UY.txt es_VE.txt
#pt_AO.txt pt_CV.txt pt_GW.txt ru_UA.txt tzm_Latn.txt
#pt_MO.txt pt_MZ.txt pt_PT.txt pt_ST.txt pt_TL.txt
#ar_EG.txt ar_LB.txt bn_IN.txt de_CH.txt es_419.txt
#ha_Latn.txt shi_Latn.txt shi_Tfng.txt shi_Tfng_MA.txt
#sr_Cyrl.txt sr_Latn.txt sr_Latn_BA.txt sr_Latn_ME.txt sr_Latn_RS.txt
#vai_Latn_LR.txt vai_Vaii.txt vai_Latn.txt yo_BJ.txt

--------locales
#LCA Slim
GENRB_SYNTHETIC_ALIAS
# az_AZ.txt bs_BA.txt en_NH.txt en_RH.txt\
# ha_GH.txt ha_NE.txt ha_NG.txt in_ID.txt\
# iw_IL.txt ja_JP_TRADITIONAL.txt kk_KZ.txt ks_IN.txt\
# ky_KG.txt mn_MN.txt ms_BN.txt ms_MY.txt\
# ms_SG.txt no_NO.txt no_NO_NY.txt pa_IN.txt\
# pa_PK.txt sh_BA.txt sh_CS.txt sh_YU.txt\
# shi_MA.txt sr_BA.txt sr_CS.txt sr_Cyrl_CS.txt sr_Cyrl_YU.txt\
# sr_Latn_CS.txt sr_Latn_YU.txt sr_ME.txt sr_RS.txt sr_XK.txt\
# sr_YU.txt th_TH_TRADITIONAL.txt tl_PH.txt tzm_MA.txt\
# ug_CN.txt uz_AF.txt uz_UZ.txt vai_LR.txt zh_CN.txt\
# zh_HK.txt zh_MO.txt zh_SG.txt zh_TW.txt
GENRB_SOURCE
#nl.txt nl_AW.txt nl_BE.txt nl_BQ.txt
#nl_CW.txt nl_NL.txt nl_SR.txt nl_SX.txt lo.txt lo_LA.txt
#he.txt he_IL.txt id.txt id_ID.txt
#cy.txt cy_GB.txt sk.txt sk_SK.txt ta.txt ta_IN.txt ta_LK.txt
#ta_MY.txt ta_SG.txt ml.txt ml_IN.txt
#te.txt te_IN.txt mr.txt mr_IN.txt gu.txt gu_IN.txt kn.txt kn_IN.txt
#ka.txt ka_GE.txt tr.txt tr_CY.txt tr_TR.txt ne.txt ne_IN.txt ne_NP.txt
#el.txt si.txt  si_LK.txt pa.txt pa_Arab.txt pa_Arab_PK.txt pa_Guru.txt pa_Guru_IN.txt
#mn.txt mn_Cyrl.txt mn_Cyrl_MN.txt bs.txt bs_Cyrl.txt bs_Cyrl_BA.txt bs_Latn.txt bs_Latn_BA.txt
#mk.txt mk_MK.txt ug.txt ug_Arab.txt ug_Arab_CN.txt
#eu.txt eu_ES.txt is.txt is_IS.txt ky.txt ky_Cyrl_KG.txt fil.txt fil_PH.txt af.txt af_NA.txt af_ZA.txt
#uz.txt uz_Arab.txt uz_Arab_AF.txt uz_Cyrl.txt uz_Cyrl_UZ.txt uz_Latn.txt uz_Latn_UZ.txt
#sq.txt sq_AL.txt sq_MK.txt sq_XK.txt az.txt az_Cyrl.txt
#az_Cyrl_AZ.txt az_Latn.txt az_Latn_AZ.txt sw.txt sw_KE.txt sw_TZ.txt sw_UG.txt 
#to.txt to_TO.txt zu.txt zu_ZA.txt
#gl.txt gl_ES.txt dz.txt dz_BT.txt ee.txt ee_GH.txt ee_TG.txt 
#kea.txt kea_CV.txt nn.txt nn_NO.txt gsw.txt gsw_CH.txt gsw_LI.txt rm.txt rm_CH.txt
#be.txt be_BY.txt chr.txt chr_US.txt jgo.txt jgo_CM.txt mgo.txt mgo_CM.txt
#brx.txt brx_IN.txt ks.txt ks_Arab.txt ks_Arab_IN.txt kl.txt kl_GL.txt nnh.txt nnh_CM.txt
#en_AI.txt en_AS.txt en_AU.txt en_BB.txt en_BE.txt\
#en_BM.txt en_BS.txt en_BW.txt en_BZ.txt en_CA.txt\
#en_CC.txt en_CK.txt en_CM.txt en_CX.txt en_DG.txt\
#en_DM.txt en_ER.txt en_FJ.txt en_FK.txt en_FM.txt\
#en_GB.txt en_GD.txt en_GG.txt en_GH.txt en_GI.txt\
#en_GM.txt en_GU.txt en_GY.txt en_HK.txt en_IE.txt\
#en_IM.txt en_IN.txt en_IO.txt en_JE.txt en_JM.txt\
#en_KE.txt en_KI.txt en_KN.txt en_KY.txt en_LC.txt\
#en_LR.txt en_LS.txt en_MG.txt en_MH.txt en_MO.txt\
#en_MP.txt en_MS.txt en_MT.txt en_MU.txt en_MW.txt\
#en_NA.txt en_NF.txt en_NG.txt en_NR.txt en_NU.txt\
#en_NZ.txt en_PG.txt en_PH.txt en_PK.txt en_PN.txt\
#en_PR.txt en_PW.txt en_RW.txt en_SB.txt en_SC.txt\
#en_SD.txt en_SG.txt en_SH.txt en_SL.txt en_SS.txt\
#en_SX.txt en_SZ.txt en_TC.txt en_TK.txt en_TO.txt\
#en_TT.txt en_TV.txt en_TZ.txt en_UG.txt en_UM.txt\
#es_CL.txt es_CO.txt es_CR.txt es_CU.txt es_DO.txt\
#es_EA.txt es_EC.txt es_ES.txt es_GQ.txt es_GT.txt\
#es_HN.txt es_IC.txt es_MX.txt es_NI.txt es_PA.txt\
#es_PE.txt es_PH.txt es_PR.txt es_PY.txt es_SV.txt\
#es_US.txt es_UY.txt es_VE.txt fr_BE.txt fr_BF.txt fr_BI.txt fr_BJ.txt\
#fr_BL.txt fr_CA.txt fr_CD.txt fr_CF.txt fr_CG.txt\
#fr_CH.txt fr_CI.txt fr_CM.txt fr_DJ.txt fr_DZ.txt\
#fr_FR.txt fr_GA.txt fr_GF.txt fr_GN.txt fr_GP.txt\
#fr_GQ.txt fr_HT.txt fr_KM.txt fr_LU.txt fr_MA.txt\
#fr_MC.txt fr_MF.txt fr_MG.txt fr_ML.txt fr_MQ.txt\
#fr_MR.txt fr_MU.txt fr_NC.txt fr_NE.txt fr_PF.txt\
#fr_PM.txt fr_RE.txt fr_RW.txt fr_SC.txt fr_SN.txt\
#fr_SY.txt fr_TD.txt fr_TG.txt fr_TN.txt fr_VU.txt\
#fr_WF.txt fr_YT.txt ar_DZ.txt ar_EG.txt ar_EH.txt ar_ER.txt ar_IL.txt\
#ar_IQ.txt ar_JO.txt ar_KM.txt ar_KW.txt ar_LB.txt\
#ar_LY.txt ar_MA.txt ar_MR.txt ar_OM.txt ar_PS.txt\
#ar_QA.txt ar_SA.txt ar_SD.txt ar_SO.txt ar_SS.txt\
#ar_SY.txt ar_TD.txt ar_TN.txt ar_YE.txt agq_CM.txt
#ak_GH.txt am_ET.txt ar_001.txt ar_AE.txt ar_BH.txt ar_DJ.txt
#as_IN.txt asa_TZ.txt bas_CM.txt bem_ZM.txt bez_TZ.txt
#bg_BG.txt bm_ML.txt bn_BD.txt bn_IN.txt bo_CN.txt bo_IN.txt
#br_FR.txt ca_AD.txt ca_ES.txt ca_FR.txt ca_IT.txt
#cgg_UG.txt cs_CZ.txt da_DK.txt da_GL.txt dav_KE.txt
#de_AT.txt de_BE.txt de_CH.txt de_DE.txt de_LI.txt
#de_LU.txt dje_NE.txt dua_CM.txt dyo_SN.txt ebu_KE.txt
#el_CY.txt el_GR.txt en_001.txt en_150.txt en_AG.txt
#en_VC.txt en_VG.txt en_VI.txt en_VU.txt en_WS.txt
#en_ZA.txt en_ZM.txt en_ZW.txt ga_IE.txt
#es_419.txt es_AR.txt es_BO.txt et_EE.txt fa_AF.txt
#fa_IR.txt ewo_CM.txt ff_SN.txt fi_FI.txt  fo_FO.txt
#guz_KE.txt gv_IM.txt ha_Latn.txt ha_Latn_GH.txt
#ha_Latn_NE.txt ha_Latn_NG.txt ko_KP.txt km_KH.txt
#haw_US.txt hi_IN.txt hr_BA.txt hr_HR.txt
#hu_HU.txt hy_AM.txt ig_NG.txt ii_CN.txt kw_GB.txt
#it_CH.txt it_IT.txt it_SM.txt ja_JP.txt jmc_TZ.txt
#kde_TZ.txt kam_KE.txt khq_ML.txt ki_KE.txt
#kkj_CM.txt kk_Cyrl.txt kk_Cyrl_KZ.txt kln_KE.txt
#kok_IN.txt ko_KR.txt ksf_CM.txt ksb_TZ.txt
#lag_TZ.txt lg_UG.txt ln_AO.txt lkt_US.txt ln_CD.txt
#ln_CF.txt ln_CG.txt lt_LT.txt lu_CD.txt
#luo_KE.txt luy_KE.txt lv_LV.txt mas_KE.txt mas_TZ.txt
#mer_KE.txt mfe_MU.txt mg_MG.txt mgh_MZ.txt
#ms_Latn.txt ms_Latn_BN.txt ms_Latn_MY.txt ms_Latn_SG.txt
#mua_CM.txt mt_MT.txt naq_NA.txt nb_NO.txt
#nus_SD.txt nmg_CM.txt nd_ZW.txt nb_SJ.txt
#om_ET.txt om_KE.txt nyn_UG.txt seh_MZ.txt sbp_TZ.txt
#ps_AF.txt or_IN.txt pl_PL.txt pt_BR.txt pt_CV.txt pt_GW.txt
#pt_MO.txt pt_MZ.txt pt_PT.txt pt_ST.txt pt_TL.txt pt_AO.txt
#ru_BY.txt ru_KG.txt rn_BI.txt ro_MD.txt ro_RO.txt
#ru_KZ.txt ru_MD.txt ru_RU.txt ru_UA.txt
#rwk_TZ.txt rw_RW.txt saq_KE.txt sg_CF.txt ses_ML.txt
#shi_Latn.txt shi_Latn_MA.txt shi_Tfng.txt shi_Tfng_MA.txt
#sn_ZW.txt sl_SI.txt so_DJ.txt so_ET.txt so_KE.txt so_SO.txt
#sr_Cyrl.txt sr_Cyrl_BA.txt sr_Cyrl_ME.txt sr_Cyrl_RS.txt
#sr_Cyrl_XK.txt sr_Latn.txt sr_Latn_BA.txt sr_Latn_ME.txt sr_Latn_RS.txt
#sr_Latn_XK.txt sv.txt sv_AX.txt sv_FI.txt sv_SE.txt
#teo_UG.txt teo_KE.txt swc_CD.txt ti_ET.txt ti_ER.txt
#twq_NE.txt tzm_Latn.txt tzm_Latn_MA.txt uk_UA.txt ur_IN.txt
#ur_PK.txt vai_Latn.txt vai_Latn_LR.txt vai_Vaii.txt
#vun_TZ.txt vi_VN.txt vai_Vaii_LR.txt yo_BJ.txt yo_NG.txt
#yav_CM.txt xog_UG.txt zgh_MA.txt

--------rbnf
#lca Slim
#be.txt el.txt he.txt mt.txt ar.txt
#ky.txt ga.txt
#pl.txt sr.txt sr_Latin.txt
#mk.txt sk.txt is.txt ta.txt fo.txt
#cy.txt bs.txt ka.txt az.txt af.txt
#sq.txt kl.txt fa_AF.txt nn.txt
#es_BO.txt es_CL.txt es_CO.txt es_CR.txt es_CU.txt\
#es_DO.txt es_EC.txt es_GT.txt es_HN.txt es_MX.txt\
#es_NI.txt es_PA.txt es_PE.txt es_PR.txt es_PY.txt\
#es_SV.txt es_US.txt es_UY.txt es_VE.txt es_AR.txt
#fr_BE.txt fr_CH.txt pt_AO.txt pt_TL.txt
#pt_GW.txt pt_MO.txt pt_MZ.txt pt_PT.txt pt_ST.txt

--------region
#LCA slim
REGION_SYNTHETIC_ALIAS
# az_AZ.txt az_Latn_AZ.txt bs_BA.txt bs_Latn_BA.txt\
# en_NH.txt en_RH.txt fil_PH.txt ha_GH.txt ha_Latn_GH.txt\
# ha_Latn_NE.txt ha_Latn_NG.txt ha_NE.txt ha_NG.txt he_IL.txt\
# id_ID.txt in_ID.txt iw_IL.txt\
# ja_JP.txt ja_JP_TRADITIONAL.txt kk_Cyrl_KZ.txt kk_KZ.txt ks_Arab_IN.txt\
# ks_IN.txt ky_Cyrl_KG.txt ky_KG.txt mn_Cyrl_MN.txt mn_MN.txt\
# ms_BN.txt ms_Latn_BN.txt ms_Latn_MY.txt ms_Latn_SG.txt\
# ms_MY.txt ms_SG.txt nb_NO.txt nn_NO.txt\
# no_NO.txt no_NO_NY.txt pa_Guru_IN.txt pa_IN.txt pa_PK.txt\
# ro_MD.txt sh_BA.txt sh_CS.txt sh_YU.txt\
# shi_MA.txt sr_BA.txt sr_CS.txt sr_Cyrl_BA.txt sr_Cyrl_CS.txt\
# sr_Cyrl_RS.txt sr_Cyrl_XK.txt sr_Cyrl_YU.txt sr_Latn_CS.txt sr_Latn_YU.txt\
# sr_ME.txt sr_RS.txt sr_XK.txt sr_YU.txt th_TH.txt\
# th_TH_TRADITIONAL.txt tl_PH.txt tzm_Latn_MA.txt tzm_MA.txt\
# ug_Arab_CN.txt ug_CN.txt uz_AF.txt uz_UZ.txt vai_LR.txt\
# vai_Vaii_LR.txt zh_CN.txt zh_HK.txt zh_Hans_CN.txt zh_MO.txt\
# zh_SG.txt zh_TW.txt
REGION_SOURCE
#dz.txt ta.txt kn.txt ka.txt te.txt ml.txt si.txt
#gu.txt or.txt mr.txt lo.txt brx.txt
#pa.txt pa_Arab.txt pa_Arab_PK.txt pa_Guru.txt
#ug.txt ug_Arab.txt hy.txt el.txt
#uk.txt mk.txt bs.txt bs_Cyrl.txt bs_Latn.txt ky.txt ky_Cyrl.txt
#ks.txt ks_Arab.txt
#uz.txt uz_Arab.txt uz_Arab_AF.txt uz_Cyrl.txt uz_Latn.txt uz_Latn_UZ.txt
#be.txt mn.txt mn_Cyrl.txt zgh.txt yo.txt
#shi.txt shi_Latn.txt shi_Tfng.txt shi_Tfng_MA.txt
#yo_BJ.txt zu.txt to.txt
#is.txt az.txt az_Cyrl.txt az_Cyrl_AZ.txt az_Latn.txt
#sq.txt eu.txt
#cy.txt hr.txt de.txt de_CH.txt tr.txt
#af.txt gl.txt ga.txt fil.txt gsw.txt br.txt
#rm.txt kea.txt nn.txt kln.txt mt.txt chr.txt agq.txt lag.txt
#vai.txt vai_Latn.txt vai_Latn_LR.txt vai_Vaii.txt
#bas.txt sg.txt ewo.txt bez.txt ln.txt rn.txt yav.txt mua.txt lg.txt
#en_AG.txt en_AI.txt en_AU.txt en_BB.txt en_BE.txt\
#en_BM.txt en_BS.txt en_BW.txt en_BZ.txt en_CC.txt\
#en_CK.txt en_CM.txt en_CX.txt en_DG.txt en_DM.txt\
#en_ER.txt en_FJ.txt en_FK.txt en_FM.txt en_GB.txt\
#en_GD.txt en_GG.txt en_GH.txt en_GI.txt en_GM.txt\
#en_GY.txt en_HK.txt en_IE.txt en_IM.txt en_IN.txt\
#en_IO.txt en_JE.txt en_JM.txt en_KE.txt en_KI.txt\
#en_KN.txt en_KY.txt en_LC.txt en_LR.txt en_LS.txt\
#en_MG.txt en_MO.txt en_MS.txt en_MT.txt en_MU.txt\
#en_MW.txt en_NA.txt en_NF.txt en_NG.txt en_NR.txt\
#en_NU.txt en_NZ.txt en_PG.txt en_PH.txt en_PK.txt\
#en_PN.txt en_PW.txt en_RW.txt en_SB.txt en_SC.txt\
#en_SD.txt en_SG.txt en_SH.txt en_SL.txt en_SS.txt\
#en_SX.txt en_SZ.txt en_TC.txt en_TK.txt en_TO.txt\
#en_TT.txt en_TV.txt en_TZ.txt en_UG.txt en_VC.txt\
#en_VG.txt en_VU.txt en_WS.txt en_ZA.txt en_ZM.txt\
#es_CL.txt es_CO.txt es_CR.txt es_CU.txt es_DO.txt\
#es_EC.txt es_GT.txt es_HN.txt es_MX.txt es_NI.txt\
#es_PA.txt es_PE.txt es_PR.txt es_PY.txt es_SV.txt\
#pt_AO.txt pt_CV.txt pt_GW.txt pt_MO.txt pt_MZ.txt\
#pt_PT.txt pt_ST.txt pt_TL.txt bn_IN.txt bo_IN.txt
#en_ZW.txt es_AR.txt es_BO.txt es_US.txt es_UY.txt
#es_VE.txt fa_AF.txt fr_CA.txt ha_Latn.txt kk_Cyrl.txt
#ms_Latn.txt nl_BE.txt sr_Cyrl.txt sr_Latn.txt
#sr_Latn_BA.txt sr_Latn_ME.txt tzm_Latn.txt
#sr_Latn_RS.txt sv.txt sv_FI.txt 

--------zone
#LCA Slim
ZONE_SYNTHETIC_ALIAS
# az_AZ.txt az_Latn_AZ.txt bs_BA.txt bs_Latn_BA.txt\
# en_NH.txt en_RH.txt fil_PH.txt ha_GH.txt ha_Latn_GH.txt\
# ha_Latn_NE.txt ha_Latn_NG.txt ha_NE.txt ha_NG.txt he_IL.txt\
# id_ID.txt in_ID.txt iw_IL.txt\
# ja_JP.txt ja_JP_TRADITIONAL.txt kk_Cyrl_KZ.txt kk_KZ.txt ks_Arab_IN.txt\
# ks_IN.txt ky_Cyrl_KG.txt ky_KG.txt mn_Cyrl_MN.txt mn_MN.txt\
# ms_BN.txt ms_Latn_BN.txt ms_Latn_MY.txt ms_Latn_SG.txt\
# ms_MY.txt ms_SG.txt nb_NO.txt nn_NO.txt\
# no_NO.txt no_NO_NY.txt pa_Guru_IN.txt pa_IN.txt pa_PK.txt\
# ro_MD.txt sh_BA.txt sh_CS.txt sh_YU.txt\
# shi_MA.txt sr_BA.txt sr_CS.txt sr_Cyrl_BA.txt sr_Cyrl_CS.txt\
# sr_Cyrl_RS.txt sr_Cyrl_XK.txt sr_Cyrl_YU.txt sr_Latn_CS.txt sr_Latn_YU.txt\
# sr_ME.txt sr_RS.txt sr_XK.txt sr_YU.txt th_TH.txt\
# th_TH_TRADITIONAL.txt tl_PH.txt tzm_Latn_MA.txt tzm_MA.txt\
# ug_Arab_CN.txt ug_CN.txt uz_AF.txt uz_UZ.txt vai_LR.txt\
# vai_Vaii_LR.txt zh_CN.txt zh_HK.txt zh_Hans_CN.txt zh_MO.txt\
# zh_SG.txt zh_TW.txt
CURR_SOURCE
#ml.txt ta.txt ta_MY.txt ta_SG.txt
#te.txt ka.txt si.txt mr.txt
#kn.txt gu.txt ne.txt ne_IN.txt lo.txt brx.txt
#pa.txt pa_Arab.txt pa_Arab_PK.txt pa_Guru.txt mk.txt
#bs_Cyrl.txt bs_Latn.txt el.txt mn.txt mn_Cyrl.txt
#ks.txt ks_Arab.txt
#sq.txt az.txt az_Cyrl.txt az_Cyrl_AZ.txt az_Latn.txt bs.txt
#gl.txt dz.txt
#ug.txt ug_Arab.txt is.txt id.txt cy.txt eu.txt
#af.txt da.txt zu.txt fil.txt rm.txt gsw.txt
#br.txt uz.txt uz_Arab.txt uz_Arab_AF.txt uz_Cyrl.txt uz_Latn.txt uz_Latn_UZ.txt
#en_AG.txt en_AI.txt en_AU.txt en_BB.txt en_BE.txt\
#en_BM.txt en_BS.txt en_BW.txt en_BZ.txt en_CA.txt\
#en_CC.txt en_CK.txt en_CM.txt en_CX.txt en_DG.txt\
#en_DM.txt en_ER.txt en_FJ.txt en_FK.txt en_FM.txt\
#en_GB.txt en_GD.txt en_GG.txt en_GH.txt en_GI.txt\
#en_GM.txt en_GU.txt en_GY.txt en_HK.txt en_IE.txt\
#en_IM.txt en_IN.txt en_IO.txt en_JE.txt en_JM.txt\
#en_KE.txt en_KI.txt en_KN.txt en_KY.txt en_LC.txt\
#en_LR.txt en_LS.txt en_MG.txt en_MH.txt en_MO.txt\
#en_MP.txt en_MS.txt en_MT.txt en_MU.txt en_MW.txt\
#en_NA.txt en_NF.txt en_NG.txt en_NR.txt en_NU.txt\
#en_NZ.txt en_PG.txt en_PH.txt en_PK.txt en_PN.txt\
#en_PW.txt en_RW.txt en_SB.txt en_SC.txt en_SD.txt\
#en_SG.txt en_SH.txt en_SL.txt en_SS.txt en_SX.txt\
#en_SZ.txt en_TC.txt en_TK.txt en_TO.txt en_TT.txt\
#en_TV.txt en_TZ.txt en_UG.txt en_VC.txt en_VG.txt\
#en_VU.txt en_WS.txt en_ZA.txt en_ZM.txt en_ZW.txt\
#es_CL.txt es_CO.txt es_CR.txt es_CU.txt es_DO.txt\
#es_EC.txt es_GT.txt es_HN.txt es_MX.txt es_NI.txt\
#es_PA.txt es_PE.txt es_PR.txt es_PY.txt es_SV.txt\
#pt_AO.txt pt_CV.txt pt_GW.txt pt_MO.txt pt_MZ.txt\
#pt_PT.txt pt_ST.txt pt_TL.txt ar_EG.txt bn_IN.txt
#es_419.txt es_AR.txt es_BO.txt es_US.txt es_UY.txt
#es_VE.txt fr_CA.txt ha_Latn.txt kk_Cyrl.txt ko_KP.txt
#ky_Cyrl.txt ms_Latn.txt shi_Latn.txt shi_Tfng.txt
#shi_Tfng_MA.txt sr_Cyrl.txt sr_Latn.txt sr_Latn_BA.txt
#sr_Latn_ME.txt sr_Latn_RS.txt sv_FI.txt tzm_Latn.txt
#ur_IN.txt vai_Latn.txt vai_Latn_LR.txt vai_Vaii.txt
