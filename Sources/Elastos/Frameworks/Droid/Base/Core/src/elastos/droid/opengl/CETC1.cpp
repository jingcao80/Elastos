
#include "CETC1.h"
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
#include "poly.h"
#include <elastos/utility/logging/Slogger.h>

#define LOGD(msg) SLOGGERD("CETC1", msg)

using Elastos::IO::INIOAccessHelper;
using Elastos::IO::CNIOAccessHelper;

namespace Elastos {
namespace Droid {
namespace Opengl {

ECode CETC1::GetPointer(
    /* [in] */ Elastos::IO::IBuffer* buffer,
    /* [in, out] */ Int32 *remaining,
    /* [out] */ Handle32* rst)
{
    Int32 position;
    Int32 limit;
    Int32 elementSizeShift;
    Int64 pointer;

    buffer->GetPosition(&position);
    buffer->GetLimit(&limit);
    buffer->GetElementSizeShift(&elementSizeShift);
    *remaining = (limit - position) << elementSizeShift;

    AutoPtr<INIOAccessHelper> helper;
    CNIOAccessHelper::AcquireSingleton((INIOAccessHelper**)&helper);

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

