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

#ifndef _ANC_IPI_MESSAGE_H
#define _ANC_IPI_MESSAGE_H

/*Just for local sim*/


typedef struct
{
   short id;
   short size;
   short *buf;
}anc_ipi_msg_t;

/* MD32/AP IPI Message ID */
#define MD32_IPI_AUDMSG_BASE 0x5F00
#define AP_IPI_AUDMSG_BASE   0x7F00

typedef enum anc_ipi_msgid_t {
   AP_IPIMSG_ANC_ENABLE = AP_IPI_AUDMSG_BASE,
   AP_IPIMSG_ANC_DISABLE,
   AP_IPIMSG_ANC_CLOSE,
   AP_IPIMSG_ANC_ENABLE_LOGGING,
   AP_IPIMSG_ANC_DISABLE_LOGGING,
   AP_IPIMSG_ANC_ANC_FILTERCOEF_NOTIFY_ACK,
   
   MD32_IPIMSG_ANC_ENABLE_ACK = AP_IPI_AUDMSG_BASE,
   MD32_IPIMSG_ANC_DISABLE_ACK,
   MD32_IPIMSG_ANC_CLOSE_ACK,
   MD32_IPIMSG_ANC_ENABLE_LOGGING_ACK,
   MD32_IPIMSG_ANC_DISABLE_LOGGING_ACK,
   MD32_IPIMSG_ANC_FILTERCOEF_NOTIFY,
   MD32_IPIMSG_ANC_TEMP
}anc_ipi_msgid_t;


//temp for test
extern void anc_ipi_handler(int id, void * data, unsigned int len);
extern bool anc_ipi_sendmsg(anc_ipi_msgid_t id, void *buf, unsigned int size, unsigned int wait);
	
#endif
