/*
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */
 /*
 * Copyright 2007 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "SkImageDecoder.h"
#include "SkImageEncoder.h"
#include "SkJpegUtility.h"
#include "SkColorPriv.h"
#include "SkDither.h"
#include "SkScaledBitmapSampler.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "SkTime.h"
#include "SkUtils.h"
#include "SkRTConf.h"
#include "SkRect.h"
#include "SkCanvas.h"

#include <sys/mman.h>
#include <cutils/ashmem.h>

#ifdef USE_MTK_ALMK
  #include "almk_hal.h"
#endif 
#ifdef MTK_JPEG_HW_DECODER
#include "MediaHal.h"
#include "DpBlitStream.h" 
#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#include "Trace.h"
#endif

#include <stdio.h>
extern "C" {
    #include "jpeglib.h"
    #include "jerror.h"
    #include "jpegint.h"
}

#ifdef SK_BUILD_FOR_ANDROID
#include <cutils/properties.h>
#include <cutils/xlog.h>

#undef LOG_TAG
#define LOG_TAG "skia" 
// Key to lookup the size of memory buffer set in system property
static const char KEY_MEM_CAP[] = "ro.media.dec.jpeg.memcap";
#endif

// These enable timing code that report milliseconds for an encoding/decoding
//#define TIME_ENCODE
//#define TIME_DECODE

// this enables our rgb->yuv code, which is faster than libjpeg on ARM
#define WE_CONVERT_TO_YUV
// the avoid the reset error when switch hardware to software codec
#define MAX_HEADER_SIZE 64 * 1024
// the limitation of memory to use hardware resize
#define HW_RESIZE_MAX_PIXELS 25 * 1024 * 1024
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


static SkMutex  gAutoTileInitMutex;
static SkMutex  gAutoTileResizeMutex;

#define CHECK_LARGE_JPEG_PROG
#define JPEG_PROG_LIMITATION_SIZE MTK_MAX_SRC_JPEG_PROG_PIXELS

#define USE_SKJPGSTREAM 


// If ANDROID_RGB is defined by in the jpeg headers it indicates that jpeg offers
// support for two additional formats (1) JCS_RGBA_8888 and (2) JCS_RGB_565.

#if defined(SK_DEBUG)
#define DEFAULT_FOR_SUPPRESS_JPEG_IMAGE_DECODER_WARNINGS false
#define DEFAULT_FOR_SUPPRESS_JPEG_IMAGE_DECODER_ERRORS false
#else  // !defined(SK_DEBUG)
#define DEFAULT_FOR_SUPPRESS_JPEG_IMAGE_DECODER_WARNINGS true
#define DEFAULT_FOR_SUPPRESS_JPEG_IMAGE_DECODER_ERRORS true
#endif  // defined(SK_DEBUG)
SK_CONF_DECLARE(bool, c_suppressJPEGImageDecoderWarnings,
                "images.jpeg.suppressDecoderWarnings",
                DEFAULT_FOR_SUPPRESS_JPEG_IMAGE_DECODER_WARNINGS,
                "Suppress most JPG warnings when calling decode functions.");
SK_CONF_DECLARE(bool, c_suppressJPEGImageDecoderErrors,
                "images.jpeg.suppressDecoderErrors",
                DEFAULT_FOR_SUPPRESS_JPEG_IMAGE_DECODER_ERRORS,
                "Suppress most JPG error messages when decode "
                "function fails.");

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void overwrite_mem_buffer_size(jpeg_decompress_struct* cinfo) {
#ifdef SK_BUILD_FOR_ANDROID
    /* Check if the device indicates that it has a large amount of system memory
     * if so, increase the memory allocation to 30MB instead of the default 5MB.
     */
#ifdef ANDROID_LARGE_MEMORY_DEVICE
    cinfo->mem->max_memory_to_use = 30 * 1024 * 1024;
#else
    cinfo->mem->max_memory_to_use = 5 * 1024 * 1024;
#endif
#endif // SK_BUILD_FOR_ANDROID
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void do_nothing_emit_message(jpeg_common_struct*, int) {
    /* do nothing */
}
static void do_nothing_output_message(j_common_ptr) {
    /* do nothing */
}

static void initialize_info(jpeg_decompress_struct* cinfo, skjpeg_source_mgr* src_mgr) {
    SkASSERT(cinfo != NULL);
    SkASSERT(src_mgr != NULL);
    jpeg_create_decompress(cinfo);
    overwrite_mem_buffer_size(cinfo);
    cinfo->src = src_mgr;
    /* To suppress warnings with a SK_DEBUG binary, set the
     * environment variable "skia_images_jpeg_suppressDecoderWarnings"
     * to "true".  Inside a program that links to skia:
     * SK_CONF_SET("images.jpeg.suppressDecoderWarnings", true); */
    if (c_suppressJPEGImageDecoderWarnings) {
        cinfo->err->emit_message = &do_nothing_emit_message;
    }
    /* To suppress error messages with a SK_DEBUG binary, set the
     * environment variable "skia_images_jpeg_suppressDecoderErrors"
     * to "true".  Inside a program that links to skia:
     * SK_CONF_SET("images.jpeg.suppressDecoderErrors", true); */
    if (c_suppressJPEGImageDecoderErrors) {
        cinfo->err->output_message = &do_nothing_output_message;
    }
}



#ifdef USE_SKJPGSTREAM

class SkJpgStream : public SkStream {

public:

    SkJpgStream(void *hw_buffer, size_t hw_buffer_size, SkStream* Src){
        //XLOGD("SkJpgStream::SkJpgStream %x, %x, %x!!\n", (unsigned int) hw_buffer, hw_buffer_size, (unsigned int)Src);
        srcStream = Src ;
        hwInputBuf = hw_buffer;
        hwInputBufSize = hw_buffer_size;
        total_read_size = 0;
    }

    virtual ~SkJpgStream(){
        //SkDebugf("SkJpgStream::~SkJpgStream!!\n");		
    }

    virtual bool rewind(){
        //SkDebugf("SkJpgStream::rewind, readSize %x, hwBuffSize %x!!\n",   total_read_size, hwInputBufSize);				
        if(total_read_size >= hwInputBufSize)
        {
            return false;
        }
        else if (total_read_size < hwInputBufSize)
        {
            total_read_size = 0;
        }
        return true ;
    }

    virtual bool isAtEnd() const {
        return false;
    }

    virtual size_t read(void* buffer, size_t size){
    size_t read_start = total_read_size;
    size_t read_end = total_read_size + size ;
    size_t size_1 = 0;
    size_t size_2 = 0;
    size_t real_size_2 = 0;

    //SkDebugf("SkJpgStream::read, buf %x, size %x, tSize %x, st %x, end %x, HWsize %x!!\n", (unsigned int)buffer, (unsigned int) size
    //, total_read_size, read_start, read_end, hwInputBufSize);  

    if (buffer == NULL && size == 0){	// special signature, they want the total size
      fSize = hwInputBufSize ;
      if(srcStream) fSize += srcStream->getLength();
      return fSize;
    }else if(size == 0){
      return 0;
    }

    // if buffer is NULL, seek ahead by size

    if( read_start <= hwInputBufSize && read_end <= hwInputBufSize)
    {
        if (buffer) 
        {
            memcpy(buffer, (const char*)hwInputBuf + read_start, size);
        }
        total_read_size += size ;
        //SkDebugf("SkJpgStream::read(HW), size %x, total_size %x!!\n", size, total_read_size);			   		  					  			
        return size ;
    }
    else if ( read_start >= hwInputBufSize  )
    {
        if(srcStream) real_size_2 += srcStream->read(buffer, size);
        total_read_size += real_size_2 ;
        //SkDebugf("SkJpgStream::read(Stream), size_2 %x, real_size %x(%x), total_size %x!!\n", size, real_size_2, srcStream->getLength(),total_read_size);
        return real_size_2;
    }
    else
    {
        size_1 = hwInputBufSize - read_start ;
        size_2 = read_end - hwInputBufSize  ;	
        if (buffer) {
            memcpy(buffer, (const char*)hwInputBuf + read_start, size_1);
        }
        total_read_size += size_1 ;
        if(srcStream && buffer) real_size_2 += srcStream->read((void *)((unsigned char *)buffer+size_1), size_2);
        total_read_size += real_size_2 ;
        //SkDebugf("SkJpgStream::read(HW+Stream), buf %x, size_2 %x, real_size %x(%x), total_size %x!!\n", buffer+size_1, size_2, real_size_2, srcStream->getLength(),total_read_size);  
        return size_1+ real_size_2 ;
    }
    }

    bool seek(size_t position){
        //SkDebugf("SkJpgStream::seek size %x!!\n", offset);			
        return false;
    }
    size_t skip(size_t size)
    {
       /*Check for size == 0, and just return 0. If we passed that
           to read(), it would interpret it as a request for the entire
           size of the stream.
           */
        //SkDebugf("SkJpgStream::skip %x!!\n", size);					
        return size ? this->read(NULL, size) : 0;
    }

private:
    size_t total_read_size ;
    SkStream* srcStream;
    void *hwInputBuf ;
    size_t hwInputBufSize ; 
    size_t fSize;

};


#define DUMP_DEC_SKIA_LVL_MCU 1
#define DUMP_DEC_SKIA_LVL_IDCT 2
#define DUMP_DEC_SKIA_LVL_COLOR 3

int mtkDumpBuf2file(unsigned int level, const char filename[], unsigned int index, unsigned char *SrcAddr, unsigned int size, unsigned int width, unsigned int height)
{
   
   FILE *fp = NULL;
   FILE *fpEn = NULL;
   unsigned char* cptr ;
   const char tag[64] = "DUMP_LIBJPEG";
   char filepath[128];
   char dumpEn[128] ;
   struct timeval t1;

#if 0 //ndef ENABLE_IMG_CODEC_DUMP_RAW
   return false ;
#endif
   
   gettimeofday(&t1, NULL);
   sprintf(  dumpEn, "//data//otis//%s_%d", tag, level);   
   //if(level == DUMP_DEC_SKIA_LVL_SRC)
   //  sprintf(filepath, "//data//otis//%s_%04d_%u_%d_%d.jpg", filename, index, (unsigned int)t1.tv_usec, width, height );   
   //else
     sprintf(filepath, "//data//otis//%s_%04d_%u_%d_%d.raw", filename, index, (unsigned int)t1.tv_usec, width, height );   
     
     
   fpEn = fopen(dumpEn, "r");
   if(fpEn == NULL)
   {
       //XLOGW("Check Dump En is zero!!\n");
       return false;
   }
   fclose(fpEn);
      
   fp = fopen(filepath, "w+");
   if (fp == NULL)
   {
       XLOGW("open Dump file fail: %s\n", filepath);
       return false;
   }

   //XLOGW("DumpRaw -> %s, En %s, addr %x, size %x !!\n", filepath,dumpEn,(unsigned int)SrcAddr, size);                     
   cptr = (unsigned char*)SrcAddr ;
   for( unsigned int i=0;i<size;i++){  /* total size in comp */
     fprintf(fp,"%c", *cptr );  
     cptr++;
   }          
   
   fclose(fp); 
   //*index++;
   
   return true ;       
}

#define MAX_LIBJPEG_AUTO_NUM 32
class JpgLibAutoClean {
public:
    JpgLibAutoClean(): idx(-1) {}
    ~JpgLibAutoClean() {
      int i ;
        for( i = idx ; i>=0 ; i-- ){
          if (ptr[i]) {
              //XLOGW("JpgLibAutoClean: idx %d, clear %x!!\n", i, ptr[i]);
              if(dump[i]) mtkDumpBuf2file(dump[i], "mtkLibJpegRegionIDCT", dump_type[i], (unsigned char *)ptr[i], dump_size[i], dump_w[i], dump_h[i]) ;
              free(ptr[i]);
          }
        }
    }
    void set(void* s) {
        idx ++ ;
        ptr[idx] = s;
        dump[idx] = 0;
        //XLOGW("JpgLibAutoClean: set idx %d, ptr %x!!\n", idx, ptr[idx]);
        
    }
    void setDump(unsigned int dumpStage, unsigned int type,unsigned int size, unsigned int w, unsigned int h){
        dump[idx] = dumpStage ;
        dump_type[idx] = type ;
        dump_size[idx] = size ;
        dump_w[idx] = w ;
        dump_h[idx] = h ;
    }
private:
    void* ptr[MAX_LIBJPEG_AUTO_NUM];
    int idx ;
    unsigned int dump[MAX_LIBJPEG_AUTO_NUM] ;
    unsigned int dump_type[MAX_LIBJPEG_AUTO_NUM] ;
    unsigned int dump_size[MAX_LIBJPEG_AUTO_NUM] ;
    unsigned int dump_w[MAX_LIBJPEG_AUTO_NUM] ;
    unsigned int dump_h[MAX_LIBJPEG_AUTO_NUM] ;
    
};


class JpgStreamAutoClean {
public:
    JpgStreamAutoClean(): ptr(NULL) {}
    ~JpgStreamAutoClean() {
        if (ptr) {
            delete ptr;
        }
    }
    void set(SkStream* s) {
        ptr = s;
    }
private:
    SkStream* ptr;
};

#endif


#ifdef SK_BUILD_FOR_ANDROID
class SkJPEGImageIndex {
public:
    SkJPEGImageIndex(SkStreamRewindable* stream, SkImageDecoder* decoder)
        : fSrcMgr(stream, decoder)
        , fInfoInitialized(false)
        , fHuffmanCreated(false)
        , fDecompressStarted(false)
#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION		
        , mtkStream(NULL)
#endif		
        {
            SkDEBUGCODE(fReadHeaderSucceeded = false;)
        }

    ~SkJPEGImageIndex() {
        if (fHuffmanCreated) {
            // Set to false before calling the libjpeg function, in case
            // the libjpeg function calls longjmp. Our setjmp handler may
            // attempt to delete this SkJPEGImageIndex, thus entering this
            // destructor again. Setting fHuffmanCreated to false first
            // prevents an infinite loop.
            fHuffmanCreated = false;
            jpeg_destroy_huffman_index(&fHuffmanIndex);
        }
        if (fDecompressStarted) {
            // Like fHuffmanCreated, set to false before calling libjpeg
            // function to prevent potential infinite loop.
            fDecompressStarted = false;
            jpeg_finish_decompress(&fCInfo);
        }
        if (fInfoInitialized) {
            this->destroyInfo();
        }
#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
        if(mtkStream) delete mtkStream;
#endif				
    }

    /**
     *  Destroy the cinfo struct.
     *  After this call, if a huffman index was already built, it
     *  can be used after calling initializeInfoAndReadHeader
     *  again. Must not be called after startTileDecompress except
     *  in the destructor.
     */
    void destroyInfo() {
        SkASSERT(fInfoInitialized);
        SkASSERT(!fDecompressStarted);
        // Like fHuffmanCreated, set to false before calling libjpeg
        // function to prevent potential infinite loop.
        fInfoInitialized = false;
        jpeg_destroy_decompress(&fCInfo);
        SkDEBUGCODE(fReadHeaderSucceeded = false;)
    }

    /**
     *  Initialize the cinfo struct.
     *  Calls jpeg_create_decompress, makes customizations, and
     *  finally calls jpeg_read_header. Returns true if jpeg_read_header
     *  returns JPEG_HEADER_OK.
     *  If cinfo was already initialized, destroyInfo must be called to
     *  destroy the old one. Must not be called after startTileDecompress.
     */
    bool initializeInfoAndReadHeader() {
        SkASSERT(!fInfoInitialized && !fDecompressStarted);
        initialize_info(&fCInfo, &fSrcMgr);
        fInfoInitialized = true;
        const bool success = (JPEG_HEADER_OK == jpeg_read_header(&fCInfo, true));
        SkDEBUGCODE(fReadHeaderSucceeded = success;)
        return success;
    }

    jpeg_decompress_struct* cinfo() { return &fCInfo; }

    huffman_index* huffmanIndex() { return &fHuffmanIndex; }

