
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkSpriteBlitter_S16_D32_DEFINED
#define SkSpriteBlitter_S16_D32_DEFINED

#include "SkSpriteBlitter.h"

class Sprite_D32_S16 : public SkSpriteBlitter {
public:
    Sprite_D32_S16(const SkBitmap& src, U8CPU alpha);
    virtual void blitRect(int x, int y, int width, int height);

private:
    typedef void (*Proc_S16_D32)(uint32_t* dst, const uint16_t* src, int count, U8CPU alpha);

    Proc_S16_D32   	fProc32;
    U8CPU           fAlpha;

    typedef SkSpriteBlitter INHERITED;
};

#endif /* SkSpriteBlitter_S16_D32_DEFINED */
