//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_GRAPHICS_YUVTOJPEGENCODER_H__
#define __ELASTOS_DROID_GRAPHICS_YUVTOJPEGENCODER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <skia/core/SkStream.h>
extern "C" {
#include <jpeg/jpeglib.h>
}

namespace Elastos {
namespace Droid {
namespace Graphics {

class YuvToJpegEncoder
{
public:
    /** Create an encoder based on the YUV format.
     *
     *  @param pixelFormat The yuv pixel format as defined in ui/PixelFormat.h.
     *  @param strides The number of row bytes in each image plane.
     *  @return an encoder based on the pixelFormat.
     */
    static CARAPI_(YuvToJpegEncoder*) Create(
        /* [in] */ Int32 pixelFormat,
        /* [in] */ Int32* strides);

    YuvToJpegEncoder(
        /* [in] */ Int32* strides);

    /** Encode YUV data to jpeg,  which is output to a stream.
     *
     *  @param stream The jpeg output stream.
     *  @param inYuv The input yuv data.
     *  @param width Width of the the Yuv data in terms of pixels.
     *  @param height Height of the Yuv data in terms of pixels.
     *  @param offsets The offsets in each image plane with respect to inYuv.
     *  @param jpegQuality Picture quality in [0, 100].
     *  @return true if successfully compressed the stream.
     */
    CARAPI_(Boolean) Encode(
        /* [in] */ SkWStream* stream,
        /* [in] */ void* inYuv,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32* offsets,
        /* [in] */ Int32 jpegQuality);

    virtual ~YuvToJpegEncoder() {}

protected:
    CARAPI_(void) SetJpegCompressStruct(
        /* [in] */ jpeg_compress_struct* cinfo,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 quality);

    virtual CARAPI_(void) ConfigSamplingFactors(
        /* [in] */ jpeg_compress_struct* cinfo) = 0;

    virtual CARAPI_(void) Compress(
        /* [in] */ jpeg_compress_struct* cinfo,
        /* [in] */ uint8_t* yuv,
        /* [in] */ Int32* offsets) = 0;

protected:
    Int32 mNumPlanes;
    Int32* mStrides;
};

class Yuv420SpToJpegEncoder : public YuvToJpegEncoder
{
public:
    Yuv420SpToJpegEncoder(
        /* [in] */ Int32* strides);

    virtual ~Yuv420SpToJpegEncoder() {}

private:
    CARAPI_(void) ConfigSamplingFactors(
        /* [in] */ jpeg_compress_struct* cinfo);

    CARAPI_(void) DeinterleaveYuv(
        /* [in] */ uint8_t* yuv,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ uint8_t*& yPlanar,
        /* [in] */ uint8_t*& uPlanar,
        /* [in] */ uint8_t*& vPlanar);

    CARAPI_(void) Deinterleave(
        /* [in] */ uint8_t* vuPlanar,
        /* [in] */ uint8_t* uRows,
        /* [in] */ uint8_t* vRows,
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) Compress(
        /* [in] */ jpeg_compress_struct* cinfo,
        /* [in] */ uint8_t* yuv,
        /* [in] */ Int32* offsets);
};

class Yuv422IToJpegEncoder : public YuvToJpegEncoder
{
public:
    Yuv422IToJpegEncoder(
        /* [in] */ Int32* strides);

    virtual ~Yuv422IToJpegEncoder() {}

private:
    CARAPI_(void) ConfigSamplingFactors(
        /* [in] */ jpeg_compress_struct* cinfo);

    CARAPI_(void) Compress(
        /* [in] */ jpeg_compress_struct* cinfo,
        /* [in] */ uint8_t* yuv,
        /* [in] */ Int32* offsets);

    CARAPI_(void) Deinterleave(
        /* [in] */ uint8_t* yuv,
        /* [in] */ uint8_t* yRows,
        /* [in] */ uint8_t* uRows,
        /* [in] */ uint8_t* vRows,
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_YUVTOJPEGENCODER_H__
