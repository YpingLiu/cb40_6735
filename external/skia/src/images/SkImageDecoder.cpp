/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "SkImageDecoder.h"
#include "SkBitmap.h"
#include "SkImagePriv.h"
#include "SkPixelRef.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "SkCanvas.h"
#include "SkImageInfo.h"
#include <stdio.h>


#ifdef MTK_89DISPLAY_ENHANCEMENT_SUPPORT
#include "DpBlitStream.h"
#endif

#include <cutils/properties.h>
#include <cutils/xlog.h>

#define LOG_TAG "skia"

#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#include <utils/Trace.h>

SkImageDecoder::SkImageDecoder()
    : fPeeker(NULL)
#ifdef SK_SUPPORT_LEGACY_IMAGEDECODER_CHOOSER
    , fChooser(NULL)
#endif
    , fAllocator(NULL)
    , fSampleSize(1)
    , fDefaultPref(kUnknown_SkColorType)
    , fDitherImage(true)
#ifdef SK_SUPPORT_LEGACY_BITMAP_CONFIG
    , fUsePrefTable(false)
#endif
    , fSkipWritingZeroes(false)
    , fPreferQualityOverSpeed(false)
    , fRequireUnpremultipliedColors(false) 
    , fIsAllowMultiThreadRegionDecode(0) 
    , fPreferSize(0)
    , fPostProc(0) 
    , fdc(NULL) {
}

SkImageDecoder::~SkImageDecoder() {
    SkSafeUnref(fPeeker);
#ifdef SK_SUPPORT_LEGACY_IMAGEDECODER_CHOOSER
    SkSafeUnref(fChooser);
#endif
    SkSafeUnref(fAllocator);
}

void SkImageDecoder::copyFieldsToOther(SkImageDecoder* other) {
    if (NULL == other) {
        return;
    }
    other->setPeeker(fPeeker);
#ifdef SK_SUPPORT_LEGACY_IMAGEDECODER_CHOOSER
    other->setChooser(fChooser);
#endif
    other->setAllocator(fAllocator);
    other->setSampleSize(fSampleSize);
#ifdef SK_SUPPORT_LEGACY_BITMAP_CONFIG
    if (fUsePrefTable) {
        other->setPrefConfigTable(fPrefTable);
    } else {
        other->fDefaultPref = fDefaultPref;
    }
#endif
    other->setDitherImage(fDitherImage);
    other->setSkipWritingZeroes(fSkipWritingZeroes);
    other->setPreferQualityOverSpeed(fPreferQualityOverSpeed);
    other->setRequireUnpremultipliedColors(fRequireUnpremultipliedColors);
}

SkImageDecoder::Format SkImageDecoder::getFormat() const {
    return kUnknown_Format;
}

const char* SkImageDecoder::getFormatName() const {
    return GetFormatName(this->getFormat());
}

const char* SkImageDecoder::GetFormatName(Format format) {
    switch (format) {
        case kUnknown_Format:
            return "Unknown Format";
        case kBMP_Format:
            return "BMP";
        case kGIF_Format:
            return "GIF";
        case kICO_Format:
            return "ICO";
        case kPKM_Format:
            return "PKM";
        case kKTX_Format:
            return "KTX";
        case kJPEG_Format:
            return "JPEG";
        case kPNG_Format:
            return "PNG";
        case kWBMP_Format:
            return "WBMP";
        case kWEBP_Format:
            return "WEBP";
        default:
            SkDEBUGFAIL("Invalid format type!");
    }
    return "Unknown Format";
}

SkImageDecoder::Peeker* SkImageDecoder::setPeeker(Peeker* peeker) {
    SkRefCnt_SafeAssign(fPeeker, peeker);
    return peeker;
}

#ifdef SK_SUPPORT_LEGACY_IMAGEDECODER_CHOOSER
SkImageDecoder::Chooser* SkImageDecoder::setChooser(Chooser* chooser) {
    SkRefCnt_SafeAssign(fChooser, chooser);
    return chooser;
}
#endif

SkBitmap::Allocator* SkImageDecoder::setAllocator(SkBitmap::Allocator* alloc) {
    SkRefCnt_SafeAssign(fAllocator, alloc);
    return alloc;
}

void SkImageDecoder::setSampleSize(int size) {
    if (size < 1) {
        size = 1;
    }
    fSampleSize = size;
}

#ifdef MTK_JPEG_ImageDecoder
void SkImageDecoder::setPreferSize(int size) {
    if (size < 0) {
        size = 0;
    }
    fPreferSize = size;
}

