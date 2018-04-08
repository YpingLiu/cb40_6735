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

#ifndef _SPK_PROTECT_H
#define _SPK_PROTECT_H

/***********************************************************************************
** Definitions
************************************************************************************/
#define SPP_DEBUG 0
#if SPP_DEBUG
   #define spp_print dprintf
#else
   #define spp_print(...)
#endif

#define ReadREG(_addr, _value) ((_value) = *(volatile unsigned int *)(_addr) )
#define WriteREG(_addr, _value) (*(volatile unsigned int *)(_addr) = (_value))
#define ReadREG16(_addr, _value) ((_value) = *(volatile unsigned short *)(_addr) )
#define WriteREG16(_addr, _value) (*(volatile unsigned short *)(_addr) = (_value))


/***********************************************************************************
** Enumeration
************************************************************************************/
typedef enum spk_protect_status_t {
   SPK_PROTECT_STATUS_IDLE = 0,
   SPK_PROTECT_STATUS_START,
   SPK_PROTECT_STATUS_STOP,
   SPK_PROTECT_STATUS_RUNNING
}spk_protect_status_t;


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





#endif
