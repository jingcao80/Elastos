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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/PixelFormat.h"
#include <ui/PixelFormat.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(PixelFormat, Object, IPixelFormat)

PixelFormat::PixelFormat()
    : mBytesPerPixel(0)
    , mBitsPerPixel(0)
{}

ECode PixelFormat::constructor()
{
    return NOERROR;
}

Boolean PixelFormat::FormatHasAlpha(
    /* [in] */ Int32 format)
{
    switch (format) {
        case IPixelFormat::A_8:
        case IPixelFormat::LA_88:
        case IPixelFormat::RGBA_4444:
        case IPixelFormat::RGBA_5551:
        case IPixelFormat::RGBA_8888:
        case IPixelFormat::TRANSLUCENT:
        case IPixelFormat::TRANSPARENT:
            return TRUE;
    }
    return FALSE;
}

ECode PixelFormat::GetPixelFormatInfo(
    /* [in] */ Int32 format,
    /* [in] */ IPixelFormat* infoObj)
{
    PixelFormat* info = (PixelFormat*)infoObj;
    switch (format) {
        case IPixelFormat::RGBA_8888:
        case IPixelFormat::RGBX_8888:
            info->mBitsPerPixel = 32;
            info->mBytesPerPixel = 4;
            break;
        case IPixelFormat::RGB_888:
            info->mBitsPerPixel = 24;
            info->mBytesPerPixel = 3;
            break;
        case IPixelFormat::RGB_565:
        case IPixelFormat::RGBA_5551:
        case IPixelFormat::RGBA_4444:
        case IPixelFormat::LA_88:
            info->mBitsPerPixel = 16;
            info->mBytesPerPixel = 2;
            break;
        case IPixelFormat::A_8:
        case IPixelFormat::L_8:
        case IPixelFormat::RGB_332:
            info->mBitsPerPixel = 8;
            info->mBytesPerPixel = 1;
            break;
        case IPixelFormat::YCbCr_422_SP:
        case IPixelFormat::YCbCr_422_I:
            info->mBitsPerPixel = 16;
            info->mBytesPerPixel = 1;
            break;
        case IPixelFormat::YCbCr_420_SP:
            info->mBitsPerPixel = 12;
            info->mBytesPerPixel = 1;
            break;
        default:
            // throw new IllegalArgumentException("unknown pixel format " + format);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Boolean PixelFormat::IsPublicFormat(
    /* [in] */ Int32 format)
{
    switch (format) {
        case IPixelFormat::RGBA_8888:
        case IPixelFormat::RGBX_8888:
        case IPixelFormat::RGB_888:
        case IPixelFormat::RGB_565:
            return TRUE;
    }

    return FALSE;
}

ECode PixelFormat::SetBitsPerPixel(
    /* [in] */ Int32 bits)
{
    mBitsPerPixel = bits;
    return NOERROR;
}

ECode PixelFormat::GetBitsPerPixel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBitsPerPixel;
    return NOERROR;
}

ECode PixelFormat::SetBytesPerPixel(
    /* [in] */ Int32 bytes)
{
    mBytesPerPixel = bytes;
    return NOERROR;
}

ECode PixelFormat::GetBytesPerPixel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBytesPerPixel;
    return NOERROR;
}


} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