void SkImageDecoder::setPostProcFlag(int flag) {
    fPostProc = flag;
}
#endif

#ifdef MTK_IMAGE_DC_SUPPORT 
void SkImageDecoder::setDynamicCon(void* pointer, int size) {
    fdc = pointer;
	fsize = size;
	//XLOGD("setDynamicCon fsize=%d", fsize);
}
#endif

#ifdef SK_SUPPORT_LEGACY_IMAGEDECODER_CHOOSER
// TODO: change Chooser virtual to take colorType, so we can stop calling SkColorTypeToBitmapConfig
//
bool SkImageDecoder::chooseFromOneChoice(SkColorType colorType, int width, int height) const {
    Chooser* chooser = fChooser;
    
    if (NULL == chooser) {    // no chooser, we just say YES to decoding :)
        return true;
    }
    chooser->begin(1);
    chooser->inspect(0, SkColorTypeToBitmapConfig(colorType), width, height);
    return chooser->choose() == 0;
}
#endif

bool SkImageDecoder::allocPixelRef(SkBitmap* bitmap,
                                   SkColorTable* ctable) const {
    return bitmap->allocPixels(fAllocator, ctable);
}

///////////////////////////////////////////////////////////////////////////////

#ifdef SK_SUPPORT_LEGACY_BITMAP_CONFIG
void SkImageDecoder::setPrefConfigTable(const PrefConfigTable& prefTable) {
    fUsePrefTable = true;
    fPrefTable = prefTable;
}
#endif

// TODO: use colortype in fPrefTable, fDefaultPref so we can stop using SkBitmapConfigToColorType()
//
SkColorType SkImageDecoder::getPrefColorType(SrcDepth srcDepth, bool srcHasAlpha) const {
    SkColorType ct = fDefaultPref;
#ifdef SK_SUPPORT_LEGACY_BITMAP_CONFIG

    if (fUsePrefTable) {
        // Until we kill or change the PrefTable, we have to go into Config land for a moment.
        SkBitmap::Config config = SkBitmap::kNo_Config;
        switch (srcDepth) {
            case kIndex_SrcDepth:
                config = srcHasAlpha ? fPrefTable.fPrefFor_8Index_YesAlpha_src
                                     : fPrefTable.fPrefFor_8Index_NoAlpha_src;
                break;
            case k8BitGray_SrcDepth:
                config = fPrefTable.fPrefFor_8Gray_src;
                break;
            case k32Bit_SrcDepth:
                config = srcHasAlpha ? fPrefTable.fPrefFor_8bpc_YesAlpha_src
                                     : fPrefTable.fPrefFor_8bpc_NoAlpha_src;
                break;
        }
        // now return to SkColorType land
        ct = SkBitmapConfigToColorType(config);
    }
#endif
    return ct;
}

