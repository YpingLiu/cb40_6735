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

#ifndef __VOW_CTP_H
#define __VOW_CTP_H

/***********************************************************************************
** Options
************************************************************************************/
#define VOW_LOCALSIM 0

/***********************************************************************************
** Include Files
************************************************************************************/
#include <stdio.h>

#if VOW_LOCALSIM
#include "test_main.h"
#else
#include <app.h>
#include <debug.h>
#include <platform/mt_typedefs.h>
#include <arch/irq.h>
#include <kernel/thread.h>
#include <kernel/mutex.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#endif


#if VOW_LOCALSIM
 #define vow_print fprintf
#else 
 #if VOW_DEBUG
  #define vow_print dprintf
 #else
  #define vow_print(...) 
 #endif
#endif

/***********************************************************************************
** Definitions
************************************************************************************/

//Test Cases
#define VOW_CTP_CASE_PMICREG_RW              0x3536
#define VOW_CTP_CASE_PMIC_CLOCKREG           0x3537
#define VOW_CTP_CASE_DMA_TO_TCM              0x3538
#define VOW_CTP_CASE_DMA_FROM_TCM            0x3539
#define VOW_CTP_CASE_FORCE_PHASE2            0x353A
#define VOW_CTP_CASE_STATUS_CHECK            0x353B
#define VOW_CTP_CASE_VOW_RECORDING_INIT      0x353C
#define VOW_CTP_CASE_PMICREG_RW_ULPOSC       0x353D

//Test Result
#define VOW_CTP_CASE_PASS                    0x3535
#define VOW_CTP_CASE_FAIL                    0x5566
#define VOW_CTP_CASE_RUNNING                 0x8888
#define VOW_CTP_CASE_STOP                    0x7777
#define VOW_CTP_CASE_DATAREADY               0x7F00

//Sysram address & Case Flag address
#define SYSRAM_START                         0xE1221000
#define SYSRAM_END                           0xE1225FFF
#define AFE_CASE_FLAG                        0xE12201D0
#define AFE_STATUS_FLAG                      0xE12201EC


//PMIC Registers Define
#define PMIC_NUM_REG                         51
#define PMIC_AUDIO_SYS_TOP_REG_BASE          0x2000

//
#define CFGREG_BASE                          0xD0000000
#define CFGREG_GENERAL_REG1                  (CFGREG_BASE + 0x30)

#define VOW_FIFO_BASE                       0xD0000400
#define VOW_FIFO_EN                         (VOW_FIFO_BASE + 0x00)
#define VOW_FIFO_STA                        (VOW_FIFO_BASE + 0x04)
#define VOW_FIFO_DATA                       (VOW_FIFO_BASE + 0x08)
#define VOW_FIFO_DATA_THRES                 (VOW_FIFO_BASE + 0x0C)
#define VOW_FIFO_IRQ_ACK                    (VOW_FIFO_BASE + 0x10)
#define VOW_RXIF_CFG0                       (VOW_FIFO_BASE + 0x14)
#define VOW_RXIF_CFG1                       (VOW_FIFO_BASE + 0x18)
#define VOW_RXIF_CFG2                       (VOW_FIFO_BASE + 0x1C)
#define VOW_DEBUG_CFG1                       (VOW_FIFO_BASE + 0x20)
#define VOW_DEBUG_CFG2                       (VOW_FIFO_BASE + 0x24)




#define SCP_CLK_CTRL_BASE                   0xD0001000
#define SCP_CLK_SEL                         (SCP_CLK_CTRL_BASE + 0x00)
#define SCP_CLK_EN                          (SCP_CLK_CTRL_BASE + 0x04)
#define SCP_CLK_SAFE_ACK                    (SCP_CLK_CTRL_BASE + 0x08) 
#define SCP_HIGH_CLK_VAL                    (SCP_CLK_CTRL_BASE + 0x18)
#define SCP_CG_CTRL                         (SCP_CLK_CTRL_BASE + 0x30)
#define SCP_PMICW_CTRL                      (SCP_CLK_CTRL_BASE + 0x34)

//=============================================================================================
//                 PMIC Registers
//=============================================================================================
#define PMIC_SYS_TOP_REG_BASE                0x0000
#define PMIC_AUDIO_SYS_TOP_REG_BASE          0x2000

#define AUDENC_ANA_CON12                     0x0D06
#define INT_STATUS0                          0x02AE
#define TOP_CKPDN_CON0_CLR                   0x023C
#define TOP_CKPDN_CON0_SET                   0x023A
#define GPIO_MODE                            0x60D0
#define INT_CON0                             0x0296
#define TOP_CKPDN_CON0                       0x0238
#define AFE_VOW_TOP                          (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x70)
#define AFE_VOW_TGEN_CFG0                    (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x8A)
#define AFE_VOW_POSDIV_CFG0                  (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x8C)
#define AFE_VOW_CFG0                         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x72)
#define AFE_VOW_CFG1                         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x74)
#define AFE_VOW_CFG2                         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x76)
#define AFE_VOW_CFG3                         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x78)
#define AFE_VOW_CFG4                         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x7A)
#define AFE_VOW_CFG5                         (PMIC_AUDIO_SYS_TOP_REG_BASE + 0x7C)

//=============================================================================================
//                 DMA Registers
//=============================================================================================
#define AP_DMA_BASE                          0xE1000000
#define AP_DMA_HIF1_BASE                     (AP_DMA_BASE + 0x100)
#define AP_DMA_HIF_1_INT_EN                  (AP_DMA_HIF1_BASE + 0x04)
#define AP_DMA_HIF_1_EN                      (AP_DMA_HIF1_BASE + 0x08)
#define AP_DMA_HIF_1_CON                     (AP_DMA_HIF1_BASE + 0x18)
#define AP_DMA_HIF_SRC_ADDR                  (AP_DMA_HIF1_BASE + 0x1C)
#define AP_DMA_HIF_1_DST_ADDR                (AP_DMA_HIF1_BASE + 0x20)
#define AP_DMA_HIF_1_LEN                     (AP_DMA_HIF1_BASE + 0x24) 

#define DMA_CFG                              0xD00580A4

/***********************************************************************************
** Macros
************************************************************************************/
#define ReadREG(_addr, _value) ((_value) = *(volatile unsigned int *)(_addr) )
#define WriteREG(_addr, _value) (*(volatile unsigned int *)(_addr) = (_value))
#define ReadREG16(_addr, _value) ((_value) = *(volatile unsigned short *)(_addr) )
#define WriteREG16(_addr, _value) (*(volatile unsigned short *)(_addr) = (_value))

/***********************************************************************************
** Extern
************************************************************************************/
extern const uint16_t pmic_reg_addr_table[PMIC_NUM_REG];
extern const uint16_t pmic_reg_default_table[PMIC_NUM_REG];
extern const uint16_t pmic_reg_mask_table[PMIC_NUM_REG];
extern uint32_t dma_table[0x100];
extern uint32_t vow_ctp_case_pmicreg_rw();
extern uint32_t vow_ctp_case_pmic_clockreg();
extern uint32_t vow_ctp_case_dmaToTCM();
extern uint32_t vow_ctp_case_dmaFromTCM();
extern uint32_t vow_ctp_case_flowCheck(int forcephase2);
extern uint32_t vow_ctp_case_recording();
extern void vow_ctp_task(void);
extern uint32_t vow_ctp_case_pmicreg_rw_ulposc();
   
#endif //__ANC_CTP_H
