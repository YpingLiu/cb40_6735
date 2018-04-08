/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Header file of Sensor driver
 *
 *
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 09 10 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .10y dual sensor
 *
 * 09 02 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .roll back dual sensor
 *
 * Mar 4 2010 mtk70508
 * [DUMA00154792] Sensor driver
 * 
 *
 * Feb 24 2010 mtk01118
 * [DUMA00025869] [Camera][YUV I/F & Query feature] check in camera code
 * 
 *
 * Aug 5 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Apr 7 2009 mtk02204
 * [DUMA00004012] [Camera] Restructure and rename camera related custom folders and folder name of came
 * 
 *
 * Mar 26 2009 mtk02204
 * [DUMA00003515] [PC_Lint] Remove PC_Lint check warnings of camera related drivers.
 * 
 *
 * Mar 2 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 * 
 *
 * Feb 24 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 * 
 *
 * Dec 27 2008 MTK01813
 * DUMA_MBJ CheckIn Files
 * created by clearfsimport
 *
 * Dec 10 2008 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 * 
 *
 * Oct 27 2008 mtk01051
 * [DUMA00000851] Camera related drivers check in
 * Modify Copyright Header
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
/* SENSOR FULL SIZE */
#ifndef __SENSOR_H
#define __SENSOR_H


typedef enum {
    SENSOR_MODE_INIT = 0,
    SENSOR_MODE_PREVIEW,
    SENSOR_MODE_CAPTURE
} OV9750MIPI_SENSOR_MODE;

typedef enum _OV9750MIPI_OP_TYPE_ {
        OV9750MIPI_MODE_NONE,
        OV9750MIPI_MODE_PREVIEW,
        OV9750MIPI_MODE_CAPTURE,
        OV9750MIPI_MODE_QCIF_VIDEO,
        OV9750MIPI_MODE_CIF_VIDEO,
        OV9750MIPI_MODE_QVGA_VIDEO
    } OV9750MIPI_OP_TYPE;

extern OV9750MIPI_OP_TYPE OV9750MIPI_g_iOV9750MIPI_Mode;

#define OV9750MIPI_ID_REG                          (0x300B)
#define OV9750MIPI_INFO_REG                        (0x300C)
 
/* sensor size */
#define OV9750MIPI_IMAGE_SENSOR_SVGA_WIDTH          (200)
#define OV9750MIPI_IMAGE_SENSOR_SVGA_HEIGHT         (200)
#define OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH         (200) 
#define OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT        (200)
#define OV9750MIPI_IMAGE_SENSOR_VIDEO_WITDH         (200) 
#define OV9750MIPI_IMAGE_SENSOR_VIDEO_HEIGHT        (200)

#define OV9750MIPI_IMAGE_SENSOR_960P_WIDTH          (200)
#define OV9750MIPI_IMAGE_SENSOR_960P_HEIGHT         (200)


/* Sesnor Pixel/Line Numbers in One Period */	
#define OV9750MIPI_PV_PERIOD_PIXEL_NUMS    		(1896)  	/* Default preview line length HTS*/
#define OV9750MIPI_PV_PERIOD_LINE_NUMS     		(984)   	/* Default preview frame length  VTS*/
#define OV9750MIPI_FULL_PERIOD_PIXEL_NUMS  		(2844)  	/* Default full size line length */
#define OV9750MIPI_FULL_PERIOD_LINE_NUMS   		(1968)  	/* Default full size frame length */

/* Sensor Exposure Line Limitation */
#define OV9750MIPI_PV_EXPOSURE_LIMITATION      	(984-4)
#define OV9750MIPI_FULL_EXPOSURE_LIMITATION    	(1968-4)

/* Config the ISP grab start x & start y, Config the ISP grab width & height */
#define OV9750MIPI_PV_GRAB_START_X 				   (0)
#define OV9750MIPI_PV_GRAB_START_Y  			(10)
#define OV9750MIPI_FULL_GRAB_START_X   			(10)
#define OV9750MIPI_FULL_GRAB_START_Y	  		(10)

/*50Hz,60Hz*/
#define OV9750MIPI_NUM_50HZ                        (50 * 2)
#define OV9750MIPI_NUM_60HZ                        (60 * 2)

/* FRAME RATE UNIT */
#define OV9750MIPI_FRAME_RATE_UNIT                 (10)

/* MAX CAMERA FRAME RATE */
#define OV9750MIPI_MAX_CAMERA_FPS                  (OV9750MIPI_FRAME_RATE_UNIT * 30)

#define OV9750_PREVIEW_MODE             0
#define OV9750_VIDEO_MODE               1
#define OV9750_PREVIEW_FULLSIZE_MODE    2


/* SENSOR READ/WRITE ID */
#define OV9750MIPI_WRITE_ID						    0x20
#define OV9750MIPI_READ_ID							0x21

/* XC6130 READ/WRITE ID */
#define XC6130_WRITE_ID						    0x36
#define XC6130_READ_ID							0x37

#define XC6130_ISP_ID							0x5843

UINT32 OV9750MIPIopen(void);
UINT32 OV9750MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 OV9750MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 OV9750MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 OV9750MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 OV9750MIPIClose(void);
UINT32 OV9750MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT pfFunc);
#endif /* __SENSOR_H */