    /**
     *  Build the index to be used for tile based decoding.
     *  Must only be called after a successful call to
     *  initializeInfoAndReadHeader and must not be called more
     *  than once.
     */
    bool buildHuffmanIndex() {
        SkASSERT(fReadHeaderSucceeded);
        SkASSERT(!fHuffmanCreated);
        jpeg_create_huffman_index(&fCInfo, &fHuffmanIndex);
        SkASSERT(1 == fCInfo.scale_num && 1 == fCInfo.scale_denom);
        fHuffmanCreated = jpeg_build_huffman_index(&fCInfo, &fHuffmanIndex);
        return fHuffmanCreated;
    }

    /**
     *  Start tile based decoding. Must only be called after a
     *  successful call to buildHuffmanIndex, and must only be
     *  called once.
     */
    bool startTileDecompress() {
        SkASSERT(fHuffmanCreated);
        SkASSERT(fReadHeaderSucceeded);
        SkASSERT(!fDecompressStarted);
        if (jpeg_start_tile_decompress(&fCInfo)) {
            fDecompressStarted = true;
            return true;
        }
        return false;
    }

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
    SkMemoryStream *mtkStream ;
#endif 

private:
    skjpeg_source_mgr  fSrcMgr;
    jpeg_decompress_struct fCInfo;
    huffman_index fHuffmanIndex;
    bool fInfoInitialized;
    bool fHuffmanCreated;
    bool fDecompressStarted;
    SkDEBUGCODE(bool fReadHeaderSucceeded;)
};
#endif

class SkJPEGImageDecoder : public SkImageDecoder {
public:
#ifdef SK_BUILD_FOR_ANDROID
    SkJPEGImageDecoder() {
        fImageIndex = NULL;
        fImageWidth = 0;
        fImageHeight = 0;
    }

    virtual ~SkJPEGImageDecoder() {
        SkDELETE(fImageIndex);
    }
#endif

    virtual Format getFormat() const {
        return kJPEG_Format;
    }

protected:
#ifdef SK_BUILD_FOR_ANDROID
    virtual bool onBuildTileIndex(SkStreamRewindable *stream, int *width, int *height) SK_OVERRIDE;
#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
    virtual bool onDecodeSubset(SkBitmap* bitmap, const SkIRect& rect, int isampleSize, void* pParam = NULL) SK_OVERRIDE;
#endif
    virtual bool onDecodeSubset(SkBitmap* bitmap, const SkIRect& rect) SK_OVERRIDE;
#endif
    virtual Result onDecode(SkStream* stream, SkBitmap* bm, Mode) SK_OVERRIDE;

private:
#ifdef SK_BUILD_FOR_ANDROID
    SkJPEGImageIndex* fImageIndex;
    int fImageWidth;
    int fImageHeight;
#endif

    /**
     *  Determine the appropriate bitmap colortype and out_color_space based on
     *  both the preference of the caller and the jpeg_color_space on the
     *  jpeg_decompress_struct passed in.
     *  Must be called after jpeg_read_header.
     */
    SkColorType getBitmapColorType(jpeg_decompress_struct*);

    typedef SkImageDecoder INHERITED;

#ifdef MTK_JPEG_HW_DECODER
    SkAutoMalloc  fAllocator;
    uint8_t* fSrc;
    uint32_t fSize;
    bool fInitRegion;
    bool fFirstTileDone;
    bool fUseHWResizer;
    bool onDecodeParser(SkBitmap* bm, Mode, uint8_t* srcBuffer, uint32_t srcSize, int srcFD);
    bool onRangeDecodeParser(uint8_t* srcBuffer, uint32_t srcSize, int *width, int *height, bool doRelease);
    bool onDecodeHW(SkBitmap* bm, uint8_t* srcBuffer, uint32_t srcBufSize, uint32_t srcSize, int srcFD);
    bool onDecodeHWRegion(SkBitmap* bm, SkIRect region, uint8_t* srcBuffer, uint32_t srcSize);
    void *fSkJpegDecHandle ;
#endif

    double g_mt_start;
    double g_mt_end;
    double g_mt_end_duration_2;
    double g_mt_hw_sum1;
    double g_mt_hw_sum2;
    int base_thread_id;

};

//////////////////////////////////////////////////////////////////////////

/* Automatically clean up after throwing an exception */
class JPEGAutoClean {
public:
    JPEGAutoClean(): cinfo_ptr(NULL) {}
    ~JPEGAutoClean() {
        if (cinfo_ptr) {
            jpeg_destroy_decompress(cinfo_ptr);
        }
    }
    void set(jpeg_decompress_struct* info) {
        cinfo_ptr = info;
    }
private:
    jpeg_decompress_struct* cinfo_ptr;
};

///////////////////////////////////////////////////////////////////////////////

/*  If we need to better match the request, we might examine the image and
     output dimensions, and determine if the downsampling jpeg provided is
     not sufficient. If so, we can recompute a modified sampleSize value to
     make up the difference.

     To skip this additional scaling, just set sampleSize = 1; below.
 */
static int recompute_sampleSize(int sampleSize,
                                const jpeg_decompress_struct& cinfo) {
    return sampleSize * cinfo.output_width / cinfo.image_width;
}

static bool valid_output_dimensions(const jpeg_decompress_struct& cinfo) {
    /* These are initialized to 0, so if they have non-zero values, we assume
       they are "valid" (i.e. have been computed by libjpeg)
     */
    return 0 != cinfo.output_width && 0 != cinfo.output_height;
}

static bool skip_src_rows(jpeg_decompress_struct* cinfo, void* buffer, int count) {
    for (int i = 0; i < count; i++) {
        JSAMPLE* rowptr = (JSAMPLE*)buffer;
        int row_count = jpeg_read_scanlines(cinfo, &rowptr, 1);
        if (1 != row_count) {
            return false;
        }
    }
    return true;
}

#ifdef SK_BUILD_FOR_ANDROID
static bool skip_src_rows_tile(jpeg_decompress_struct* cinfo,
                               huffman_index *index, void* buffer, int count) {
    for (int i = 0; i < count; i++) {
        JSAMPLE* rowptr = (JSAMPLE*)buffer;
        int row_count = jpeg_read_tile_scanline(cinfo, index, &rowptr);
        if (1 != row_count) {
            return false;
        }
    }
    return true;
}
#endif


#ifdef MTK_JPEG_HW_DECODER

static int roundToTwoPower(int a)
{
    int ans = 1;

    if(a>=8) return a; 

    while (a > 0)
    {
        a = a >> 1;
        ans *= 2;
    }

    return (ans >> 1);
}

static int skjdiv_round_up (unsigned int a, unsigned int b)/* Compute a/b rounded up to next integer, ie, ceil(a/b) *//* Assumes a >= 0, b > 0 */
{  
   return (a + b - 1) / b;
}



bool SkJPEGImageDecoder::onDecodeParser(SkBitmap* bm, Mode mode, uint8_t* srcBuffer, uint32_t srcSize, int srcFD)
{
    int width, height;
    int sampleSize = this->getSampleSize();
    int preferSize = 0;    //this->getPreferSize();
    MHAL_JPEG_DEC_INFO_OUT  outInfo;
    MHAL_JPEG_DEC_SRC_IN    srcInfo;

    unsigned int cinfo_output_width, cinfo_output_height;
    unsigned int re_sampleSize ;
    sampleSize = roundToTwoPower(sampleSize);
    fSkJpegDecHandle = srcInfo.jpgDecHandle = NULL;
     
    
    SkColorType colorType = this->getPrefColorType(k32Bit_SrcDepth, false);
    const SkAlphaType alphaType = kAlpha_8_SkColorType == colorType ?
                                      kPremul_SkAlphaType : kOpaque_SkAlphaType;
    // only these make sense for jpegs
    if (colorType != kN32_SkColorType &&
        colorType != kARGB_4444_SkColorType &&
        colorType != kRGB_565_SkColorType) {
        colorType = kN32_SkColorType;
    }

    if (colorType != kN32_SkColorType &&
        colorType != kRGB_565_SkColorType) {
        XLOGW("HW Not support format: %d\n", colorType);
        return false;
    }
     

    int result ;
    int try_times = 5;
    // parse the file
    do
    {
        try_times++;
        srcInfo.srcBuffer = srcBuffer;
        srcInfo.srcLength = srcSize;
        srcInfo.srcFD = srcFD;
        result = mHalJpeg(MHAL_IOCTL_JPEG_DEC_PARSER, (void *)&srcInfo, sizeof(srcInfo), NULL, 0, NULL);
        if(result == MHAL_INVALID_RESOURCE && try_times < 5)
        {
            XLOGD("onDecodeParser : HW busy ! Sleep 10ms & try again");
            usleep(10 * 1000);
        }
        else if (MHAL_NO_ERROR != result)
        {
            return false;
        }
    } while(result == MHAL_INVALID_RESOURCE && try_times < 5);
    fSkJpegDecHandle = srcInfo.jpgDecHandle ;

    // get file dimension
    outInfo.jpgDecHandle = fSkJpegDecHandle;
    if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_GET_INFO, NULL, 0, 
                                   (void *)&outInfo, sizeof(outInfo), NULL))
    {
        if (SkImageDecoder::kDecodeBounds_Mode != mode) {
            XLOGW("mHalJpeg() - JPEG Decoder false get information !!\n");
        }
        return false;
    }

    if (preferSize == 0)
    {
        if(sampleSize <= 8 ) // 1, 2, 4, 8
        {
            cinfo_output_width = skjdiv_round_up(outInfo.srcWidth, sampleSize);
            cinfo_output_height = skjdiv_round_up(outInfo.srcHeight, sampleSize);
        }
        else //  use 8
        {
            cinfo_output_width = skjdiv_round_up(outInfo.srcWidth, 8);
            cinfo_output_height = skjdiv_round_up(outInfo.srcHeight, 8);
        }

        re_sampleSize = sampleSize * cinfo_output_width / outInfo.srcWidth;

        if( re_sampleSize != 1 )
        {
          unsigned int dx = (re_sampleSize > cinfo_output_width )? cinfo_output_width : re_sampleSize ;
          unsigned int dy = (re_sampleSize > cinfo_output_height )? cinfo_output_height : re_sampleSize ;

          width  = cinfo_output_width / dx;  
          height = cinfo_output_height / dy; 
        }
        else
        {
          width = cinfo_output_width ;
          height = cinfo_output_height ;

        }

#if 0
        width  = outInfo.srcWidth / sampleSize;
        height = outInfo.srcHeight / sampleSize;
        if(outInfo.srcWidth % sampleSize != 0) width++;
        if(outInfo.srcHeight % sampleSize != 0) height++;
#endif
    }
    else
    {
        int maxDimension = (outInfo.srcWidth > outInfo.srcHeight) ? outInfo.srcWidth : outInfo.srcHeight;
        
        if (maxDimension <= preferSize)
        {
            width  = outInfo.srcWidth / sampleSize;
            height = outInfo.srcHeight / sampleSize;
        }
        else if (outInfo.srcWidth > outInfo.srcHeight)
        {
            width = preferSize;
            height = (int)outInfo.srcHeight * width / (int)outInfo.srcWidth;
        }
        else
        {
            height = preferSize;
            width = (int)outInfo.srcWidth * height / (int)outInfo.srcHeight;
        }
    }    
    if( re_sampleSize != 1  )
    XLOGD("onDecodeParser pSize %d, src %d %d, dst %d %d(%d %d), sample %d->%d!\n", preferSize, outInfo.srcWidth, outInfo.srcHeight,
          width, height,cinfo_output_width, cinfo_output_height, sampleSize, re_sampleSize);	

    bm->lockPixels();
    void* rowptr = bm->getPixels();
    bm->unlockPixels();
    bool reuseBitmap = (rowptr != NULL);

    if(reuseBitmap)
    {
        if((bm->width() != width) || (bm->height() != height) || (bm->colorType()!= colorType))
        {
            if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
            {
                XLOGW("Can not release JPEG HW Decoder\n");
                return false;
            }
            XLOGW("Reuse bitmap but dimensions not match\n");
            return false;            
        }
    }
    else 
    {
        bm->setInfo(SkImageInfo::Make(width, height, colorType, alphaType));    }
    
    if (SkImageDecoder::kDecodeBounds_Mode == mode) {
        if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
        {
            XLOGW("Can not release JPEG HW Decoder\n");
            return false;
        }
        return true;    
    //} else if(width <= 128 && height <= 128) {
    //    mHalJpeg(MHAL_IOCTL_JPEG_DEC_START, NULL, 0, NULL, 0, NULL);
    //    return false;
    }

    XLOGD("The file input width: %d, height: %d, output width: %d, height: %d, format: %d, prefer size: %d, dither: %d\n", 
           outInfo.srcWidth, outInfo.srcHeight, width, height, colorType, preferSize, getDitherImage());

    return true;
}

bool SkJPEGImageDecoder::onRangeDecodeParser(uint8_t* srcBuffer, uint32_t srcSize, int *width, int *height, bool doRelease)
{
    MHAL_JPEG_DEC_INFO_OUT outInfo;
    int result ;
    int try_times = 0;
    // parse the file
    do
    {
        try_times++;
        result = mHalJpeg(MHAL_IOCTL_JPEG_DEC_PARSER, (void *)srcBuffer, srcSize, NULL, 0, NULL);
        if(result == MHAL_INVALID_RESOURCE && try_times < 5)
        {
            XLOGD("onRangeDecodeParser : HW busy ! Sleep 100ms & try again");
            usleep(100 * 1000);
        }
        else if (MHAL_NO_ERROR != result)
        {
            return false;
        }
    } while(result == MHAL_INVALID_RESOURCE && try_times < 5);

    // get file dimension
    if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_GET_INFO, NULL, 0, 
                                   (void *)&outInfo, sizeof(outInfo), NULL))
    {
        // SkDebugf("onRangeDecodeParser : get info Error !");
        return false;
    }

    *width = (int)outInfo.srcWidth;
    *height = (int)outInfo.srcHeight;

    if(doRelease)
    {
        if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
        {
            XLOGW("Can not release JPEG HW Decoder\n");
            return false;
        }
    }
    return true;
}

bool SkJPEGImageDecoder::onDecodeHW(SkBitmap* bm, uint8_t* srcBuffer, uint32_t srcBufSize, uint32_t srcSize, int srcFD)
{
    MHAL_JPEG_DEC_START_IN inParams;

#ifdef MTK_6572DISPLAY_ENHANCEMENT_SUPPORT
    unsigned int enTdshp = this->getPostProcFlag();
#endif

    switch (bm->config())
    {
        case SkBitmap::kARGB_8888_Config: 
            inParams.dstFormat = JPEG_OUT_FORMAT_ARGB8888;
            break;

        case SkBitmap::kRGB_565_Config:
            inParams.dstFormat = JPEG_OUT_FORMAT_RGB565;
            break;
        /*    
        case SkBitmap::kYUY2_Pack_Config:
            inParams.dstFormat = JPEG_OUT_FORMAT_YUY2;
            break;
            
        case SkBitmap::kUYVY_Pack_Config:
            inParams.dstFormat = JPEG_OUT_FORMAT_UYVY;
            break;
*/
        default:
            inParams.dstFormat = JPEG_OUT_FORMAT_ARGB8888;
            break;
    }

    bm->lockPixels();
    JSAMPLE* rowptr = (JSAMPLE*)bm->getPixels();
    bool reuseBitmap = (rowptr != NULL);
    bm->unlockPixels();

    if(!reuseBitmap) {
        if (!this->allocPixelRef(bm, NULL)) {
            if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
            {
                XLOGW("Can not release JPEG HW Decoder\n");
                return false;
            }
            return false;
        }
    }

    //inParams.timeout = outInfo.srcWidth * outInfo.srcHeight / 2000;
    //if (inParams.timeout < 100)  inParams.timeout = 100;

    inParams.srcBuffer = srcBuffer;
    inParams.srcBufSize = srcBufSize ;
    inParams.srcLength= srcSize;
    inParams.srcFD = srcFD;
    
    inParams.dstWidth = bm->width();
    inParams.dstHeight = bm->height();
    inParams.dstVirAddr = (UINT8*) bm->getPixels();
    inParams.dstPhysAddr = NULL;

    inParams.doDithering = getDitherImage() ? 1 : 0;
    inParams.doRangeDecode = 0;
    inParams.doPostProcessing = 0;
#ifdef MTK_6572DISPLAY_ENHANCEMENT_SUPPORT    
    inParams.doPostProcessing = enTdshp;
#endif

#ifdef MTK_IMAGE_DC_SUPPORT
    inParams.postProcessingParam = this->getDynamicCon();
#endif


    inParams.PreferQualityOverSpeed = this->getPreferQualityOverSpeed() ;
    inParams.jpgDecHandle = fSkJpegDecHandle ;

    // start decode
    SkAutoLockPixels alp(*bm);
    XLOGW("Skia JPEG HW Decoder trigger!!\n");
    if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_START, 
                                   (void *)&inParams, sizeof(inParams), 
                                   NULL, 0, NULL))
    {
        //SkDebugf("JPEG HW not support this image\n");
        //if(!reuseBitmap)
        //    bm->setPixels(NULL, NULL);
        XLOGW("JPEG HW Decoder return Fail!!\n");
        return false;
    }
    if (reuseBitmap) {
        bm->notifyPixelsChanged();
    }
    XLOGW("JPEG HW Decoder return Successfully!!\n");
    return true;
}

