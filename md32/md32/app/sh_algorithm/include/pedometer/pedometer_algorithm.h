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
 *    Pedometer_algorithm.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This Module is for pedometer algorithm interface header.
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
 ****************************************************************************/
#ifndef __PEDOMETER_ALGORITHM_H
#define __PEDOMETER_ALGORITHM_H

//milli-g
typedef struct {
   int xmg;
   int ymg;
   int zmg;
} accelerometer_g_data_t;

typedef enum{
   still,
   moving,
   run
} pedometer_status_e;

#define PedometerAlgorithmStructSize 524
typedef struct
{
    unsigned int  array[524];
} AlgorithmWrapStruct;

typedef struct {
   unsigned int step_count;
   unsigned int accumulated_step_count;
   unsigned int step_length; //(in milli-meter)
   unsigned int step_frequency; //frequency multiple 1024
   unsigned int accumulated_step_length; //(in milli-meter)
   pedometer_status_e status;
} pedometer_algorithm_output_t;

#define ACCVERTSTD_TH              9500000
#define ACCHORIZSTD_TH             1150000
#define ACCHORIZMEAN_TH            2800

#define L1 53394
#define L2 -1070
#define L3 25258

//extern void set_pdr_share_memory(unsigned int sensorFrequency, AlgorithmWrapStruct* algorithm);
//extern void pedometer_algorithm_init(unsigned int sensorFrequency, AlgorithmWrapStruct* algorithm);
extern void pedometer_algorithm_init(unsigned int sensorFrequency);
extern int pedometer_algorithm_run(accelerometer_g_data_t data, pedometer_algorithm_output_t *output_data);

#endif
