//
#if 1


/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.c
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/io.h>
//#include <asm/system.h>  
#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"
#include "ov9750mipiyuv_Sensor.h"
#include "ov9750mipiyuv_Camera_Sensor_para.h"
#include "ov9750mipiyuv_CameraCustomized.h" 
#define OV9750MIPIYUV_DEBUG
#ifdef OV9750MIPIYUV_DEBUG
//#define OV9750MIPISENSORDB printk
#define OV9750MIPISENSORDB(a,arg... ) printk("[XC6130========================]" a,##arg)
#define TV_SWITCH_PRINT(a,arg... ) printk("[TV_SWITCH/XC6130]" a,##arg)


#else
#define OV9750MIPISENSORDB(x,...)
#define TV_SWITCH_PRINT(x,...)

#endif



// GM7150, by zhangxueping
//extern UINT32 GM7150_On(bool force);
//extern struct mutex TV_switch_lock;

#define CAMERA_INIT_NONE              0
#define CAMERA_INIT_ONCE              1
#define CAMERA_INIT_ALWAYS           2


#define IME_OV9750_MIRROR


// VGA size, by zhangxueping
#undef OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH
#undef OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT
#define OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH        960    //640
#define OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT       960     //480

#define OV9750MIPI_SENSOR_ID 0x9750

#define mutex_lock(...)
#define mutex_unlock(...)





//bool is_KERN_EMERG = false;

bool isCameraOpen = false;

static DEFINE_SPINLOCK(OV9750mipi_drv_lock);
static MSDK_SCENARIO_ID_ENUM CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);

//#define OV9750MIPI_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,OV9750MIPI_WRITE_ID)
//#define OV9750MIPI_write_cmos_sensor(addr, para) do {iWriteReg((u16) addr , (u32) para ,1,OV9750MIPI_WRITE_ID); OV9750MIPISENSORDB("OV9750MIPI_write, addr=%x\n", addr);	}while(0)


void OV9750MIPI_write_cmos_sensor(addr, para)
{
    iWriteReg((u16) addr , (u32) para ,1,OV9750MIPI_WRITE_ID);
}
static void XC6130InitialSetting(void);
extern	int Af_VCOMA2_Power_On(void);

static void XC6130_write_cmos_sensor(addr, para)
{
	//OV9750MIPISENSORDB("[XC6130RegWrite,0411.1.3] addr:%d  value:%x\n",addr,para);  		
	iWriteReg((u16) addr , (u32) para ,1,XC6130_WRITE_ID);
  
}

static kal_uint16 XC6130_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
	
    iReadReg((u16) addr ,(u8*)&get_byte,XC6130_WRITE_ID);

	//OV9750MIPISENSORDB("XC6130_read_cmos_sensor addr=%x, get_byte=%x\n", addr,get_byte);		 // by zhangxueping
	
    return get_byte;
}

#define mDELAY(ms)  mdelay(ms)

typedef enum
{
    PRV_W=1280,
    PRV_H=960
}PREVIEW_VIEW_SIZE;
kal_uint16 OV9750MIPIYUV_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;

	

    iReadReg((u16) addr ,(u8*)&get_byte,OV9750MIPI_WRITE_ID);
	
	//OV9750MIPISENSORDB("OV9750MIPIYUV_read_cmos_sensor addr=%x, get_byte=%x\n", addr, get_byte);	 // by zhangxueping
	
    return get_byte;
}

#if 0
static struct
{
    //kal_uint8   Banding;
    kal_bool      NightMode;
    kal_bool      VideoMode;
    kal_uint16  Fps;
    kal_uint16  ShutterStep;
    kal_uint8   IsPVmode;
    kal_uint32  PreviewDummyPixels;
    kal_uint32  PreviewDummyLines;
    kal_uint32  CaptureDummyPixels;
    kal_uint32  CaptureDummyLines;
    kal_uint32  PreviewPclk;
    kal_uint32  CapturePclk;
    kal_uint32  ZsdturePclk;
    kal_uint32  PreviewShutter;
    kal_uint32  PreviewExtraShutter;
    kal_uint32  SensorGain;
    kal_bool        manualAEStart;
    kal_bool        userAskAeLock;
    kal_bool     userAskAwbLock;
    kal_uint32      currentExposureTime;
    kal_uint32      currentShutter;
    kal_uint32      currentextshutter;
    kal_uint32      currentAxDGain;
    kal_uint32      sceneMode;
    unsigned char isoSpeed;
    unsigned char zsd_flag;
    OV9750MIPI_SENSOR_MODE SensorMode;
} OV9750MIPISensor;

#endif

/* Global Valuable */
MSDK_SENSOR_CONFIG_STRUCT OV9750MIPISensorConfigData;

#define OV9750_TEST_PATTERN_CHECKSUM (0x39ff61ce)  
//#define OV9750_TEST_PATTERN_CHECKSUM (0x3b969447)  

//CRC_accum 0x3b969447


// by zhangxueping
//#define OV9750MIPI_SENSOR_ID   OV9750_SENSOR_ID 		   

static int saturation_level=ISP_SAT_MIDDLE;          // by zhangxueping


void OV9750MIPIGetExifInfo(UINT32 exifAddr)
{
#if 0 //brat
     OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIGetExifInfo function\n");
    SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)(uintptr_t)exifAddr;
    pExifInfo->FNumber = 20;
    //pExifInfo->AEISOSpeed = OV9750MIPISensor.isoSpeed;
    pExifInfo->FlashLightTimeus = 0;
    //pExifInfo->RealISOValue = OV9750MIPISensor.isoSpeed;
    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIGetExifInfo function\n");
#endif
}

// add by zhangxueping
UINT32 OV9750SetTestPatternMode(kal_bool bEnable)
{
    OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750SetTestPatternMode 0331\n");
#if 1
    if(bEnable)
    {
		XC6130_write_cmos_sensor(0xfffd ,0x80);
		XC6130_write_cmos_sensor(0xfffe ,0x80);
		XC6130_write_cmos_sensor(0x004d ,0x01);
		OV9750MIPI_write_cmos_sensor(0x5E00, 0xa0);
		XC6130_write_cmos_sensor(0x004d ,0x00);
    }
    else
    {
		XC6130_write_cmos_sensor(0xfffd ,0x80);
		XC6130_write_cmos_sensor(0xfffe ,0x80);
		XC6130_write_cmos_sensor(0x004d ,0x01);
		OV9750MIPI_write_cmos_sensor(0x5E00, 0x00);
		XC6130_write_cmos_sensor(0x004d ,0x00);

    } 
#else

	if(bEnable)
	{
		XC6130_write_cmos_sensor(0xfffd ,0x80);
		XC6130_write_cmos_sensor(0xfffE ,0x06); 
		XC6130_write_cmos_sensor(0x3022 ,0x82);
		XC6130_write_cmos_sensor(0xfffE ,0x21); 
		XC6130_write_cmos_sensor(0x0000 ,0x01);
		XC6130_write_cmos_sensor(0x0001 ,0x00);
		XC6130_write_cmos_sensor(0x0003 ,0x20);
		XC6130_write_cmos_sensor(0xfffe ,0x80);
		XC6130_write_cmos_sensor(0x004d ,0x01);
		OV9750MIPI_write_cmos_sensor(0x5E00, 0xa0);
		XC6130_write_cmos_sensor(0x004d ,0x00);
	}
	else
	{
		XC6130_write_cmos_sensor(0xfffd ,0x80);
		XC6130_write_cmos_sensor(0xfffE ,0x06); 
		XC6130_write_cmos_sensor(0x3022 ,0x80);
		XC6130_write_cmos_sensor(0xfffE ,0x21); 
		XC6130_write_cmos_sensor(0x0000 ,0x47);
		XC6130_write_cmos_sensor(0x0001 ,0x82);
		XC6130_write_cmos_sensor(0x0003 ,0xb1);
		XC6130_write_cmos_sensor(0xfffe ,0x80);
		XC6130_write_cmos_sensor(0x004d ,0x01);
		OV9750MIPI_write_cmos_sensor(0x5E00, 0x00);
		XC6130_write_cmos_sensor(0x004d ,0x00);

	}


#endif
    return ERROR_NONE;
}


#define OV9750_REG_READBACK

#if defined(OV9750_REG_READBACK)

#define READ_PERIOD_S       2
    // 2 second

static struct hrtimer ov9750_readback_timer;
static   bool isOV9750HrtimerStart= KAL_FALSE;

static struct work_struct readback_work;
static struct workqueue_struct * readback_workqueue = NULL;
enum hrtimer_restart ov9750_read_hrtimer_func(struct hrtimer *timer);

static void reg_readback(void);