#ifdef MTK_89DISPLAY_ENHANCEMENT_SUPPORT
void PostProcess(SkImageDecoder * decoder, SkBitmap* bm) {
    unsigned long u4TimeOut = 0;
    unsigned long u4PQOpt;
    unsigned long u4Flag = 0;
    unsigned long *u4RGB888pointer;
    unsigned int  *u4RGB565pointer;
    unsigned long bkPoints[8];
    
    char value[PROPERTY_VALUE_MAX];
    property_get("persist.PQ", value, "1");

    u4PQOpt = atol(value);

    if((NULL == decoder) || (NULL == bm)) {
        XLOGD("decoder or bm is null\n");
        return ;
    }


    if(0 == u4PQOpt) {
        return ;
    }

    u4Flag = decoder->getPostProcFlag();
    if(0 == (0x1 & u4Flag)) {
        XLOGD("Flag is not 1%x" , u4Flag);
        return ;
    }

    if((SkImageDecoder::kPNG_Format == decoder->getFormat()) && (0 == (u4Flag >> 4))) {
        XLOGD("PNG , and flag does not force to do PQ Sharpeness %x" , u4Flag);
        return ;
    }
    
    if(bm->width()>1280) {
        XLOGD("Cannot support PQ Sharpeness when picture width %d > 1280 \n" , bm->width());
        return ;
    }

    bm->lockPixels();

    if(NULL == bm->getPixels()) {
        XLOGD("bm does not get any pixels\n");
        goto NULL_EXIT;
    }

    if ((bm->colorType() == kRGB_565_SkColorType) || (bm->colorType() == kN32_SkColorType)) {
        DpColorFormat fmt;
        
        switch(bm->config()) {
            case kN32_SkColorType:
                fmt = eARGB8888;
                if(bm->width()%2==1) {     
                   u4RGB888pointer = (unsigned long *)bm->getPixels();
                   bkPoints[0] = *(u4RGB888pointer + (bm->width()*bm->height()-1));
                   bkPoints[1] = *(u4RGB888pointer + (bm->width()*bm->height()-2));
                   bkPoints[2] = *(u4RGB888pointer + (bm->width()*bm->height()-3));
                   bkPoints[3] = *(u4RGB888pointer + (bm->width()*bm->height()-4));
                   bkPoints[4] = *(u4RGB888pointer + (bm->width()*(bm->height()-1)-1));
                   bkPoints[5] = *(u4RGB888pointer + (bm->width()*(bm->height()-1)-2));
                   bkPoints[6] = *(u4RGB888pointer + (bm->width()*(bm->height()-1)-3));
                   bkPoints[7] = *(u4RGB888pointer + (bm->width()*(bm->height()-1)-4));
                }
                break;
            case kRGB_565_SkColorType:
                fmt = eRGB565;
                if(bm->width()%2==1) {     
                    u4RGB565pointer = (unsigned int *)bm->getPixels();
                    bkPoints[0] = (unsigned long)*(u4RGB565pointer + (bm->width()*bm->height()-1));
                    bkPoints[1] = (unsigned long)*(u4RGB565pointer + (bm->width()*bm->height()-2));
                    bkPoints[2] = (unsigned long)*(u4RGB565pointer + (bm->width()*bm->height()-3));
                    bkPoints[3] = (unsigned long)*(u4RGB565pointer + (bm->width()*bm->height()-4));
                    bkPoints[4] = (unsigned long)*(u4RGB565pointer + (bm->width()*(bm->height()-1)-1));
                    bkPoints[5] = (unsigned long)*(u4RGB565pointer + (bm->width()*(bm->height()-1)-2));
                    bkPoints[6] = (unsigned long)*(u4RGB565pointer + (bm->width()*(bm->height()-1)-3));
                    bkPoints[7] = (unsigned long)*(u4RGB565pointer + (bm->width()*(bm->height()-1)-4));
                }  
                break;
            default :
                goto NULL_EXIT;
            break;
        }

        XLOGD("Image Processing %d %d\n",bm->width(),bm->height());
        
        ////////////////////////////////////////////////////////////////////
        
        if(5 == u4PQOpt) {                    
        	XLOGE("Output Pre-EE Result...\n");       
        	FILE *fp;          
        	fp = fopen("/sdcard/testOri.888", "w");
        	
        	if(fp!=NULL) {
        		fwrite(bm->getPixels(),1,bm->getSize(),fp);
        	} else {
        		XLOGE("Output Pre-EE Result fail !\n");
        	}
			
        	fclose(fp);
        	XLOGE("Output Pre-EE Result Done!\n");
      	}
        
        DpBlitStream* stream = 0;
    
        stream = new DpBlitStream();

        stream->setSrcBuffer(bm->getPixels(), bm->getSize());
        stream->setSrcConfig(bm->width(), bm->height(), fmt);
        stream->setDstBuffer(bm->getPixels(), bm->getSize());
        stream->setDstConfig(bm->width(), bm->height(), fmt);

        stream->setRotate(0);
        stream->setTdshp(1);
        
        if(!stream->invalidate()) {
            XLOGE("TDSHP Bitblt Stream Failed!\n");
        }
        
        if((bm->width()%2==1) && fmt == eARGB8888) {
            XLOGD("6589 PQ EE Workaround at odd width picture\n");
            *(u4RGB888pointer + (bm->width()*bm->height()-1))      = bkPoints[0];
            *(u4RGB888pointer + (bm->width()*bm->height()-2))      = bkPoints[1];
            *(u4RGB888pointer + (bm->width()*bm->height()-3))      = bkPoints[2];
            *(u4RGB888pointer + (bm->width()*bm->height()-4))      = bkPoints[3];
            *(u4RGB888pointer + (bm->width()*(bm->height()-1)-1))  = bkPoints[4];
            *(u4RGB888pointer + (bm->width()*(bm->height()-1)-2))  = bkPoints[5];
            *(u4RGB888pointer + (bm->width()*(bm->height()-1)-3))  = bkPoints[6];
            *(u4RGB888pointer + (bm->width()*(bm->height()-1)-4))  = bkPoints[7];
        } else if((bm->width()%2==1) && fmt == eRGB565) {          
            XLOGD("6589 PQ EE Workaround at odd width picture\n");
            *(u4RGB565pointer + (bm->width()*bm->height()-1))      = bkPoints[0];
            *(u4RGB565pointer + (bm->width()*bm->height()-2))      = bkPoints[1];
            *(u4RGB565pointer + (bm->width()*bm->height()-3))      = bkPoints[2];
            *(u4RGB565pointer + (bm->width()*bm->height()-4))      = bkPoints[3];
            *(u4RGB565pointer + (bm->width()*(bm->height()-1)-1))  = bkPoints[4];
            *(u4RGB565pointer + (bm->width()*(bm->height()-1)-2))  = bkPoints[5];
            *(u4RGB565pointer + (bm->width()*(bm->height()-1)-3))  = bkPoints[6];
            *(u4RGB565pointer + (bm->width()*(bm->height()-1)-4))  = bkPoints[7];
        }   
            
            
        if(5 == u4PQOpt) {                    
        	XLOGE("Output EE Result...\n");       
        	FILE *fp;          
        	fp = fopen("/sdcard/test.888", "w");
        	
        	if(fp!=NULL) {
        		fwrite(bm->getPixels(),1,bm->getSize(),fp);
        	} else {
        		XLOGE("Output EE Result fail !\n");
        	}
			
        	fclose(fp);
        	XLOGE("Output EE Result Done!\n");
      	}

        delete stream;

    }

    XLOGD("Image Process Done\n");

NULL_EXIT:
    bm->unlockPixels();

    return ;
}

