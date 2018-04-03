
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "SkSpriteBlitter.h"
#include "SkBlitRow.h"
#include "SkColorFilter.h"
#include "SkColorPriv.h"
#include "SkTemplates.h"
#include "SkUtils.h"
#include "SkXfermode.h"
#include "SkSpriteBlitter_S16_D32.h"

extern void blend16_32_row_neon_opt(SkPMColor* SK_RESTRICT dst,
                                 		   const uint16_t* SK_RESTRICT src,
                                 		   int count);

static void S16_Opaque_D32(SkPMColor* SK_RESTRICT dst,
                                   const uint16_t* SK_RESTRICT src,
                                   int count, U8CPU alpha) {
    SkASSERT(255 == alpha);
#if defined(__ARM_HAVE_NEON_COMMON) && defined(SK_CPU_LENDIAN)
	blend16_32_row_neon_opt(dst, src, count);
#else	
	if (count > 0) {
		do {
			*dst = SkPixel16ToPixel32(*src);
			src += 1;
			dst += 1;
		} while (--count > 0);
	}
#endif
}

static void S16_Blend_D32(SkPMColor* SK_RESTRICT dst,
                                 const uint16_t* SK_RESTRICT src,
                                 int count, U8CPU alpha) {
    SkASSERT(alpha <= 255);
    if (count > 0) {
        do {
            *dst = SkAlphaMulQ(SkPixel16ToPixel32(*src), alpha);
            src += 1;
            dst += 1;
        } while (--count > 0);
    }
}


Sprite_D32_S16::Sprite_D32_S16(const SkBitmap& src, U8CPU alpha)  
	: INHERITED(src) 
{
	SkASSERT(src.config() == SkBitmap::kRGB_565_Config);

	if (255 != alpha) {
		fProc32 = S16_Blend_D32;
	}
	else {
		fProc32 = S16_Opaque_D32;
	}
	fAlpha = alpha;
}

void Sprite_D32_S16::blitRect(int x, int y, int width, int height) {
	SkASSERT(width > 0 && height > 0);
	uint32_t* SK_RESTRICT dst = fDevice->getAddr32(x, y);
	const uint16_t* SK_RESTRICT src = fSource->getAddr16(x - fLeft, y - fTop);
	size_t dstRB = fDevice->rowBytes();
	size_t srcRB = fSource->rowBytes();
	Proc_S16_D32 proc = fProc32;
	U8CPU  alpha = fAlpha;

	do {
		proc(dst, src, width, alpha);
		dst = (uint32_t* SK_RESTRICT)((char*)dst + dstRB);
		src = (const uint16_t* SK_RESTRICT)((const char*)src + srcRB);
	} while (--height != 0);
}