static void ov9750_read_detect_hrtimer_init(void)
{
    ktime_t ktime;


    OV9750MIPISENSORDB("ov9750_read_detect_hrtimer_init\n" );

	if(!readback_workqueue)
	{
		readback_workqueue = create_singlethread_workqueue("ov9750_read_det");
		if(!readback_workqueue) {
			OV9750MIPISENSORDB("Unable to create workqueue\n");
			return -ENODATA;
		}
		INIT_WORK(&readback_work, reg_readback);
	}	

    ktime = ktime_set(READ_PERIOD_S, 0);	// 3s, 10* 1000 ms
    hrtimer_init(&ov9750_readback_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    ov9750_readback_timer.function = ov9750_read_hrtimer_func;    
    hrtimer_start(&ov9750_readback_timer, ktime, HRTIMER_MODE_REL);

	
}

enum hrtimer_restart ov9750_read_hrtimer_func(struct hrtimer *timer)
{
    ktime_t ktime;

    OV9750MIPISENSORDB("ov9750_read_hrtimer_func, by zxp \n" );
	
	queue_work(readback_workqueue, &readback_work);
	
	ktime = ktime_set(READ_PERIOD_S, 0);	
	hrtimer_start(&ov9750_readback_timer, ktime, HRTIMER_MODE_REL);
	
    return HRTIMER_NORESTART;		

}
#endif  //  OV9750_REG_READBACK

void reg_readback(void)
{
	int ret=0;
	int var1,var2,var3,var4;
	int gain = 0;
    static int night_mode = 0;
	printk(KERN_WARNING"[XC6130_REG]reg_read_back\n ");  

	
	
    XC6130_write_cmos_sensor(0xfffe ,0x14);
	  ret=XC6130_read_cmos_sensor(0x0002);
    printk(KERN_WARNING"[XC6130_REG]AE_ON,0x0002=%x\n ",ret);      //AE, on/off
    
	ret=XC6130_read_cmos_sensor(0x0026);
    printk(KERN_WARNING"[XC6130_REG]LINE,0x0026=%x\n ",ret);      // exposure line, max 0x3c
    
	 gain = XC6130_read_cmos_sensor(0x002b);
	printk(KERN_WARNING"[XC6130_REG]GAIN,0x002b=%x\n ",gain);     //gain 0x10~0x78(0x0037)
    
	ret=XC6130_read_cmos_sensor(0x003c);
    printk(KERN_WARNING"[XC6130_REG]LUM,0x003c=%x\n ",ret);     // lum value
    
	ret=XC6130_read_cmos_sensor(0x0022);
    printk(KERN_WARNING"[XC6130_REG]0x0022=%x\n ",ret);    // 0x00/0x01
   
    if((gain > 0x60) && (night_mode == 0))
   	{    
   		printk(KERN_WARNING"[XC6130_REG]------ inter night mode" );    // 0x00/0x01

   		XC6130_write_cmos_sensor(0xfffe,0x14); 
      XC6130_write_cmos_sensor(0x003d,0x80); 
      XC6130_write_cmos_sensor(0xfffe,0x21); 
      XC6130_write_cmos_sensor(0x031a,0x00); 
      XC6130_write_cmos_sensor(0x031b,0x30); 
      XC6130_write_cmos_sensor(0x031c,0x00); 
      XC6130_write_cmos_sensor(0x031d,0x10); 
      XC6130_write_cmos_sensor(0x031e,0x0f); 
      XC6130_write_cmos_sensor(0x031f,0x0f); 
      XC6130_write_cmos_sensor(0x0320,0x06); 
      XC6130_write_cmos_sensor(0x0321,0x08); 
      XC6130_write_cmos_sensor(0xfffe,0x21);
      XC6130_write_cmos_sensor(0x0580,0x09);
      XC6130_write_cmos_sensor(0x0581,0x50);
      XC6130_write_cmos_sensor(0x0582,0x80);

   		
   		 night_mode = 1;
   		}
   else if(gain < 0x40 && night_mode == 1)
   	{
   		printk(KERN_WARNING"[XC6130_REG]------ inter day mode" );    // 0x00/0x01

      XC6130_write_cmos_sensor(0xfffe,0x14); 
      XC6130_write_cmos_sensor(0x003d,0x50); 
      XC6130_write_cmos_sensor(0xfffe,0x21); 
      XC6130_write_cmos_sensor(0x031a,0x00); 
      XC6130_write_cmos_sensor(0x031b,0x30); 
      XC6130_write_cmos_sensor(0x031c,0x00); 
      XC6130_write_cmos_sensor(0x031d,0x10); 
      XC6130_write_cmos_sensor(0x031e,0x0f); 
      XC6130_write_cmos_sensor(0x031f,0x0f); 
      XC6130_write_cmos_sensor(0x0320,0x06); 
      XC6130_write_cmos_sensor(0x0321,0x08); 
      XC6130_write_cmos_sensor(0xfffe,0x21);
      XC6130_write_cmos_sensor(0x0580,0x09);
      XC6130_write_cmos_sensor(0x0581,0x50);
      XC6130_write_cmos_sensor(0x0582,0x80);

   		
   		night_mode = 0;
   		}
   
 	//mutex_unlock(&TV_switch_lock);
	//is_KERN_EMERG = false;
	
    return ERROR_NONE ;
    
}	

//#endif  //  OV9750_REG_READBACK


/*************************************************************************
* FUNCTION
*   OV9750MIPI_GetSensorID
*
* DESCRIPTION
*   This function get the sensor ID
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint32 OV9750MIPI_GetSensorID_dummy(kal_uint32 *sensorID)
{
	OV9750MIPISENSORDB("OV9750MIPI_GetSensorID: FAIL, ONLY FOR DEBUG\n");
	*sensorID = 0xFFFFFFFF;
	return ERROR_SENSOR_CONNECT_FAIL;

    //*sensorID=OV9750MIPI_SENSOR_ID;
	//OV9750MIPISENSORDB("[OV9750MIPI] OV9750MIPI_GetSensorID: OK, only for test");
   //return ERROR_NONE; 	
}

static kal_uint32 gSensorID = 0;

kal_uint32 OV9750MIPI_GetSensorID(kal_uint32 *sensorID)
{
    volatile signed char i;
    kal_uint32 sensor_id=0;
    //kal_uint8 temp_sccb_addr = 0;
    kal_uint32 isp_id[4]={0};
    kal_uint32 j;

    //OV9750MIPISENSORDB("OV9750MIPI_GetSensorID: ZHENGFENG_0320\n ");
    OV9750MIPISENSORDB("OV9750MIPI_GetSensorID-4: 150618\n ");
	
    //mDELAY(10);
    XC6130_write_cmos_sensor(0xfffd ,0x80);
    XC6130_write_cmos_sensor(0xfffe ,0x80);
    //XC6130_write_cmos_sensor(0x004D ,0x00);
    mDELAY(10);
    for (j=0;j<4;j++)
    {
        isp_id[j]=XC6130_read_cmos_sensor(j);
        OV9750MIPISENSORDB("HZTCT : XC6130_ID[%d]=0x%x \n",j,isp_id[j]);       // 0x01, 0x11, 0x43, 0x58
    }
   
	if ( (isp_id[0]==0x01) && (isp_id[1]==0x11) && (isp_id[2]==0x43) && (isp_id[3]==0x58) )
	{		
		 OV9750MIPISENSORDB("HZTCT : start get img sensor id \n");  
	    mDELAY(10);
		XC6130InitialSetting();


        XC6130_write_cmos_sensor(0xfffe,0x80);
        //XC6130_write_cmos_sensor(0x004d,0x1); //  I2C bypass
        XC6130_write_cmos_sensor(0x0050,0x06);  //  PWDN,GPIO1,GPIO0 en
        XC6130_write_cmos_sensor(0x0054,0x06);  //  PWDN,GPIO1,ouput 控制sensorPWDN&reset ; GPIO0 被MTK6572来控制XC6130 PWDN

        XC6130_write_cmos_sensor(0x0058,0x00); //   将GPIO1（sensor reset）先拉低
        mDELAY(15);
        XC6130_write_cmos_sensor(0x0058,0x06); //    将GPIO1（sensor reset）再拉高 PWDN低
        mDELAY(15);

    XC6130_write_cmos_sensor(0xfffd ,0x80);
    XC6130_write_cmos_sensor(0xfffe ,0x80);
    XC6130_write_cmos_sensor(0x004d ,0x01);   //by Nathan 20150719

		//OV9750MIPISENSORDB("OV9750MIPI_GetSensorID\n");    // by zhangxueping
	    //for(i=0;i<3;i++)
	    {
	    	// by zhangxueping
	        sensor_id = (OV9750MIPIYUV_read_cmos_sensor(0x300B) << 8) | OV9750MIPIYUV_read_cmos_sensor(0x300C);
			
	        OV9750MIPISENSORDB("HZTCT : OV9750MIPI_GetSensorID: ID=%x\n",sensor_id);
			
			//sensor_id = OV9750MIPI_SENSOR_ID;   // by zhangxueping
				
	        if(sensor_id != OV9750MIPI_SENSOR_ID)
	        {   
	            *sensorID =0xffffffff;
				OV9750MIPISENSORDB("OV9750MIPI_GetSensorID: FAIL\n");
	            //return ERROR_SENSOR_CONNECT_FAIL;
				
	        }
	        else
	        {
	            *sensorID=OV9750MIPI_SENSOR_ID;
	                //break;
	        }
	    }

	}

	XC6130_write_cmos_sensor(0xfffe,0x80);
    XC6130_write_cmos_sensor(0x004d,0x0); // 

	// add by zhangxueping
#if !defined(__IME_N84__)					
	if(OV9750MIPI_SENSOR_ID ==sensor_id)
	{
		gSensorID = OV9750MIPI_SENSOR_ID;
  		*sensorID=OV9750MIPI_SENSOR_ID;
		OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPI_GetSensorID zhuw1\n ");
*sensorID = TW9992_SENSOR_ID;
		return ERROR_NONE;    
	}
	else
	{
    	*sensorID=0;
		OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPI_GetSensorID zhuw2\n ");
		return ERROR_SENSOR_CONNECT_FAIL;
	}
#else
	 *sensorID=sensor_id;
#endif	 
	// GM7150, by zhangxueping
	//gOV9750_has_SensorID = 1;
	
	OV9750MIPISENSORDB("HZTCT444 : [OV9750MIPI]exit OV9750MIPI_GetSensorID \n ");
*sensorID = TW9992_SENSOR_ID;
   return ERROR_NONE;    

}   


/*************************************************************************
* FUNCTION
*    OV9750MIPIInitialSetting
*
* DESCRIPTION
*    This function initialize the registers of CMOS sensor.
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
 static    void XC6130InitialSetting(void)
{
    //mipi_2lane, MCLK=24M , mipi=768M
    OV9750MIPISENSORDB("XC6130_ISP_InitialSetting start \n");

	XC6130_write_cmos_sensor(0xfffd,0x80);
	XC6130_write_cmos_sensor(0xfffe,0x80);
	XC6130_write_cmos_sensor(0x001c,0xff);
	XC6130_write_cmos_sensor(0x001d,0xff);
	XC6130_write_cmos_sensor(0x001e,0xff);
	XC6130_write_cmos_sensor(0x001f,0xef);
	XC6130_write_cmos_sensor(0x0018,0x00);
	XC6130_write_cmos_sensor(0x0019,0x00);
	XC6130_write_cmos_sensor(0x001a,0x00);
	XC6130_write_cmos_sensor(0x001b,0x00);
	XC6130_write_cmos_sensor(0x00bc,0x11);
	XC6130_write_cmos_sensor(0x00bd,0x00);
	XC6130_write_cmos_sensor(0x00be,0x00);
	XC6130_write_cmos_sensor(0x00bf,0x00);
	XC6130_write_cmos_sensor(0x0020,0x01);   
	XC6130_write_cmos_sensor(0x0021,0x0e);
	XC6130_write_cmos_sensor(0x0022,0x00);
	XC6130_write_cmos_sensor(0x0023,0x00);
	XC6130_write_cmos_sensor(0x0024,0x08);
	XC6130_write_cmos_sensor(0x0025,0x0e);
	XC6130_write_cmos_sensor(0x0026,0x01);    
	XC6130_write_cmos_sensor(0x0027,0x0e);
  XC6130_write_cmos_sensor(0x0030,0x61);
  XC6130_write_cmos_sensor(0x0031,0x20);  
  XC6130_write_cmos_sensor(0x0032,0x70);	
	XC6130_write_cmos_sensor(0x0033,0x12);
	XC6130_write_cmos_sensor(0xFFFE,0x25);
	XC6130_write_cmos_sensor(0x0002,0x40);  //80
	XC6130_write_cmos_sensor(0xfffd,0x80);
	XC6130_write_cmos_sensor(0xfffe,0x26);
	XC6130_write_cmos_sensor(0x0002,0x00);
	XC6130_write_cmos_sensor(0xfffd,0x80); //AWB_init
	XC6130_write_cmos_sensor(0xfffe,0x14);
	XC6130_write_cmos_sensor(0x02b3,0x01);
	XC6130_write_cmos_sensor(0x02b6,0x06);
	XC6130_write_cmos_sensor(0x02b7,0x00);
	XC6130_write_cmos_sensor(0x02c2,0x04);
	XC6130_write_cmos_sensor(0x02c3,0x04);	                                      
	XC6130_write_cmos_sensor(0x030a,0x07);
	XC6130_write_cmos_sensor(0x030b,0x00);
	XC6130_write_cmos_sensor(0x030e,0x05);
	XC6130_write_cmos_sensor(0x030f,0xa0);

		XC6130_write_cmos_sensor(0xfffd,0x80);
	XC6130_write_cmos_sensor(0xfffe,0x80);
	XC6130_write_cmos_sensor(0x00bc,0x91);
	XC6130_write_cmos_sensor(0x001b,0x00);
	XC6130_write_cmos_sensor(0x0090,0x28);
	XC6130_write_cmos_sensor(0x0038,0x03);
	XC6130_write_cmos_sensor(0xfffe,0x26);
	XC6130_write_cmos_sensor(0x0000,0x20);
	XC6130_write_cmos_sensor(0x0002,0x00);
	XC6130_write_cmos_sensor(0x0009,0x04);
	XC6130_write_cmos_sensor(0x4000,0xF9);
	XC6130_write_cmos_sensor(0x6001,0x17);
	XC6130_write_cmos_sensor(0x6005,0x04);
	XC6130_write_cmos_sensor(0x6006,0x08);
	XC6130_write_cmos_sensor(0x6007,0x34);
	XC6130_write_cmos_sensor(0x6008,0x07);
	XC6130_write_cmos_sensor(0x6009,0x7C);
	XC6130_write_cmos_sensor(0x8000,0x3f);
	XC6130_write_cmos_sensor(0x8001,0x00);
	XC6130_write_cmos_sensor(0x8002,0x05);
	XC6130_write_cmos_sensor(0x8003,0xC0);
	XC6130_write_cmos_sensor(0x8004,0x03);
	XC6130_write_cmos_sensor(0x8005,0x03);
	XC6130_write_cmos_sensor(0x8006,0x05);
	XC6130_write_cmos_sensor(0x8007,0x99);
	XC6130_write_cmos_sensor(0x8010,0x05);
	XC6130_write_cmos_sensor(0x2019,0x03);
	XC6130_write_cmos_sensor(0x201a,0xC0);
	XC6130_write_cmos_sensor(0x201b,0x03);
	XC6130_write_cmos_sensor(0x201c,0xC0);
	XC6130_write_cmos_sensor(0x201d,0x00);
	XC6130_write_cmos_sensor(0x201e,0x00);
	XC6130_write_cmos_sensor(0x201f,0x00);
	XC6130_write_cmos_sensor(0x2020,0x00);
	XC6130_write_cmos_sensor(0x2015,0x81);
	XC6130_write_cmos_sensor(0x2017,0x1e);
	XC6130_write_cmos_sensor(0x2018,0x1e);
	XC6130_write_cmos_sensor(0x2023,0x03);
	XC6130_write_cmos_sensor(0x8012,0xC0);
	XC6130_write_cmos_sensor(0x8013,0x03);
	XC6130_write_cmos_sensor(0x8014,0xC0);
	XC6130_write_cmos_sensor(0x8015,0x03);
	XC6130_write_cmos_sensor(0x8016,0xA0);
	XC6130_write_cmos_sensor(0x8017,0x00);
	XC6130_write_cmos_sensor(0x8018,0x00);
	XC6130_write_cmos_sensor(0x8019,0x00);
	XC6130_write_cmos_sensor(0xfffe,0x21);
	XC6130_write_cmos_sensor(0x0001,0x01);
	XC6130_write_cmos_sensor(0x0004,0x10);
	XC6130_write_cmos_sensor(0x0708,0x00);
	XC6130_write_cmos_sensor(0x0072,0x00);
	XC6130_write_cmos_sensor(0x0074,0x00);
	XC6130_write_cmos_sensor(0x0006,0x03);
	XC6130_write_cmos_sensor(0x0007,0xC0);
	XC6130_write_cmos_sensor(0x0008,0x03);
	XC6130_write_cmos_sensor(0x0009,0xC0);
	XC6130_write_cmos_sensor(0x000a,0x03);
	XC6130_write_cmos_sensor(0x000b,0xC0);
	XC6130_write_cmos_sensor(0x000c,0x03);
	XC6130_write_cmos_sensor(0x000d,0xC0);
	XC6130_write_cmos_sensor(0x001e,0x03);
	XC6130_write_cmos_sensor(0x001f,0xC0);
	XC6130_write_cmos_sensor(0x0020,0x03);
	XC6130_write_cmos_sensor(0x0021,0xC0);
	XC6130_write_cmos_sensor(0x005e,0xBF);
	XC6130_write_cmos_sensor(0x005f,0x03);
	XC6130_write_cmos_sensor(0x0060,0xBF);
	XC6130_write_cmos_sensor(0x0061,0x03);
	XC6130_write_cmos_sensor(0x0064,0xC0);
	XC6130_write_cmos_sensor(0x0065,0x03);
	XC6130_write_cmos_sensor(0x0066,0xC0);
	XC6130_write_cmos_sensor(0x0067,0x03);
	XC6130_write_cmos_sensor(0x0076,0x03);
	XC6130_write_cmos_sensor(0x0077,0xC0);
	XC6130_write_cmos_sensor(0x0078,0x03);
	XC6130_write_cmos_sensor(0x0079,0xC0);
	XC6130_write_cmos_sensor(0x0700,0x00);
	XC6130_write_cmos_sensor(0x0701,0x00);
	XC6130_write_cmos_sensor(0x0702,0x00);
	XC6130_write_cmos_sensor(0x0703,0x00);
	XC6130_write_cmos_sensor(0x0704,0x02);
	XC6130_write_cmos_sensor(0x0705,0x80);
	XC6130_write_cmos_sensor(0x0706,0x01);
	XC6130_write_cmos_sensor(0x0707,0xE0);

	mDELAY(10);

	OV9750MIPISENSORDB("XC6130_ISP_InitialSetting end\n");
    
}


   
void OV9750MIPIInitialSetting(void)
{
    OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIInitialSetting function:\n ");

OV9750MIPI_write_cmos_sensor(0x0103,0x01);
OV9750MIPI_write_cmos_sensor(0x0100,0x00);
OV9750MIPI_write_cmos_sensor(0x0300,0x04);
OV9750MIPI_write_cmos_sensor(0x0302,0x64);
OV9750MIPI_write_cmos_sensor(0x0303,0x00);
OV9750MIPI_write_cmos_sensor(0x0304,0x03);
OV9750MIPI_write_cmos_sensor(0x0305,0x01);
OV9750MIPI_write_cmos_sensor(0x0306,0x01);
OV9750MIPI_write_cmos_sensor(0x030a,0x00);
OV9750MIPI_write_cmos_sensor(0x030b,0x00);
OV9750MIPI_write_cmos_sensor(0x030d,0x1e);
OV9750MIPI_write_cmos_sensor(0x030e,0x01);
OV9750MIPI_write_cmos_sensor(0x030f,0x04);
OV9750MIPI_write_cmos_sensor(0x0312,0x01);
OV9750MIPI_write_cmos_sensor(0x031e,0x04);
OV9750MIPI_write_cmos_sensor(0x3000,0x00);
OV9750MIPI_write_cmos_sensor(0x3001,0x00);
OV9750MIPI_write_cmos_sensor(0x3002,0x21);
OV9750MIPI_write_cmos_sensor(0x3005,0xf0);
OV9750MIPI_write_cmos_sensor(0x3011,0x00);
OV9750MIPI_write_cmos_sensor(0x3016,0x53);
OV9750MIPI_write_cmos_sensor(0x3018,0x32);
OV9750MIPI_write_cmos_sensor(0x301a,0xf0);
OV9750MIPI_write_cmos_sensor(0x301b,0xf0);
OV9750MIPI_write_cmos_sensor(0x301c,0xf0);
OV9750MIPI_write_cmos_sensor(0x301d,0xf0);
OV9750MIPI_write_cmos_sensor(0x301e,0xf0);
OV9750MIPI_write_cmos_sensor(0x3022,0x01);
OV9750MIPI_write_cmos_sensor(0x3031,0x0a);
OV9750MIPI_write_cmos_sensor(0x3032,0x80);
OV9750MIPI_write_cmos_sensor(0x303c,0xff);
OV9750MIPI_write_cmos_sensor(0x303e,0xff);
OV9750MIPI_write_cmos_sensor(0x3040,0xf0);
OV9750MIPI_write_cmos_sensor(0x3041,0x00);
OV9750MIPI_write_cmos_sensor(0x3042,0xf0);
OV9750MIPI_write_cmos_sensor(0x3104,0x01);
OV9750MIPI_write_cmos_sensor(0x3106,0x15);
OV9750MIPI_write_cmos_sensor(0x3107,0x01);//0x10
OV9750MIPI_write_cmos_sensor(0x3500,0x00);
OV9750MIPI_write_cmos_sensor(0x3501,0x3d);
OV9750MIPI_write_cmos_sensor(0x3502,0x00);
OV9750MIPI_write_cmos_sensor(0x3503,0x08);
OV9750MIPI_write_cmos_sensor(0x3504,0x03);
OV9750MIPI_write_cmos_sensor(0x3505,0x83);
OV9750MIPI_write_cmos_sensor(0x3508,0x02);
OV9750MIPI_write_cmos_sensor(0x3509,0x80);
OV9750MIPI_write_cmos_sensor(0x3600,0x65);
OV9750MIPI_write_cmos_sensor(0x3601,0x60);
OV9750MIPI_write_cmos_sensor(0x3602,0x22);
OV9750MIPI_write_cmos_sensor(0x3610,0xe8);
OV9750MIPI_write_cmos_sensor(0x3611,0x56);
OV9750MIPI_write_cmos_sensor(0x3612,0x48);
OV9750MIPI_write_cmos_sensor(0x3613,0x5a);
OV9750MIPI_write_cmos_sensor(0x3614,0x91);
OV9750MIPI_write_cmos_sensor(0x3615,0x79);
OV9750MIPI_write_cmos_sensor(0x3617,0x57);
OV9750MIPI_write_cmos_sensor(0x3621,0x90);
OV9750MIPI_write_cmos_sensor(0x3622,0x00);
OV9750MIPI_write_cmos_sensor(0x3623,0x00);
OV9750MIPI_write_cmos_sensor(0x3625,0x07);
OV9750MIPI_write_cmos_sensor(0x3633,0x10);
OV9750MIPI_write_cmos_sensor(0x3634,0x10);
OV9750MIPI_write_cmos_sensor(0x3635,0x14);
OV9750MIPI_write_cmos_sensor(0x3636,0x13);
OV9750MIPI_write_cmos_sensor(0x3650,0x00);
OV9750MIPI_write_cmos_sensor(0x3652,0xff);
OV9750MIPI_write_cmos_sensor(0x3654,0x00);
OV9750MIPI_write_cmos_sensor(0x3653,0x34);
OV9750MIPI_write_cmos_sensor(0x3655,0x20);
OV9750MIPI_write_cmos_sensor(0x3656,0xff);
OV9750MIPI_write_cmos_sensor(0x3657,0xc4);   
OV9750MIPI_write_cmos_sensor(0x365a,0xff);	 
OV9750MIPI_write_cmos_sensor(0x365b,0xff);  
OV9750MIPI_write_cmos_sensor(0x365e,0xff);  
OV9750MIPI_write_cmos_sensor(0x365f,0x00);  
OV9750MIPI_write_cmos_sensor(0x3668,0x00);  
OV9750MIPI_write_cmos_sensor(0x366a,0x07);  
OV9750MIPI_write_cmos_sensor(0x366d,0x00);  
OV9750MIPI_write_cmos_sensor(0x366e,0x10);  
OV9750MIPI_write_cmos_sensor(0x3702,0x1d);  
OV9750MIPI_write_cmos_sensor(0x3703,0x10);  
OV9750MIPI_write_cmos_sensor(0x3704,0x14);  
OV9750MIPI_write_cmos_sensor(0x3705,0x00);  
OV9750MIPI_write_cmos_sensor(0x3706,0x27);  
OV9750MIPI_write_cmos_sensor(0x3709,0x24);  
OV9750MIPI_write_cmos_sensor(0x370a,0x00);  
OV9750MIPI_write_cmos_sensor(0x370b,0x7d);  
OV9750MIPI_write_cmos_sensor(0x3714,0x24);  
OV9750MIPI_write_cmos_sensor(0x371a,0x5e);  
OV9750MIPI_write_cmos_sensor(0x3730,0x82);  
OV9750MIPI_write_cmos_sensor(0x3733,0x10);  
OV9750MIPI_write_cmos_sensor(0x373e,0x18);  
OV9750MIPI_write_cmos_sensor(0x3755,0x00);  
OV9750MIPI_write_cmos_sensor(0x3758,0x00);  
OV9750MIPI_write_cmos_sensor(0x375b,0x13);  
OV9750MIPI_write_cmos_sensor(0x3772,0x23);  
OV9750MIPI_write_cmos_sensor(0x3773,0x05);  
OV9750MIPI_write_cmos_sensor(0x3774,0x16);  
OV9750MIPI_write_cmos_sensor(0x3775,0x12);  
OV9750MIPI_write_cmos_sensor(0x3776,0x08);  
OV9750MIPI_write_cmos_sensor(0x37a8,0x38);  
OV9750MIPI_write_cmos_sensor(0x37b5,0x36);  
OV9750MIPI_write_cmos_sensor(0x37c2,0x04);  
OV9750MIPI_write_cmos_sensor(0x37c5,0x00);  
OV9750MIPI_write_cmos_sensor(0x37c7,0x30);  
OV9750MIPI_write_cmos_sensor(0x37c8,0x00);  
OV9750MIPI_write_cmos_sensor(0x37d1,0x13);  
OV9750MIPI_write_cmos_sensor(0x3800,0x00);  
OV9750MIPI_write_cmos_sensor(0x3801,0x00);  
OV9750MIPI_write_cmos_sensor(0x3802,0x00);
OV9750MIPI_write_cmos_sensor(0x3803,0x04);  
OV9750MIPI_write_cmos_sensor(0x3804,0x05);  
OV9750MIPI_write_cmos_sensor(0x3805,0x0f);  
OV9750MIPI_write_cmos_sensor(0x3806,0x03);  
OV9750MIPI_write_cmos_sensor(0x3807,0xcb);  
OV9750MIPI_write_cmos_sensor(0x3808,0x05);  
OV9750MIPI_write_cmos_sensor(0x3809,0x00);  
OV9750MIPI_write_cmos_sensor(0x380a,0x03);  
OV9750MIPI_write_cmos_sensor(0x380b,0xc0);  
OV9750MIPI_write_cmos_sensor(0x380c,0x03);  
OV9750MIPI_write_cmos_sensor(0x380d,0x2a);  
OV9750MIPI_write_cmos_sensor(0x380e,0x03);  
OV9750MIPI_write_cmos_sensor(0x380f,0xdc);  
OV9750MIPI_write_cmos_sensor(0x3810,0x00);  
OV9750MIPI_write_cmos_sensor(0x3811,0x08);  
OV9750MIPI_write_cmos_sensor(0x3812,0x00);  
OV9750MIPI_write_cmos_sensor(0x3813,0x04);  
OV9750MIPI_write_cmos_sensor(0x3814,0x01);  
OV9750MIPI_write_cmos_sensor(0x3815,0x01);  
OV9750MIPI_write_cmos_sensor(0x3816,0x00);  
OV9750MIPI_write_cmos_sensor(0x3817,0x00);  
OV9750MIPI_write_cmos_sensor(0x3818,0x00);  
OV9750MIPI_write_cmos_sensor(0x3819,0x00);  
OV9750MIPI_write_cmos_sensor(0x3820,0x80);  
OV9750MIPI_write_cmos_sensor(0x3821,0x40);  
OV9750MIPI_write_cmos_sensor(0x3826,0x00);  
OV9750MIPI_write_cmos_sensor(0x3827,0x08);  
OV9750MIPI_write_cmos_sensor(0x382a,0x01);  
OV9750MIPI_write_cmos_sensor(0x382b,0x01);  
OV9750MIPI_write_cmos_sensor(0x3836,0x02);  
OV9750MIPI_write_cmos_sensor(0x3838,0x10);  
OV9750MIPI_write_cmos_sensor(0x3861,0x00);  
OV9750MIPI_write_cmos_sensor(0x3862,0x00);  
OV9750MIPI_write_cmos_sensor(0x3863,0x02);	
OV9750MIPI_write_cmos_sensor(0x3b00,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c00,0x89);  
OV9750MIPI_write_cmos_sensor(0x3c01,0xab);  
OV9750MIPI_write_cmos_sensor(0x3c02,0x01);  
OV9750MIPI_write_cmos_sensor(0x3c03,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c04,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c05,0x03);  
OV9750MIPI_write_cmos_sensor(0x3c06,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c07,0x05);  
OV9750MIPI_write_cmos_sensor(0x3c0c,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c0d,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c0e,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c0f,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c40,0x00);  
OV9750MIPI_write_cmos_sensor(0x3c41,0xa3);  
OV9750MIPI_write_cmos_sensor(0x3c43,0x7d);  
OV9750MIPI_write_cmos_sensor(0x3c56,0x80);  
OV9750MIPI_write_cmos_sensor(0x3c80,0x08);  
OV9750MIPI_write_cmos_sensor(0x3c82,0x01);  
OV9750MIPI_write_cmos_sensor(0x3c83,0x61);  
OV9750MIPI_write_cmos_sensor(0x3d85,0x17);  
OV9750MIPI_write_cmos_sensor(0x3f08,0x08);  
OV9750MIPI_write_cmos_sensor(0x3f0a,0x00);  
OV9750MIPI_write_cmos_sensor(0x3f0b,0x30);  
OV9750MIPI_write_cmos_sensor(0x4000,0xcd);  
OV9750MIPI_write_cmos_sensor(0x4003,0x40);  
OV9750MIPI_write_cmos_sensor(0x4009,0x0d);  
OV9750MIPI_write_cmos_sensor(0x4010,0xf0);  
OV9750MIPI_write_cmos_sensor(0x4011,0x70);  
OV9750MIPI_write_cmos_sensor(0x4017,0x10);  
OV9750MIPI_write_cmos_sensor(0x4040,0x00);  
OV9750MIPI_write_cmos_sensor(0x4041,0x00);  
OV9750MIPI_write_cmos_sensor(0x4303,0x00);  
OV9750MIPI_write_cmos_sensor(0x4307,0x30);  
OV9750MIPI_write_cmos_sensor(0x4500,0x30);  
OV9750MIPI_write_cmos_sensor(0x4502,0x40);  
OV9750MIPI_write_cmos_sensor(0x4503,0x06);  
OV9750MIPI_write_cmos_sensor(0x4508,0xaa);
OV9750MIPI_write_cmos_sensor(0x450b,0x00);  
OV9750MIPI_write_cmos_sensor(0x450c,0x00);  
OV9750MIPI_write_cmos_sensor(0x4600,0x00);  
OV9750MIPI_write_cmos_sensor(0x4601,0x80);  
OV9750MIPI_write_cmos_sensor(0x4700,0x04);  
OV9750MIPI_write_cmos_sensor(0x4704,0x00);  
OV9750MIPI_write_cmos_sensor(0x4705,0x04);  
OV9750MIPI_write_cmos_sensor(0x4837,0x14);  
OV9750MIPI_write_cmos_sensor(0x484a,0x3f);  
OV9750MIPI_write_cmos_sensor(0x5000,0x10);  
OV9750MIPI_write_cmos_sensor(0x5001,0x01);  
OV9750MIPI_write_cmos_sensor(0x5002,0x28);  
OV9750MIPI_write_cmos_sensor(0x5004,0x0c);  
OV9750MIPI_write_cmos_sensor(0x5006,0x0c);  
OV9750MIPI_write_cmos_sensor(0x5007,0xe0);  
OV9750MIPI_write_cmos_sensor(0x5008,0x01);  
OV9750MIPI_write_cmos_sensor(0x5009,0xb0);  
OV9750MIPI_write_cmos_sensor(0x502a,0x18);  
OV9750MIPI_write_cmos_sensor(0x5901,0x00);  
OV9750MIPI_write_cmos_sensor(0x5a01,0x00);  
OV9750MIPI_write_cmos_sensor(0x5a03,0x00);  
OV9750MIPI_write_cmos_sensor(0x5a04,0x0c);  
OV9750MIPI_write_cmos_sensor(0x5a05,0xe0);  
OV9750MIPI_write_cmos_sensor(0x5a06,0x09);  
OV9750MIPI_write_cmos_sensor(0x5a07,0xb0);  
OV9750MIPI_write_cmos_sensor(0x5a08,0x06);  
OV9750MIPI_write_cmos_sensor(0x5e00,0x00);  
OV9750MIPI_write_cmos_sensor(0x5e10,0xfc);  
OV9750MIPI_write_cmos_sensor(0x300f,0x00);  
OV9750MIPI_write_cmos_sensor(0x3733,0x10);  
OV9750MIPI_write_cmos_sensor(0x3610,0xe8);  
OV9750MIPI_write_cmos_sensor(0x3611,0x56);
OV9750MIPI_write_cmos_sensor(0x3635,0x14);  
OV9750MIPI_write_cmos_sensor(0x3636,0x13);	
OV9750MIPI_write_cmos_sensor(0x3620,0x84);  
OV9750MIPI_write_cmos_sensor(0x3614,0x96);  
OV9750MIPI_write_cmos_sensor(0x481f,0x30);  
OV9750MIPI_write_cmos_sensor(0x3788,0x00);  
OV9750MIPI_write_cmos_sensor(0x3789,0x04);  
OV9750MIPI_write_cmos_sensor(0x378a,0x01);  
OV9750MIPI_write_cmos_sensor(0x378b,0x60);  
OV9750MIPI_write_cmos_sensor(0x3799,0x27);  
OV9750MIPI_write_cmos_sensor(0x0100,0x01);  
                                        


        
OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIInitialSetting function:\n ");

    
} 
/*****************************************************************
* FUNCTION
*    OV9750MIPIPreviewSetting
*
* DESCRIPTION
*    This function config Preview setting related registers of CMOS sensor.
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/

// 1952x1092 -> active array size: 1280x960,  by zhangxueping

// SVGA 800 * 600,  by zhangxueping 

void OV9750MIPIPreviewSetting_SVGA(void)
{
    
    OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIPreviewSetting_SVGA function:\n ");

#if defined(IME_OV9750_MIRROR)
	OV9750MIPI_write_cmos_sensor(0x3820, 0x86);
	OV9750MIPI_write_cmos_sensor(0x3821, 0x46);
	OV9750MIPI_write_cmos_sensor(0x3811, 0x07);
#endif

    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIPreviewSetting_SVGA function:\n ");
}
static void XC6130PreviewSetting_SVGA(void)
{
     OV9750MIPISENSORDB("[XC6130]ENTER XC6130PreviewSetting_SVGA function:\n ");
    mDELAY(5);


 //AE
XC6130_write_cmos_sensor(0xfffe,0x14);
XC6130_write_cmos_sensor(0x0021,0x00);
XC6130_write_cmos_sensor(0x002e,0x04);
XC6130_write_cmos_sensor(0x002f,0xa0);
XC6130_write_cmos_sensor(0x0032,0x00);
XC6130_write_cmos_sensor(0x0033,0x20);
XC6130_write_cmos_sensor(0x0037,0xe0);
XC6130_write_cmos_sensor(0x003f,0x01);
XC6130_write_cmos_sensor(0x003e,0x20);
XC6130_write_cmos_sensor(0x003d,0x60);
XC6130_write_cmos_sensor(0x0070,0x00);
XC6130_write_cmos_sensor(0x0022,0x01);
XC6130_write_cmos_sensor(0x0070,0x00);
XC6130_write_cmos_sensor(0x0002,0x01);
XC6130_write_cmos_sensor(0x0060,0x35);
XC6130_write_cmos_sensor(0x0061,0x08);
XC6130_write_cmos_sensor(0x0062,0x35);
XC6130_write_cmos_sensor(0x0063,0x09);
XC6130_write_cmos_sensor(0xfffd,0x80);
XC6130_write_cmos_sensor(0xfffe,0x21);
XC6130_write_cmos_sensor(0x0680,0x01);
XC6130_write_cmos_sensor(0x0681,0x00);
XC6130_write_cmos_sensor(0x0682,0x00);
XC6130_write_cmos_sensor(0x0683,0x60);
XC6130_write_cmos_sensor(0x0684,0x03);
XC6130_write_cmos_sensor(0x0685,0x00);
XC6130_write_cmos_sensor(0x0686,0x03);
XC6130_write_cmos_sensor(0x0687,0x00);
XC6130_write_cmos_sensor(0x0688,0x41);
XC6130_write_cmos_sensor(0x0689,0x14);
XC6130_write_cmos_sensor(0x068a,0x44);
XC6130_write_cmos_sensor(0x068b,0x44);
XC6130_write_cmos_sensor(0x068c,0x44);
XC6130_write_cmos_sensor(0x068d,0x44);
XC6130_write_cmos_sensor(0x068e,0x41);
XC6130_write_cmos_sensor(0x068f,0x14);
XC6130_write_cmos_sensor(0x0690,0x03);
XC6130_write_cmos_sensor(0xfffd,0x80);
XC6130_write_cmos_sensor(0xfffe,0x14);
XC6130_write_cmos_sensor(0x0022,0x01);
XC6130_write_cmos_sensor(0x0088,0x10);
XC6130_write_cmos_sensor(0x0089,0x10);

XC6130_write_cmos_sensor(0xfffd,0x80);
XC6130_write_cmos_sensor(0xfffe,0x21);
XC6130_write_cmos_sensor(0x0000,0xd7); 
XC6130_write_cmos_sensor(0xfffe,0x21); //LENC start 70%
XC6130_write_cmos_sensor(0x0800,0x07);   
XC6130_write_cmos_sensor(0x0801,0x03);   
XC6130_write_cmos_sensor(0x0802,0x02);   
XC6130_write_cmos_sensor(0x0803,0x02);   
XC6130_write_cmos_sensor(0x0804,0x02);   
XC6130_write_cmos_sensor(0x0805,0x06);   
XC6130_write_cmos_sensor(0x0806,0x02);   
XC6130_write_cmos_sensor(0x0807,0x01);   
XC6130_write_cmos_sensor(0x0808,0x01);   
XC6130_write_cmos_sensor(0x0809,0x01);   
XC6130_write_cmos_sensor(0x080a,0x01);   
XC6130_write_cmos_sensor(0x080b,0x01);   
XC6130_write_cmos_sensor(0x080c,0x01);   
XC6130_write_cmos_sensor(0x080d,0x00);   
XC6130_write_cmos_sensor(0x080e,0x00);   
XC6130_write_cmos_sensor(0x080f,0x00);   
XC6130_write_cmos_sensor(0x0810,0x00);   
XC6130_write_cmos_sensor(0x0811,0x01);   
XC6130_write_cmos_sensor(0x0812,0x01);   
XC6130_write_cmos_sensor(0x0813,0x00);   
XC6130_write_cmos_sensor(0x0814,0x00);   
XC6130_write_cmos_sensor(0x0815,0x00);   
XC6130_write_cmos_sensor(0x0816,0x00);   
XC6130_write_cmos_sensor(0x0817,0x01);   
XC6130_write_cmos_sensor(0x0818,0x02);   
XC6130_write_cmos_sensor(0x0819,0x01);   
XC6130_write_cmos_sensor(0x081a,0x01);   
XC6130_write_cmos_sensor(0x081b,0x01);   
XC6130_write_cmos_sensor(0x081c,0x01);   
XC6130_write_cmos_sensor(0x081d,0x01);   
XC6130_write_cmos_sensor(0x081e,0x05);   
XC6130_write_cmos_sensor(0x081f,0x02);   
XC6130_write_cmos_sensor(0x0820,0x02);   
XC6130_write_cmos_sensor(0x0821,0x02);   
XC6130_write_cmos_sensor(0x0822,0x02);   
XC6130_write_cmos_sensor(0x0823,0x04);   
XC6130_write_cmos_sensor(0x0824,0x43);   
XC6130_write_cmos_sensor(0x0825,0x21);   
XC6130_write_cmos_sensor(0x0826,0x21);   
XC6130_write_cmos_sensor(0x0827,0x21);   
XC6130_write_cmos_sensor(0x0828,0x03);   
XC6130_write_cmos_sensor(0x082a,0x21);   
XC6130_write_cmos_sensor(0x082b,0x61);   
XC6130_write_cmos_sensor(0x082c,0x61);   
XC6130_write_cmos_sensor(0x082d,0x41);   
XC6130_write_cmos_sensor(0x082e,0x41);   
XC6130_write_cmos_sensor(0x0830,0x41);   
XC6130_write_cmos_sensor(0x0831,0x61);   
XC6130_write_cmos_sensor(0x0832,0x61);   
XC6130_write_cmos_sensor(0x0833,0x61);   
XC6130_write_cmos_sensor(0x0834,0x41);   
XC6130_write_cmos_sensor(0x0836,0x21);   
XC6130_write_cmos_sensor(0x0837,0x41);   
XC6130_write_cmos_sensor(0x0838,0x61);   
XC6130_write_cmos_sensor(0x0839,0x41);   
XC6130_write_cmos_sensor(0x083a,0x41);   
XC6130_write_cmos_sensor(0x083c,0x03);   
XC6130_write_cmos_sensor(0x083d,0x21);   
XC6130_write_cmos_sensor(0x083e,0x21);   
XC6130_write_cmos_sensor(0x083f,0x23);   
XC6130_write_cmos_sensor(0x0840,0x01);   
XC6130_write_cmos_sensor(0x0848,0xaf);   

XC6130_write_cmos_sensor(0x084d,0x02);	
XC6130_write_cmos_sensor(0x084e,0x66);
XC6130_write_cmos_sensor(0x084f,0x03);	
XC6130_write_cmos_sensor(0x0850,0x33);
XC6130_write_cmos_sensor(0x0851,0x03);	
XC6130_write_cmos_sensor(0x0852,0x33);
XC6130_write_cmos_sensor(0x0853,0x02);	
XC6130_write_cmos_sensor(0x0854,0x22);  	

XC6130_write_cmos_sensor(0xfffd,0x80);            
XC6130_write_cmos_sensor(0xfffe,0x14);           
XC6130_write_cmos_sensor(0x02b3,0x01);              
XC6130_write_cmos_sensor(0x02b6,0x06);              
XC6130_write_cmos_sensor(0x02b7,0x00);              
XC6130_write_cmos_sensor(0x02ba,0x04);              
XC6130_write_cmos_sensor(0x02bb,0x00);              
XC6130_write_cmos_sensor(0x02be,0x04);              
XC6130_write_cmos_sensor(0x02bf,0x00);              
XC6130_write_cmos_sensor(0x02c2,0x04);              
XC6130_write_cmos_sensor(0x02c3,0x04);              
XC6130_write_cmos_sensor(0x030a,0x07);              
XC6130_write_cmos_sensor(0x030b,0x00);              
XC6130_write_cmos_sensor(0x030c,0x04);              
XC6130_write_cmos_sensor(0x030d,0x00);              
XC6130_write_cmos_sensor(0x030e,0x05);              
XC6130_write_cmos_sensor(0x030f,0xa0);            
XC6130_write_cmos_sensor(0xfffe,0x14);            
XC6130_write_cmos_sensor(0x0000,0x01);               
XC6130_write_cmos_sensor(0x02b0,0x00);               
XC6130_write_cmos_sensor(0x02b2,0x01);               
XC6130_write_cmos_sensor(0x02d4,0x00);
XC6130_write_cmos_sensor(0x02d5,0xf0);
XC6130_write_cmos_sensor(0x02d6,0x0d);                
XC6130_write_cmos_sensor(0x02d7,0xff);            
XC6130_write_cmos_sensor(0xfffe,0x21);            
XC6130_write_cmos_sensor(0x0091,0xb0);              
XC6130_write_cmos_sensor(0x0092,0x10);              
XC6130_write_cmos_sensor(0x0003,0xf1);              
XC6130_write_cmos_sensor(0x0051,0x03);              
XC6130_write_cmos_sensor(0x0096,0x83);              
XC6130_write_cmos_sensor(0x019c,0x0a);              
XC6130_write_cmos_sensor(0x0019,0x48);              
XC6130_write_cmos_sensor(0x01b0,0x50);              // win1 startx         
XC6130_write_cmos_sensor(0x01b1,0x6a);              // win1 endx           
XC6130_write_cmos_sensor(0x01b2,0x64);              // win1 starty        
XC6130_write_cmos_sensor(0x01b3,0x88);              // win1 endy          
XC6130_write_cmos_sensor(0x01b4,0x80);              // win2 startx        
XC6130_write_cmos_sensor(0x01b5,0xb6);              // win2 endx          
XC6130_write_cmos_sensor(0x01b6,0x3b);              // win2 starty        
XC6130_write_cmos_sensor(0x01b7,0x50);              // win2 endy          
XC6130_write_cmos_sensor(0x01b8,0x78);              // win3 startx        
XC6130_write_cmos_sensor(0x01b9,0x9e);              // win3 endx          
XC6130_write_cmos_sensor(0x01ba,0x60);              // win3 starty        
XC6130_write_cmos_sensor(0x01bb,0x78);              // win3 endy          
XC6130_write_cmos_sensor(0x01bc,0x60);              // win4 startx        
XC6130_write_cmos_sensor(0x01bd,0x78);              // win4 endx          
XC6130_write_cmos_sensor(0x01be,0x60);              // win4 starty        
XC6130_write_cmos_sensor(0x01bf,0x92);              // win4 endy          
XC6130_write_cmos_sensor(0x01c0,0x74);              // win5 startx        
XC6130_write_cmos_sensor(0x01c1,0x9e);              // win5 endx          
XC6130_write_cmos_sensor(0x01c2,0x46);             // win5 starty         
XC6130_write_cmos_sensor(0x01c3,0x68);              // win5 endy          
XC6130_write_cmos_sensor(0x01c4,0x00);              // win6 startx        
XC6130_write_cmos_sensor(0x01c5,0x00);              // win6 endx          
XC6130_write_cmos_sensor(0x01c6,0x00);              // win6 starty        
XC6130_write_cmos_sensor(0x01c7,0x00);              // win6 endy          
XC6130_write_cmos_sensor(0x01c8,0x00);              // win7 startx        
XC6130_write_cmos_sensor(0x01c9,0x00);              // win7 endx          
XC6130_write_cmos_sensor(0x01ca,0x00);              // win7 starty        
XC6130_write_cmos_sensor(0x01cb,0x00);              // win7 endy          
XC6130_write_cmos_sensor(0x01cc,0x00);              // win8 startx        
XC6130_write_cmos_sensor(0x01cd,0x00);              // win8 endx          
XC6130_write_cmos_sensor(0x01ce,0x00);              // win8 starty        
XC6130_write_cmos_sensor(0x01cf,0x00);              // win8 endy          
XC6130_write_cmos_sensor(0x01d0,0x00);              // win9 startx        
XC6130_write_cmos_sensor(0x01d1,0x00);              // win9 endx          
XC6130_write_cmos_sensor(0x01d2,0x00);              // win9 starty        
XC6130_write_cmos_sensor(0x01d3,0x00);              // win9 endy          
XC6130_write_cmos_sensor(0x01d4,0x00);              // win10 startx       
XC6130_write_cmos_sensor(0x01d5,0x00);              // win10 endx         
XC6130_write_cmos_sensor(0x01d6,0x00);              // win10 starty       
XC6130_write_cmos_sensor(0x01d7,0x00);              // win10 endy         
XC6130_write_cmos_sensor(0x01d8,0x00);              // win11 startx       
XC6130_write_cmos_sensor(0x01d9,0x00);              // win11 endx         
XC6130_write_cmos_sensor(0x01da,0x00);              // win11 starty       
XC6130_write_cmos_sensor(0x01db,0x00);              // win11 endy         
XC6130_write_cmos_sensor(0x01dc,0x00);              // win12 startx       
XC6130_write_cmos_sensor(0x01dd,0x00);              // win12 endx         
XC6130_write_cmos_sensor(0x01de,0x00);              // win12 starty       
XC6130_write_cmos_sensor(0x01df,0x00);              // win12 endy         
XC6130_write_cmos_sensor(0x01e0,0x00);              // win13 startx       
XC6130_write_cmos_sensor(0x01e1,0x00);              // win13 endx         
XC6130_write_cmos_sensor(0x01e2,0x00);              // win13 starty       
XC6130_write_cmos_sensor(0x01e3,0x00);              // win13 endy         
XC6130_write_cmos_sensor(0x01e4,0x00);              // win14 startx       
XC6130_write_cmos_sensor(0x01e5,0x00);              // win14 endx         
XC6130_write_cmos_sensor(0x01e6,0x00);              // win14 starty       
XC6130_write_cmos_sensor(0x01e7,0x00);              // win14 endy         
XC6130_write_cmos_sensor(0x01e8,0x00);              // win15 startx       
XC6130_write_cmos_sensor(0x01e9,0x00);              //win15 endx          
XC6130_write_cmos_sensor(0x01ea,0x00);              // win15 starty       
XC6130_write_cmos_sensor(0x01eb,0x00);              // win15 endy         
XC6130_write_cmos_sensor(0x01ec,0x00);              // win16 startx       
XC6130_write_cmos_sensor(0x01ed,0x00);              // win16 endx         
XC6130_write_cmos_sensor(0x01ee,0x00);              // win16 starty       
XC6130_write_cmos_sensor(0x01ef,0x00);              // win16 endy         
XC6130_write_cmos_sensor(0x01f0,0x22);            // wt1 wt2            
XC6130_write_cmos_sensor(0x01f1,0x21);            // wt3 wt4            
XC6130_write_cmos_sensor(0x01f2,0x10);            // wt5 wt6            
XC6130_write_cmos_sensor(0x01f3,0x00);            // wt7 wt8            
XC6130_write_cmos_sensor(0x01f4,0x00);            // wt9 wt10           
XC6130_write_cmos_sensor(0x01f5,0x00);            // wt11 wt12          
XC6130_write_cmos_sensor(0x01f6,0x00);            // wt13 wt14          
XC6130_write_cmos_sensor(0x01f7,0x00);            // wt15 wt16      

XC6130_write_cmos_sensor(0xfffe,0x21);             //gamma  
XC6130_write_cmos_sensor(0x0480,0x0c);            
XC6130_write_cmos_sensor(0x0481,0x18);            
XC6130_write_cmos_sensor(0x0482,0x30);            
XC6130_write_cmos_sensor(0x0483,0x58);            
XC6130_write_cmos_sensor(0x0484,0x6a);            
XC6130_write_cmos_sensor(0x0485,0x7a);            
XC6130_write_cmos_sensor(0x0486,0x88);            
XC6130_write_cmos_sensor(0x0487,0x95);            
XC6130_write_cmos_sensor(0x0488,0xa0);            
XC6130_write_cmos_sensor(0x0489,0xaa);            
XC6130_write_cmos_sensor(0x048a,0xbb);            
XC6130_write_cmos_sensor(0x048b,0xc9);            
XC6130_write_cmos_sensor(0x048c,0xdd);            
XC6130_write_cmos_sensor(0x048d,0xeb);            
XC6130_write_cmos_sensor(0x048e,0xf5);            
XC6130_write_cmos_sensor(0x048f,0x07); 


XC6130_write_cmos_sensor(0xfffd,0x80);             
XC6130_write_cmos_sensor(0xfffe,0x21);            
XC6130_write_cmos_sensor(0x0d00,0x01);           //CMX  
XC6130_write_cmos_sensor(0x0d01,0xf2);           
XC6130_write_cmos_sensor(0x0d02,0x01);            
XC6130_write_cmos_sensor(0x0d03,0xcd);           
XC6130_write_cmos_sensor(0x0d04,0x01);            
XC6130_write_cmos_sensor(0x0d05,0x4f);           
XC6130_write_cmos_sensor(0x0d06,0x00);            
XC6130_write_cmos_sensor(0x0d07,0xa9);           
XC6130_write_cmos_sensor(0x0d08,0x00);            
XC6130_write_cmos_sensor(0x0d09,0x12);          
XC6130_write_cmos_sensor(0x0d0a,0x01);            
XC6130_write_cmos_sensor(0x0d0b,0x00);            
XC6130_write_cmos_sensor(0x0d0c,0x04);            
XC6130_write_cmos_sensor(0x0d0d,0x49);           
XC6130_write_cmos_sensor(0x0d0e,0x02);            
XC6130_write_cmos_sensor(0x0d0f,0xdc);           
XC6130_write_cmos_sensor(0x0d10,0x00);            
XC6130_write_cmos_sensor(0x0d11,0x50);           
XC6130_write_cmos_sensor(0x0d12,0x00);            
XC6130_write_cmos_sensor(0x0d13,0x0e);            
XC6130_write_cmos_sensor(0x0d14,0x01);            
XC6130_write_cmos_sensor(0x0d15,0x38);           
XC6130_write_cmos_sensor(0x0d16,0x00);            
XC6130_write_cmos_sensor(0x0d17,0xca);           
XC6130_write_cmos_sensor(0x0d18,0x02);            
XC6130_write_cmos_sensor(0x0d19,0x83);           
XC6130_write_cmos_sensor(0x0d1a,0x02);            
XC6130_write_cmos_sensor(0x0d1b,0x04);            
XC6130_write_cmos_sensor(0x0d1c,0x00);            
XC6130_write_cmos_sensor(0x0d1d,0x85);           
XC6130_write_cmos_sensor(0x0d1e,0x00);           
XC6130_write_cmos_sensor(0x0d1f,0x00);           
XC6130_write_cmos_sensor(0x0d20,0x00);           
XC6130_write_cmos_sensor(0x0d21,0x26);           
XC6130_write_cmos_sensor(0x0d22,0x01);            
XC6130_write_cmos_sensor(0x0d23,0xa4);           
XC6130_write_cmos_sensor(0x0d2e,0x06);            
XC6130_write_cmos_sensor(0x0d2f,0x05);            
XC6130_write_cmos_sensor(0x0d30,0x06);            
XC6130_write_cmos_sensor(0x0d31,0x03);            
XC6130_write_cmos_sensor(0x0d28,0x00);            
XC6130_write_cmos_sensor(0x0d29,0x5c);           
XC6130_write_cmos_sensor(0x0d2a,0x00);            
XC6130_write_cmos_sensor(0x0d2b,0xab);           
XC6130_write_cmos_sensor(0x0d2c,0x01);            
XC6130_write_cmos_sensor(0x0d2d,0x2c);  


XC6130_write_cmos_sensor(0xfffd,0x80);            
XC6130_write_cmos_sensor(0xfffe,0x21);            
XC6130_write_cmos_sensor(0x0301,0x10);            
XC6130_write_cmos_sensor(0x0303,0x80);             //CIP start 
XC6130_write_cmos_sensor(0x0304,0x00);            
XC6130_write_cmos_sensor(0x0305,0x10);            
XC6130_write_cmos_sensor(0x0306,0x01);            
XC6130_write_cmos_sensor(0x0307,0xdc);           
XC6130_write_cmos_sensor(0x0308,0x10);           
XC6130_write_cmos_sensor(0x0309,0x30);           
XC6130_write_cmos_sensor(0x030a,0x03);            
XC6130_write_cmos_sensor(0x030b,0x02);            
XC6130_write_cmos_sensor(0x030c,0x02);            
XC6130_write_cmos_sensor(0x030d,0x0c);            
XC6130_write_cmos_sensor(0x030e,0x18);           
XC6130_write_cmos_sensor(0x030f,0xc0);           
XC6130_write_cmos_sensor(0x0310,0x20);           
XC6130_write_cmos_sensor(0x0311,0xff);           
XC6130_write_cmos_sensor(0x0312,0x40);           
XC6130_write_cmos_sensor(0x0313,0x18);            
XC6130_write_cmos_sensor(0x031a,0x00);                       
XC6130_write_cmos_sensor(0x031b,0x30);                      
XC6130_write_cmos_sensor(0x031c,0x00);                       
XC6130_write_cmos_sensor(0x031d,0x10);                      
XC6130_write_cmos_sensor(0x031e,0x0f);                       
XC6130_write_cmos_sensor(0x031f,0x0f);                       
XC6130_write_cmos_sensor(0x0320,0x06);                       
XC6130_write_cmos_sensor(0x0321,0x08);   

XC6130_write_cmos_sensor(0xfffd,0x80);       //Denoise                 
XC6130_write_cmos_sensor(0xfffe,0x21);                       
XC6130_write_cmos_sensor(0x0290,0x06);                       
XC6130_write_cmos_sensor(0x0291,0x08);                       
XC6130_write_cmos_sensor(0x0292,0x00);                       
XC6130_write_cmos_sensor(0x029a,0x00);                       
XC6130_write_cmos_sensor(0x029b,0x02);                       
XC6130_write_cmos_sensor(0x029c,0x04);                       
XC6130_write_cmos_sensor(0x029d,0x06);                       
XC6130_write_cmos_sensor(0x029e,0x08);                       
XC6130_write_cmos_sensor(0x029f,0x09);                       
XC6130_write_cmos_sensor(0x02a0,0x0a);   

XC6130_write_cmos_sensor(0xfffd,0x80);                       
XC6130_write_cmos_sensor(0xfffe,0x21);                       
XC6130_write_cmos_sensor(0x0000,0x47);                       
XC6130_write_cmos_sensor(0x0001,0x82);            
XC6130_write_cmos_sensor(0x0003,0xb1);            
XC6130_write_cmos_sensor(0x0005,0x90);                       
XC6130_write_cmos_sensor(0x0013,0x10);            
XC6130_write_cmos_sensor(0x0580,0x09);                       
XC6130_write_cmos_sensor(0x0581,0x50);            
XC6130_write_cmos_sensor(0x0582,0x80);           //BLC      
XC6130_write_cmos_sensor(0x0584,0x22);            
XC6130_write_cmos_sensor(0xfffd,0x80);            
XC6130_write_cmos_sensor(0xfffe,0x80);            
XC6130_write_cmos_sensor(0x012c,0x00);            
XC6130_write_cmos_sensor(0xfffd,0x80);            
XC6130_write_cmos_sensor(0xfffe,0x14);            
XC6130_write_cmos_sensor(0x0094,0x04);            
XC6130_write_cmos_sensor(0x0095,0x10);           
XC6130_write_cmos_sensor(0x02b3,0x00);

XC6130_write_cmos_sensor(0xfffe,0x14); 
XC6130_write_cmos_sensor(0x003d,0x60); 
XC6130_write_cmos_sensor(0xfffe,0x21); 
XC6130_write_cmos_sensor(0x031a,0x00); 
XC6130_write_cmos_sensor(0x031b,0x30); 
XC6130_write_cmos_sensor(0x031c,0x00); 
XC6130_write_cmos_sensor(0x031d,0x10); 
XC6130_write_cmos_sensor(0x031e,0x0f); 
XC6130_write_cmos_sensor(0x031f,0x0f); 
XC6130_write_cmos_sensor(0x0320,0x06); 
XC6130_write_cmos_sensor(0x0321,0x08); 
XC6130_write_cmos_sensor(0xfffe,0x21);
XC6130_write_cmos_sensor(0x0580,0x09);
XC6130_write_cmos_sensor(0x0581,0x50);
XC6130_write_cmos_sensor(0x0582,0x80);
XC6130_write_cmos_sensor(0x0584,0x22);    
     


  OV9750MIPISENSORDB("[XC6130]exit XC6130PreviewSetting_SVGA function end :\n ");
}

/*************************************************************************
* FUNCTION
*     XC6130 ISP FullSizeCaptureSetting
*
* DESCRIPTION
*    This function config full size capture setting by XC6130.
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void XC6130FullSizeCaptureSetting(void)
{
OV9750MIPISENSORDB("XC6130_ISP_5MCaptureSetting start \n");








OV9750MIPISENSORDB("XC6130_ISP_5MCaptureSetting  end \n");

}




void  OV9750MIPI_Preview_Init(void); // by zhangxueping

/*************************************************************************
* FUNCTION
*   OV9750MIPIOpen
*
* DESCRIPTION
*   This function initialize the registers of CMOS sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
//#define XC6130_ISP_ID 0x5843

UINT32 OV9750MIPIOpen(void)
{
    kal_uint32 isp_id[4]={0};
    kal_uint32 j;

    
    volatile signed int i;
    //kal_uint16 isp_id = 0;
     kal_uint16 sensor_id = 0;    
     kal_uint16 ret;
		
	//TV_SWITCH_PRINT("[OV9750MIPI]OV9750MIPIOpen: ZHENGFENG_0320\n ");
	TV_SWITCH_PRINT("[OV9750MIPI]OV9750MIPIOpen-4: 150618\n ");

	if(isCameraOpen)
	{
		TV_SWITCH_PRINT("[OV9750MIPI]OV9750MIPIOpen, has opened\n ");
		return ERROR_NONE;
	}

	//is_KERN_EMERG = true;
	//mutex_lock(&TV_switch_lock);
		
		#if 1
    XC6130_write_cmos_sensor(0xfffd,0x80);
    XC6130_write_cmos_sensor(0xfffe,0x80);
        mDELAY(10);
    
    for (j=0;j<4;j++)
    {
        isp_id[j]=XC6130_read_cmos_sensor(j);
        OV9750MIPISENSORDB("[OV9750MIPI_ISP]ISP_id[%d]=0x%x \n",j,isp_id[j]);
    }
    
    XC6130InitialSetting(); 
    XC6130_write_cmos_sensor(0xfffe,0x80);
    XC6130_write_cmos_sensor(0x0050,0x06);  //  PWDN,GPIO1,GPIO0 en   by Nathan 20150719
    XC6130_write_cmos_sensor(0x0054,0x06);  //  PWDN,GPIO1,ouput 控制sensorPWDN&reset ; GPIO0 被MTK6572来控制XC6130 PWDN
    XC6130_write_cmos_sensor(0x0058,0x00); //   将GPIO1（sensor reset）先拉低
    mDELAY(5);
    XC6130_write_cmos_sensor(0x0058,0x06); //    将GPIO1（sensor reset）再拉高 PWDN低
    mDELAY(5);
    XC6130_write_cmos_sensor(0xfffd ,0x80);
    XC6130_write_cmos_sensor(0xfffe ,0x80);
    XC6130_write_cmos_sensor(0x004d ,0x01);


    ret=XC6130_read_cmos_sensor(0x0030);
    OV9750MIPISENSORDB("[XC6130]XC6130 Preview ret:0x0030---%x\n ",ret);
    ret=XC6130_read_cmos_sensor(0x0031);
    OV9750MIPISENSORDB("[XC6130]XC6130 Preview ret:0x0031---%x\n ",ret);
    ret=XC6130_read_cmos_sensor(0x0058);
    OV9750MIPISENSORDB("[XC6130]XC6130 Preview ret:0x0058---%x\n ",ret);
    ret=XC6130_read_cmos_sensor(0x004d);
    OV9750MIPISENSORDB("[XC6130]XC6130 Preview ret:0x004d---%x\n ",ret);
    
    #endif
    mDELAY(10);
    for(i=0;i<3;i++)
    {
        sensor_id = (OV9750MIPIYUV_read_cmos_sensor(0x300B) << 8) | OV9750MIPIYUV_read_cmos_sensor(0x300C);
        OV9750MIPISENSORDB("OV9750MIPIOpen: READ ID :%x\n",sensor_id);
        if(sensor_id != 0x9750)
    	{
    		//mutex_unlock(&TV_switch_lock);
			//is_KERN_EMERG = false;
			
			TV_SWITCH_PRINT("[OV9750MIPI] return FAIL\n ");
            return ERROR_SENSOR_CONNECT_FAIL;
        }
    }

    OV9750MIPIInitialSetting();
    
       ret=OV9750MIPIYUV_read_cmos_sensor(0x0300);
    OV9750MIPISENSORDB("[OV9750]OV9750MIPIPreview ret:0x0300---%x\n ",ret);
       ret=OV9750MIPIYUV_read_cmos_sensor(0x0301);
    OV9750MIPISENSORDB("[OV9750]OV9750MIPIPreview ret:0x0301---%x\n ",ret);
       ret=OV9750MIPIYUV_read_cmos_sensor(0x0302);
    OV9750MIPISENSORDB("[OV9750]OV9750MIPIPreview ret:0x0302---%x\n ",ret);
    ret=OV9750MIPIYUV_read_cmos_sensor(0x0303);
    OV9750MIPISENSORDB("[OV9750]OV9750MIPIPreview ret:0x0303---%x\n ",ret);


    XC6130_write_cmos_sensor(0xfffe ,0x80);
    XC6130_write_cmos_sensor(0xfffe ,0x80);
    XC6130_write_cmos_sensor(0x004d ,0x00);
   
	OV9750MIPI_Preview_Init(); // by zhangxueping
	 
    TV_SWITCH_PRINT("[OV9750MIPI]exit OV9750MIPIOpen\n ");

	
    return ERROR_NONE;
}   /* OV9750MIPIOpen() */

/*************************************************************************
* FUNCTION
*   OV9750MIPIClose
*
* DESCRIPTION
*   This function is to turn off sensor module power.
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 OV9750MIPIClose(void)
{
	TV_SWITCH_PRINT("OV9750MIPIClose\n ");
	
#ifdef OV9750_REG_READBACK
		if(isOV9750HrtimerStart)
		{
			OV9750MIPISENSORDB("Cancel hrtimer\n ");
			hrtimer_cancel(&ov9750_readback_timer);
			isOV9750HrtimerStart = KAL_FALSE;
		}	  
#endif	

	
  //CISModulePowerOn(FALSE);
    return ERROR_NONE;
}   /* OV9750MIPIClose() */

/*************************************************************************
* FUNCTION
*   OV9750MIPIPreview
*
* DESCRIPTION
*   This function start the sensor preview.
*
* PARAMETERS
*   *image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 OV9750MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIPreview function:\n ");
    kal_uint32 zsdshutter = 0;

				// by zhangxueping
#ifdef OV9750_REG_READBACK   
	if(!isOV9750HrtimerStart)
	{	
		OV9750MIPISENSORDB("Start hrtimer\n ");
		ov9750_read_detect_hrtimer_init();
		isOV9750HrtimerStart = KAL_TRUE;
	}		
#endif	

	
	return ERROR_NONE ;
}


void OV9750MIPI_Preview_Init(void)
{
	int ret=0;
   
	XC6130_write_cmos_sensor(0xfffd ,0x80);
	XC6130_write_cmos_sensor(0xfffe ,0x80);
	XC6130_write_cmos_sensor(0x004d ,0x01);
	
	OV9750MIPIPreviewSetting_SVGA();
    
    ret=OV9750MIPIYUV_read_cmos_sensor(0x0300);
    OV9750MIPISENSORDB("HZTCT :REG:0x0300---%x\n ",ret);    // sensor clock
	ret=OV9750MIPIYUV_read_cmos_sensor(0x0301); 
    OV9750MIPISENSORDB("HZTCT :REG:0x0301---%x\n ",ret); 
	ret=OV9750MIPIYUV_read_cmos_sensor(0x0302);
    OV9750MIPISENSORDB("HZTCT :REG:0x0302---%x\n ",ret);
    ret=OV9750MIPIYUV_read_cmos_sensor(0x0303);                                 
    OV9750MIPISENSORDB("HZTCT :REG:0x0303---%x\n ",ret);   // sensor clock
    
    XC6130_write_cmos_sensor(0xfffd ,0x80);
	XC6130_write_cmos_sensor(0xfffe ,0x80);
    XC6130_write_cmos_sensor(0x004d ,0x00);       

	XC6130PreviewSetting_SVGA();                                     
															  
    XC6130_write_cmos_sensor(0xfffe ,0x21);    
	
    ret=XC6130_read_cmos_sensor(0x0004);                             
	OV9750MIPISENSORDB("HZTCT :reg::0x0004=%x\n ",ret);

    
    XC6130_write_cmos_sensor(0xfffe ,0x26);
    ret=XC6130_read_cmos_sensor(0x0038);
    OV9750MIPISENSORDB("HZTCT :reg::0x0038---%x\n ",ret);   // snesor size  , 0x60
    ret=XC6130_read_cmos_sensor(0x0039);
    OV9750MIPISENSORDB("HZTCT :reg::0x0039---%x\n ",ret);   // snesor size  , 0x09
    ret=XC6130_read_cmos_sensor(0x003a);
    OV9750MIPISENSORDB("HZTCT :reg::0x003a---%x\n ",ret);   // snesor size  , 0x38
    ret=XC6130_read_cmos_sensor(0x003b);
    OV9750MIPISENSORDB("HZTCT :reg::0x003b---%x\n ",ret);   // snesor size  , 0x04
    
    XC6130_write_cmos_sensor(0xfffe ,0x21);
	   
    ret=XC6130_read_cmos_sensor(0x0098);
    OV9750MIPISENSORDB("HZTCT :reg::0x0098---%x\n ",ret);    //awb, change 
    ret=XC6130_read_cmos_sensor(0x0099);
    OV9750MIPISENSORDB("HZTCT :reg::0x0099---%x\n ",ret);
    ret=XC6130_read_cmos_sensor(0x009c);
    OV9750MIPISENSORDB("HZTCT :reg::0x009a---%x\n ",ret);
    ret=XC6130_read_cmos_sensor(0x009d);
	
    XC6130_write_cmos_sensor(0xfffe ,0x14);
	ret=XC6130_read_cmos_sensor(0x0002);
    OV9750MIPISENSORDB("HZTCT :reg::0x0002---%x\n ",ret);      //AE, on/off
	ret=XC6130_read_cmos_sensor(0x0026);
    OV9750MIPISENSORDB("HZTCT :reg::0x0026---%x\n ",ret);      // change
	ret=XC6130_read_cmos_sensor(0x002b);
    OV9750MIPISENSORDB("HZTCT :reg::0x002b---%x\n ",ret);     //gain 0x10~0x78(0x0037)
	ret=XC6130_read_cmos_sensor(0x003c);
    OV9750MIPISENSORDB("HZTCT :reg::0x003c---%x\n ",ret);     // lum value
	ret=XC6130_read_cmos_sensor(0x0022);
    OV9750MIPISENSORDB("HZTCT :reg::0x0022---%x\n ",ret);    // 0x00/0x01
	 	
	
    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIPreview function:\n ");   

    return ERROR_NONE ;
    
}   /* OV9750MIPIPreview() */


UINT32 OV9750MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    TV_SWITCH_PRINT("[OV9750MIPI]OV9750MIPICapture\n ");

 	//is_KERN_EMERG = true;
#if 0	
    //mutex_lock(&TV_switch_lock);     

	 XC6130_write_cmos_sensor(0xfffd ,0x80);
    XC6130_write_cmos_sensor(0xfffe ,0x80);
    XC6130_write_cmos_sensor(0x004d ,0x01);

	//mutex_unlock(&TV_switch_lock);
#endif	

//#if defined(OV9750_REG_READBACK)
	reg_readback();          
//#endif
	
	//is_KERN_EMERG = false;
	
    TV_SWITCH_PRINT("[OV9750MIPI]exit OV9750MIPICapture\n ");
    return ERROR_NONE; 
}/* OV9750MIPICapture() */