int index_file = 0;
bool store_raw_data(SkBitmap* bm)
{
    FILE *fp;

    char name[150];

    unsigned long u4PQOpt;
    char value[PROPERTY_VALUE_MAX];
    property_get("decode.hw.dump", value, "0");

    u4PQOpt = atol(value);

    if( u4PQOpt == 0) return false;

    if(bm->config() == SkBitmap::kARGB_8888_Config)
        sprintf(name, "/sdcard/dump_%d_%d_%d.888", bm->width(), bm->height(), index_file++);
    else if(bm->config() == SkBitmap::kRGB_565_Config)
        sprintf(name, "/sdcard/dump_%d_%d_%d.565", bm->width(), bm->height(), index_file++);
    else
        return false;

    SkDebugf("store file : %s ", name);


    fp = fopen(name, "wb");
    if(fp == NULL)
    {
        SkDebugf(" open file error ");
        return false;
    }
    if(bm->config() == SkBitmap::kRGB_565_Config)
    {
        fwrite(bm->getPixels(), 1 , bm->getSize(), fp);
        fclose(fp);
        return true;
    }

    unsigned char* addr = (unsigned char*)bm->getPixels();
    SkDebugf("bitmap addr : 0x%x, size : %d ", addr, bm->getSize());
    for(unsigned int i = 0 ; i < bm->getSize() ; i += 4)
    {
        fprintf(fp, "%c", addr[i]);
        fprintf(fp, "%c", addr[i+1]);
        fprintf(fp, "%c", addr[i+2]);
    }
    fclose(fp);
    return true;
        
}

bool SkJPEGImageDecoder::onDecodeHWRegion(SkBitmap* bm, SkIRect region, uint8_t* srcBuffer, uint32_t srcSize )
{
    int width, height, regionWidth, regionHeight;
    int sampleSize = this->getSampleSize();
    MHAL_JPEG_DEC_START_IN inParams;
    MHAL_JPEG_DEC_INFO_OUT outInfo;

    SkColorType colorType = this->getPrefColorType(k32Bit_SrcDepth, false);
    const SkAlphaType alphaType = kAlpha_8_SkColorType == colorType ?
                                      kPremul_SkAlphaType : kOpaque_SkAlphaType;
    // only these make sense for jpeg
    if (colorType != kN32_SkColorType &&
        colorType != kARGB_4444_SkColorType &&
        colorType != kRGB_565_SkColorType) {
        colorType = kN32_SkColorType;
    }

    if (colorType != kN32_SkColorType &&
        colorType != kRGB_565_SkColorType) {
        XLOGW("HW Not support format: %d\n", colorType);
        if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
        {
            XLOGW("Can not release JPEG HW Decoder\n");
        }  
        return false;
    }

    // get file dimension
    if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_GET_INFO, NULL, 0, 
                                   (void *)&outInfo, sizeof(outInfo), NULL))
    {
        return false;
    }

    width  = (int)outInfo.srcWidth / sampleSize;
    height = (int)outInfo.srcHeight / sampleSize; 

    regionWidth = region.width() / sampleSize;
    regionHeight = region.height() / sampleSize;

    SkBitmap *bitmap = new SkBitmap;
    SkAutoTDelete<SkBitmap> adb(bitmap);
    
    bitmap->setInfo(SkImageInfo::Make(regionWidth, regionHeight, colorType, alphaType));
            
    if (!this->allocPixelRef(bitmap, NULL)) {
        if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
        {
            XLOGD("Can not release JPEG HW Decoder\n");
        }        
        return false;
    }

    SkAutoLockPixels alp(*bitmap);
    
    if (bitmap->config() == SkBitmap::kARGB_8888_Config) {
        inParams.dstFormat = JPEG_OUT_FORMAT_ARGB8888;
    } else {
        inParams.dstFormat = JPEG_OUT_FORMAT_RGB565;
    }

    XLOGD("The file input width: %d, height: %d, output width: %d, height: %d, format: %d, \n", 
           outInfo.srcWidth, outInfo.srcHeight, width, height, colorType);
    XLOGD("Range Decode Range %d %d %d %d \n", region.fLeft, region.fTop, region.fRight, region.fBottom);
    inParams.srcBuffer = srcBuffer;
    inParams.srcLength= srcSize;
    inParams.dstWidth = width;
    inParams.dstHeight = height;
    inParams.dstVirAddr = (UINT8*) bitmap->getPixels();
    inParams.dstPhysAddr = NULL;

    inParams.doDithering = getDitherImage() ? 1 : 0;
    inParams.doRangeDecode = 1;
    inParams.rangeLeft = region.fLeft;
    inParams.rangeTop = region.fTop;
    inParams.rangeRight = region.fRight;
    inParams.rangeBottom = region.fBottom;

    // start decode
    
    //XLOGD("Before Trigger HW");
    if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_START, 
                                   (void *)&inParams, sizeof(inParams), 
                                   NULL, 0, NULL))
    {
        XLOGD("JPEG HW not support this image (Range Decode)\n");
        return false;
    }

    bm->swap(*bitmap);
    
    return true;
}

#endif
 
// This guy exists just to aid in debugging, as it allows debuggers to just
// set a break-point in one place to see all error exists.
static void print_jpeg_decoder_errors(const jpeg_decompress_struct& cinfo,
                         int width, int height, const char caller[]) {
    if (!(c_suppressJPEGImageDecoderErrors)) {
        char buffer[JMSG_LENGTH_MAX];
        cinfo.err->format_message((const j_common_ptr)&cinfo, buffer);
        SkDebugf("libjpeg error %d <%s> from %s [%d %d]\n",
                 cinfo.err->msg_code, buffer, caller, width, height);
    }
}

static bool return_false(const jpeg_decompress_struct& cinfo,
                         const SkBitmap& bm, const char caller[]) {
    print_jpeg_decoder_errors(cinfo, bm.width(), bm.height(), caller);
    return false;
}

static SkImageDecoder::Result return_failure(const jpeg_decompress_struct& cinfo,
                                             const SkBitmap& bm, const char caller[]) {
    print_jpeg_decoder_errors(cinfo, bm.width(), bm.height(), caller);
    return SkImageDecoder::kFailure;
}

///////////////////////////////////////////////////////////////////////////////

// Convert a scanline of CMYK samples to RGBX in place. Note that this
// method moves the "scanline" pointer in its processing
static void convert_CMYK_to_RGB(uint8_t* scanline, unsigned int width) {
    // At this point we've received CMYK pixels from libjpeg. We
    // perform a crude conversion to RGB (based on the formulae
    // from easyrgb.com):
    //  CMYK -> CMY
    //    C = ( C * (1 - K) + K )      // for each CMY component
    //  CMY -> RGB
    //    R = ( 1 - C ) * 255          // for each RGB component
    // Unfortunately we are seeing inverted CMYK so all the original terms
    // are 1-. This yields:
    //  CMYK -> CMY
    //    C = ( (1-C) * (1 - (1-K) + (1-K) ) -> C = 1 - C*K
    // The conversion from CMY->RGB remains the same
    for (unsigned int x = 0; x < width; ++x, scanline += 4) {
        scanline[0] = SkMulDiv255Round(scanline[0], scanline[3]);
        scanline[1] = SkMulDiv255Round(scanline[1], scanline[3]);
        scanline[2] = SkMulDiv255Round(scanline[2], scanline[3]);
        scanline[3] = 255;
    }
}

/**
 *  Common code for setting the error manager.
 */
static void set_error_mgr(jpeg_decompress_struct* cinfo, skjpeg_error_mgr* errorManager) {
    SkASSERT(cinfo != NULL);
    SkASSERT(errorManager != NULL);
    cinfo->err = jpeg_std_error(errorManager);
    errorManager->error_exit = skjpeg_error_exit;
}

/**
 *  Common code for turning off upsampling and smoothing. Turning these
 *  off helps performance without showing noticable differences in the
 *  resulting bitmap.
 */
static void turn_off_visual_optimizations(jpeg_decompress_struct* cinfo) {
    SkASSERT(cinfo != NULL);
    /* this gives about 30% performance improvement. In theory it may
       reduce the visual quality, in practice I'm not seeing a difference
     */
    cinfo->do_fancy_upsampling = 0;

    /* this gives another few percents */
    cinfo->do_block_smoothing = 0;
}

/**
 * Common code for setting the dct method.
 */
static void set_dct_method(const SkImageDecoder& decoder, jpeg_decompress_struct* cinfo) {
    SkASSERT(cinfo != NULL);
#ifdef DCT_IFAST_SUPPORTED
    if (decoder.getPreferQualityOverSpeed()) {
        cinfo->dct_method = JDCT_ISLOW;
    } else {
        cinfo->dct_method = JDCT_IFAST;
    }
#else
    cinfo->dct_method = JDCT_ISLOW;
#endif
}

/* return current time in milliseconds */
static double now_ms(void) {

    struct timespec res;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID , &res); //CLOCK_REALTIME
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;    
}

SkColorType SkJPEGImageDecoder::getBitmapColorType(jpeg_decompress_struct* cinfo) {
    SkASSERT(cinfo != NULL);

    SrcDepth srcDepth = k32Bit_SrcDepth;
    if (JCS_GRAYSCALE == cinfo->jpeg_color_space) {
        srcDepth = k8BitGray_SrcDepth;
    }

    SkColorType colorType = this->getPrefColorType(srcDepth, /*hasAlpha*/ false);
    switch (colorType) {
        case kAlpha_8_SkColorType:
            // Only respect A8 colortype if the original is grayscale,
            // in which case we will treat the grayscale as alpha
            // values.
            if (cinfo->jpeg_color_space != JCS_GRAYSCALE) {
                colorType = kN32_SkColorType;
            }
            break;
        case kN32_SkColorType:
            // Fall through.
        case kARGB_4444_SkColorType:
            // Fall through.
        case kRGB_565_SkColorType:
            // These are acceptable destination colortypes.
            break;
        default:
            // Force all other colortypes to 8888.
            colorType = kN32_SkColorType;
            break;
    }

    switch (cinfo->jpeg_color_space) {
        case JCS_CMYK:
            // Fall through.
        case JCS_YCCK:
            // libjpeg cannot convert from CMYK or YCCK to RGB - here we set up
            // so libjpeg will give us CMYK samples back and we will later
            // manually convert them to RGB
            cinfo->out_color_space = JCS_CMYK;
            break;
        case JCS_GRAYSCALE:
            if (kAlpha_8_SkColorType == colorType) {
                cinfo->out_color_space = JCS_GRAYSCALE;
                break;
            }
            // The data is JCS_GRAYSCALE, but the caller wants some sort of RGB
            // colortype. Fall through to set to the default.
        default:
            cinfo->out_color_space = JCS_RGB;
            break;
    }
    return colorType;
}

/**
 *  Based on the colortype and dither mode, adjust out_color_space and
 *  dither_mode of cinfo. Only does work in ANDROID_RGB
 */
static void adjust_out_color_space_and_dither(jpeg_decompress_struct* cinfo,
                                              SkColorType colorType,
                                              const SkImageDecoder& decoder) {
    SkASSERT(cinfo != NULL);
#ifdef ANDROID_RGB
    cinfo->dither_mode = JDITHER_NONE;
    if (JCS_CMYK == cinfo->out_color_space) {
        return;
    }
    switch (colorType) {
        case kN32_SkColorType:
            cinfo->out_color_space = JCS_RGBA_8888;
            break;
        case kRGB_565_SkColorType:
            cinfo->out_color_space = JCS_RGB_565;
            if (decoder.getDitherImage()) {
                cinfo->dither_mode = JDITHER_ORDERED;
            }
            break;
        default:
            break;
    }
#endif
}

/**
   Sets all pixels in given bitmap to SK_ColorWHITE for all rows >= y.
   Used when decoding fails partway through reading scanlines to fill
   remaining lines. */
static void fill_below_level(int y, SkBitmap* bitmap) {
    SkIRect rect = SkIRect::MakeLTRB(0, y, bitmap->width(), bitmap->height());
    SkCanvas canvas(*bitmap);
    canvas.clipRect(SkRect::Make(rect));
    canvas.drawColor(SK_ColorWHITE);
}

/**
 *  Get the config and bytes per pixel of the source data. Return
 *  whether the data is supported.
 */
static bool get_src_config(const jpeg_decompress_struct& cinfo,
                           SkScaledBitmapSampler::SrcConfig* sc,
                           int* srcBytesPerPixel) {
    SkASSERT(sc != NULL && srcBytesPerPixel != NULL);
    if (JCS_CMYK == cinfo.out_color_space) {
        // In this case we will manually convert the CMYK values to RGB
        *sc = SkScaledBitmapSampler::kRGBX;
        // The CMYK work-around relies on 4 components per pixel here
        *srcBytesPerPixel = 4;
    } else if (3 == cinfo.out_color_components && JCS_RGB == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kRGB;
        *srcBytesPerPixel = 3;
#ifdef ANDROID_RGB
    } else if (JCS_RGBA_8888 == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kRGBX;
        *srcBytesPerPixel = 4;
    } else if (JCS_RGB_565 == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kRGB_565;
        *srcBytesPerPixel = 2;
#endif
    } else if (1 == cinfo.out_color_components &&
               JCS_GRAYSCALE == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kGray;
        *srcBytesPerPixel = 1;
    } else {
        return false;
    }
    return true;
}

#ifdef MTK_JPEG_HW_DECODER
class SkAshmemMalloc
{
public:
    SkAshmemMalloc(): fAddr(NULL), fFD(-1) {}
    ~SkAshmemMalloc() { free(); }
    void* reset(size_t size) 
    {
        if(fAddr != NULL) 
            free();

        fSize = size;
        fFD = ashmem_create_region("decodeSrc", size);
        if (-1 == fFD)
        {
            SkDebugf("------- ashmem create failed %d\n", size);
            return NULL;
        }

        int err = ashmem_set_prot_region(fFD, PROT_READ | PROT_WRITE);
        if (err) 
        {
            SkDebugf("------ ashmem_set_prot_region(%d) failed %d\n", fFD, err);
            close(fFD);
            return NULL;
        }

        fAddr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fFD, 0);

        if (-1 == (long)fAddr) 
        {
            fAddr = NULL;
            SkDebugf("------- mmap failed for ashmem size=%d \n", size);
            close(fFD);
            return NULL;
        }

        ashmem_pin_region(fFD, 0, 0);
        
        return fAddr;
    }

    void free()
    {
        if(fAddr != NULL)
        {
            ashmem_unpin_region(fFD, 0, 0);
            munmap(fAddr, fSize);
            close(fFD);
            fAddr = NULL;
        }
    }

    void* getAddr() { return fAddr; }
    int getFD() { return fFD; }
    
private:
    void*   fAddr;
    int     fFD;
    size_t  fSize;
    bool    fPinned;

    
};

