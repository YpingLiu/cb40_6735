
/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// Define NAME_WRAP(x) before including this header to perform name-wrapping
// E.g. for ARM NEON, defined it as 'x ## _neon' to ensure all important
// identifiers have a _neon suffix.
//#ifndef NAME_WRAP
//#error "Please define NAME_WRAP() before including this file"
//#endif

#undef FILTER_PROC
#define FILTER_PROC(x, y, a, b, c, d, dst)   Filter_32_opaque(x, y, a, b, c, d, dst)
#define MAKENAME(suffix)        S32_Opaque_D32 ## suffix
#define SRCTYPE                 uint32_t
#define DSTTYPE                 uint32_t
#define TILEX_PROCF(fx, max)    SkClampMax((fx) >> 16, max)
#define TILEY_PROCF(fy, max)    SkClampMax((fy) >> 16, max)
#define TILEX_LOW_BITS(fx, max) (((fx) >> 12) & 0xF)
#define TILEY_LOW_BITS(fy, max) (((fy) >> 12) & 0xF)
#define SRC_TO_FILTER(src)      src

extern void S32_Opaque_D32_filter_DX_shaderproc_neon(const unsigned int* image0, const unsigned int* image1,
											SkFixed fx, unsigned int maxX, unsigned int subY,
											unsigned int* colors, SkFixed dx, int count);
#include "SkBitmapProcState_shaderproc_S32_D32_filter_DX.h"