UINT32 OV9750MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIGetResolution:%d,%d\n ", OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH, OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT);

    pSensorResolution->SensorPreviewWidth=  (OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH - 48);   //OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*OV9750MIPI_PV_GRAB_START_X;
    pSensorResolution->SensorPreviewHeight= (OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT - 36);   //OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*OV9750MIPI_PV_GRAB_START_Y;
    pSensorResolution->SensorFullWidth= (OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH - 48);    //OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*OV9750MIPI_FULL_GRAB_START_X; 
    pSensorResolution->SensorFullHeight= (OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT - 36);   //OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*OV9750MIPI_FULL_GRAB_START_Y;
    pSensorResolution->SensorVideoWidth= (OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH - 48);   //OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*OV9750MIPI_PV_GRAB_START_X; 
    pSensorResolution->SensorVideoHeight= (OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT - 36);   //OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*OV9750MIPI_PV_GRAB_START_Y;;
    OV9750MIPISENSORDB("Nathan - >>>>>[OV9750MIPI]exit OV9750MIPIGetResolution function:\n ");
    return ERROR_NONE;
}   /* OV9750MIPIGetResolution() */

UINT32 OV9750MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,MSDK_SENSOR_INFO_STRUCT *pSensorInfo,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    OV9750MIPISENSORDB("[OV9750MIPI]OV9750MIPIGetInfo-619: ScenarioId=%d %d, %d\n ", ScenarioId, OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH,OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT);
    //switch (ScenarioId)
    //{
        //case MSDK_SCENARIO_ID_CAMERA_ZSD:
        //  pSensorInfo->SensorPreviewResolutionX=OV9750MIPI_IMAGE_SENSOR_SVGA_WIDTH-2*OV9750MIPI_FULL_GRAB_START_X;//OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH ;
        //  pSensorInfo->SensorPreviewResolutionY=OV9750MIPI_IMAGE_SENSOR_SVGA_HEIGHT-2*OV9750MIPI_FULL_GRAB_START_Y;//OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT ;
        //  pSensorInfo->SensorCameraPreviewFrameRate=15;
        //  break;
        //default:
            pSensorInfo->SensorPreviewResolutionX=(OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH - 32);//OV9750MIPI_IMAGE_SENSOR_SVGA_WIDTH-2*OV9750MIPI_PV_GRAB_START_X; ;
            pSensorInfo->SensorPreviewResolutionY=(OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT  - 24);//OV9750MIPI_IMAGE_SENSOR_SVGA_HEIGHT-2*OV9750MIPI_PV_GRAB_START_Y;
            pSensorInfo->SensorCameraPreviewFrameRate=30;
            //break;
    //}             
    pSensorInfo->SensorFullResolutionX= (OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH - 32);//OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH-2*OV9750MIPI_FULL_GRAB_START_X;
    pSensorInfo->SensorFullResolutionY= (OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT  - 24);//OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT-2*OV9750MIPI_FULL_GRAB_START_Y;
    //pSensorInfo->SensorCameraPreviewFrameRate=30;
    pSensorInfo->SensorVideoFrameRate=30;
    pSensorInfo->SensorStillCaptureFrameRate=5;
    pSensorInfo->SensorWebCamCaptureFrameRate=15;
    pSensorInfo->SensorResetActiveHigh=FALSE;
    pSensorInfo->SensorResetDelayCount=4;
    pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	
    pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW; 
    pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;

	//by zhangxueping
    //pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;  
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;  
	
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    //pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
	
    pSensorInfo->SensorInterruptDelayLines = 2;
    pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_MIPI;

    pSensorInfo->CaptureDelayFrame = 3;
    pSensorInfo->PreviewDelayFrame = 3; 
	
    pSensorInfo->VideoDelayFrame = 3;       
    pSensorInfo->SensorMasterClockSwitch = 0; 
    pSensorInfo->YUVAwbDelayFrame = 5;
    pSensorInfo->YUVEffectDelayFrame= 3; 
    pSensorInfo->AEShutDelayFrame= 0;
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;        


	// imx179, add by zhangxueping
	pSensorInfo->MIPIsensorType = MIPI_OPHY_NCSI2;       // MIPI_OPHY_CSI2; 
	
	pSensorInfo->SettleDelayMode = MIPI_SETTLEDELAY_AUTO;
	//pSensorInfo->SettleDelayMode = MIPI_SETTLEDELAY_MANUAL;  

	pSensorInfo->HighSpeedVideoDelayFrame = 4;    
	//pSensorInfo->SlimVideoDelayFrame = 4;    
	//pSensorInfo->SensorModeNum = 5;

	
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:

