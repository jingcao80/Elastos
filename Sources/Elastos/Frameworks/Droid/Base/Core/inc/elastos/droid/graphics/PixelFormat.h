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

#ifndef __ELASTOS_DROID_GRAPHICS_PIXELFORMAT_H__
#define __ELASTOS_DROID_GRAPHICS_PIXELFORMAT_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

class PixelFormat
    : public Object
    , public IPixelFormat
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(Boolean) FormatHasAlpha(
        /* [in] */ Int32 format);

    static CARAPI GetPixelFormatInfo(
        /* [in] */ Int32 format,
        /* [in] */ IPixelFormat* info);

    /**
     * Determine whether or not this is a public-visible and non-deprecated {@code format}.
     *
     * <p>In particular, {@code @hide} formats will return {@code false}.</p>
     *
     * <p>Any other indirect formats (such as {@code TRANSPARENT} or {@code TRANSLUCENT})
     * will return {@code false}.</p>
     *
     * @param format an integer format
     * @return a boolean
     *
     * @hide
     */
    static CARAPI_(Boolean) IsPublicFormat(
        /* [in] */ Int32 format);

    PixelFormat();

    CARAPI constructor();

    CARAPI SetBitsPerPixel(
        /* [in] */ Int32 bits);

    CARAPI GetBitsPerPixel(
        /* [out] */ Int32* result);

    CARAPI SetBytesPerPixel(
        /* [in] */ Int32 bytes);

    CARAPI GetBytesPerPixel(
        /* [out] */ Int32* result);

public:
    Int32  mBytesPerPixel;
    Int32  mBitsPerPixel;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_PIXELFORMAT_H__
