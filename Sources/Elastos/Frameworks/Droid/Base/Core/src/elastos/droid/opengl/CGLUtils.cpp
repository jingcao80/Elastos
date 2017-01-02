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

#include "elastos/droid/opengl/CGLUtils.h"
#include "Elastos.Droid.Graphics.h"
#include <skia/core/SkBitmap.h>
#include <GLES/gl.h>
#include <ETC1/etc1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <elastos/utility/logging/Slogger.h>

namespace android {
extern void setGLDebugLevel(int level);
}

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_SINGLETON_IMPL(CGLUtils)

CAR_INTERFACE_IMPL(CGLUtils, Object, IGLUtils)

ECode CGLUtils::GetInternalFormat(
    /* [in] */ IBitmap* bitmap,
    /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)

    if (bitmap == NULL) {
        SLOGGERD("CGLUtils", "getInternalFormat can't be used with a null Bitmap")
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean recycled;
    bitmap->IsRecycled(&recycled);
    if (recycled) {
        SLOGGERD("CGLUtils", "bitmap is recycled")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 result = Native_getInternalFormat(bitmap);
    if (result < 0) {
        SLOGGERD("CGLUtils", "Unknown internalformat")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *rst = result;
    return NOERROR;
}

ECode CGLUtils::GetType(
    /* [in] */ IBitmap* bitmap,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    if (bitmap == NULL) {
        SLOGGERD("CGLUtils", "texImage2D can't be used with a null Bitmap")
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean recycled;
    if (bitmap->IsRecycled(&recycled), recycled) {
        SLOGGERD("CGLUtils", "bitmap is recycled")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 result = Native_getType(bitmap);
    if (result < 0) {
        SLOGGERD("CGLUtils", "Unknown type")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *type = result;
    return NOERROR;
}

ECode CGLUtils::TexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 border)
{
    if (bitmap == NULL) {
        SLOGGERD("CGLUtils", "texImage2D can't be used with a null Bitmap")
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean recycled;
    if (bitmap->IsRecycled(&recycled), recycled) {
        SLOGGERD("CGLUtils", "bitmap is recycled")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (Native_texImage2D(target, level, internalformat, bitmap, -1, border)!=0) {
        SLOGGERD("CGLUtils", "invalid Bitmap format")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CGLUtils::TexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 type,
    /* [in] */ Int32 border)
{
    if (bitmap == NULL) {
        SLOGGERD("CGLUtils", "texImage2D can't be used with a null Bitmap")
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean recycled;
    if (bitmap->IsRecycled(&recycled), recycled) {
        SLOGGERD("CGLUtils", "bitmap is recycled")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (Native_texImage2D(target, level, internalformat, bitmap, type, border) != 0) {
        SLOGGERD("CGLUtils", "invalid Bitmap format")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CGLUtils::TexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 border)
{
    if (bitmap == NULL) {
        SLOGGERD("CGLUtils", "texImage2D can't be used with a null Bitmap")
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean recycled;
    if (bitmap->IsRecycled(&recycled), recycled) {
        SLOGGERD("CGLUtils", "bitmap is recycled")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (Native_texImage2D(target, level, -1, bitmap, -1, border) != 0) {
        SLOGGERD("CGLUtils", "invalid Bitmap format")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CGLUtils::TexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ IBitmap* bitmap)
{
    return NOERROR;
}

ECode CGLUtils::TexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type)
{
    return NOERROR;
}

ECode CGLUtils::GetEGLErrorString(
    /* [in] */ Int32 error,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return NOERROR;
}

ECode CGLUtils::SetTracingLevel(
    /* [in] */ Int32 level)
{
    android::setGLDebugLevel(level);
    return NOERROR;
}

static int getInternalFormat(SkColorType colorType)
{
    switch(colorType) {
        case kAlpha_8_SkColorType:
            return GL_ALPHA;
        case kARGB_4444_SkColorType:
            return GL_RGBA;
        case kN32_SkColorType:
            return GL_RGBA;
        case kIndex_8_SkColorType:
            return GL_PALETTE8_RGBA8_OES;
        case kRGB_565_SkColorType:
            return GL_RGB;
        default:
            return -1;
    }
}

Int32  CGLUtils::Native_getInternalFormat(
    /* [in] */ IBitmap* bitmap)
{
    Handle64 nativeMapTmp;
    bitmap->GetNativeBitmap(&nativeMapTmp);
    SkBitmap const * nativeBitmap =
            (SkBitmap const *)nativeMapTmp;

    return getInternalFormat(nativeBitmap->colorType());
}

static int getType(SkColorType colorType)
{
    switch(colorType) {
        case kAlpha_8_SkColorType:
            return GL_UNSIGNED_BYTE;
        case kARGB_4444_SkColorType:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        case kN32_SkColorType:
            return GL_UNSIGNED_BYTE;
        case kIndex_8_SkColorType:
            return -1; // No type for compressed data.
        case kRGB_565_SkColorType:
            return GL_UNSIGNED_SHORT_5_6_5;
        default:
            return -1;
    }
}

Int32  CGLUtils::Native_getType(
    /* [in] */ IBitmap* bitmap)
{
    Handle64 nativeMapTmp;
    bitmap->GetNativeBitmap(&nativeMapTmp);
    SkBitmap const * nativeBitmap =
            (SkBitmap const *)nativeMapTmp;

    return getType(nativeBitmap->colorType());
}

static int checkFormat(SkColorType colorType, int format, int type)
{
    switch(colorType) {
        case kIndex_8_SkColorType:
            if (format == GL_PALETTE8_RGBA8_OES)
                return 0;
        case kN32_SkColorType:
        case kAlpha_8_SkColorType:
            if (type == GL_UNSIGNED_BYTE)
                return 0;
        case kARGB_4444_SkColorType:
        case kRGB_565_SkColorType:
            switch (type) {
                case GL_UNSIGNED_SHORT_4_4_4_4:
                case GL_UNSIGNED_SHORT_5_6_5:
                case GL_UNSIGNED_SHORT_5_5_5_1:
                    return 0;
                case GL_UNSIGNED_BYTE:
                    if (format == GL_LUMINANCE_ALPHA)
                        return 0;
            }
            break;
        default:
            break;
    }
    return -1;
}

Int32  CGLUtils::Native_texImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 type,
    /* [in] */ Int32 border)
{
    Handle64 nativeMapTmp;
    bitmap->GetNativeBitmap(&nativeMapTmp);
    SkBitmap const * nativeBitmap =
            (SkBitmap const *)nativeMapTmp;
    const SkBitmap& skBitmap(*nativeBitmap);
    SkColorType colorType = skBitmap.colorType();
    if (internalformat < 0) {
        internalformat = getInternalFormat(colorType);
    }
    if (type < 0) {
        type = getType(colorType);
    }
    int err = checkFormat(colorType, internalformat, type);
    if (err)
        return err;
    skBitmap.lockPixels();
    const int w = skBitmap.width();
    const int h = skBitmap.height();
    const void* p = skBitmap.getPixels();
    if (internalformat == GL_PALETTE8_RGBA8_OES) {
        if (sizeof(SkPMColor) != sizeof(uint32_t)) {
            err = -1;
            goto error;
        }
        const size_t size = skBitmap.getSize();
        const size_t palette_size = 256*sizeof(SkPMColor);
        const size_t imageSize = size + palette_size;
        void* const data = malloc(imageSize);
        if (data) {
            void* const pixels = (char*)data + palette_size;
            SkColorTable* ctable = skBitmap.getColorTable();
            memcpy(data, ctable->lockColors(), ctable->count() * sizeof(SkPMColor));
            memcpy(pixels, p, size);
            ctable->unlockColors();
            glCompressedTexImage2D(target, level, internalformat, w, h, border, imageSize, data);
            free(data);
        } else {
            err = -1;
        }
    } else {
        glTexImage2D(target, level, internalformat, w, h, border, internalformat, type, p);
    }
error:
    skBitmap.unlockPixels();
    return err;
}

Int32  CGLUtils::Native_texSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type)
{
    Handle64 nativeMapTmp;
    bitmap->GetNativeBitmap(&nativeMapTmp);
    SkBitmap const * nativeBitmap =
            (SkBitmap const *)nativeMapTmp;
    const SkBitmap& skBitmap(*nativeBitmap);
    SkColorType colorType = skBitmap.colorType();
    if (format < 0) {
        format = getInternalFormat(colorType);
        if (format == GL_PALETTE8_RGBA8_OES)
            return -1; // glCompressedTexSubImage2D() not supported
    }
    int err = checkFormat(colorType, format, type);
    if (err)
        return err;
    skBitmap.lockPixels();
    const int w = skBitmap.width();
    const int h = skBitmap.height();
    const void* p = skBitmap.getPixels();
    glTexSubImage2D(target, level, xoffset, yoffset, w, h, format, type, p);
    skBitmap.unlockPixels();
    return 0;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