#if 1			
            pSensorInfo->SensorClockFreq=26;
			
            pSensorInfo->SensorClockDividCount= 3;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
#endif
			
            pSensorInfo->SensorGrabStartX = OV9750MIPI_PV_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = OV9750MIPI_PV_GRAB_START_Y;   
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;    

			
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;

			//pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 4; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; // 130;  // 13;  // 17; // 4; // 14;    // 4; 
            
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;		
			
            


#if 0

MUINT8 MIPIDataLowPwr2HighSpeedTermDelayCount;
MUINT8 MIPIDataLowPwr2HighSpeedSettleDelayCount;
MUINT8 MIPICLKLowPwr2HighSpeedTermDelayCount;

#endif
			
			
            pSensorInfo->SensorWidthSampling = 0; 
            pSensorInfo->SensorHightSampling = 0;   
            pSensorInfo->SensorPacketECCOrder = 1;      
            break;

			
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount= 3;
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2;
            pSensorInfo->SensorPixelClockCount= 3;
            pSensorInfo->SensorDataLatchCount= 2;
            pSensorInfo->SensorGrabStartX = OV9750MIPI_FULL_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = OV9750MIPI_FULL_GRAB_START_Y;             
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;         
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount =4; 
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->SensorWidthSampling = 0; 
            pSensorInfo->SensorHightSampling = 0;
            pSensorInfo->SensorPacketECCOrder = 1;
            break;
        default:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount=3;
            pSensorInfo->SensorClockRisingCount=0;
            pSensorInfo->SensorClockFallingCount=2;
            pSensorInfo->SensorPixelClockCount=3;
            pSensorInfo->SensorDataLatchCount=2;
            pSensorInfo->SensorGrabStartX = OV9750MIPI_PV_GRAB_START_X; 
            pSensorInfo->SensorGrabStartY = OV9750MIPI_PV_GRAB_START_Y;             
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE; //SENSOR_MIPI_1_LANE;         
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 4; 
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
            pSensorInfo->SensorWidthSampling = 0;
            pSensorInfo->SensorHightSampling = 0;   
            pSensorInfo->SensorPacketECCOrder = 1;
          break;


    }
    memcpy(pSensorConfigData, &OV9750MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));  
    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIGetInfo function:\n ");  
    return ERROR_NONE;
}   /* OV9750MIPIGetInfo() */

