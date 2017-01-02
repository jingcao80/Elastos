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

#ifndef __ELASTOS_DROID_GRAPHICS_BITMAPFACTORY_H__
#define __ELASTOS_DROID_GRAPHICS_BITMAPFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Content::Res::IResources;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Graphics {

class BitmapFactory
{
public:
    static CARAPI DecodeFile(
        /* [in] */ const String& pathName,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeFile(
        /* [in] */ const String& pathName,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeResourceStream(
        /* [in] */ IResources* res,
        /* [in] */ ITypedValue* value,
        /* [in] */ IInputStream* is,
        /* [in] */ IRect* pad,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeResource(
        /* [in] */ IResources* res,
        /* [in] */ Int32 id,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeResource(
        /* [in] */ IResources* res,
        /* [in] */ Int32 id,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ IBitmap** bitmap);

    /**
     * Decode an input stream into a bitmap. If the input stream is null, or
     * cannot be used to decode a bitmap, the function returns null.
     * The stream's position will be where ever it was after the encoded data
     * was read.
     *
     * @param is The input stream that holds the raw data to be decoded into a
     *           bitmap.
     * @param outPadding If not null, return the padding rect for the bitmap if
     *                   it exists, otherwise set padding to [-1,-1,-1,-1]. If
     *                   no bitmap is returned (null) then padding is
     *                   unchanged.
     * @param opts null-ok; Options that control downsampling and whether the
     *             image should be completely decoded, or just is size returned.
     * @return The decoded bitmap, or null if the image data could not be
     *         decoded, or, if opts is non-null, if opts requested only the
     *         size be returned (in opts.outWidth and opts.outHeight)
     *
     * <p class="note">Prior to {@link android.os.Build.VERSION_CODES#KITKAT},
     * if {@link InputStream#markSupported is.markSupported()} returns true,
     * <code>is.mark(1024)</code> would be called. As of
     * {@link android.os.Build.VERSION_CODES#KITKAT}, this is no longer the case.</p>
     */
    static CARAPI DecodeStream(
        /* [in] */ IInputStream* is,
        /* [in] */ IRect* outPadding,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IBitmap** bitmap);

    /**
     * Decode an input stream into a bitmap. If the input stream is null, or
     * cannot be used to decode a bitmap, the function returns null.
     * The stream's position will be where ever it was after the encoded data
     * was read.
     *
     * @param is The input stream that holds the raw data to be decoded into a
     *           bitmap.
     * @return The decoded bitmap, or null if the image data could not be decoded.
     */
    static CARAPI DecodeStream(
        /* [in] */ IInputStream* is,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeFileDescriptor(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IRect* outPadding,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IBitmap** bitmap);

    static CARAPI DecodeFileDescriptor(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IBitmap** bitmap);

private:
    // static CARAPI_(AutoPtr<IBitmap>) FinishDecode(
    //     /* [in] */ IBitmap* bm,
    //     /* [in] */ IRect* outPadding,
    //     /* [in] */ IBitmapFactoryOptions* opts);

    static CARAPI_(AutoPtr<IBitmap>) NativeDecodeStream(
        /* [in] */ IInputStream* is,
        /* [in] */ ArrayOf<Byte>* storage,
        /* [in] */ IRect* padding,
        /* [in] */ IBitmapFactoryOptions* opts);

    static CARAPI_(AutoPtr<IBitmap>) NativeDecodeFileDescriptor(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IRect* padding,
        /* [in] */ IBitmapFactoryOptions* opts);

    static CARAPI_(AutoPtr<IBitmap>) NativeDecodeAsset(
        /* [in] */ Int64 asset,
        /* [in] */ IRect* padding,
        /* [in] */ IBitmapFactoryOptions* opts);

    static CARAPI_(AutoPtr<IBitmap>) NativeDecodeByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ IBitmapFactoryOptions* opts);

    // static CARAPI_(void) NativeScaleNinePatch(
    //     /* [in] */ ArrayOf<Byte>* chunkObject,
    //     /* [in] */ Float scale,
    //     /* [in] */ IRect* padding);

    static CARAPI_(Boolean) NativeIsSeekable(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Set the newly decoded bitmap's density based on the Options.
     */
    static CARAPI_(void) SetDensityFromOptions(
        /* [in] */ IBitmap* outputBitmap,
        /* [in] */ IBitmapFactoryOptions* opts);

    /**
     * Private helper function for decoding an InputStream natively. Buffers the input enough to
     * do a rewind as needed, and supplies temporary storage if necessary. is MUST NOT be null.
     */
    static CARAPI_(AutoPtr<IBitmap>) DecodeStreamInternal(
        /* [in] */ IInputStream* is,
        /* [in] */ IRect* outPadding,
        /* [in] */ IBitmapFactoryOptions* opts);

private:
    static const Int32 DECODE_BUFFER_SIZE;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_BITMAPFACTORY_H__
