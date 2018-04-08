/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __DEV_TOUCH_H__
#define __DEV_TOUCH_H__

#include <platform/mt_typedefs.h>

typedef void (*GES_CBFUNC)(u8);

typedef enum
{
	GES_DOUBLE_CLICK,
    GES_SLIDE_RIGHT,
    GES_SLIDE_LEFT,
    GES_SLIDE_UP,
    GES_SLIDE_DOWN,
    GES_A,
    GES_B,
    GES_C,
    GES_D,
    GES_E,
    GES_F,
    GES_G,
    GES_H,
    GES_I,
    GES_J,
    GES_K,
    GES_L,
    GES_M,
    GES_N,
    GES_O,
    GES_P,
    GES_Q,
    GES_R,
    GES_S,
    GES_T,
    GES_U,
    GES_V,
    GES_W,
    GES_X,
    GES_Y,
    GES_Z,
	GES_HAT,	
	GES_LEFT_ANGLE_BRACKET,

	GES_MAX_NUM,
	GES_NONE = 0xFF
}TOUCH_GESTURE_ID;

void touch_gesture_switch(BOOL enable);
void touch_init();
bool touch_gesture_register(u8 gid, bool set, GES_CBFUNC cbfunc);

#endif /* __DEV_TOUCH_H__ */


