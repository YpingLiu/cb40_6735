/*
 * Copyright (c) 2009 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __ANC_CTP_H
#define __ANC_CTP_H

/***********************************************************************************
** Include Files
************************************************************************************/
 #include <app.h>
#include <debug.h>
#include <stdio.h>
#include <platform/mt_typedefs.h>
#include <arch/irq.h>
#include <kernel/thread.h>
#include <kernel/mutex.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#include "anc_service.h"

/***********************************************************************************
** Definitions
************************************************************************************/

//Test Cases
#define ANC_CTP_CASE_MicLBypass              0x3536
#define ANC_CTP_CASE_MicRBypass              0x3537
#define ANC_CTP_CASE_DLBypass                0x3538
#define ANC_CTP_CASE_LRMix                   0x3539
#define ANC_CTP_CASE_PmicCoef                0x353A
#define ANC_CTP_CASE_GenIO2                  0x353B
#define ANC_CTP_CASE_ANCFILTER1              0x353C
#define ANC_CTP_CASE_ANCFILTER2              0x353D
#define ANC_CTP_CASE_ANCFILTER3              0x353E
#define ANC_CTP_CASE_ANCFILTER4              0x353F
#define ANC_CTP_CASE_EXIT                    0x3540 

#define ANC_CTP_ANCFILTER_HANDSHAKE          0x5566

//Sysram address & Case Flag address
#define SYSRAM_START                         0xE1221000
#define SYSRAM_END                           0xE1224FFF
#define AFE_CASE_FLAG                        0xE12201D0

//ANC Filter Table
#define PMIC_COEF_LENGTH_A                   28
#define PMIC_COEF_LENGTH_B                   16
#define PMIC_COEF_LENGTH_C                    9
#define PMIC_COEF_ADDR_A                     32
#define PMIC_COEF_ADDR_B                      0
#define PMIC_COEF_ADDR_C                     20

//Toggle Bit
#define TOGGLE1                              0x80000
#define TOGGLE2                              0x00000

