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
 *    shake_algorithm.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This Module is for shake algorithm interface header.
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
 * $Modtime$	20140707
 * $Log$
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 * 
 * Usage:
 * ---------
 *
 * #include <shake_algorithm.h>
 *
 *
 * int totalShake = 0;
 *
 * void shake_run(void)
 * {		
 * 	int shakeDetect;
 * 
 *	// to update sensor buffer, Param are X,Y,Z
 *	updateSensorBuffer(&Param, &bufferParam);	// 2. to update sensor buffer
 * 	shakeDetect = shakeQuery();			// 3. do shakeQuery
 * 	if (shakeDetect == DETECTED)			// 4. record happened count
 * 	{
 * 	    totalShake++;
 * 	}
 * }
 * 
 * void shf_process_run()
 * {
 * 	shake_run();
 * }
 * 
 * void shf_process_init()
 * {
 * 	shake_init();					// 1. do shake_init
 * }
 *
 ****************************************************************************/
#ifndef __SHAKE_ALGORITHM_H
#define __SHAKE_ALGORITHM_H

#include "lib.h"

extern int shakeQuery();
extern int shakeDetection(int cond_still, int accUpdateIdx, int accBuff[NUM_DIRECTIONS][ACC_BUFFER_SIZE], int* stillTimer, int* shakePrdTimer, int* twoShakeIntervalTimer, int* noDetectShakePrd, int* state, int* corrleationCnt, int* usingCorrType, int* firstShakeUsingCorrType, int* shakeCrossCnt, int* signXY, int* signXZ);
extern void shake_init(void);
extern int setShakeSensorInterval(int newInterval);// return set sensor sampling interval

#endif