UINT32 OV9750MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
      OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIControl function:\n ");
      spin_lock(&OV9750mipi_drv_lock);
      CurrentScenarioId = ScenarioId;
      spin_unlock(&OV9750mipi_drv_lock);
      switch (ScenarioId)
      {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
             OV9750MIPIPreview(pImageWindow, pSensorConfigData);
             break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
             OV9750MIPICapture(pImageWindow, pSensorConfigData);
             break;
        default:
            return ERROR_INVALID_SCENARIO_ID;
    }
    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIControl function:\n ");
    return ERROR_NONE;
}   /* OV9750MIPIControl() */

UINT32 OV9750MIPIYUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{

   // OV9750MIPISENSORDB("OV9750MIPIYUVSensorSetting:iCmd=%d,iPara=%d, %d \n",iCmd, iPara);
    OV9750MIPISENSORDB("[OV9750MIPI]enter OV9750MIPIYUVSensorSetting function:\n ");
    switch (iCmd) {
        case FID_SCENE_MODE:
            //OV9750MIPI_set_scene_mode(iPara);
            break;      
        case FID_AWB_MODE:
                //OV9750MIPI_set_param_wb(iPara);
              break;
        case FID_COLOR_EFFECT:              
                //OV9750MIPI_set_param_effect(iPara);
              break;
        case FID_AE_EV:   
                //OV9750MIPI_set_param_exposure(iPara);
            break;
        case FID_AE_FLICKER:                    
                //OV9750MIPI_set_param_banding(iPara);
              break;
        case FID_AE_SCENE_MODE: 
                if (iPara == AE_MODE_OFF) 
                {
                    spin_lock(&OV9750mipi_drv_lock);
                    //OV9750MIPI_AE_ENABLE = KAL_FALSE; 
                    spin_unlock(&OV9750mipi_drv_lock);
        }
        else 
        {
                    spin_lock(&OV9750mipi_drv_lock);
                    //OV9750MIPI_AE_ENABLE = KAL_TRUE; 
                    spin_unlock(&OV9750mipi_drv_lock);
            }
                //OV9750MIPI_set_AE_mode(OV9750MIPI_AE_ENABLE);
        break; 
        case FID_ISP_CONTRAST:
            //OV9750MIPI_set_contrast(iPara);
            break;
        case FID_ISP_BRIGHT:
            //OV9750MIPI_set_brightness(iPara);
            break;
        case FID_ISP_SAT:
            //OV9750MIPI_set_saturation(iPara);
        break; 
    case FID_ZOOM_FACTOR:
            OV9750MIPISENSORDB("FID_ZOOM_FACTOR:%d\n", iPara);      
                    spin_lock(&OV9750mipi_drv_lock);
            //zoom_factor = iPara; 
                    spin_unlock(&OV9750mipi_drv_lock);
            break; 
        case FID_AE_ISO:
            //OV9750MIPI_set_iso(iPara);
            break;
#if 0 //afc
        case FID_AF_MODE:
             OV9750MIPI_set_param_afmode(iPara);
                    break;     
#endif            
      default:
                  break;
    }
    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIYUVSensorSetting function:\n ");

      return TRUE;
}   /* OV9750MIPIYUVSensorSetting */



