/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
 *
 * Filename:
 * ---------
 *    activity_algorithm.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This Module is for activity algorithm interface header.
 *
 * Author:
 * -------
 *  Lomen Wu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision$
 * $Modtime$	20140716
 * $Log$
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 * 
 * Usage:
 * ---------
 *
 * #include <activity_algorithm.h>
 *
 *
 * int activityDetect;
 * int putDataCount;
 * int windowSize;
 *
 * 
 * void shf_process_run()
 * {	
 *      activity_algorithm_output_t output_data = {0,};		// 3. prepare output buffer
 *	activity_accelerometer_g_data_t data = {32,32,32};      // 4. get X,Y,Z at every run
 *	sensorRawDataRecordWindow(data);			// 4. and update sensorWindow
 *	putDataCount++;						// 4. and update putCount
 * 
 * 	if((putDataCount >= windowSize) && (putDataCount%(windowSize/2) == 0))
 *	{
 *	    activityDetect = activity_algorithm_run(&output_data);	// 5. run ActivityRecognition, activityDetect as a return value can tempararily ignore now
 *	}
 * }
 * 
 * void shf_process_init()
 * {
 * 	activity_algorithm_init();				// 1. do activity_init for reset
 *	windowSize = get_sensorRawDataWindowSize();		// 2. get sensorWindow size
 *	putDataCount=0;
 * }
 *
 ****************************************************************************/
#ifndef __ACTIVITY_ALGORITHM_H
#define __ACTIVITY_ALGORITHM_H

//milli-g
typedef struct {
   int xmg;
   int ymg;
   int zmg;
} activity_accelerometer_g_data_t;

typedef struct {
   int in_vehicle;
   int on_bicycle;
   int on_foot;
   int still;
   int unknown;
   int tilt;
} activity_algorithm_output_t;

extern void activity_algorithm_init(void);
extern int get_sensorRawDataWindowSize(void);
extern void sensorRawDataRecordWindow(activity_accelerometer_g_data_t data);
extern int activity_algorithm_run(activity_algorithm_output_t *output_data);

#endif