#endif


SkImageDecoder::Result SkImageDecoder::decode(SkStream* stream, SkBitmap* bm, SkColorType pref, Mode mode) {
    ATRACE_CALL();

	// we reset this to false before calling onDecode
    fShouldCancelDecode = false;
    // assign this, for use by getPrefColorType(), in case fUsePrefTable is false
    fDefaultPref = pref;

    // pass a temporary bitmap, so that if we return false, we are assured of
    // leaving the caller's bitmap untouched.
    SkBitmap    tmp;
	if (this->getFormat() != kPNG_Format) {
		MtkSkDebugf("onDecode start stream=%p,bm=%p,pref=%d,mode=%d,format=%s\n",
				stream, bm, pref, mode, this->getFormatName());
	}
    const Result result = this->onDecode(stream, &tmp, mode);	
    if (kFailure == result) {
        return kFailure;
    }
			
	if (this->getFormat() != kPNG_Format) {
		MtkSkDebugf("onDecode return true,format=%s\n",this->getFormatName());
	}		
#ifdef MTK_89DISPLAY_ENHANCEMENT_SUPPORT
    PostProcess(this , &tmp);
#endif	
	
    bm->swap(tmp);
    return result;
}

bool SkImageDecoder::decodeSubset(SkBitmap* bm, const SkIRect& rect, SkColorType pref) {
    // we reset this to false before calling onDecodeSubset
    fShouldCancelDecode = false;
    // assign this, for use by getPrefColorType(), in case fUsePrefTable is false
    fDefaultPref = pref;
    MtkSkDebugf("onDecodeSubset,bm=%p,pref=%d,format=%s\n", bm, pref,this->getFormatName());
    if (! this->onDecodeSubset(bm, rect)) {
        return false;
    }

#ifdef MTK_89DISPLAY_ENHANCEMENT_SUPPORT
    PostProcess(this , bm);
#endif
    MtkSkDebugf("decodeSubset %s End,return true",this->getFormatName());
    return true;
}

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
bool SkImageDecoder::decodeSubset(SkBitmap* bm, const SkIRect& rect, SkColorType pref, 
                                  int sampleSize, void* fdc) {
    // we reset this to false before calling onDecodeSubset
    fShouldCancelDecode = false;
    // assign this, for use by getPrefColorType(), in case fUsePrefTable is false
    fDefaultPref = pref;
    MtkSkDebugf("multi onDecodeSubset,bm=%p,pref=%d,format=%s\n", bm, pref,this->getFormatName());
    
#ifdef MTK_IMAGE_DC_SUPPORT
    if (! this->onDecodeSubset(bm, rect, sampleSize, fdc)) {
        return false;
    }
#else
    if (! this->onDecodeSubset(bm, rect, sampleSize, NULL)) {
        return false;
    }
#endif

#ifdef MTK_89DISPLAY_ENHANCEMENT_SUPPORT
    PostProcess(this , bm);
#endif
    MtkSkDebugf("multi decodeSubset %s End,return true",this->getFormatName());
    return true;
}
#endif  //MTK_SKIA_MULTI_THREAD_JPEG_REGION