UINT32 OV9750MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	unsigned long long *feature_data=(unsigned long long *) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
    UINT32 Tony_Temp1 = 0;
    UINT32 Tony_Temp2 = 0;
    Tony_Temp1 = pFeaturePara[0];
    Tony_Temp2 = pFeaturePara[1];
    OV9750MIPISENSORDB("[OV9750MIPI][OV9750MIPIFeatureControl]feature id=%d \n",FeatureId);
    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH;
            *pFeatureReturnPara16=OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT;
            *pFeatureParaLen=4;
            break;
			
        case SENSOR_FEATURE_GET_PERIOD:

#if 0 			
            switch(CurrentScenarioId)
            {
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                    *pFeatureReturnPara16++=OV9750MIPI_FULL_PERIOD_PIXEL_NUMS + OV9750MIPISensor.CaptureDummyPixels;
                    *pFeatureReturnPara16=OV9750MIPI_FULL_PERIOD_LINE_NUMS + OV9750MIPISensor.CaptureDummyLines;
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara16++=OV9750MIPI_PV_PERIOD_PIXEL_NUMS + OV9750MIPISensor.PreviewDummyPixels;
                    *pFeatureReturnPara16=OV9750MIPI_PV_PERIOD_LINE_NUMS + OV9750MIPISensor.PreviewDummyLines;
                    *pFeatureParaLen=4;
                    break;
            }
