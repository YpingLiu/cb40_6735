/*
 * Copyright (c) 2008 Travis Geiselbrecht
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
#ifndef __SENSOR_MANAGER_FW_H__
#define __SENSOR_MANAGER_FW_H__

#include <sys/types.h>
#include "sensor_manager.h"

/*
* Arg[in]: 0: deactivate, 1:activate
* Flush fifo in SCP side.
* return successful or not
*/
#define	SENSOR_ACTIVATE_CMD	0X01

/*
* Arg[in]: ms delay for one event
* return actual delay (influenced by android)
*/
#define	SENSOR_SETDELAY_CMD	0X02

/*
* Arg[in]: pointer for event_t[ ]
* return event length
*/
#define	SENSOR_POLL_CMD	       	0X03

/*
* For sensor to save data (ex: virtual context sensor)
* Arg[out]: pointer for event_t[ ]
* return successful or not
*/
#define	SENSOR_SAVE_CMD	       	0X04

/*
* return activate, deactivate, close.
*/
#define	SENSOR_STATUS_CMD	0x05

/*
* Arg[in]: config batch or single event
*/
#define SENSOR_BATCH_CMD 	0x06 

/*
* Arg[in]: config batch or single event
*/
#define SENSOR_REGISTER_ACTIVATE_CALLBACK_CMD 	0x07 

extern int SCP_Sensor_Manager_close(uint32_t sensor_type);
extern int SCP_Sensor_Manager_open(uint32_t sensor_type);
extern int SCP_Sensor_Manager_control(uint32_t sensor_type, uint32_t cmd, void *arg);


#endif

