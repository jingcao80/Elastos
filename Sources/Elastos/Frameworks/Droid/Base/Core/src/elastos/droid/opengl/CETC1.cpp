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

#include "elastos/droid/opengl/CETC1.h"

#include "utils/misc.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <skia/core/SkBitmap.h>
#include <gl.h>
#include <ETC1/etc1.h>
#include <elastos/utility/logging/Slogger.h>

#define LOGD(msg) SLOGGERD("CETC1", msg)

using Elastos::IO::INIOAccess;
using Elastos::IO::CNIOAccess;

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(CETC1, Singleton, IETC1)

CAR_SINGLETON_IMPL(CETC1)

ECode CETC1::GetPointer(
    /* [in] */ Elastos::IO::IBuffer* buffer,
    /* [in, out] */ Int32 *remaining,
    /* [out] */ Handle32* rst)
{
    VALIDATE_NOT_NULL(rst)

    Int32 position;
    Int32 limit;
    Int32 elementSizeShift;
    Int64 pointer;

    buffer->GetPosition(&position);
    buffer->GetLimit(&limit);
    buffer->GetElementSizeShift(&elementSizeShift);
    *remaining = (limit - position) << elementSizeShift;

    AutoPtr<INIOAccess> helper;
    CNIOAccess::AcquireSingleton((INIOAccess**)&helper);

    helper->GetBasePointer(buffer, &pointer);
    if (pointer != 0L) {
        *rst = (Handle32)pointer;
        return NOERROR;
    }
    *rst = 0;
    return NOERROR;
}

