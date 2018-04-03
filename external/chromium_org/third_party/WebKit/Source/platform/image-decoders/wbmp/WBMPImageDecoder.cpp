/*
 * Copyright (c) 2008, 2009, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/image-decoders/wbmp/WBMPImageDecoder.h"

#include "platform/PlatformInstrumentation.h"
#include "SkColorPriv.h"

namespace blink {

static const size_t sizeOfFileHeader = 4;

WBMPImageDecoder::WBMPImageDecoder(ImageSource::AlphaOption alphaOption,
                                 ImageSource::GammaAndColorProfileOption gammaAndColorProfileOption,
                                 size_t maxDecodedBytes)
    : ImageDecoder(alphaOption, gammaAndColorProfileOption, maxDecodedBytes)
    , m_decodedOffset(0)
{
}

bool WBMPImageDecoder::isSizeAvailable()
{
    if (!ImageDecoder::isSizeAvailable())
        decode(true);

    return ImageDecoder::isSizeAvailable();
}

ImageFrame* WBMPImageDecoder::frameBufferAtIndex(size_t index)
{
    if (index)
        return 0;

    if (m_frameBufferCache.isEmpty()) {
        m_frameBufferCache.resize(1);
        m_frameBufferCache.first().setPremultiplyAlpha(m_premultiplyAlpha);
    }

    ImageFrame* buffer = &m_frameBufferCache.first();
    if (buffer->status() != ImageFrame::FrameComplete) {
        PlatformInstrumentation::willDecodeImage("WBMP");
        decode(false);
        PlatformInstrumentation::didDecodeImage();
    }
    return buffer;
}

#define PACK_WHITE SkPackARGB32NoCheck(0xFF, 0xFF, 0xFF, 0xFF)
#define PACK_BLACK SkPackARGB32NoCheck(0xFF, 0x0, 0x0, 0x0)

void WBMPImageDecoder::procssRow(uint32_t* dst, unsigned bits)
{
    int bytes = bits >> 3;

    for (int i = 0; i < bytes; i++) {
        unsigned mask = m_data->data()[m_decodedOffset++];
        for (int j = 7; j >= 0; j--)
            *dst++ = ((mask >> j) & 1) ? PACK_WHITE : PACK_BLACK;
    }

    bits &= 7;
    if (bits > 0) {
        unsigned mask = m_data->data()[m_decodedOffset++];
        do {
            *dst++ = ((mask >> 7) & 1) ? PACK_WHITE : PACK_BLACK;
            mask <<= 1;
        } while (--bits != 0);
    }
}

bool WBMPImageDecoder::decode(bool onlySize)
{
    if (failed())
        return false;

    WBMP_Header header;

    if (!m_data.get() || m_data->size() < sizeOfFileHeader || !header.init(m_data.get()))
        return setFailed();

    m_decodedOffset = header.m_offset;
    unsigned width = header.m_width;
    unsigned height = header.m_height;

    setSize(width, height);

    if (onlySize)
        return true;

    ImageFrame* buffer = &m_frameBufferCache.first();

    if (buffer->status() == ImageFrame::FrameEmpty) {
        if (!buffer->setSize(width, height))
            return setFailed(); // Unable to allocate.
        buffer->setStatus(ImageFrame::FramePartial);
        // setSize() calls eraseARGB(), which resets the alpha flag, so we force
        // it back to false here.  We'll set it true below in all cases where
        // these 0s could actually show through.
        buffer->setHasAlpha(false);

        // For WBMPs, the frame always fills the entire image.
        buffer->setOriginalFrameRect(IntRect(0, 0, width, height));
    }

    // Decode row by row
    for (unsigned y = 0; y < height; y++) {
        uint32_t* dst = buffer->getSkBitmap().getAddr32(0, y);
        procssRow(dst, width);
    }

    // Done!
    buffer->setStatus(ImageFrame::FrameComplete);

    return true;
}


} // namespace blink
