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

#ifndef _SPK_PROTECT_IPI_MESSAGE_H
#define _SPK_PROTECT_IPI_MESSAGE_H


/* MD32/AP IPI Message ID, temp need to update */
#define MD32_IPI_AUDMSG_BASE 0x5F00
#define AP_IPI_AUDMSG_BASE   0x9F00

typedef enum spk_protect_ipi_msgid_t {
   AP_IPIMSG_SPK_PROTECT_ENABLE = AP_IPI_AUDMSG_BASE,
   AP_IPIMSG_SPK_PROTECT_READ_DATA_REQUEST_ACK,
   AP_IPIMSG_SPK_PROTECT_DISABLE,
   
   MD32_IPIMSG_SPK_PROTECT_ENABLE_ACK = MD32_IPI_AUDMSG_BASE,
   MD32_IPIMSG_SPK_PROTECT_READ_DATA_REQUEST,
   MD32_IPIMSG_SPK_PROTECT_DISABLE_ACK,
   
}spk_protect_ipi_msgid_t;


//temp for test
extern void spk_protect_ipi_handler(int id, void * data, unsigned int len);
extern bool spk_protect_ipi_sendmsg(spk_protect_ipi_msgid_t id, void *buf, unsigned int size, unsigned int wait);
	
#endif
