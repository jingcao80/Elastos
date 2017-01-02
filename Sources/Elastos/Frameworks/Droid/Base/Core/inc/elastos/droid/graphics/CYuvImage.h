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

#ifndef __ELASTOS_DROID_GRAPHICS_CYUVIMAGE_H__
#define __ELASTOS_DROID_GRAPHICS_CYUVIMAGE_H__

#include "_Elastos_Droid_Graphics_CYuvImage.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/Object.h>

using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * YuvImage contains YUV data and provides a method that compresses a region of
 * the YUV data to a Jpeg. The YUV data should be provided as a single byte
 * array irrespective of the number of image planes in it.
 * Currently only ImageFormat.NV21 and ImageFormat.YUY2 are supported.
 *
 * To compress a rectangle region in the YUV data, users have to specify the
 * region by left, top, width and height.
 */
CarClass(CYuvImage)
    , public Object
    , public IYuvImage
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Construct an YuvImage.
     *
     * @param yuv     The YUV data. In the case of more than one image plane, all the planes must be
     *                concatenated into a single byte array.
     * @param format  The YUV data format as defined in {@link ImageFormat}.
     * @param width   The width of the YuvImage.
     * @param height  The height of the YuvImage.
     * @param strides (Optional) Row bytes of each image plane. If yuv contains padding, the stride
     *                of each image must be provided. If strides is null, the method assumes no
     *                padding and derives the row bytes by format and width itself.
     * @throws IllegalArgumentException if format is not support; width or height <= 0; or yuv is
     *                null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* yuv,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ArrayOf<Int32>* strides);

    /**
     * Compress a rectangle region in the YuvImage to a jpeg.
     * Only ImageFormat.NV21 and ImageFormat.YUY2
     * are supported for now.
     *
     * @param rectangle The rectangle region to be compressed. The medthod checks if rectangle is
     *                  inside the image. Also, the method modifies rectangle if the chroma pixels
     *                  in it are not matched with the luma pixels in it.
     * @param quality   Hint to the compressor, 0-100. 0 meaning compress for
     *                  small size, 100 meaning compress for max quality.
     * @param stream    OutputStream to write the compressed data.
     * @return          True if the compression is successful.
     * @throws IllegalArgumentException if rectangle is invalid; quality is not within [0,
     *                  100]; or stream is null.
     */
    CARAPI CompressToJpeg(
        /* [in] */ IRect* rectangle,
        /* [in] */ Int32 quality,
        /* [in] */ IOutputStream* stream,
        /* [out] */ Boolean* result);

    /**
     * @return the height of the image.
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * @return the number of row bytes in each image plane.
     */
    CARAPI GetStrides(
        /* [out, callee] */ ArrayOf<Int32>** strides);

    /**
     * @return the width of the image.
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * @return the YUV data.
     */
    CARAPI GetYuvData(
        /* [out, callee] */ ArrayOf<Byte>** data);

    /**
     * @return the YUV format as defined in {@link PixelFormat}.
     */
    CARAPI GetYuvFormat(
        /* [out] */ Int32* format);

    CARAPI_(AutoPtr< ArrayOf<Int32> >) CalculateOffsets(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top);

private:
    CARAPI_(AutoPtr< ArrayOf<Int32> >) CalculateStrides(
        /* [in] */ Int32 width,
        /* [in] */ Int32 format);

    CARAPI_(void) AdjustRectangle(
        /* [in] */ IRect* rect);

    static CARAPI_(Boolean) NativeCompressToJpeg(
        /* [in] */ ArrayOf<Byte>* oriYuv,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ArrayOf<Int32>* offsets,
        /* [in] */ ArrayOf<Int32>* strides,
        /* [in] */ Int32 quality,
        /* [in] */ IOutputStream* stream,
        /* [out] */ ArrayOf<Byte>* tempStorage);

private:
    /**
     * Number of bytes of temp storage we use for communicating between the
     * native compressor and the java OutputStream.
     */
    static const Int32 WORKING_COMPRESS_STORAGE = 4096;

    /**
     * The YUV format as defined in {@link PixelFormat}.
     */
    Int32 mFormat;

    /**
     * The raw YUV data.
     * In the case of more than one image plane, the image planes must be
     * concatenated into a single byte array.
     */
    AutoPtr< ArrayOf<Byte> > mData;

    /**
     * The number of row bytes in each image plane.
     */
    AutoPtr< ArrayOf<Int32> > mStrides;

    /**
     * The width of the image.
     */
    Int32 mWidth;

    /**
     * The height of the the image.
     */
    Int32 mHeight;

};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CYUVIMAGE_H__