bool SkImageDecoder::buildTileIndex(SkStreamRewindable* stream, int *width, int *height) {
    // we reset this to false before calling onBuildTileIndex
    fShouldCancelDecode = false;

    return this->onBuildTileIndex(stream, width, height);
}

bool SkImageDecoder::cropBitmap(SkBitmap *dst, SkBitmap *src, int sampleSize,
                                int dstX, int dstY, int width, int height,
                                int srcX, int srcY) {
    int w = width / sampleSize;
    int h = height / sampleSize;

    // if the destination has no pixels then we must allocate them.
    if(sampleSize > 1 && width > 0 && w == 0) {
		XLOGW("Skia::cropBitmap W/H %d %d->%d %d, Sample %d, force width != 0 !!!!!!\n", width, height,w, h, sampleSize );
		w = 1;
    }
    if(sampleSize > 1 && height > 0 && h == 0) {
		XLOGW("Skia::cropBitmap W/H %d %d->%d %d, Sample %d, force height != 0 !!!!!!\n", width, height,w, h, sampleSize );
		h = 1;
    }	

    if (src->colorType() == kIndex_8_SkColorType) {
        // kIndex8 does not allow drawing via an SkCanvas, as is done below.
        // Instead, use extractSubset. Note that this shares the SkPixelRef and
        // SkColorTable.
        // FIXME: Since src is discarded in practice, this holds on to more
        // pixels than is strictly necessary. Switch to a copy if memory
        // savings are more important than speed here. This also means
        // that the pixels in dst can not be reused (though there is no
        // allocation, which was already done on src).
        int x = (dstX - srcX) / sampleSize;
        int y = (dstY - srcY) / sampleSize;
        SkIRect subset = SkIRect::MakeXYWH(x, y, w, h);
        return src->extractSubset(dst, subset);
    }
    // if the destination has no pixels then we must allocate them.
    if (dst->isNull()) {
        dst->setInfo(src->info().makeWH(w, h));

        if (!this->allocPixelRef(dst, NULL)) {
            SkDEBUGF(("failed to allocate pixels needed to crop the bitmap"));
            return false;
        }
    }
    // check to see if the destination is large enough to decode the desired
    // region. If this assert fails we will just draw as much of the source
    // into the destination that we can.
    if (dst->width() < w || dst->height() < h) {
        SkDEBUGF(("SkImageDecoder::cropBitmap does not have a large enough bitmap.\n"));
    }

    // Set the Src_Mode for the paint to prevent transparency issue in the
    // dest in the event that the dest was being re-used.
    SkPaint paint;
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);

    SkCanvas canvas(*dst);
    canvas.drawSprite(*src, (srcX - dstX) / sampleSize,
                            (srcY - dstY) / sampleSize,
                            &paint);
    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool SkImageDecoder::DecodeFile(const char file[], SkBitmap* bm, SkColorType pref,  Mode mode,
                                Format* format) {
    SkASSERT(file);
    SkASSERT(bm);

    SkAutoTUnref<SkStreamRewindable> stream(SkStream::NewFromFile(file));
    if (stream.get()) {
        if (SkImageDecoder::DecodeStream(stream, bm, pref, mode, format)) {
            bm->pixelRef()->setURI(file);
            return true;
        }
    }
    return false;
}

bool SkImageDecoder::DecodeMemory(const void* buffer, size_t size, SkBitmap* bm, SkColorType pref,
                                  Mode mode, Format* format) {
    if (0 == size) {
        return false;
    }
    SkASSERT(buffer);

    SkMemoryStream  stream(buffer, size);
    return SkImageDecoder::DecodeStream(&stream, bm, pref, mode, format);
}

bool SkImageDecoder::DecodeStream(SkStreamRewindable* stream, SkBitmap* bm, SkColorType pref,
                                  Mode mode, Format* format) {
    SkASSERT(stream);
    SkASSERT(bm);

    bool success = false;
    SkImageDecoder* codec = SkImageDecoder::Factory(stream);

    if (NULL != codec) {
        success = codec->decode(stream, bm, pref, mode);
        if (success && format) {
            *format = codec->getFormat();
            if (kUnknown_Format == *format) {
                if (stream->rewind()) {
                    *format = GetStreamFormat(stream);
                }
            }
        }
        delete codec;
    }
    return success;
}
