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
#ifndef _LIB_H
#define _LIB_H

#ifdef PC_DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif

enum Directions{ X = 0, Y = 1, Z = 2};
enum proximity{ CLOSED = 0, FAR = 1, PS_INVALID = 2};
enum detectState{ NOT_DETECTED = 0, DETECTED = 1, INVALID = 2};

// Sensor Frequency Setting
#define SENSOR_SAMPLING_INTERVAL_FACE_DOWN       20       // 20ms, face down can down to 40ms
#define SENSOR_SAMPLING_INTERVAL_SHAKE           20       // 20ms
#define SENSOR_SAMPLING_INTERVAL_PICKUP          20       // 20ms
#define SENSOR_SAMPLING_INTERVAL_POCKET          20       // 20ms

#define NUM_DIRECTIONS                   3
// size of accBuffer[][] and Norm[]
// [Note] if sensor sampling > 50Hz, the buffer size should be increased
#define ACC_BUFFER_SIZE                 32       // unit: samples @ 50Hz, total 640ms

#define STILL_WINDOW_SIZE              640       // unit: ms, 640s buffer

#define SHIFT_BIT                       10
#define SHIFT_VALUE         (1<<SHIFT_BIT)

#define GRAVITY                       9810       // unit (milli-m/s^2), 9.8m/s^2
// tiltDetector constant
#define COS_0_DEGREES  int(1*SHIFT_VALUE)        // cos(0 degree) = 1 * SHIFT_VALUE
// stillDetect constant
#define STILL_THRESHOLD         360000 /* 0.6*0.6*1000000 */ // threshold for StandardDev^2, unit (milli-m/s^2)^2
// [check still] constant definition
#define COS_35_DEGREES                839        // cos(35 degree) = 0.8192 * SHIFT_VALUE

#define Q32_FRAC_BITS                  32

typedef struct
{
    int accBuff[NUM_DIRECTIONS][ACC_BUFFER_SIZE];  //4*32*4=512
    int accNorm[ACC_BUFFER_SIZE];
    int accUpdateIdx;
    int cond_still;
    int buffReady;                      // to check if buffer is ready
}acc_BufferParam_t;

acc_BufferParam_t *getBufferParam();

unsigned int dsp_fixed_sqrt (unsigned int x);
int getAccNorm_dsp (int x, int y, int z);
int getAccStddev_2(int* ipSignal, int windowLength, int startIndex);

int tiltDetector(int firstAccVector[NUM_DIRECTIONS], int secondAccVector[NUM_DIRECTIONS]);
int chkAccBuffIdx(int idx, int bufferSize);
void avgAccVector(int acc[NUM_DIRECTIONS][ACC_BUFFER_SIZE], int* avgAcc, int start_idx, int avgSize);
int updateBuffIndex(int idx, int buffSize);
int absVal(int val);
int getAccHorizBFMax(int accHoriz[], int windowLength, int start_idx);
int getAccGlobalMaxDiff(int acc[NUM_DIRECTIONS][ACC_BUFFER_SIZE],int windowLength,  int start_idx);
int longIntegerDiv(long long int numerator, int shiftbit);
int integerDiv(int numerator, int denominator);
int bitOfInteger(unsigned int input);
unsigned int divide32(unsigned int nu, unsigned int de);
long long divide64(long long nu, long long de);

#endif  // _LIB_H