//PMIC Registers Define
#define NUM_PMIC_REG                         108
#define PMIC_TOP_CKPDN0                      (0x102)
#define PMIC_TOP_CKPDN1                      (0x108)
#define PMIC_AUDIO_SYS_TOP_REG_BASE          (0x2000)
#define PMIC_AFE_UL_DL_CON0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0000)
#define PMIC_AFE_DL_SRC2_CON0_H              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0002)
#define PMIC_AFE_DL_SRC2_CON0_L              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0004)
#define PMIC_AFE_DL_SDM_CON0                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0006)
#define PMIC_AFE_DL_SDM_CON1                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0008)
#define PMIC_AFE_UL_SRC_CON0_H               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x000A)
#define PMIC_AFE_UL_SRC_CON0_L               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x000C)
#define PMIC_AFE_UL_SRC_CON1_H               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x000E)
#define PMIC_AFE_UL_SRC_CON1_L               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0010)
#define PMIC_AFE_TOP_CON0                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0012)
#define PMIC_AFE_AUDIO_TOP_CON0              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0014)
#define PMIC_AFE_DL_SRC_MON0                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0016)
#define PMIC_AFE_DL_SDM_TEST0                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0018)
#define PMIC_AFE_MON_DEBUG0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x001A)
#define PMIC_AFUNC_AUD_CON0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x001C)
#define PMIC_AFUNC_AUD_CON1                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x001E)
#define PMIC_AFUNC_AUD_CON2                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0020)
#define PMIC_AFUNC_AUD_CON3                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0022)
#define PMIC_AFUNC_AUD_CON4                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0024)
#define PMIC_AFUNC_AUD_MON0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0026)
#define PMIC_AFUNC_AUD_MON1                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0028)
#define PMIC_AUDRC_TUNE_MON0                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x002A)
#define PMIC_AFE_UP8X_FIFO_CFG0              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x002C)
#define PMIC_AFE_UP8X_FIFO_LOG_MON0          (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x002E)
#define PMIC_AFE_UP8X_FIFO_LOG_MON1          (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0030)
#define PMIC_AFE_DL_DC_COMP_CFG0             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0032)
#define PMIC_AFE_DL_DC_COMP_CFG1             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0034)
#define PMIC_AFE_DL_DC_COMP_CFG2             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0036)
#define PMIC_AFE_PMIC_NEWIF_CFG0             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0038)
#define PMIC_AFE_PMIC_NEWIF_CFG1             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x003A)
#define PMIC_AFE_PMIC_NEWIF_CFG2             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x003C)
#define PMIC_AFE_PMIC_NEWIF_CFG3             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x003E)
#define PMIC_AFE_SGEN_CFG0                   (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0040)
#define PMIC_AFE_SGEN_CFG1                   (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0042)
#define PMIC_AFE_ADDA2_UL_SRC_CON0_H         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0044)
#define PMIC_AFE_ADDA2_UL_SRC_CON0_L         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0046)
#define PMIC_AFE_UL_SRC_CON1_H1              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0048)
#define PMIC_AFE_ADDA2_UL_SRC_CON1_L         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x004A)
#define PMIC_AFE_ADDA2_UP8X_FIFO_LOG_MON0    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x004C)
#define PMIC_AFE_ADDA2_UP8X_FIFO_LOG_MON1    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x004E)
#define PMIC_AFE_ADDA2_PMIC_NEWIF_CFG0       (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0050)
#define PMIC_AFE_ADDA2_PMIC_NEWIF_CFG1       (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0052)
#define PMIC_AFE_ADDA2_PMIC_NEWIF_CFG2       (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0054)
#define PMIC_AFE_MIC_ARRAY_CFG               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0056)
#define PMIC_AFE_ADC_ASYNC_FIFO_CFG          (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0058)
#define PMIC_AFE_ANC_CFG0                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x005A)
#define PMIC_AFE_ANC_CFG1                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x005C)
#define PMIC_AFE_ANC_COEF_B00                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x005E)
#define PMIC_AFE_ANC_COEF_ADDR               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0060)
#define PMIC_AFE_ANC_COEF_WDATA              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0062)
#define PMIC_AFE_ANC_COEF_RDATA              (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0064)
#define PMIC_AUDRC_TUNE_UL2_MON0             (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0066)
#define PMIC_AFE_MBIST_CFG0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0068)
#define PMIC_AFE_MBIST_CFG1                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x006A)
#define PMIC_AFE_MBIST_CFG2                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x006C)
#define PMIC_AFE_MBIST_CFG3                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x006E)
#define PMIC_AFE_MAD_TOP                     (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0070)
#define PMIC_AFE_MAD_CFG0                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0072)
#define PMIC_AFE_MAD_CFG1                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0074)
#define PMIC_AFE_MAD_CFG2                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0076)
#define PMIC_AFE_MAD_CFG3                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0078)
#define PMIC_AFE_MAD_CFG4                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x007A)
#define PMIC_AFE_MAD_CFG5                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x007C)
#define PMIC_AFE_MAD_MON0                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x007E)
#define PMIC_AFE_MAD_MON1                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0080)
#define PMIC_AFE_MAD_MON2                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0082)
#define PMIC_AFE_MAD_MON3                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0084)
#define PMIC_AFE_MAD_MON4                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0086)
#define PMIC_AFE_MAD_MON5                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0088)
#define PMIC_AFE_CLASSH_CFG0                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x008A)
#define PMIC_AFE_CLASSH_CFG1                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x008C)
#define PMIC_AFE_CLASSH_CFG2                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x008E)
#define PMIC_AFE_CLASSH_CFG3                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0090)
#define PMIC_AFE_CLASSH_CFG4                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0092)
#define PMIC_AFE_CLASSH_CFG5                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0094)
#define PMIC_AFE_CLASSH_CFG6                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0096)
#define PMIC_AFE_CLASSH_CFG7                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x0098)
#define PMIC_AFE_CLASSH_CFG8                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x009A)
#define PMIC_AFE_CLASSH_CFG9                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x009C)
#define PMIC_AFE_CLASSH_CFG10                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x009E)
#define PMIC_AFE_CLASSH_CFG11                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00A0)
#define PMIC_AFE_CLASSH_CFG12                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00A2)
#define PMIC_AFE_CLASSH_CFG13                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00A4)
#define PMIC_AFE_CLASSH_CFG14                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00A6)
#define PMIC_AFE_CLASSH_CFG15                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00A8)
#define PMIC_AFE_CLASSH_CFG16                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00AA)
#define PMIC_AFE_CLASSH_CFG17                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00AC)
#define PMIC_AFE_CLASSH_CFG18                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00AE)
#define PMIC_AFE_CLASSH_CFG19                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00B0)
#define PMIC_AFE_CLASSH_CFG20                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00B2)
#define PMIC_AFE_CLASSH_CFG21                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00B4)
#define PMIC_AFE_CLASSH_CFG22                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00B6)
#define PMIC_AFE_CLASSH_CFG23                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00B8)
#define PMIC_AFE_CLASSH_CFG24                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00BA)
#define PMIC_AFE_CLASSH_CFG25                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00BC)
#define PMIC_AFE_CLASSH_CFG26                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00BE)
#define PMIC_AFE_CLASSH_CFG27                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00C0)
#define PMIC_AFE_CLASSH_CFG28                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00C2)
#define PMIC_AFE_CLASSH_CFG29                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00C4)
#define PMIC_AFE_CLASSH_CFG30                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00C6)
#define PMIC_AFE_CLASSH_MON00                (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00C8)
#define PMIC_AFE_CLASSH_MON1                 (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00CA)
#define PMIC_AFE_CLASSH_RESERVED0            (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00CC)
#define PMIC_AFE_CLASSH_RESERVED1            (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00CE)
#define PMIC_AFE_DCCLK_CFG0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00D0)
#define PMIC_AFE_ANC_COEF_MON1               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00D2)
#define PMIC_AFE_ANC_COEF_MON2               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00D4)
#define PMIC_AFE_ANC_COEF_MON3               (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x00D6)

/***********************************************************************************
** Extern
************************************************************************************/
extern const kal_uint16 pmic_reg_default_table[NUM_PMIC_REG];
extern const kal_uint16 pmic_reg_mask_table[NUM_PMIC_REG];
extern const int16_t PmicCoefTableA1[PMIC_COEF_LENGTH_A];
extern const int16_t PmicCoefTableB1[PMIC_COEF_LENGTH_B];
extern const int16_t PmicCoefTableC1[PMIC_COEF_LENGTH_C];
extern const int16_t PmicCoefTableA2[PMIC_COEF_LENGTH_A];
extern const int16_t PmicCoefTableB2[PMIC_COEF_LENGTH_B];
extern const int16_t PmicCoefTableC2[PMIC_COEF_LENGTH_C];
extern const int16_t sinetable[105];

extern void anc_ctp_task(void);
extern void anc_ctp_init(void);

extern uint32_t anc_ctp_case_MicLBypass(void);	
extern uint32_t anc_ctp_case_MicRBypass(void);
extern uint32_t anc_ctp_case_DLBypass(void);
extern uint32_t anc_ctp_case_LRMix(void);
extern uint32_t anc_ctp_case_PmicCoef(void);
extern uint32_t anc_ctp_case_GenIO2(void);
extern uint32_t anc_ctp_caseANCfilter(uint8_t testcase);


   
#endif //__ANC_CTP_H
