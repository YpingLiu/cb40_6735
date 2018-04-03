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

#ifndef WBMPImageDecoder_h
#define WBMPImageDecoder_h

#include "platform/image-decoders/ImageDecoder.h"
#include "SkTypes.h"

namespace blink {

    // This class decodes the WBMP image format.
class PLATFORM_EXPORT WBMPImageDecoder : public ImageDecoder {
public:
    WBMPImageDecoder(ImageSource::AlphaOption, ImageSource::GammaAndColorProfileOption, size_t maxDecodedBytes);

    // ImageDecoder
    virtual String filenameExtension() const OVERRIDE { return "wbmp"; }
    virtual bool isSizeAvailable() OVERRIDE;
    virtual ImageFrame* frameBufferAtIndex(size_t) OVERRIDE;

    struct WBMP_Header {
        unsigned m_width;
        unsigned m_height;
        int m_offset;

        bool init(SharedBuffer* data)
        {
            uint8_t value;
            m_offset = 0;
            value = read_byte(data, m_offset++);
            if (value != 0) // 0 for monochrome bitmaps.
                return false;

            value = read_byte(data, m_offset++);
            if (value != 0) // Fixed header, always 0.
                return false;

            m_width = (unsigned) read_mbf(data, &m_offset);
            m_height = (unsigned) read_mbf(data, &m_offset);

            size_t srcRB = SkAlign8(m_width) >> 3;
            size_t srcSize = m_height * srcRB + m_offset;
            size_t dataSize = data->size();

            // Check the data size, only return true when data fully received.
            if (srcSize != dataSize)
                return false;
            return m_width != 0 && m_height != 0 && m_width < 0xFFFF && m_height < 0xFFFF;
        }
    };

private:
    // Read a value from |data[offset]|, converting from little to native
    // endianness.
    static inline uint8_t read_byte(SharedBuffer* data, int offset)
    {
        uint8_t result;
        memcpy(&result, &data->data()[offset], 1);
        return result;
    }

    static inline int read_mbf(SharedBuffer* data, int* offset)
    {
        int n = 0;
        uint8_t result;
        do {
            result = read_byte(data, (*offset)++);
            n = (n << 7) | (result & 0x7F);
        } while (result & 0x80);
        return n;
    }

    void procssRow(uint32_t*, unsigned bits);
    // Decodes the image.  If |onlySize| is true, stops decoding after
    // calculating the image size.  If decoding fails but there is no more
    // data coming, sets the "decode failure" flag.
    bool decode(bool onlySize);

    // An index into |m_data| representing how much we've already decoded.
    // Note that this only tracks data _this_ class decodes; once the
    // BMPImageReader takes over this will not be updated further.
    size_t m_decodedOffset;

};

} // namespace blink

#endif
