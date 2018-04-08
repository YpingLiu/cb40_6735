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

#ifndef _ANC_SERVICE_H
#define _ANC_SERVICE_H

#include <platform/mt_typedefs.h>
/***********************************************************************************
** Definitions
************************************************************************************/
#define ANC_DEBUG 0
#if ANC_DEBUG
   #define anc_print dprintf
#else
   #define anc_print(...)
#endif

#define ReadREG(_addr, _value) ((_value) = *(volatile unsigned int *)(_addr) )
#define WriteREG(_addr, _value) (*(volatile unsigned int *)(_addr) = (_value))
#define ReadREG16(_addr, _value) ((_value) = *(volatile unsigned short *)(_addr) )
#define WriteREG16(_addr, _value) (*(volatile unsigned short *)(_addr) = (_value))

#define ANC_MIC_INPUT_BUFFER_SIZE         864
#define ANC_MIC_INPUT_BUFFER_HALFSIZE     432
#define ANC_DL_INPUT_BUFFER_SIZE          100
#define ANC_COEF_BUFFER_SIZE              116
#define ANC_COEF_BUFFER_HALFSIZE           58

#define PMIC_ANC_COEF_LENGTH               62

#define AUDIO_IN_DATAD1                   0xD0000074
#define AUDIO_IN_DATAD2                   0xD0000078
#define AUDIO_IN_DATAD3                   0xD000007C
#define AUDIO_OUT_DATA                    0xD0000080

#define PMIC_COEF_LENGTH_A                   28
#define PMIC_COEF_LENGTH_B                   16
#define PMIC_COEF_LENGTH_C                    9
#define PMIC_COEF_ADDR_A                     32
#define PMIC_COEF_ADDR_B                      0
#define PMIC_COEF_ADDR_C                     20

/***********************************************************************************
** Enumeration
************************************************************************************/
typedef enum anc_service_status_t {
   ANC_STATUS_IDLE = 0,
   ANC_STATUS_START,
   ANC_STATUS_ENABLED,
   ANC_STATUS_STOP,
   ANC_STATUS_DISABLED,
   ANC_STATUS_CLOSE
}anc_service_status_t;

typedef enum anc_service_log_t {
   ANC_LOG_IDLE = 0,
   ANC_LOG_START,
   ANC_LOG_ENABLED,
   ANC_LOG_STOP
}anc_service_log_t;

typedef struct
{
  short addr;
	short bit_start;
	short bit_len;
	short value;
}anc_pmicwrap_t;

typedef struct
{
   bool ready;
   short buf[ANC_COEF_BUFFER_HALFSIZE];
}anc_coefbuf_t;

/***********************************************************************************
** API for Inter Communication between AP/MD32
************************************************************************************/
extern void anc_service_Init();
extern void anc_service_Enable();
extern void anc_service_Close();
extern void anc_service_Disable();
extern void anc_service_EnableLog();
extern void anc_service_DisableLog();

//fake
extern void anc_algo_init();
extern void anc_algo_task();
extern void anc_algo_start();
extern void anc_algo_stop();
extern void anc_algo_log_start();
extern void anc_algo_log_stop();
extern void anc_algo_close();

/***********************************************************************************
** API for ANC Control
************************************************************************************/

/***********************************************************************************
** Extern
************************************************************************************/
extern short anc_buf_in_mic[ANC_MIC_INPUT_BUFFER_SIZE];
extern short anc_buf_in_dl[ANC_DL_INPUT_BUFFER_SIZE];
extern anc_coefbuf_t  anc_buf_coef[2];
extern const kal_int8 pmic_audio_mapping[PMIC_ANC_COEF_LENGTH];
extern anc_pmicwrap_t pmic_audio_addr[10];


#endif