bool getEOImarker(unsigned char* start, unsigned char* end, unsigned int *bs_offset)
{
  unsigned int eoi_flag = 0; 
  unsigned char* bs_tail ;

  //test_va = 
  
  //XLOGW("SkiaJpeg:getEOImarker start %x, end %x, L:%d!! \n",(unsigned int)start, (unsigned int) end, __LINE__);
  if((start+1 >= end) || start == NULL || end == NULL || (*(uint8_t*)(end) != 0x00)){
    XLOGW("SkiaJpeg:getEOImarker find no EOI [%p %p], L:%d!! \n", start, end, __LINE__);
    return false ;
  }

  bs_tail = start+1;//(uint8_t*)(tSrc + rSize - zSize) ;
  for( ;bs_tail < end ; bs_tail++){
    if( (*(uint8_t*)(bs_tail-1) == 0xFF) && (*(uint8_t*)(bs_tail) == 0xD9) ){
       *bs_offset = bs_tail - start ;
       eoi_flag = 1;
       XLOGW("SkiaJpeg:getEOImarker get EOI at %p, oft %x, [%p %p], L:%d!! \n",bs_tail, *bs_offset, start, end, __LINE__);
    }
  }  
  
  
  if(eoi_flag == 0){
    XLOGW("SkiaJpeg:getEOImarker find no EOI [%p %p], L:%d!! \n", start, end, __LINE__);
    return false ;
  }else
    return true ;   
}
#endif

SkImageDecoder::Result SkJPEGImageDecoder::onDecode(SkStream* stream, SkBitmap* bm, Mode mode) {
#ifdef TIME_DECODE
    SkAutoTime atm("JPEG Decode");
#endif

#ifdef MTK_JPEG_HW_DECODER
    ATRACE_CALL();
    SkAshmemMalloc    tAllocator;
#ifdef USE_SKJPGSTREAM
	JpgStreamAutoClean jpgStreamAutoClean;
#endif

    size_t sLength = stream->getLength() + MAX_HEADER_SIZE + 64;
    size_t tmpLength;
    uint8_t* tSrc = NULL;
    size_t rSize = 0;
    size_t rHWbsSize = 0;
    size_t tmpSize = 0;
    size_t align_rSize =0;
    size_t no_eoi = 0;
    size_t skip_hw_path = 0;
    
#if 0
    char acBuf[256];
    sprintf(acBuf, "/proc/%d/cmdline", getpid());
    FILE *fp = fopen(acBuf, "r");
    if (fp){
    read(acBuf, 1, sizeof(acBuf), fp);
    fclose(fp);
    if(strncmp(acBuf, "com.android.gallery3d", 21) == 0){				   
      try_hw = 1;
    }
    //else 
    //if(strncmp(acBuf, "mJpegTest", 9) == 0){			 	   
    //  try_hw = 1; 
    //}
    //SkDebugf("skia: process name2: %s!!\n", acBuf);
    }
#endif

#ifdef MTK_6572DISPLAY_ENHANCEMENT_SUPPORT 
    unsigned int try_hw = 0;
    {
        char value[PROPERTY_VALUE_MAX];     
        unsigned long u4PQOpt;

        property_get("persist.PQ", value, "1");
        u4PQOpt = atol(value);
        if(0 != u4PQOpt)
        {
            try_hw = (this->getPostProcFlag()) & 0x1;
        }
    }
#endif

#ifdef MTK_JPEG_HW_DECODER_658X
    try_hw = 1;
#endif

  if(try_hw && mode != SkImageDecoder::kDecodeBounds_Mode){  

    tSrc = (uint8_t*)tAllocator.reset(sLength);
    
    if (tSrc != NULL) 
    {
        if((unsigned long)tSrc % 32 != 0)
        {
            tmpLength = 32 - ((unsigned long)tSrc % 32);
            tSrc += tmpLength;
            sLength -= tmpLength;
        }

        if(sLength % 32 != 0)
            sLength -= (sLength % 32);

        rSize = stream->read(tSrc, MAX_HEADER_SIZE);
    }
     

    if (rSize == 0) 
    {
        if (tSrc != NULL) 
        {
            tAllocator.free();
            if (true != stream->rewind()) 
            {
                XLOGW("onDecode(), rewind fail\n");
                return kFailure;       
            }
        }
    } 
    else 
    {
           
        XLOGW("enter Skia Jpeg try_path %d, PPF %d, mode %d, bsLength %x, L:%d!! \n",try_hw,this->getPostProcFlag(), mode, stream->getLength(),__LINE__);           

        if(try_hw && mode != SkImageDecoder::kDecodeBounds_Mode && true == onDecodeParser(bm, mode, tSrc, rSize, tAllocator.getFD()))
        {
            if(mode == SkImageDecoder::kDecodeBounds_Mode)
            {
                tAllocator.free();
                return kSuccess;        
            }
            else
            {
                if(rSize == MAX_HEADER_SIZE)
                {
                    SkAshmemMalloc  tmpAllocator;
                    uint8_t* tmpBuffer = NULL;
                    tmpLength = stream->getLength();
                    size_t timeout_flag = 0;
                    struct timeval t1, t2;
                    gettimeofday(&t1, NULL);
                    //SkDebugf("Readed Size : %d, Buffer Size : %d, Remain Stream Size : %d", rSize, sLength, tmpLength);
                    do
                    {
                        if(sLength <= rSize + 16)
                        {
                            XLOGD("Try to Add Buffer Size");
                            sLength = rSize + tmpLength + MAX_HEADER_SIZE + 64;

                            tmpBuffer = (uint8_t*)tmpAllocator.reset(rSize);
                            memcpy(tmpBuffer, tSrc, rSize);
                            tAllocator.free();
                            tSrc = (uint8_t*)tAllocator.reset(sLength);
                            if((unsigned long)tSrc % 32 != 0)
                            {
                                tmpLength = 32 - ((unsigned long)tSrc % 32);
                                tSrc += tmpLength;
                                sLength -= tmpLength;
                            }

                            if(sLength % 32 != 0)
                                sLength -= (sLength % 32);
            
                            memcpy(tSrc, tmpBuffer, rSize);
                            tmpAllocator.free();
                        }
                        tmpSize = stream->read((tSrc + rSize), (sLength - rSize));
                        rSize += tmpSize;
                        tmpLength = stream->getLength();
                        gettimeofday(&t2, NULL);
                        if( ((t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec)) > 3000000L ){
                           XLOGW("SkiaJpeg: loading bitstream timeout, rSize %d, total rSize %d, remainSize %d, L:%d!!\n", tmpSize, rSize, tmpLength,__LINE__);
                           timeout_flag = 1;
                        }
                        //SkDebugf("Readed Size : %d, Remain Buffer Size : %d, Remain Stream Size : %d", tmpSize, (sLength - rSize), tmpLength);
                    } while(tmpSize != 0 && timeout_flag == 0);
                } 

                rHWbsSize = rSize ;
#if 1                
                {
                  uint8_t* bs_tail = (uint8_t*)(tSrc + rSize) ;
                  uint8_t zSize = 128 ;
                  unsigned int ofset = 0 ;
                  if( (*(uint8_t*)(bs_tail-2) != 0xFF) || (*(uint8_t*)(bs_tail-1) != 0xD9) ){
                    //XLOGW("SkiaJpeg:broken bitstream!!\n");  
                    XLOGW("SkiaJpeg: broken_jpeg_bs b %p,s %x, bs: b %x %x, e %x %x %x %x %x, L:%d!!\n", tSrc, rSize,*tSrc, *(tSrc+1)
                    , *(uint8_t*)(bs_tail-4),*(uint8_t*)(bs_tail-3),*(uint8_t*)(bs_tail-2), *(uint8_t*)(bs_tail-1), *bs_tail,__LINE__);                                    
                    no_eoi =1;
                    if(zSize < rSize){                     
                      if(getEOImarker(bs_tail-zSize, bs_tail-1, &ofset))
                        no_eoi = 0;
                    }                    
                  }
                }                
#endif                
                if( no_eoi 
                    //|| (bm->width() == 200 && bm->height() == 200)
                   ){
                  if (MHAL_NO_ERROR != mHalJpeg(MHAL_IOCTL_JPEG_DEC_CANCEL, (void*) fSkJpegDecHandle, 0, NULL, 0, NULL))
                  {
                      XLOGW("Can not release JPEG HW Decoder\n");
                      return kFailure;
                  }
                  skip_hw_path = 1;
                }
                if(sLength > rSize){
                    memset((tSrc + rSize), 0, sLength - rSize);
#ifndef MTK_JPEG_HW_DECODER_658X                    
                    rSize += 64 ;
                    SkDebugf("JPEG_BS mSize %x, rSize %x, align rSize %x, LENGTH %x!!\n", sLength, rSize, (rSize + 31) & (~31), stream->getLength());
                    rSize = (rSize + 31) & (~31);    
#endif                    
                }
                SkDebugf("SkiaJpeg: skip %d, BufSize %x, BitsSize %x, BitsAlignSize %x, GetLength %x, L:%d!!\n",skip_hw_path, sLength, rSize, (rSize + 31) & (~31), stream->getLength(), __LINE__); 
                    
                //if(true != onDecodeHW(bm, tSrc, rSize, tAllocator.getFD()) )
                //if(skip_hw_path || true != onDecodeHW(bm, tSrc, sLength, ((sLength - 256)>rSize) ? sLength-256: rSize, tAllocator.getFD()) )
                if(skip_hw_path || true != onDecodeHW(bm, tSrc, sLength, rSize, tAllocator.getFD()) )
                {
                    XLOGD("SkiaJpeg:TRY_SW_PATH no_eoi %d, mSize %x, rSize %x, align rSize %x, skSize %x!!\n", no_eoi, sLength, rSize, (rSize + 31) & (~31), stream->getLength());
                    if(rSize > MAX_HEADER_SIZE)
                    {
#ifdef USE_SKJPGSTREAM
                        stream = new SkJpgStream(tSrc, rHWbsSize, stream);
                        jpgStreamAutoClean.set(stream);
#else
                        stream = new SkMemoryStream(tSrc, sLength);   
#endif
                        XLOGW("Use JPEG SW Decoder (temp stream)\n");
                    }
                    else
                    {
#ifdef USE_SKJPGSTREAM
                        XLOGD("SkiaJpeg:TRY_SW_PATH tSrc %x, rSize %x, skSize %x, L:%d!!\n", tSrc, rSize,  stream->getLength(), __LINE__);
                        stream = new SkJpgStream(tSrc, rHWbsSize, stream);
                        jpgStreamAutoClean.set(stream);
#else
                        tAllocator.free();
#endif                      
                        XLOGW("Use JPEG SW Decoder\n");
                        if(true != stream->rewind())
                        {
                            XLOGW("onDecode(), rewind fail\n");
                            return kFailure;       
                        }
                    }
                }
                else
                {
                    return kSuccess;
                }
            }
        }
        else
        {
#ifdef USE_SKJPGSTREAM
            XLOGD("SkiaJpeg:TRY_SW_PATH tSrc %x, rSize %x, skSize %x, L:%d!!\n", tSrc, rSize,  stream->getLength(), __LINE__);
            stream = new SkJpgStream(tSrc, rSize, stream);
            jpgStreamAutoClean.set(stream);
#else        
            tAllocator.free();
#endif            
            XLOGW("Use JPEG SW Decoder\n");
            if(true != stream->rewind())
            {
                XLOGW("onDecode(), rewind fail\n");
                return kFailure;       
            }
        }
    }
  }
    
#endif

    JPEGAutoClean autoClean;

    jpeg_decompress_struct  cinfo;
    skjpeg_source_mgr       srcManager(stream, this);

    skjpeg_error_mgr errorManager;
    set_error_mgr(&cinfo, &errorManager);

    // All objects need to be instantiated before this setjmp call so that
    // they will be cleaned up properly if an error occurs.
    if (setjmp(errorManager.fJmpBuf)) {
        return return_failure(cinfo, *bm, "setjmp");
    }

    initialize_info(&cinfo, &srcManager);
    autoClean.set(&cinfo);

    int status = jpeg_read_header(&cinfo, true);
    if (status != JPEG_HEADER_OK) {
        return return_failure(cinfo, *bm, "read_header");
    }

#ifdef CHECK_LARGE_JPEG_PROG 
    if (SkImageDecoder::kDecodeBounds_Mode != mode)
    {
#ifdef USE_MTK_ALMK 
        if(cinfo.progressive_mode)
        {
            unsigned int rSize = cinfo.image_width * cinfo.image_height*6;   
            unsigned int maxSize ;
            if(!almkGetMaxSafeSize(getpid(), &maxSize))
            {
                maxSize = JPEG_PROG_LIMITATION_SIZE ;
                SkDebugf("ALMK::ProgJPEG::get Max Safe bmp (Max %d bytes) for BMP file (%d bytes) fail!!\n", maxSize, rSize);
            }
            else
            {
                if(maxSize > rSize)
                    SkDebugf("ALMK::ProgJPEG::Max Safe Size (Max %d bytes) for BMP file(%d bytes)=> PASS !! \n", maxSize, rSize);
                else
                    SkDebugf("ALMK::ProgJPEG::Max Safe Size (Max %d bytes) for BMP file(%d bytes)=> MemoryShortage!! \n", maxSize, rSize);        
            }
            
            if(cinfo.progressive_mode && (rSize > maxSize) )
            {
                SkDebugf("too Large Progressive Image (%d, %d %d)", cinfo.progressive_mode,cinfo.image_width, cinfo.image_height);
                return return_failure(cinfo, *bm, "Not support too Large Progressive Image!!");		 
            }
        }
#else
        if(cinfo.progressive_mode && (cinfo.image_width * cinfo.image_height > JPEG_PROG_LIMITATION_SIZE) )
        {
            SkDebugf("too Large Progressive Image (%d, %d x %d)> limit(%d)", cinfo.progressive_mode,cinfo.image_width, cinfo.image_height, JPEG_PROG_LIMITATION_SIZE);
            return return_failure(cinfo, *bm, "Not support too Large Progressive Image!!");		 
        }
#endif
    }
#endif

    /*  Try to fulfill the requested sampleSize. Since jpeg can do it (when it
        can) much faster that we, just use their num/denom api to approximate
        the size.
    */
    int sampleSize = this->getSampleSize();

    set_dct_method(*this, &cinfo);

    SkASSERT(1 == cinfo.scale_num);
    cinfo.scale_denom = sampleSize;

    turn_off_visual_optimizations(&cinfo);

    const SkColorType colorType = this->getBitmapColorType(&cinfo);
    const SkAlphaType alphaType = kAlpha_8_SkColorType == colorType ?
                                      kPremul_SkAlphaType : kOpaque_SkAlphaType;

    adjust_out_color_space_and_dither(&cinfo, colorType, *this);
    SkDebugf("jpeg_decoder mode %d, colorType %d, w %d, h %d, sample %d, bsLength %x!!\n",mode,colorType,cinfo.image_width, cinfo.image_height, sampleSize, stream->getLength());
    if (1 == sampleSize && SkImageDecoder::kDecodeBounds_Mode == mode) {
        // Assume an A8 bitmap is not opaque to avoid the check of each
        // individual pixel. It is very unlikely to be opaque, since
        // an opaque A8 bitmap would not be very interesting.
        // Otherwise, a jpeg image is opaque.
        bool success = bm->setInfo(SkImageInfo::Make(cinfo.image_width, cinfo.image_height,
                                                     colorType, alphaType));
        return success ? kSuccess : kFailure;
    }

    /*  image_width and image_height are the original dimensions, available
        after jpeg_read_header(). To see the scaled dimensions, we have to call
        jpeg_start_decompress(), and then read output_width and output_height.
    */
    if (!jpeg_start_decompress(&cinfo)) {
        /*  If we failed here, we may still have enough information to return
            to the caller if they just wanted (subsampled bounds). If sampleSize
            was 1, then we would have already returned. Thus we just check if
            we're in kDecodeBounds_Mode, and that we have valid output sizes.

            One reason to fail here is that we have insufficient stream data
            to complete the setup. However, output dimensions seem to get
            computed very early, which is why this special check can pay off.
         */
        if (SkImageDecoder::kDecodeBounds_Mode == mode && valid_output_dimensions(cinfo)) {
            SkScaledBitmapSampler smpl(cinfo.output_width, cinfo.output_height,
                                       recompute_sampleSize(sampleSize, cinfo));
            // Assume an A8 bitmap is not opaque to avoid the check of each
            // individual pixel. It is very unlikely to be opaque, since
            // an opaque A8 bitmap would not be very interesting.
            // Otherwise, a jpeg image is opaque.
            bool success = bm->setInfo(SkImageInfo::Make(smpl.scaledWidth(), smpl.scaledHeight(),
                                                         colorType, alphaType));
            return success ? kSuccess : kFailure;
        } else {
            return return_failure(cinfo, *bm, "start_decompress");
        }
    }
    sampleSize = recompute_sampleSize(sampleSize, cinfo);

#ifdef SK_SUPPORT_LEGACY_IMAGEDECODER_CHOOSER
    // should we allow the Chooser (if present) to pick a colortype for us???
    if (!this->chooseFromOneChoice(colorType, cinfo.output_width, cinfo.output_height)) {
        return return_failure(cinfo, *bm, "chooseFromOneChoice");
    }
#endif

    SkScaledBitmapSampler sampler(cinfo.output_width, cinfo.output_height, sampleSize);
    // Assume an A8 bitmap is not opaque to avoid the check of each
    // individual pixel. It is very unlikely to be opaque, since
    // an opaque A8 bitmap would not be very interesting.
    // Otherwise, a jpeg image is opaque.
    bm->setInfo(SkImageInfo::Make(sampler.scaledWidth(), sampler.scaledHeight(),
                                  colorType, alphaType));
    if (SkImageDecoder::kDecodeBounds_Mode == mode) {
        return kSuccess;
    }
    if (!this->allocPixelRef(bm, NULL)) {
        return return_failure(cinfo, *bm, "allocPixelRef");
    }

    SkAutoLockPixels alp(*bm);

#ifdef ANDROID_RGB
    /* short-circuit the SkScaledBitmapSampler when possible, as this gives
       a significant performance boost.
    */
    if (sampleSize == 1 &&
        ((kN32_SkColorType == colorType && cinfo.out_color_space == JCS_RGBA_8888) ||
         (kRGB_565_SkColorType == colorType && cinfo.out_color_space == JCS_RGB_565)))
    {
        JSAMPLE* rowptr = (JSAMPLE*)bm->getPixels();
        INT32 const bpr =  bm->rowBytes();

        while (cinfo.output_scanline < cinfo.output_height) {
            int row_count = jpeg_read_scanlines(&cinfo, &rowptr, 1);
            if (0 == row_count) {
                // if row_count == 0, then we didn't get a scanline,
                // so return early.  We will return a partial image.
                fill_below_level(cinfo.output_scanline, bm);
                cinfo.output_scanline = cinfo.output_height;
                jpeg_finish_decompress(&cinfo);
                return kPartialSuccess;
            }
            if (this->shouldCancelDecode()) {
                return return_failure(cinfo, *bm, "shouldCancelDecode");
            }
            rowptr += bpr;
        }
        jpeg_finish_decompress(&cinfo);
        XLOGD("jpeg_decoder finish successfully, L:%d!!!\n",__LINE__);
        return kSuccess;
    }
#endif

    // check for supported formats
    SkScaledBitmapSampler::SrcConfig sc;
    int srcBytesPerPixel;

    if (!get_src_config(cinfo, &sc, &srcBytesPerPixel)) {
        return return_failure(cinfo, *bm, "jpeg colorspace");
    }

    if (!sampler.begin(bm, sc, *this)) {
        return return_failure(cinfo, *bm, "sampler.begin");
    }

    SkAutoMalloc srcStorage(cinfo.output_width * srcBytesPerPixel);
    uint8_t* srcRow = (uint8_t*)srcStorage.get();

    //  Possibly skip initial rows [sampler.srcY0]
    if (!skip_src_rows(&cinfo, srcRow, sampler.srcY0())) {
        return return_failure(cinfo, *bm, "skip rows");
    }

    // now loop through scanlines until y == bm->height() - 1
    for (int y = 0;; y++) {
        JSAMPLE* rowptr = (JSAMPLE*)srcRow;
        int row_count = jpeg_read_scanlines(&cinfo, &rowptr, 1);
        if (0 == row_count) {
            // if row_count == 0, then we didn't get a scanline,
            // so return early.  We will return a partial image.
            fill_below_level(y, bm);
            cinfo.output_scanline = cinfo.output_height;
            jpeg_finish_decompress(&cinfo);
            return kSuccess;
        }
        if (this->shouldCancelDecode()) {
            return return_failure(cinfo, *bm, "shouldCancelDecode");
        }

        if (JCS_CMYK == cinfo.out_color_space) {
            convert_CMYK_to_RGB(srcRow, cinfo.output_width);
        }

        sampler.next(srcRow);
        if (bm->height() - 1 == y) {
            // we're done
            break;
        }

        if (!skip_src_rows(&cinfo, srcRow, sampler.srcDY() - 1)) {
            return return_failure(cinfo, *bm, "skip rows");
        }
    }

    // we formally skip the rest, so we don't get a complaint from libjpeg
    if (!skip_src_rows(&cinfo, srcRow,
                       cinfo.output_height - cinfo.output_scanline)) {
        return return_failure(cinfo, *bm, "skip rows");
    }
    jpeg_finish_decompress(&cinfo);
    XLOGD("jpeg_decoder finish successfully, L:%d!!!\n",__LINE__);

    return kSuccess;
}

