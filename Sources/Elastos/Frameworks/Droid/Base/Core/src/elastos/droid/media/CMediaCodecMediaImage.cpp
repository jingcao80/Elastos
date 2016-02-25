#include "elastos/droid/media/CMediaCodecMediaImage.h"
#include "elastos/droid/media/Utils.h"
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::IRect;
using Elastos::IO::CNioUtils;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::INioUtils;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Media {

//==============================================================================
//  CMediaCodecMediaImage::MediaPlane
//==============================================================================

ECode CMediaCodecMediaImage::MediaPlane::GetBuffer(
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)
    mHost->CheckValid();
    *result = mData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecMediaImage::MediaPlane::GetPixelStride(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->CheckValid();
    *result = mColInc;
    return NOERROR;
}

ECode CMediaCodecMediaImage::MediaPlane::GetRowStride(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->CheckValid();
    *result = mRowInc;
    return NOERROR;
}

//==============================================================================
//  CMediaCodecMediaImage
//==============================================================================

const Int32 CMediaCodecMediaImage::TYPE_YUV = 1;

CAR_INTERFACE_IMPL(CMediaCodecMediaImage, Object, IMediaCodecMediaImage)

CAR_OBJECT_IMPL(CMediaCodecMediaImage)

CMediaCodecMediaImage::CMediaCodecMediaImage()
    : mIsReadOnly(FALSE)
    , mIsValid(FALSE)
    , mWidth(0)
    , mHeight(0)
    , mFormat(0)
    , mTimestamp(0)
    , mXOffset(0)
    , mYOffset(0)
{
}

CMediaCodecMediaImage::~CMediaCodecMediaImage()
{
}

ECode CMediaCodecMediaImage::constructor()
{
    return NOERROR;
}

ECode CMediaCodecMediaImage::constructor(
    /* [in] */ IByteBuffer* _buffer,
    /* [in] */ IByteBuffer* info,
    /* [in] */ Boolean readOnly,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 xOffset,
    /* [in] */ Int32 yOffset,
    /* [in] */ IRect* cropRect)
{
    mFormat = IImageFormat::YUV_420_888;
    mTimestamp = timestamp;
    mIsValid = TRUE;
    AutoPtr<IByteBuffer> buffer = _buffer;
    IBuffer::Probe(buffer)->IsReadOnly(&mIsReadOnly);
    buffer->Duplicate((IByteBuffer**)&mBuffer);
    if (cropRect != NULL) {
        cropRect->Offset(-xOffset, -yOffset);
    }
    Image::SetCropRect(cropRect);

    // save offsets and info
    mXOffset = xOffset;
    mYOffset = yOffset;
    mInfo = info;

    // read media-info.  the size of media info can be 80 or 156 depending on
    // whether it was created on a 32- or 64-bit process.  See MediaImage
    Int32 remaining;
    IBuffer::Probe(info)->GetRemaining(&remaining);
    if (remaining == 80 || remaining == 156) {
        Boolean sizeIsLong = remaining == 156;
        Int32 type;
        info->GetInt32(&type);
        if (type != TYPE_YUV) {
            // throw new UnsupportedOperationException("unsupported type: " + type);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
        Int32 numPlanes = ReadInt32(info, sizeIsLong);
        if (numPlanes != 3) {
            // throw new RuntimeException("unexpected number of planes: " + numPlanes);
            return E_RUNTIME_EXCEPTION;
        }
        mWidth = ReadInt32(info, sizeIsLong);
        mHeight = ReadInt32(info, sizeIsLong);
        if (mWidth < 1 || mHeight < 1) {
            // throw new UnsupportedOperationException(
            //         "unsupported size: " + mWidth + "x" + mHeight);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
        Int32 bitDepth = ReadInt32(info, sizeIsLong);
        if (bitDepth != 8) {
            // throw new UnsupportedOperationException("unsupported bit depth: " + bitDepth);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
        mPlanes = ArrayOf<IImagePlane*>::Alloc(numPlanes);

        for (Int32 ix = 0; ix < numPlanes; ix++) {
            Int32 planeOffset = ReadInt32(info, sizeIsLong);
            Int32 colInc = ReadInt32(info, sizeIsLong);
            Int32 rowInc = ReadInt32(info, sizeIsLong);
            Int32 horiz = ReadInt32(info, sizeIsLong);
            Int32 vert = ReadInt32(info, sizeIsLong);
            if (horiz != vert || horiz != (ix == 0 ? 1 : 2)) {
                // throw new UnsupportedOperationException("unexpected subsampling: "
                //         + horiz + "x" + vert + " on plane " + ix);
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }

            IBuffer::Probe(buffer)->Clear();
            Int32 pos;
            IBuffer::Probe(mBuffer)->GetPosition(&pos);
            IBuffer::Probe(buffer)->SetPosition(pos + planeOffset
                    + (xOffset / horiz) * colInc + (yOffset / vert) * rowInc);

            IBuffer::Probe(buffer)->GetPosition(&pos);
            Int32 d;
            d = Utils::DivUp(bitDepth, 8);
            IBuffer::Probe(buffer)->SetLimit(pos + d + (mHeight / vert - 1) * rowInc +
                    (mWidth / horiz - 1) * colInc);

            AutoPtr<IByteBuffer> buf;
            buffer->Slice((IByteBuffer**)&buf);
            AutoPtr<MediaPlane> plane = new MediaPlane(buf, rowInc, colInc, this);
            mPlanes->Set(ix, plane);
        }
    }
    else {
        // throw new UnsupportedOperationException(
        //         "unsupported info length: " + info.remaining());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaCodecMediaImage::GetFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    CheckValid();
    *result = mFormat;
    return NOERROR;
}

ECode CMediaCodecMediaImage::GetHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    CheckValid();
    *result = mHeight;
    return NOERROR;
}

ECode CMediaCodecMediaImage::GetWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    CheckValid();
    *result = mWidth;
    return NOERROR;
}

ECode CMediaCodecMediaImage::GetTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    CheckValid();
    *result = mTimestamp;
    return NOERROR;
}

ECode CMediaCodecMediaImage::GetPlanes(
    /* [out, callee] */ ArrayOf<IImagePlane*>** result)
{
    VALIDATE_NOT_NULL(result)
    CheckValid();
    return Arrays::CopyOf(mPlanes, mPlanes->GetLength(), result);
}

ECode CMediaCodecMediaImage::Close()
{
    if (mIsValid) {
        AutoPtr<INioUtils> utils;
        CNioUtils::AcquireSingleton((INioUtils**)&utils);
        utils->FreeDirectBuffer(mBuffer);
        mIsValid = FALSE;
    }
    return NOERROR;
}

ECode CMediaCodecMediaImage::SetCropRect(
    /* [in] */ IRect* cropRect)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    return Image::SetCropRect(cropRect);
}

ECode CMediaCodecMediaImage::CheckValid()
{
    if (!mIsValid) {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Int32 CMediaCodecMediaImage::ReadInt32(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Boolean asLong)
{
    Int32 val;
    if (asLong) {
        Int64 tmp;
        buffer->GetInt64(&tmp);
        val = tmp;
    }
    else {
        buffer->GetInt32(&val);
    }
    return val;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