#else
			*pFeatureReturnPara16++=OV9750MIPI_IMAGE_SENSOR_QSXGA_WITDH;
			*pFeatureReturnPara16=OV9750MIPI_IMAGE_SENSOR_QSXGA_HEIGHT;
			*pFeatureParaLen=4;

						
#endif			
            break;

        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:

#if 0   
            switch(CurrentScenarioId)
            {

         
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                    *pFeatureReturnPara32 = OV9750MIPISensor.ZsdturePclk * 1000 *100;    //unit: Hz             
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara32 = OV9750MIPISensor.PreviewPclk * 1000 *100;    //unit: Hz
                    *pFeatureParaLen=4;
					
                    break;
            }

#else
			*pFeatureReturnPara32 = 48000000;	 //unit: Hz
			*pFeatureParaLen=4;
			
#endif
			
            break;

        case SENSOR_FEATURE_SET_ESHUTTER:
            break;
        case SENSOR_FEATURE_GET_EXIF_INFO:
            OV9750MIPIGetExifInfo(*feature_data);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            //OV9750MIPI_night_mode((BOOL) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            break;
        case SENSOR_FEATURE_SET_REGISTER:
			
			//mutex_lock(&TV_switch_lock);
			
            if(pSensorRegData->RegAddr & 0x10000)            
            {            	
				OV9750MIPISENSORDB("SENSOR_FEATURE_SET_REGISTER(OV9750): addr:%x  value:%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
				XC6130_write_cmos_sensor(0xfffd ,0x80);
				XC6130_write_cmos_sensor(0xfffe ,0x80);
				XC6130_write_cmos_sensor(0x004d ,0x01);
				OV9750MIPI_write_cmos_sensor(pSensorRegData->RegAddr&0xffff, pSensorRegData->RegData);
				XC6130_write_cmos_sensor(0x004d ,0x00);
            }
			else
			{
				OV9750MIPISENSORDB("SENSOR_FEATURE_SET_REGISTER(XC6130): addr:%x  value:%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
				XC6130_write_cmos_sensor(pSensorRegData->RegAddr&0xffff, pSensorRegData->RegData);
			}
			//mutex_unlock(&TV_switch_lock);
			
            break;
        case SENSOR_FEATURE_GET_REGISTER:

			//mutex_lock(&TV_switch_lock);										
            if(pSensorRegData->RegAddr & 0x10000)            
            {
				XC6130_write_cmos_sensor(0xfffd ,0x80);
				XC6130_write_cmos_sensor(0xfffe ,0x80);
				XC6130_write_cmos_sensor(0x004d ,0x01);
				pSensorRegData->RegData = OV9750MIPIYUV_read_cmos_sensor(pSensorRegData->RegAddr&0xffff);				
				XC6130_write_cmos_sensor(0x004d ,0x00);				
				OV9750MIPISENSORDB("SENSOR_FEATURE_GET_REGISTER(OV9750): addr:0x%x  value:0x%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
            }
			else
			{	
				pSensorRegData->RegData = XC6130_read_cmos_sensor(pSensorRegData->RegAddr&0xffff);
				OV9750MIPISENSORDB("SENSOR_FEATURE_GET_REGISTER(XC6130): addr:0x%x  value:0x%x\n",pSensorRegData->RegAddr,pSensorRegData->RegData);
    		}
			//mutex_unlock(&TV_switch_lock);	
			
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pSensorConfigData, &OV9750MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
            *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
        case SENSOR_FEATURE_GET_CCT_REGISTER:
        case SENSOR_FEATURE_SET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
        case SENSOR_FEATURE_GET_GROUP_INFO:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
            break;
        case SENSOR_FEATURE_GET_GROUP_COUNT:
            *pFeatureReturnPara32++=0;
            *pFeatureParaLen=4;    
            break; 
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_SET_TEST_PATTERN:            
            OV9750SetTestPatternMode((BOOL)*pFeatureData16);            
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            OV9750MIPI_GetSensorID(pFeatureData32);
            break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
            *pFeatureReturnPara32=OV9750_TEST_PATTERN_CHECKSUM;
            *pFeatureParaLen=4;
            break;

			
        case SENSOR_FEATURE_SET_YUV_CMD:
            OV9750MIPIYUVSensorSetting((FEATURE_ID)*feature_data, *(feature_data+1));
            break;
#if 0

			
        case SENSOR_FEATURE_SET_YUV_3A_CMD:
            OV9750MIPI_3ACtrl((ACDK_SENSOR_3A_LOCK_ENUM)*pFeatureData32);
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            OV9750MIPIYUVSetVideoMode(*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_EV_AWB_REF:
            OV9750MIPIGetEvAwbRef(*pFeatureData32);
            break;
        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
            OV9750MIPIGetCurAeAwbInfo(*pFeatureData32);         
            break;
        case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
            OV9750MIPIMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32,*(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
            OV9750MIPIGetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32,(MUINT32 *)*(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
            //OV9750MIPI_get_AEAWB_lock(*pFeatureData32, *(pFeatureData32+1));  // by zhangxueping
            break;
        case SENSOR_FEATURE_GET_DELAY_INFO:
            OV9750MIPISENSORDB("SENSOR_FEATURE_GET_DELAY_INFO\n");
            //OV9750MIPI_GetDelayInfo(*pFeatureData32);         // by zhangxueping
            break;
        case SENSOR_FEATURE_AUTOTEST_CMD:
            OV9750MIPISENSORDB("SENSOR_FEATURE_AUTOTEST_CMD\n");
            OV9750MIPI_AutoTestCmd(*feature_data,*(feature_data+1));
            break;
        case SENSOR_FEATURE_INITIALIZE_AF:  
	     // OV9750_FOCUS_OVT_AFC_Init();         
             break;
        case SENSOR_FEATURE_MOVE_FOCUS_LENS:
            OV9750_FOCUS_Move_to(*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_AF_STATUS:
            OV9750_FOCUS_OVT_AFC_Get_AF_Status(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_AF_INF:
            OV9750_FOCUS_Get_AF_Inf(pFeatureReturnPara32);
            *pFeatureParaLen=4;            
            break;
        case SENSOR_FEATURE_GET_AF_MACRO:
            OV9750_FOCUS_Get_AF_Macro(pFeatureReturnPara32);
            *pFeatureParaLen=4;            
            break;
        case SENSOR_FEATURE_CONSTANT_AF:
            //OV9750_FOCUS_OVT_AFC_Constant_Focus();
             break;
        case SENSOR_FEATURE_SET_AF_WINDOW:       
            OV9750_FOCUS_Set_AF_Window(*feature_data);
            break;
        case SENSOR_FEATURE_SINGLE_FOCUS_MODE:
            //OV9750_FOCUS_OVT_AFC_Single_Focus();
            break;  
        case SENSOR_FEATURE_CANCEL_AF:
            OV9750_FOCUS_OVT_AFC_Cancel_Focus();
            break;                  
        case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
            OV9750_FOCUS_Get_AF_Max_Num_Focus_Areas(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;        
        case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
            OV9750_FOCUS_Get_AE_Max_Num_Metering_Areas(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;        
        case SENSOR_FEATURE_SET_AE_WINDOW:
//	            OV9750MIPISENSORDB("AE zone addr = 0x%x\n",*feature_data);            
            OV9750_FOCUS_Set_AE_Window(*pFeatureData32);
            break; 
#endif

        default:
            OV9750MIPISENSORDB("OV9750MIPIFeatureControl:default \n");
            break;          
    }
    OV9750MIPISENSORDB("[OV9750MIPI]exit OV9750MIPIFeatureControl function:\n ");
    return ERROR_NONE;
}   /* OV9750MIPIFeatureControl() */

SENSOR_FUNCTION_STRUCT  SensorFuncOV9750MIPI=
{
    OV9750MIPIOpen,
    OV9750MIPIGetInfo,
    OV9750MIPIGetResolution,
    OV9750MIPIFeatureControl,
    OV9750MIPIControl,
    OV9750MIPIClose
};

UINT32 TW9992_MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /* To Do : Check Sensor status here */
    if (pfFunc!=NULL)
        *pfFunc=&SensorFuncOV9750MIPI;
    return ERROR_NONE;
}/* SensorInit() */
	

#endif  // defined(IME_OV9750_2) || defined(IME_OV9750_ZHENGFENG_0318)