ECode CETC1::EncodeBlock(
    /* [in] */ Elastos::IO::IBuffer* inBuf,
    /* [in] */ Int32 validPixelMask,
    /* [in] */ Elastos::IO::IBuffer* outBuf)
{
    if (validPixelMask < 0 || validPixelMask > 15) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    BufferHelper inB(inBuf);
    BufferHelper outB(outBuf);
    Boolean inCheck, outCheck;
    inB.CheckPointer("in", &inCheck);
    outB.CheckPointer("out", &outCheck);
    if (inCheck && outCheck) {
        if (inB.Remaining() < ETC1_DECODED_BLOCK_SIZE) {
            LOGD("EncodeBlock: in's remaining data < DECODED_BLOCK_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if (outB.Remaining() < ETC1_ENCODED_BLOCK_SIZE) {
            LOGD("EncodeBlock: out's remaining data < ENCODED_BLOCK_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            etc1_encode_block((etc1_byte*) inB.GetData(), validPixelMask,
                    (etc1_byte*) outB.GetData());
        }
    }
    return NOERROR;
}

ECode CETC1::DecodeBlock(
    /* [in] */ Elastos::IO::IBuffer* inBuf,
    /* [in] */ Elastos::IO::IBuffer* outBuf)
{
    BufferHelper inB(inBuf);
    BufferHelper outB(outBuf);
    Boolean inCheck, outCheck;
    inB.CheckPointer("in", &inCheck);
    outB.CheckPointer("out", &outCheck);
    if (inCheck && outCheck) {
        if (inB.Remaining() < ETC1_ENCODED_BLOCK_SIZE) {
            LOGD("DecodeBlock: in's remaining data < ENCODED_BLOCK_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if (outB.Remaining() < ETC1_DECODED_BLOCK_SIZE) {
            LOGD("DecodeBlock: out's remaining data < DECODED_BLOCK_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            etc1_decode_block((etc1_byte*) inB.GetData(),
                    (etc1_byte*) outB.GetData());
        }
    }
    return NOERROR;
}

ECode CETC1::GetEncodedDataSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    *size = etc1_get_encoded_data_size(width, height);
    return NOERROR;
}

ECode CETC1::EncodeImage(
    /* [in] */ Elastos::IO::IBuffer* inBuf,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelSize,
    /* [in] */ Int32 stride,
    /* [in] */ Elastos::IO::IBuffer* outBuf)
{
    if (pixelSize < 2 || pixelSize > 3) {
        LOGD("EncodeImage: pixelSize must be 2 or 3")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    BufferHelper inB(inBuf);
    BufferHelper outB(outBuf);
    Boolean inCheck, outCheck;
    inB.CheckPointer("in", &inCheck);
    outB.CheckPointer("out", &outCheck);
    if (inCheck && outCheck) {
        Int32 imageSize = stride * height;
        Int32 encodedImageSize = etc1_get_encoded_data_size(width, height);
        if (inB.Remaining() < imageSize) {
            LOGD("EncodeImage: in's remaining data < image size")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if (outB.Remaining() < encodedImageSize) {
            LOGD("EncodeImage: out's remaining data < encoded image size")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            etc1_encode_image((etc1_byte*) inB.GetData(),
                    width, height, pixelSize,
                    stride,
                    (etc1_byte*) outB.GetData());
        }
    }
    return NOERROR;
}

ECode CETC1::DecodeImage(
    /* [in] */ Elastos::IO::IBuffer* inBuf,
    /* [in] */ Elastos::IO::IBuffer* outBuf,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelSize,
    /* [in] */ Int32 stride)
{
    if (pixelSize < 2 || pixelSize > 3) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    BufferHelper inB(inBuf);
    BufferHelper outB(outBuf);
    Boolean inCheck, outCheck;
    inB.CheckPointer("in", &inCheck);
    outB.CheckPointer("out", &outCheck);
    if (inCheck && outCheck) {
        Int32 imageSize = stride * height;
        Int32 encodedImageSize = etc1_get_encoded_data_size(width, height);
        if (inB.Remaining() < encodedImageSize) {
            LOGD("DecodeImage: in's remaining data < encoded image size")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if (outB.Remaining() < imageSize) {
            LOGD("DecodeImage: out's remaining data < image size")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            etc1_decode_image((etc1_byte*) inB.GetData(),
                    (etc1_byte*) outB.GetData(),
                    width, height, pixelSize,
                    stride);
        }
    }
    return NOERROR;
}

ECode CETC1::FormatHeader(
    /* [in] */ Elastos::IO::IBuffer* header,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    BufferHelper headerB(header);
    Boolean headerCheck;
    if (headerB.CheckPointer("header", &headerCheck), headerCheck)
    {
        if (headerB.Remaining() < ETC_PKM_HEADER_SIZE) {
            LOGD("FormatHeader: header's remaining data < ETC_PKM_HEADER_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            etc1_pkm_format_header((etc1_byte*) headerB.GetData(), width, height);
        }
    }
    return NOERROR;
}

ECode CETC1::IsValid(
    /* [in] */ Elastos::IO::IBuffer* header,
    /* [out] */ Boolean* isvalid)
{
    VALIDATE_NOT_NULL(isvalid)

    Boolean result = FALSE;
    BufferHelper headerB(header);
    Boolean headerCheck;
    if (headerB.CheckPointer("header", &headerCheck), headerCheck)
    {
        if (headerB.Remaining() < ETC_PKM_HEADER_SIZE) {
            LOGD("IsValid: header's remaining data < ETC_PKM_HEADER_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            result = etc1_pkm_is_valid((etc1_byte*) headerB.GetData());
        }
    }
    *isvalid = result;
    return NOERROR;
}

ECode CETC1::GetWidth(
    /* [in] */ Elastos::IO::IBuffer* header,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)

    Int32 result = 0;
    BufferHelper headerB(header);
    Boolean headerCheck;
    if (headerB.CheckPointer("header", &headerCheck), headerCheck)
    {
        if (headerB.Remaining() < ETC_PKM_HEADER_SIZE) {
            LOGD("GetWidth: header's remaining data < ETC_PKM_HEADER_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            result = etc1_pkm_get_width((etc1_byte*) headerB.GetData());
        }
    }
    *width = result;
    return NOERROR;
}

ECode CETC1::GetHeight(
    /* [in] */ Elastos::IO::IBuffer* header,
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    Int32 result = 0;
    BufferHelper headerB(header);
    Boolean headerCheck;
    if (headerB.CheckPointer("header", &headerCheck), headerCheck)
    {
        if (headerB.Remaining() < ETC_PKM_HEADER_SIZE) {
            LOGD("GetHeight: header's remaining data < ETC_PKM_HEADER_SIZE")
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else {
            result = etc1_pkm_get_height((etc1_byte*) headerB.GetData());
        }
    }
    *height = result;
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