#ifdef SK_BUILD_FOR_ANDROID
bool SkJPEGImageDecoder::onBuildTileIndex(SkStreamRewindable* stream, int *width, int *height) {

#if MTK_JPEG_HW_DECODER
    fFirstTileDone = false;
    fUseHWResizer = false;
#endif
//#if MTK_JPEG_HW_DECODER
#if 0
    size_t sLength = stream->getLength() + MAX_HEADER_SIZE;
    size_t tmpLength;
    size_t tmpSize = 0;

    fInitRegion = true;
    fSrc = (uint8_t*)fAllocator.reset(sLength);
    fSize = 0;
    if (fSrc != NULL) 
    {
        if((uint32_t)fSrc % 32 != 0)
        {
            tmpLength = 32 - ((uint32_t)fSrc % 32);
            fSrc += tmpLength;
            sLength -= tmpLength;
        }

        if(sLength % 32 != 0)
            sLength -= (sLength % 32);

        fSize = stream->read(fSrc, MAX_HEADER_SIZE);
    }

    if (fSize != 0) 
    {
        if(true == onRangeDecodeParser(fSrc, fSize, width, height, true))
        {

            android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"onDecodeBS");         
            if(fSize == MAX_HEADER_SIZE)
            {
                SkAutoMalloc  tmpAllocator;
                uint8_t* tmpBuffer = NULL;
                tmpLength = stream->getLength();
                //SkDebugf("Readed Size : %d, Buffer Size : %d, Remain Stream Size : %d", rSize, sLength, tmpLength);
                do
                {
                    if(sLength <= fSize + 16)
                    {
                        XLOGD("Try to Add Buffer Size");
                        sLength = tmpLength + MAX_HEADER_SIZE;

                        tmpBuffer = (uint8_t*)tmpAllocator.reset(sLength);
                        memcpy(tmpBuffer, fSrc, fSize);
                        fAllocator.free();
                        fSrc = (uint8_t*)fAllocator.reset(sLength);
                        if((uint32_t)fSrc % 32 != 0)
                        {
                            tmpLength = 32 - ((uint32_t)fSrc % 32);
                            fSrc += tmpLength;
                            sLength -= tmpLength;
                        }

                        if(sLength % 32 != 0)
                            sLength -= (sLength % 32);
            
                        memcpy(fSrc, tmpBuffer, fSize);
                        tmpAllocator.free();
                    }
                    tmpSize = stream->read((fSrc + fSize), (sLength - fSize));
                    fSize += tmpSize;
                    tmpLength = stream->getLength();
                        //SkDebugf("Readed Size : %d, Remain Buffer Size : %d, Remain Stream Size : %d", tmpSize, (sLength - rSize), tmpLength);
                } while(tmpSize != 0);
            } 
            android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);    
            if(sLength > fSize)
            {
                memset((fSrc + fSize), 0, sLength - fSize);
                //fSize = sLength;
            }

            if(fSize > MAX_HEADER_SIZE)
            {
                stream = new SkMemoryStream(fSrc, sLength);
            }
            else
            {
                if(true != stream->rewind())
                {
                    XLOGW("onBuildTileIndex, rewind fail\n");
                    return false;       
                }
            }
        }
        else
        {
            fSize = 0;
        }
    } 

    if(fSize == 0)
    {
        fAllocator.free();
        fSrc = NULL;
        if(true != stream->rewind())
        {
            SkDebugf("onBuildTileIndex(), rewind fail\n");
            return false;       
        }
    }
    
#endif

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION

    size_t length = stream->getLength();
    if (length <= 0 ) {
        SkDebugf("buildTileIndex fail, length is 0. L:%d!!\n", __LINE__ );
        return false;
    }

    SkAutoTMalloc<uint8_t> allocMemory(length);
    
//    stream->rewind();
    stream->read(allocMemory.get(), length) ;
    SkMemoryStream* mtkPxyStream = new SkMemoryStream(allocMemory ,  length,  true);

    SkAutoTDelete<SkJPEGImageIndex> imageIndex(SkNEW_ARGS(SkJPEGImageIndex, (stream, this)));
    
#else
    SkAutoTDelete<SkJPEGImageIndex> imageIndex(SkNEW_ARGS(SkJPEGImageIndex, (stream, this)));
#endif


    jpeg_decompress_struct* cinfo = imageIndex->cinfo();

    skjpeg_error_mgr sk_err;
    set_error_mgr(cinfo, &sk_err);

    // All objects need to be instantiated before this setjmp call so that
    // they will be cleaned up properly if an error occurs.
    if (setjmp(sk_err.fJmpBuf)) {
        return false;
    }

    // create the cinfo used to create/build the huffmanIndex
    if (!imageIndex->initializeInfoAndReadHeader()) {
        return false;
    }

    g_mt_start = 0;
    g_mt_end = 0;
    g_mt_end_duration_2 = 0;
    g_mt_hw_sum1 = 0;
    g_mt_hw_sum2 = 0;    

    if (!imageIndex->buildHuffmanIndex()) {
        return false;
    }


    // destroy the cinfo used to create/build the huffman index
    imageIndex->destroyInfo();

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
    if(mtkPxyStream){
        fIsAllowMultiThreadRegionDecode = true ; 
        imageIndex->mtkStream = mtkPxyStream ;
    }
#endif

#ifdef MTK_SKIA_DISABLE_MULTI_THREAD_JPEG_REGION
    fIsAllowMultiThreadRegionDecode = false ; 
    SkDebugf("MTR_JPEG:compile force disable jpeg Multi-Thread Region decode, L:%d!!\n", __LINE__);
#endif

    // Init decoder to image decode mode
    if (!imageIndex->initializeInfoAndReadHeader()) {
        return false;
    }

    // FIXME: This sets cinfo->out_color_space, which we may change later
    // based on the config in onDecodeSubset. This should be fine, since
    // jpeg_init_read_tile_scanline will check out_color_space again after
    // that change (when it calls jinit_color_deconverter).
    (void) this->getBitmapColorType(cinfo);

    turn_off_visual_optimizations(cinfo);

    // instead of jpeg_start_decompress() we start a tiled decompress
    if (!imageIndex->startTileDecompress()) {
        return false;
    }

    SkASSERT(1 == cinfo->scale_num);
    fImageWidth = cinfo->output_width;
    fImageHeight = cinfo->output_height;

    if (width) {
        *width = fImageWidth;
    }
    if (height) {
        *height = fImageHeight;
    }

    SkDELETE(fImageIndex);
    fImageIndex = imageIndex.detach();

    if ((cinfo->comps_in_scan < cinfo->num_components )&& !cinfo->progressive_mode){
      SkDebugf("buildTileIndex fail, region_decoder unsupported format : prog %d, comp %d, scan_comp %d!!\n"
      , cinfo->progressive_mode, cinfo->num_components, cinfo->comps_in_scan );
      return false;
    }

    return true;
}

#if MTK_JPEG_HW_DECODER
#ifdef MTK_JPEG_HW_REGION_RESIZER
bool MDPCrop(void* src, int width, int height, SkBitmap* bm, int tdsp, void* pPPParam)
{
    if((NULL == bm))
    {
        XLOGW("MDP_Crop : null bitmap");
        return false;
    }
    if(NULL == bm->getPixels())
    {
        XLOGW("MDP_Crop : null pixels");
        return false;
    }
    if((bm->config() == SkBitmap::kARGB_8888_Config && bm->isOpaque()) || 
       (bm->config() == SkBitmap::kRGB_565_Config))
    {
        DpBlitStream bltStream;  //mHalBltParam_t bltParam;
        void* src_addr[3];
        unsigned int src_size[3];        
        unsigned int plane_num = 1;
        DpColorFormat dp_out_fmt ;
        DpColorFormat dp_in_fmt ;
        unsigned int src_pByte = 4;
        src_addr[0] = src ;
        DP_STATUS_ENUM rst ;
        
        switch(bm->config())
        {
            case SkBitmap::kARGB_8888_Config:
                dp_out_fmt = eRGBX8888; //eABGR8888;    //bltParam.dstFormat = MHAL_FORMAT_ABGR_8888;
                src_pByte = 4;
                break;
            case SkBitmap::kRGB_565_Config:                
                dp_out_fmt = eRGB565;    //bltParam.dstFormat = MHAL_FORMAT_RGB_565;
                src_pByte = 2;
                break;
            default :
                XLOGW("MDP_Crop : unvalid bitmap config d!!\n", bm->config());
                return false;
        }
        dp_in_fmt = dp_out_fmt ;

        #if 0 //def JPEG_DRAW_B4_BLIT
        if(src_pByte == 4){
          
          unsigned int *drawptr = (unsigned int *)src;
          unsigned int buf_width = width;
          unsigned int buf_height = height;
          unsigned int line = 0;
          unsigned int draw_x=0, draw_y=0 ;
          
          for(draw_y = 0; draw_y < buf_height ; draw_y++){
            
              for(draw_x = 0; draw_x < buf_width ; draw_x++){
                
                //if( ( draw_y == 0 || draw_y == 1) || 
                //    ( draw_y == buf_height-1 || draw_y == buf_height-2) || 
                //    ( (draw_x == 0 || draw_x == 1) || (draw_x == buf_width -1 || draw_x == buf_width -2) ) )
                if( draw_y >= (buf_height/2)  && draw_y <= ((buf_height/2)+3))
                  *drawptr = 0xFFFF0000 ;
                drawptr ++;
              }
            
          }
        }
        #endif

        
        
        src_size[0] = width * height * src_pByte ;
        XLOGW("MDP_Crop: wh (%d %d)->(%d %d), fmt %d, size %d->%d, regionPQ %d!!\n", width, height, bm->width(), bm->height()
        , bm->config(), src_size[0], bm->rowBytes() * bm->height(), tdsp);
        
    #ifdef MTK_IMAGE_DC_SUPPORT
        {
            DpPqParam pqParam;
            uint32_t* pParam = &pqParam.u.image.info[0];
            
            pqParam.enable = (tdsp == 0)? false:true;
            pqParam.scenario = MEDIA_PICTURE;
            if (pPPParam)
            {
                XLOGW("MDP_Crop: enable imgDc pParam %p", pPPParam);
                pqParam.u.image.withHist = true;
                memcpy((void*)pParam, pPPParam, 20 * sizeof(uint32_t));
            }
            else
                pqParam.u.image.withHist = false;
            
            bltStream.setPQParameter(pqParam);
        }
    #else
        bltStream.setTdshp((tdsp == 0)? false:true);
    #endif
        
        //XLOGW("MDP_Crop: CONFIG_SRC_BUF, go L:%d!!\n", __LINE__);
        bltStream.setSrcBuffer((void**)src_addr, src_size, plane_num);
        DpRect src_roi;
        src_roi.x = 0;
        src_roi.y = 0;
        src_roi.w = width;
        src_roi.h = height;
        //XLOGW("MDP_Crop: CONFIG_SRC_SIZE, go L:%d!!\n", __LINE__);
        bltStream.setSrcConfig(width, height, width * src_pByte, 0, dp_in_fmt, DP_PROFILE_JPEG);

        // set dst buffer
        ///XLOGW("MDP_Crop: CONFIG_DST_BUF, go L:%d!!\n", __LINE__);
        bltStream.setDstBuffer((void *)bm->getPixels(), bm->rowBytes() * bm->height() );  // bm->width() * bm->height() * dst_bitperpixel / 8);
        DpRect dst_roi;
        dst_roi.x = 0;
        dst_roi.y = 0;
        dst_roi.w = bm->width();
        dst_roi.h = bm->height();

        //XLOGW("MDP_Crop: CONFIG_DST_SIZE, go L:%d!!\n", __LINE__);
        bltStream.setDstConfig(bm->width(), bm->height(), bm->rowBytes(), 0, dp_out_fmt, DP_PROFILE_JPEG);

        //XLOGW("MDP_Crop: GO_BITBLIT, go L:%d!!\n", __LINE__);
        rst = bltStream.invalidate() ;

        #if 0 //def JPEG_DRAW_AF_BLIT
        if(src_pByte == 4){
          
          unsigned int *drawptr = (unsigned int *)bm->getPixels();
          unsigned int pxl_width = bm->width();
          unsigned int pxl_height = bm->height();
          unsigned int line = 0;
          unsigned int draw_x=0, draw_y=0 ;
          
          for(draw_y = 0; draw_y < pxl_height ; draw_y++){
            
              for(draw_x = 0; draw_x < pxl_width ; draw_x++){
                
                //if( ( draw_y == 0 || draw_y == 1) || 
                //    ( draw_y == pxl_height-1 || draw_y == pxl_height-2) || 
                //    ( (draw_x == 0 || draw_x == 1) || (draw_x == pxl_width -1 || draw_x == pxl_width -2) ) )
                if( draw_x >= (pxl_width/2)  && draw_x <= ((pxl_width/2)+3))
                  *drawptr = 0xFF00FF00 ;
                drawptr ++;
              }
            
          }
        }
        #endif
        
        if ( rst < 0) {
            XLOGE("region Resizer: DpBlitStream invalidate failed, L:%d!!\n", __LINE__);
            return false;
        }else{
            return true ;
        }
       
    }
    return false;
}
bool MDPResizer(void* src, int width, int height, SkScaledBitmapSampler::SrcConfig sc, SkBitmap* bm, int tdsp, void* pPPParam)
{

   
    if((NULL == bm))
    {
        XLOGW("MDPResizer : null bitmap");
        return false;
    }
    if(NULL == bm->getPixels())
    {
        XLOGW("MDPResizer : null pixels");
        return false;
    }
    if((bm->config() == SkBitmap::kARGB_8888_Config && bm->isOpaque()) || 
       (bm->config() == SkBitmap::kRGB_565_Config))
    {
        DpBlitStream bltStream;  //mHalBltParam_t bltParam;
        void* src_addr[3];
        unsigned int src_size[3];        
        unsigned int plane_num = 1;
        DpColorFormat dp_out_fmt ;
        DpColorFormat dp_in_fmt ;
        unsigned int src_pByte = 4;
        src_addr[0] = src ;
        DP_STATUS_ENUM rst ;
        switch(bm->config())
        {
            case SkBitmap::kARGB_8888_Config:
                dp_out_fmt = eRGBX8888; //eABGR8888;    //bltParam.dstFormat = MHAL_FORMAT_ABGR_8888;
                break;
            case SkBitmap::kRGB_565_Config:                
                dp_out_fmt = eRGB565;    //bltParam.dstFormat = MHAL_FORMAT_RGB_565;
                break;
            default :
                XLOGW("MDPResizer : unvalid bitmap config d", bm->config());
                return false;
        }
        switch(sc)
        {
            case SkScaledBitmapSampler::kRGB:
                dp_in_fmt = eRGB888;         //bltParam.srcFormat = MHAL_FORMAT_BGR_888;
                src_pByte = 3;
                break;
            case SkScaledBitmapSampler::kRGBX:
                dp_in_fmt = eRGBX8888;//eABGR8888;         //bltParam.srcFormat = MHAL_FORMAT_ABGR_8888;
                src_pByte = 4;
                break;
            case SkScaledBitmapSampler::kRGB_565:
                dp_in_fmt = eRGB565;         //bltParam.srcFormat = MHAL_FORMAT_RGB_565;
                src_pByte = 2;
                break;
            case SkScaledBitmapSampler::kGray:
                dp_in_fmt = eGREY;           //bltParam.srcFormat = MHAL_FORMAT_Y800;
                src_pByte = 1;
                break;
            default :
                XLOGW("MDPResizer : unvalid src format %d", sc);
                return false;
            break;
        }

        #if 0 //def JPEG_DRAW_B4_BLIT
        if(src_pByte == 4){ 
          
          unsigned int *drawptr = (unsigned int *)src;
          unsigned int buf_width = width;
          unsigned int buf_height = height;
          unsigned int line = 0;
          unsigned int draw_x=0, draw_y=0 ;
          
          for(draw_y = 0; draw_y < buf_height ; draw_y++){
            
              for(draw_x = 0; draw_x < buf_width ; draw_x++){
                
                //if( ( draw_y == 0 || draw_y == 1) || 
                //    ( draw_y == buf_height-1 || draw_y == buf_height-2) || 
                //    ( (draw_x == 0 || draw_x == 1) || (draw_x == buf_width -1 || draw_x == buf_width -2) ) )
                if( draw_y >= (buf_height/2)  && draw_y <= ((buf_height/2)+3))
                  *drawptr = 0xFFFF0000 ;
                drawptr ++;
              }
            
          }
        }
        #endif

        
        
        src_size[0] = width * height * src_pByte ;
        XLOGW("MDPResizer: wh (%d %d)->(%d %d), fmt %d->%d, size %d->%d, regionPQ %d!!\n", width, height, bm->width(), bm->height()
        ,sc, bm->config(), src_size[0], bm->rowBytes() * bm->height(), tdsp);
        
    #ifdef MTK_IMAGE_DC_SUPPORT
        {
            DpPqParam pqParam;
            uint32_t* pParam = &pqParam.u.image.info[0];
            
            pqParam.enable = (tdsp == 0)? false:true;
            pqParam.scenario = MEDIA_PICTURE;
            if (pPPParam)
            {
                XLOGW("MDPResizer: enable imgDc pParam %p", pPPParam);
                pqParam.u.image.withHist = true;
                memcpy((void*)pParam, pPPParam, 20 * sizeof(uint32_t));
            }
            else
                pqParam.u.image.withHist = false;
            
            bltStream.setPQParameter(pqParam);
        }
    #else
        bltStream.setTdshp((tdsp == 0)? false:true);
    #endif
        
        //XLOGW("MDPResizer: CONFIG_SRC_BUF, go L:%d!!\n", __LINE__);
        bltStream.setSrcBuffer((void**)src_addr, src_size, plane_num);
        DpRect src_roi;
        src_roi.x = 0;
        src_roi.y = 0;
        src_roi.w = width;
        src_roi.h = height;
        //XLOGW("MDPResizer: CONFIG_SRC_SIZE, go L:%d!!\n", __LINE__);
        //bltStream.setSrcConfig(width, height, dp_in_fmt, eInterlace_None, &src_roi);
        bltStream.setSrcConfig(width, height, width * src_pByte, 0, dp_in_fmt, DP_PROFILE_JPEG);

        // set dst buffer
        ///XLOGW("MDPResizer: CONFIG_DST_BUF, go L:%d!!\n", __LINE__);
        bltStream.setDstBuffer((void *)bm->getPixels(), bm->rowBytes() * bm->height() );  // bm->width() * bm->height() * dst_bitperpixel / 8);
        DpRect dst_roi;
        dst_roi.x = 0;
        dst_roi.y = 0;
        dst_roi.w = bm->width();
        dst_roi.h = bm->height();

        //XLOGW("MDPResizer: CONFIG_DST_SIZE, go L:%d!!\n", __LINE__);
        //bltStream.setDstConfig(bm->width(), bm->height(), dp_out_fmt, eInterlace_None, &dst_roi);
        bltStream.setDstConfig(bm->width(), bm->height(), bm->rowBytes(), 0, dp_out_fmt, DP_PROFILE_JPEG);

        //XLOGW("MDPResizer: GO_BITBLIT, go L:%d!!\n", __LINE__);
        rst = bltStream.invalidate() ;

        #if 0 //def JPEG_DRAW_AF_BLIT
        if(src_pByte == 4){
          
          unsigned int *drawptr = (unsigned int *)bm->getPixels();
          unsigned int pxl_width = bm->width();
          unsigned int pxl_height = bm->height();
          unsigned int line = 0;
          unsigned int draw_x=0, draw_y=0 ;
          
          for(draw_y = 0; draw_y < pxl_height ; draw_y++){
            
              for(draw_x = 0; draw_x < pxl_width ; draw_x++){
                
                //if( ( draw_y == 0 || draw_y == 1) || 
                //    ( draw_y == pxl_height-1 || draw_y == pxl_height-2) || 
                //    ( (draw_x == 0 || draw_x == 1) || (draw_x == pxl_width -1 || draw_x == pxl_width -2) ) )
                if( draw_x >= (pxl_width/2)  && draw_x <= ((pxl_width/2)+3))
                  *drawptr = 0xFF00FF00 ;
                drawptr ++;
              }
            
          }
        }
        #endif
        
        if ( rst < 0) {
            XLOGE("region Resizer: DpBlitStream invalidate failed, L:%d!!\n", __LINE__);
            return false;
        }else{
            return true ;
        }
/*
        bltParam.orientation = MHAL_BITBLT_ROT_0;
        bltParam.srcAddr = (unsigned int)src;
        bltParam.dstAddr = (unsigned int)bm->getPixels();
        bltParam.srcX = bltParam.srcY = 0;

        
        bltParam.srcW = bltParam.srcWStride = width;
        bltParam.dstW = bm->width();
        
        bltParam.srcH = bltParam.srcHStride = height;
        bltParam.dstH = bm->height();

        bltParam.pitch = bm->width();
        
        if (MHAL_NO_ERROR != mHalMdpIpc_BitBlt(&bltParam))
        {
            return false;
        }
        else
        {
            return true;
        }
*/        
    }
    return false;
}
#endif
#if 0
bool MDPResizer(void* src, int width, int height, SkScaledBitmapSampler::SrcConfig sc, SkBitmap* bm, int tdsp)
{
    if((NULL == bm))
    {
        XLOGW("MDPResizer : null bitmap");
        return false;
    }

    if(NULL == bm->getPixels())
    {
        XLOGW("MDPResizer : null pixels");
        return false;
    }

    if((bm->config() == SkBitmap::kARGB_8888_Config && bm->isOpaque()) || 
       (bm->config() == SkBitmap::kRGB_565_Config))
    {
        mHalBltParam_t bltParam;
        memset(&bltParam, 0, sizeof(mHalBltParam_t));
        
        switch(bm->config())
        {
            case SkBitmap::kARGB_8888_Config:
                bltParam.dstFormat = MHAL_FORMAT_ABGR_8888;
                break;

            case SkBitmap::kRGB_565_Config:                
                bltParam.dstFormat = MHAL_FORMAT_RGB_565;
                break;
                
            default :
                XLOGW("MDPResizer : unvalid bitmap config d", bm->config());
                return false;
        }

        switch(sc)
        {
            case SkScaledBitmapSampler::kRGB:
                bltParam.srcFormat = MHAL_FORMAT_BGR_888;
                break;
                
            case SkScaledBitmapSampler::kRGBX:
                bltParam.srcFormat = MHAL_FORMAT_ABGR_8888;
                break;
                
            case SkScaledBitmapSampler::kRGB_565:
                bltParam.srcFormat = MHAL_FORMAT_RGB_565;
                break;
                
            case SkScaledBitmapSampler::kGray:
                bltParam.srcFormat = MHAL_FORMAT_Y800;
                break;
            
            default :
                XLOGW("MDPResizer : unvalid src format %d", sc);
                return false;
            break;
        }

        bltParam.orientation = MHAL_BITBLT_ROT_0;
        bltParam.srcAddr = (unsigned int)src;
        bltParam.dstAddr = (unsigned int)bm->getPixels();
        bltParam.srcX = bltParam.srcY = 0;

        
        bltParam.srcW = bltParam.srcWStride = width;
        bltParam.dstW = bm->width();
        
        bltParam.srcH = bltParam.srcHStride = height;
        bltParam.dstH = bm->height();

        bltParam.pitch = bm->width();
        
        if (MHAL_NO_ERROR != mHalMdpIpc_BitBlt(&bltParam))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return false;
}
#endif

#endif


bool SkJPEGImageDecoder::onDecodeSubset(SkBitmap* bm, const SkIRect& region) {
#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
    int sampleSize = this->getSampleSize();
    #ifdef MTK_IMAGE_DC_SUPPORT
        void* pParam = this->getDynamicCon();
        return this->onDecodeSubset(bm, region, sampleSize, pParam);
    #else
        return this->onDecodeSubset(bm, region, sampleSize, NULL);
    #endif
}
bool SkJPEGImageDecoder::onDecodeSubset(SkBitmap* bm, const SkIRect& region, int isampleSize, void* pParam) {
#endif //MTK_SKIA_MULTI_THREAD_JPEG_REGION


    double mt_start = now_ms(); // start time
    if(g_mt_start == 0){
        g_mt_start = mt_start;
        base_thread_id = gettid();      
    }else if(mt_start < g_mt_start){
        g_mt_start = mt_start;
        base_thread_id = gettid();
    }
    //g_mt_end = 0;
    SkDebugf("JPEG: debug_onDecodeSubset ++ , dur = %f,  id = %d,L:%d!!\n", mt_start - g_mt_start, gettid() , __LINE__);


#if MTK_JPEG_HW_DECODER
    unsigned int enTdshp = (this->getPostProcFlag()? 1 : 0);

    if (fFirstTileDone == false)
    {
        unsigned long u4PQOpt;
        char value[PROPERTY_VALUE_MAX];
    
        property_get("persist.PQ", value, "1");
        u4PQOpt = atol(value);
        if(0 != u4PQOpt)
        {
            if (!enTdshp && !pParam)
            {
                fFirstTileDone = true;
                fUseHWResizer = false;
            }
        }
    }
#endif

//#if MTK_JPEG_HW_DECODER
#if 0
    if(fSize != 0 || fInitRegion) 
    {       
        int width, height;
        int try_times = 0;
        do {
            if(true == onRangeDecodeParser(fSrc, fSize, &width, &height, false)) {
                if(true == onDecodeHWRegion(bm, region, fSrc, fSize)) {
                    if(fInitRegion)
                    {
                        fInitRegion = false;
                    }
                    return true;
                }
            }
            if(!fInitRegion)
            {
                SkDebugf("onDecodeRegion HW Failed, try again!!!");
                usleep(100*1000);
            }
            try_times++;    
        } while (!fInitRegion && try_times < 5);
        
        SkDebugf("onDecodeRegion HW Failed, Use SW!!!");
        if(fInitRegion)
        {
            fInitRegion = false;
            fSize = 0;
        }
    }
#endif

    if (NULL == fImageIndex) {
        return false;
    }
#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION 

    JpgLibAutoClean auto_clean_cinfo ;
    jpeg_decompress_struct *cinfo ;
    SkStream *stream ;
    skjpeg_source_mgr       *sk_stream = NULL;
      
    if(fImageIndex->mtkStream){
        //SkDebugf("MTR_JPEG: mtkStream  length = %d ,  L:%d!!\n", fImageIndex->mtkStream->getLength(),__LINE__);
      
        stream = new SkMemoryStream(fImageIndex->mtkStream->getMemoryBase() ,fImageIndex->mtkStream->getLength() , true);
       
        sk_stream = new skjpeg_source_mgr(stream, this); 
       
        cinfo = (jpeg_decompress_struct *)malloc(sizeof(struct jpeg_decompress_struct));
        memset(cinfo, 0, sizeof(struct jpeg_decompress_struct));
        auto_clean_cinfo.set(cinfo);

        cinfo->src = (jpeg_source_mgr *)malloc(sizeof(struct jpeg_source_mgr));
        memset(cinfo->src, 0, sizeof(struct jpeg_source_mgr));
        auto_clean_cinfo.set(cinfo->src);

        skjpeg_error_mgr sk_err;
        set_error_mgr(cinfo, &sk_err);

        // All objects need to be instantiated before this setjmp call so that
        // they will be cleaned up properly if an error occurs.
        if (setjmp(sk_err.fJmpBuf)) {
           SkDebugf("MTR_JPEG: setjmp L:%d!!\n ", __LINE__ );
           return false;
        }
          
        // Init decoder to image decode mode
        //if (!localImageIndex->initializeInfoAndReadHeader()) 
        {
            initialize_info(cinfo, sk_stream);
            const bool success = (JPEG_HEADER_OK == jpeg_read_header(cinfo, true));
            if(!success){
               SkDebugf("MTR_JPEG: initializeInfoAndReadHeader error L:%d!!\n ", __LINE__ );
               return false;
            }
        }

        // FIXME: This sets cinfo->out_color_space, which we may change later
        // based on the config in onDecodeSubset. This should be fine, since
        // jpeg_init_read_tile_scanline will check out_color_space again after
        // that change (when it calls jinit_color_deconverter).
        (void) this->getBitmapColorType(cinfo);

        turn_off_visual_optimizations(cinfo);

        // instead of jpeg_start_decompress() we start a tiled decompress
        //if (!localImageIndex->startTileDecompress()) {
       
        if (!jpeg_start_tile_decompress(cinfo)) {
           SkDebugf("MTR_JPEG: startTileDecompress error L:%d!!\n ", __LINE__ );
           return false;
        }
         
        SkSafeUnref(stream) ;   
         
        //SkAutoTDelete<skjpeg_source_mgr> adjpg(sk_stream);
       
       
    }
       
    //SkDebugf("MTR_JPEG: testmt_init -- ,mt_end_2 = %f , L:%d!!\n",now_ms() - g_mt_start  , __LINE__);
       

#else
    jpeg_decompress_struct* cinfo = fImageIndex->cinfo();
#endif

    SkIRect rect = SkIRect::MakeWH(fImageWidth, fImageHeight);
    if (!rect.intersect(region)) {
        // If the requested region is entirely outside the image return false
        return false;
    }


    SkAutoMalloc  srcStorage;
    skjpeg_error_mgr errorManager;
    set_error_mgr(cinfo, &errorManager);

    if (setjmp(errorManager.fJmpBuf)) {
        return false;
    }

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
    if(isampleSize == 0x0){
        isampleSize = this->getSampleSize();
        SkDebugf("JPEG: debug isampleSize = %d , L:%d!!\n",isampleSize ,__LINE__);
    }
    int requestedSampleSize = isampleSize; //this->getSampleSize();
#else
    int requestedSampleSize = this->getSampleSize();
#endif

#ifdef MTK_JPEG_HW_REGION_RESIZER //MTK_JPEG_HW_DECODER
    #if 0 // legacy limitation for frame mode HW resizer
    // region size has been padding 80 pixels for later usage
    if((region.width() + 80) * (region.height() + 80) > HW_RESIZE_MAX_PIXELS)
    {
        fFirstTileDone = true;
        fUseHWResizer = false;
        XLOGW("It is too large pixels (%d) to use hw resizer! Use sw sampler ", this->fImageWidth * this->fImageHeight);
    }
    #endif

    if(!fFirstTileDone || fUseHWResizer)
    {
        SkIRect rectHWResz;
        // create new region which is padding 40 pixels for each boundary
        rectHWResz.set((region.left() >= 40)? region.left() - 40: region.left(),
                       (region.top() >= 40)? region.top() - 40: region.top(),
                       (region.right() + 40 <= fImageWidth)? region.right() + 40: fImageWidth,
                       (region.bottom() + 40 <= fImageHeight)? region.bottom() + 40: fImageHeight); 
        // set rect to enlarged size to fit HW resizer constraint
        rect.set(0,0,fImageWidth, fImageHeight);
        rect.intersect(rectHWResz);
    }
#endif
    cinfo->scale_denom = requestedSampleSize;

    set_dct_method(*this, cinfo);

    const SkColorType colorType = this->getBitmapColorType(cinfo);
    adjust_out_color_space_and_dither(cinfo, colorType, *this);

    int startX = rect.fLeft;
    int startY = rect.fTop;
    int width = rect.width();
    int height = rect.height();

      //XLOGW("SKIA_MT_REGION: wait  init_tile mutex, L:%d!!", __LINE__);    
    {
#if 0 //def MTK_SKIA_MULTI_THREAD_JPEG_REGION       
      SkAutoMutexAcquire ac(gAutoTileInitMutex);
#endif      
      //XLOGW("SKIA_MT_REGION: +  init_tile mutex,%d %d %d %d ,L:%d!!", startX, startY, width, height ,__LINE__);    
      jpeg_init_read_tile_scanline(cinfo, fImageIndex->huffmanIndex(),&startX, &startY, &width, &height);

      //XLOGW("SKIA_MT_REGION: - init_tile mutex, %d %d %d %d ,L:%d!!", startX, startY, width, height , __LINE__);    
    }
    int skiaSampleSize = recompute_sampleSize(requestedSampleSize, *cinfo);
    int actualSampleSize = skiaSampleSize * (DCTSIZE / cinfo->min_DCT_scaled_size);

    SkScaledBitmapSampler sampler(width, height, skiaSampleSize);

    SkBitmap bitmap;
    // Assume an A8 bitmap is not opaque to avoid the check of each
    // individual pixel. It is very unlikely to be opaque, since
    // an opaque A8 bitmap would not be very interesting.
    // Otherwise, a jpeg image is opaque.
    bitmap.setInfo(SkImageInfo::Make(sampler.scaledWidth(), sampler.scaledHeight(), colorType,
                                     kAlpha_8_SkColorType == colorType ?
                                         kPremul_SkAlphaType : kOpaque_SkAlphaType));

    // Check ahead of time if the swap(dest, src) is possible or not.
    // If yes, then we will stick to AllocPixelRef since it's cheaper with the
    // swap happening. If no, then we will use alloc to allocate pixels to
    // prevent garbage collection.
    int w = rect.width() / actualSampleSize;
    int h = rect.height() / actualSampleSize;
    bool swapOnly = (rect == region) && bm->isNull() &&
                    (w == bitmap.width()) && (h == bitmap.height()) &&
                    ((startX - rect.x()) / actualSampleSize == 0) &&
                    ((startY - rect.y()) / actualSampleSize == 0);
    if (swapOnly) {
        if (!this->allocPixelRef(&bitmap, NULL)) {
            return return_false(*cinfo, bitmap, "allocPixelRef");
        }
    } else {
        if (!bitmap.allocPixels()) {
            return return_false(*cinfo, bitmap, "allocPixels");
        }
    }

    SkAutoLockPixels alp(bitmap);

#ifdef ANDROID_RGB
    /* short-circuit the SkScaledBitmapSampler when possible, as this gives
       a significant performance boost.
    */
    //SkDebugf("MTR_JPEG: testmt -- skiaSampleSize = %d , config = %d , ,  L:%d!!\n",skiaSampleSize,config , __LINE__);
    if (skiaSampleSize == 1 &&
        ((kN32_SkColorType == colorType && cinfo->out_color_space == JCS_RGBA_8888) ||
         (kRGB_565_SkColorType == colorType && cinfo->out_color_space == JCS_RGB_565)))
    {
        JSAMPLE* rowptr = (JSAMPLE*)bitmap.getPixels();
        INT32 const bpr = bitmap.rowBytes();
        int rowTotalCount = 0;
        //SkDebugf("MTR_JPEG: testmt -- enter 1  L:%d!!\n" , __LINE__);

        #ifdef MTK_JPEG_HW_REGION_RESIZER 
        uint8_t* hwBuffer ;
        if(!fFirstTileDone || fUseHWResizer){
          hwBuffer = (uint8_t*)srcStorage.reset(bitmap.height() * bitmap.rowBytes() );     
          rowptr = hwBuffer ;
        }
        #endif                        

        //SkDebugf("MTR_JPEG: jpeg_read_tile_scanline 1+ , time = %f , L:%d!!\n",now_ms() - g_mt_start  , __LINE__);

        while (rowTotalCount < height) {
            int rowCount = jpeg_read_tile_scanline(cinfo,
                                                   fImageIndex->huffmanIndex(),
                                                   &rowptr);
            // if rowCount == 0, then we didn't get a scanline, so abort.
            // onDecodeSubset() relies on onBuildTileIndex(), which
            // needs a complete image to succeed.
            if (0 == rowCount) {
                return return_false(*cinfo, bitmap, "read_scanlines");
            }
            if (this->shouldCancelDecode()) {
                return return_false(*cinfo, bitmap, "shouldCancelDecode");
            }
            
            if (JCS_CMYK == cinfo->out_color_space) {
                convert_CMYK_to_RGB(rowptr, bitmap.width());
            }
            rowTotalCount += rowCount;
            rowptr += bpr;
        }

        //SkDebugf("MTR_JPEG: jpeg_read_tile_scanline 1- , time = %f , L:%d!!\n",now_ms() - g_mt_start  , __LINE__);

        #ifdef MTK_JPEG_HW_REGION_RESIZER 
        
        double hw_resize = now_ms() ;
        //SkDebugf("MTR_JPEG: testmt_hw_resize ++ , time = %f , L:%d!!\n",hw_resize - g_mt_start  , __LINE__);
        
        if(!fFirstTileDone || fUseHWResizer)
        {
            //XLOGD("use hw crop : width height (%d %d)-> (%d %d), L:%d!!\n", width, height, bitmap->width(), bitmap->height(), __LINE__);
            XLOGW("SkRegionJPEG::region crop (%d %d)->(%d %d), region (%d %d %d %d), swap %d, L:%d!!\n", bitmap.width(), bitmap.height(), bm->width(), bm->height()
            ,region.x(), region.y(),region.width(), region.height(),swapOnly,__LINE__);	        
            
            int try_times = 5;
            bool result = false;
            do
            {
                result = MDPCrop(hwBuffer, width, height, &bitmap, enTdshp, pParam);
            
                if(!result && ++try_times < 5)
                {
                    XLOGD("Hardware resize fail, sleep 100 us and then try again, L:%d!!\n", __LINE__);
                    usleep(100*1000);
                }
            }while(!result && try_times < 5);
            
            
            if(!result)
            {
                {
                  #ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION 
                    SkAutoMutexAcquire ac(gAutoTileResizeMutex);
                  #endif
                  fFirstTileDone = true;
                }
                XLOGW("Hardware resize fail, use sw crop, L:%d!!\n", __LINE__);
                rowptr = (JSAMPLE*)bitmap.getPixels();
                memcpy(rowptr, hwBuffer,bitmap.height() * bitmap.rowBytes());
            }
            else
            {
                {
                  #ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
                    SkAutoMutexAcquire ac(gAutoTileResizeMutex);
                  #endif
                  fUseHWResizer = true;
                  fFirstTileDone = true;
                }
                XLOGD("Hardware resize successfully, L:%d!!\n", __LINE__);
            }
        } 
        
        if(base_thread_id == gettid()){
            g_mt_hw_sum1 = g_mt_hw_sum1 +(now_ms() - hw_resize);
            //SkDebugf("MTR_JPEG: testmt_hw_resize -- , time = %f ,sum1 = %f , L:%d!!\n",now_ms() - hw_resize, g_mt_hw_sum1  , __LINE__);        
        }else{
            g_mt_hw_sum2 = g_mt_hw_sum2 +(now_ms() - hw_resize);
            //SkDebugf("MTR_JPEG: testmt_hw_resize -- , time = %f ,sum2 = %f , L:%d!!\n",now_ms() - hw_resize, g_mt_hw_sum2  , __LINE__);        

        }
        
        #endif //MTK_JPEG_HW_REGION_RESIZER 

        if (swapOnly) {
            bm->swap(bitmap);
        } else {
            cropBitmap(bm, &bitmap, actualSampleSize, region.x(), region.y(),
                       region.width(), region.height(), startX, startY);
            if (bm->pixelRef() == NULL) {
              XLOGW("SkiaJPEG::cropBitmap allocPixelRef FAIL L:%d !!!!!!\n", __LINE__);
              return return_false(*cinfo, bitmap, "cropBitmap Allocate Pixel Fail!! ");
            }
        }

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
        //SkDebugf("MTR_JPEG: testmt_dinit ++ , time = %f , L:%d!!\n",now_ms() - g_mt_start  , __LINE__);
        SkAutoTDelete<skjpeg_source_mgr> adjpg(sk_stream);
        jpeg_finish_decompress(cinfo);

        jpeg_destroy_decompress(cinfo);
        //SkDebugf("MTR_JPEG: testmt_dinit -- , time = %f , L:%d!!\n",now_ms() - g_mt_start  , __LINE__);
#endif        


		
        #ifdef JPEG_DRAW_RECT
        {
          SkAutoLockPixels alp(*bm);
          unsigned char *drawptr = (unsigned char *)bm->getPixels();
          unsigned int width = bm->width();
          unsigned int height = bm->height();
          unsigned int line = 0;
          unsigned int draw_x=0, draw_y=0 ;
          
          for(draw_y = 0; draw_y < height ; draw_y++){
            
            for(draw_x = 0; draw_x < width ; draw_x++){
              //if(bm->bytesPerPixel() == 4)
              if( ( draw_y == 0 || draw_y == 1) || 
                  ( draw_y == height-1 || draw_y == height-2) || 
                  ( (draw_x == 0 || draw_x == 1) || (draw_x == width -1 || draw_x == width -2) ) )
                *drawptr = 0xFF ;
              drawptr += bm->bytesPerPixel();
            }
            
          }
        }
        #endif
        if(base_thread_id == gettid()){
            g_mt_end = now_ms() - g_mt_start;
        }else{
            g_mt_end_duration_2 = now_ms() - g_mt_start;
        }
        SkDebugf("JPEG: debug_onDecodeSubset -- , dur = %f, dur = %f, all dur = %f , L:%d!!\n", g_mt_end , g_mt_end_duration_2, g_mt_end+g_mt_end_duration_2 ,  __LINE__);
        //XLOGW("SkiaJPEG::return true L:%d !\n", __LINE__);
        return true;
    }
#endif

    // check for supported formats
    SkScaledBitmapSampler::SrcConfig sc;
    int srcBytesPerPixel;

    if (!get_src_config(*cinfo, &sc, &srcBytesPerPixel)) {
        return return_false(*cinfo, *bm, "jpeg colorspace");
    }

    if (!sampler.begin(&bitmap, sc, *this)) {
        return return_false(*cinfo, bitmap, "sampler.begin");
    }

    //SkAutoMalloc  srcStorage(width * srcBytesPerPixel);
    uint8_t* srcRow = (uint8_t*)srcStorage.reset(width * srcBytesPerPixel);

#ifdef MTK_JPEG_HW_REGION_RESIZER //MTK_JPEG_HW_DECODER
if(!fFirstTileDone || fUseHWResizer)
{
    SkAutoMalloc hwStorage;
    uint8_t* hwBuffer = (uint8_t*)srcStorage.reset(width * height * srcBytesPerPixel + 4);

    hwBuffer[width * height * srcBytesPerPixel + 4 - 1] = 0xF0;
    hwBuffer[width * height * srcBytesPerPixel + 4 - 2] = 0xF0;
    hwBuffer[width * height * srcBytesPerPixel + 4 - 3] = 0xF0;
    hwBuffer[width * height * srcBytesPerPixel + 4 - 4] = 0xF0;
    int row_total_count = 0;
    int bpr = width * srcBytesPerPixel;
    JSAMPLE* rowptr = (JSAMPLE*)hwBuffer;
    
    //SkDebugf("MTR_JPEG: jpeg_read_tile_scanline 2+ , time = %f , L:%d!!\n",now_ms() - g_mt_start  , __LINE__);
    
    while (row_total_count < height) {
        int row_count = jpeg_read_tile_scanline(cinfo, fImageIndex->huffmanIndex(), &rowptr);
        // if row_count == 0, then we didn't get a scanline, so abort.
        // if we supported partial images, we might return true in this case
        if (0 == row_count) {
            return return_false(*cinfo, bitmap, "read_scanlines");
        }
        if (this->shouldCancelDecode()) {
            return return_false(*cinfo, bitmap, "shouldCancelDecode");
        }
        
        if (JCS_CMYK == cinfo->out_color_space) {
            convert_CMYK_to_RGB(rowptr, width);
        }
            row_total_count += row_count;
            rowptr += bpr;
    }

    XLOGD("use hw resizer : width height (%d %d)-> (%d %d)", width, height, bitmap.width(), bitmap.height());

    double hw_resize = now_ms() ;
    //SkDebugf("MTR_JPEG: testmt_hw_resize 2++ , time = %f , L:%d!!\n",hw_resize - g_mt_start  , __LINE__);


    int try_times = 5;
    bool result = false;
    do
    {
        result = MDPResizer(hwBuffer, width, height, sc, &bitmap, enTdshp, pParam);

        if(!result && ++try_times < 5)
        {
            XLOGD("Hardware resize fail, sleep 100 us and then try again ");
            usleep(100*1000);
        }
    }while(!result && try_times < 5);

    
    if(!result)
    {

        {
          #ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
            SkAutoMutexAcquire ac(gAutoTileResizeMutex);
          #endif
          fFirstTileDone = true;
        }      
        XLOGW("Hardware resize fail, use sw sampler");

        //  Possibly skip initial rows [sampler.srcY0]
        row_total_count = 0;
        rowptr = (JSAMPLE*)hwBuffer;
        rowptr += (bpr * sampler.srcY0());
        row_total_count += sampler.srcY0();
        for (int y = 0;; y++) {

            if (this->shouldCancelDecode()) {
                return return_false(*cinfo, bitmap, "shouldCancelDecode");
            }

            sampler.next(rowptr);
            if (bitmap.height() - 1 == y) {
                // we're done
                XLOGD("total row count %d\n", row_total_count);
                break;
            }
            rowptr += bpr;
            row_total_count ++;

            rowptr += (bpr * (sampler.srcDY() - 1));
            row_total_count += (sampler.srcDY() - 1);
        }
        
    }
    else
    {
        {
          #ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
            SkAutoMutexAcquire ac(gAutoTileResizeMutex);
          #endif
          fUseHWResizer = true;
          fFirstTileDone = true;
        }
        XLOGD("Hardware resize successfully ");
    }

    
    if(base_thread_id == gettid()){
        g_mt_hw_sum1 = g_mt_hw_sum1 +(now_ms() - hw_resize);
        //SkDebugf("MTR_JPEG: testmt_hw_resize 2-- , time = %f ,sum1 = %f , L:%d!!\n",now_ms() - hw_resize, g_mt_hw_sum1  , __LINE__);        
    }else{
        g_mt_hw_sum2 = g_mt_hw_sum2 +(now_ms() - hw_resize);
        //SkDebugf("MTR_JPEG: testmt_hw_resize 2-- , time = %f ,sum2 = %f , L:%d!!\n",now_ms() - hw_resize, g_mt_hw_sum2  , __LINE__);        
    
    }
    
} else {
#endif
    //  Possibly skip initial rows [sampler.srcY0]
    if (!skip_src_rows_tile(cinfo, fImageIndex->huffmanIndex(), srcRow, sampler.srcY0())) {
        return return_false(*cinfo, bitmap, "skip rows");
    }

    // now loop through scanlines until y == bitmap->height() - 1
    for (int y = 0;; y++) {
        JSAMPLE* rowptr = (JSAMPLE*)srcRow;
        int row_count = jpeg_read_tile_scanline(cinfo, fImageIndex->huffmanIndex(), &rowptr);
        // if row_count == 0, then we didn't get a scanline, so abort.
        // onDecodeSubset() relies on onBuildTileIndex(), which
        // needs a complete image to succeed.
        if (0 == row_count) {
            return return_false(*cinfo, bitmap, "read_scanlines");
        }
        if (this->shouldCancelDecode()) {
            return return_false(*cinfo, bitmap, "shouldCancelDecode");
        }

        if (JCS_CMYK == cinfo->out_color_space) {
            convert_CMYK_to_RGB(srcRow, width);
        }

        sampler.next(srcRow);
        if (bitmap.height() - 1 == y) {
            // we're done
            break;
        }

        if (!skip_src_rows_tile(cinfo, fImageIndex->huffmanIndex(), srcRow,
                                sampler.srcDY() - 1)) {
            return return_false(*cinfo, bitmap, "skip rows");
        }
    }

#ifdef MTK_JPEG_HW_REGION_RESIZER //MTK_JPEG_HW_DECODER
}
#endif


    if (swapOnly) {
        bm->swap(bitmap);
    } else {
        cropBitmap(bm, &bitmap, actualSampleSize, region.x(), region.y(),
                   region.width(), region.height(), startX, startY);
        if (bm->pixelRef() == NULL) {
          XLOGW("SkiaJPEG::cropBitmap allocPixelRef FAIL L:%d !!!!!!\n", __LINE__);			
          return return_false(*cinfo, bitmap, "cropBitmap Allocate Pixel Fail!! ");
        }       
    }

#ifdef MTK_SKIA_MULTI_THREAD_JPEG_REGION
    SkAutoTDelete<skjpeg_source_mgr> adjpg(sk_stream);
    jpeg_finish_decompress(cinfo);
    
    jpeg_destroy_decompress(cinfo);
#endif


    
    #ifdef JPEG_DRAW_RECT
    {
      SkAutoLockPixels alp(*bm);
      unsigned char *drawptr = (unsigned char *)bm->getPixels();
      unsigned int width = bm->width();
      unsigned int height = bm->height();
      unsigned int line = 0;
      unsigned int draw_x=0, draw_y=0 ;
      
      for(draw_y = 0; draw_y < height ; draw_y++){
        
        for(draw_x = 0; draw_x < width ; draw_x++){
          //if(bm->bytesPerPixel() == 4)
          if( ( draw_y == 0 || draw_y == 1) || 
              ( draw_y == height-1 || draw_y == height-2) || 
              ( (draw_x == 0 || draw_x == 1) || (draw_x == width -1 || draw_x == width -2) ) )
            *drawptr = 0xFF ;
          drawptr += bm->bytesPerPixel();
        }
        
      }
    }
    #endif
    
    
    if(base_thread_id == gettid()){
        g_mt_end = now_ms() - g_mt_start;
    }else{
        g_mt_end_duration_2 = now_ms() - g_mt_start;
    }
    SkDebugf("JPEG: debug_onDecodeSubset 2 -- , dur = %f, dur = %f, all dur = %f , L:%d!!\n", g_mt_end , g_mt_end_duration_2, g_mt_end+g_mt_end_duration_2 ,  __LINE__);

    
        
    //XLOGW("SkiaJPEG::return true 2 L:%d !\n", __LINE__);
    
    return true;
}
#endif

///////////////////////////////////////////////////////////////////////////////

#include "SkColorPriv.h"

// taken from jcolor.c in libjpeg
#if 0   // 16bit - precise but slow
    #define CYR     19595   // 0.299
    #define CYG     38470   // 0.587
    #define CYB      7471   // 0.114

    #define CUR    -11059   // -0.16874
    #define CUG    -21709   // -0.33126
    #define CUB     32768   // 0.5

    #define CVR     32768   // 0.5
    #define CVG    -27439   // -0.41869
    #define CVB     -5329   // -0.08131

    #define CSHIFT  16
#else      // 8bit - fast, slightly less precise
    #define CYR     77    // 0.299
    #define CYG     150    // 0.587
    #define CYB      29    // 0.114

    #define CUR     -43    // -0.16874
    #define CUG    -85    // -0.33126
    #define CUB     128    // 0.5

    #define CVR      128   // 0.5
    #define CVG     -107   // -0.41869
    #define CVB      -21   // -0.08131

    #define CSHIFT  8
#endif

static void rgb2yuv_32(uint8_t dst[], SkPMColor c) {
    int r = SkGetPackedR32(c);
    int g = SkGetPackedG32(c);
    int b = SkGetPackedB32(c);

    int  y = ( CYR*r + CYG*g + CYB*b ) >> CSHIFT;
    int  u = ( CUR*r + CUG*g + CUB*b ) >> CSHIFT;
    int  v = ( CVR*r + CVG*g + CVB*b ) >> CSHIFT;

    dst[0] = SkToU8(y);
    dst[1] = SkToU8(u + 128);
    dst[2] = SkToU8(v + 128);
}

static void rgb2yuv_4444(uint8_t dst[], U16CPU c) {
    int r = SkGetPackedR4444(c);
    int g = SkGetPackedG4444(c);
    int b = SkGetPackedB4444(c);

    int  y = ( CYR*r + CYG*g + CYB*b ) >> (CSHIFT - 4);
    int  u = ( CUR*r + CUG*g + CUB*b ) >> (CSHIFT - 4);
    int  v = ( CVR*r + CVG*g + CVB*b ) >> (CSHIFT - 4);

    dst[0] = SkToU8(y);
    dst[1] = SkToU8(u + 128);
    dst[2] = SkToU8(v + 128);
}

static void rgb2yuv_16(uint8_t dst[], U16CPU c) {
#ifdef MTK_AOSP_ENHANCEMENT
    // use precise computation to get better color transform result
    int r = SkPacked16ToR32(c);
    int g = SkPacked16ToG32(c);
    int b = SkPacked16ToB32(c);

    int  y = ( CYR*r + CYG*g + CYB*b ) >> (CSHIFT);
    int  u = ( CUR*r + CUG*g + CUB*b ) >> (CSHIFT);
    int  v = ( CVR*r + CVG*g + CVB*b ) >> (CSHIFT);
#else
    int r = SkGetPackedR16(c);
    int g = SkGetPackedG16(c);
    int b = SkGetPackedB16(c);

    int  y = ( 2*CYR*r + CYG*g + 2*CYB*b ) >> (CSHIFT - 2);
    int  u = ( 2*CUR*r + CUG*g + 2*CUB*b ) >> (CSHIFT - 2);
    int  v = ( 2*CVR*r + CVG*g + 2*CVB*b ) >> (CSHIFT - 2);
#endif

    dst[0] = SkToU8(y);
    dst[1] = SkToU8(u + 128);
    dst[2] = SkToU8(v + 128);
}

///////////////////////////////////////////////////////////////////////////////

typedef void (*WriteScanline)(uint8_t* SK_RESTRICT dst,
                              const void* SK_RESTRICT src, int width,
                              const SkPMColor* SK_RESTRICT ctable);

static void Write_32_YUV(uint8_t* SK_RESTRICT dst,
                         const void* SK_RESTRICT srcRow, int width,
                         const SkPMColor*) {
    const uint32_t* SK_RESTRICT src = (const uint32_t*)srcRow;
    while (--width >= 0) {
#ifdef WE_CONVERT_TO_YUV
        rgb2yuv_32(dst, *src++);
#else
        uint32_t c = *src++;
        dst[0] = SkGetPackedR32(c);
        dst[1] = SkGetPackedG32(c);
        dst[2] = SkGetPackedB32(c);
#endif
        dst += 3;
    }
}

static void Write_4444_YUV(uint8_t* SK_RESTRICT dst,
                           const void* SK_RESTRICT srcRow, int width,
                           const SkPMColor*) {
    const SkPMColor16* SK_RESTRICT src = (const SkPMColor16*)srcRow;
    while (--width >= 0) {
#ifdef WE_CONVERT_TO_YUV
        rgb2yuv_4444(dst, *src++);
#else
        SkPMColor16 c = *src++;
        dst[0] = SkPacked4444ToR32(c);
        dst[1] = SkPacked4444ToG32(c);
        dst[2] = SkPacked4444ToB32(c);
#endif
        dst += 3;
    }
}

static void Write_16_YUV(uint8_t* SK_RESTRICT dst,
                         const void* SK_RESTRICT srcRow, int width,
                         const SkPMColor*) {
    const uint16_t* SK_RESTRICT src = (const uint16_t*)srcRow;
    while (--width >= 0) {
#ifdef WE_CONVERT_TO_YUV
        rgb2yuv_16(dst, *src++);
#else
        uint16_t c = *src++;
        dst[0] = SkPacked16ToR32(c);
        dst[1] = SkPacked16ToG32(c);
        dst[2] = SkPacked16ToB32(c);
#endif
        dst += 3;
    }
}

static void Write_Index_YUV(uint8_t* SK_RESTRICT dst,
                            const void* SK_RESTRICT srcRow, int width,
                            const SkPMColor* SK_RESTRICT ctable) {
    const uint8_t* SK_RESTRICT src = (const uint8_t*)srcRow;
    while (--width >= 0) {
#ifdef WE_CONVERT_TO_YUV
        rgb2yuv_32(dst, ctable[*src++]);
#else
        uint32_t c = ctable[*src++];
        dst[0] = SkGetPackedR32(c);
        dst[1] = SkGetPackedG32(c);
        dst[2] = SkGetPackedB32(c);
#endif
        dst += 3;
    }
}

static WriteScanline ChooseWriter(const SkBitmap& bm) {
    switch (bm.colorType()) {
        case kN32_SkColorType:
            return Write_32_YUV;
        case kRGB_565_SkColorType:
            return Write_16_YUV;
        case kARGB_4444_SkColorType:
            return Write_4444_YUV;
        case kIndex_8_SkColorType:
            return Write_Index_YUV;
        default:
            return NULL;
    }
}

class SkJPEGImageEncoder : public SkImageEncoder {
protected:
    virtual bool onEncode(SkWStream* stream, const SkBitmap& bm, int quality) {
#ifdef TIME_ENCODE
        SkAutoTime atm("JPEG Encode");
#endif

        SkAutoLockPixels alp(bm);
        if (NULL == bm.getPixels()) {
            return false;
        }

        jpeg_compress_struct    cinfo;
        skjpeg_error_mgr        sk_err;
        skjpeg_destination_mgr  sk_wstream(stream);

        // allocate these before set call setjmp
        SkAutoMalloc    oneRow;
        SkAutoLockColors ctLocker;

        cinfo.err = jpeg_std_error(&sk_err);
        sk_err.error_exit = skjpeg_error_exit;
        if (setjmp(sk_err.fJmpBuf)) {
            return false;
        }

        // Keep after setjmp or mark volatile.
        const WriteScanline writer = ChooseWriter(bm);
        if (NULL == writer) {
            return false;
        }

        jpeg_create_compress(&cinfo);
        cinfo.dest = &sk_wstream;
        cinfo.image_width = bm.width();
        cinfo.image_height = bm.height();
        cinfo.input_components = 3;
#ifdef WE_CONVERT_TO_YUV
        cinfo.in_color_space = JCS_YCbCr;
#else
        cinfo.in_color_space = JCS_RGB;
#endif
        cinfo.input_gamma = 1;

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
#ifdef DCT_IFAST_SUPPORTED
        cinfo.dct_method = JDCT_IFAST;
#endif

        jpeg_start_compress(&cinfo, TRUE);

        const int       width = bm.width();
        uint8_t*        oneRowP = (uint8_t*)oneRow.reset(width * 3);

        const SkPMColor* colors = ctLocker.lockColors(bm);
        const void*      srcRow = bm.getPixels();

        while (cinfo.next_scanline < cinfo.image_height) {
            JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */

            writer(oneRowP, srcRow, width, colors);
            row_pointer[0] = oneRowP;
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            srcRow = (const void*)((const char*)srcRow + bm.rowBytes());
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);

        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////
DEFINE_DECODER_CREATOR(JPEGImageDecoder);
DEFINE_ENCODER_CREATOR(JPEGImageEncoder);
///////////////////////////////////////////////////////////////////////////////

static bool is_jpeg(SkStreamRewindable* stream) {
    static const unsigned char gHeader[] = { 0xFF, 0xD8, 0xFF };
    static const size_t HEADER_SIZE = sizeof(gHeader);

    char buffer[HEADER_SIZE];
    size_t len = stream->read(buffer, HEADER_SIZE);

    if (len != HEADER_SIZE) {
        return false;   // can't read enough
    }
    if (memcmp(buffer, gHeader, HEADER_SIZE)) {
        return false;
    }
    return true;
}


static SkImageDecoder* sk_libjpeg_dfactory(SkStreamRewindable* stream) {
    if (is_jpeg(stream)) {
        return SkNEW(SkJPEGImageDecoder);
    }
    return NULL;
}

static SkImageDecoder::Format get_format_jpeg(SkStreamRewindable* stream) {
    if (is_jpeg(stream)) {
        return SkImageDecoder::kJPEG_Format;
    }
    return SkImageDecoder::kUnknown_Format;
}

static SkImageEncoder* sk_libjpeg_efactory(SkImageEncoder::Type t) {
    return (SkImageEncoder::kJPEG_Type == t) ? SkNEW(SkJPEGImageEncoder) : NULL;
}

static SkImageDecoder_DecodeReg gDReg(sk_libjpeg_dfactory);
static SkImageDecoder_FormatReg gFormatReg(get_format_jpeg);
static SkImageEncoder_EncodeReg gEReg(sk_libjpeg_efactory);
