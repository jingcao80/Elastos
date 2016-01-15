
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CYuvImage.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/YuvToJpegEncoder.h"
#include <skia/core/SkStream.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

const Int32 CYuvImage::WORKING_COMPRESS_STORAGE;

CAR_OBJECT_IMPL(CYuvImage);
CAR_INTERFACE_IMPL(CYuvImage, Object, IYuvImage);
ECode CYuvImage::constructor(
    /* [in] */ ArrayOf<Byte>* yuv,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ArrayOf<Int32>* strides)
{
    if (format != IImageFormat::NV21 &&
                format != IImageFormat::YUY2) {
        // throw new IllegalArgumentException(
        //             "only support ImageFormat.NV21 " +
        //             "and ImageFormat.YUY2 for now");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (width <= 0  || height <= 0) {
        // throw new IllegalArgumentException(
        //             "width and height must large than 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (yuv == NULL) {
        // throw new IllegalArgumentException("yuv cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (strides == NULL) {
        mStrides = CalculateStrides(width, format);
    }
    else {
        mStrides = strides;
    }

    mData = yuv;
    mFormat = format;
    mWidth = width;
    mHeight = height;
    return NOERROR;
}

ECode CYuvImage::CompressToJpeg(
    /* [in] */ IRect* rectangle,
    /* [in] */ Int32 quality,
    /* [in] */ IOutputStream* stream,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IRect> wholeImage;
    CRect::New(0, 0, mWidth, mHeight, (IRect**)&wholeImage);

    Boolean res;
    if (wholeImage->Contains(rectangle, &res), !res) {
        // throw new IllegalArgumentException(
        //             "rectangle is not inside the image");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (quality < 0 || quality > 100) {
        // throw new IllegalArgumentException("quality must be 0..100");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (stream == NULL) {
        // throw new IllegalArgumentException("stream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AdjustRectangle(rectangle);
    AutoPtr< ArrayOf<Int32> > offsets = CalculateOffsets(
        ((CRect*)rectangle)->mLeft,
        ((CRect*)rectangle)->mTop);

    Int32 width;
    Int32 height;
    rectangle->GetWidth(&width);
    rectangle->GetHeight(&height);
    AutoPtr< ArrayOf<Byte> > tempStorage = ArrayOf<Byte>::Alloc(WORKING_COMPRESS_STORAGE);
    *result = NativeCompressToJpeg(
                mData,
                mFormat,
                width,
                height,
                offsets,
                mStrides,
                quality,
                stream,
                tempStorage);
    return NOERROR;
}

ECode CYuvImage::GetYuvData(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CYuvImage::GetYuvFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);

    *format = mFormat;
    return NOERROR;
}

ECode CYuvImage::GetStrides(
    /* [out, callee] */ ArrayOf<Int32>** strides)
{
    VALIDATE_NOT_NULL(strides);

    *strides = mStrides;
    REFCOUNT_ADD(*strides);
    return NOERROR;
}

ECode CYuvImage::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode CYuvImage::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > CYuvImage::CalculateOffsets(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top)
{
    AutoPtr< ArrayOf<Int32> > offsets;
    if (mFormat == IImageFormat::NV21) {
        offsets = ArrayOf<Int32>::Alloc(2);
        (*offsets)[0] = top * (*mStrides)[0] + left;
        (*offsets)[1] = mHeight * (*mStrides)[0] + top / 2 * (*mStrides)[1] + left / 2 * 2;
        return offsets;
    }

    if (mFormat == IImageFormat::YUY2) {
        offsets = ArrayOf<Int32>::Alloc(1);
        (*offsets)[0] = top * (*mStrides)[0] + left / 2 * 4;
        return offsets;
    }

    return offsets;
}

AutoPtr< ArrayOf<Int32> > CYuvImage::CalculateStrides(
    /* [in] */ Int32 width,
    /* [in] */ Int32 format)
{
    AutoPtr< ArrayOf<Int32> > strides;
    if (format == IImageFormat::NV21) {
        strides = ArrayOf<Int32>::Alloc(2);
        (*strides)[0] = (*strides)[1] = width;
        return strides;
    }

    if (format == IImageFormat::YUY2) {
        strides = ArrayOf<Int32>::Alloc(1);
        (*strides)[0] = width * 2;
        return strides;
    }

    return strides;
}

void CYuvImage::AdjustRectangle(
    /* [in] */ IRect* rect)
{
    Int32 width;
    Int32 height;
    rect->GetWidth(&width);
    rect->GetHeight(&height);
    CRect* rectObj = (CRect*)rect;
    if (mFormat == IImageFormat::NV21) {
       // Make sure left, top, width and height are all even.
       width &= ~1;
       height &= ~1;
       rectObj->mLeft &= ~1;
       rectObj->mTop &= ~1;
       rectObj->mRight = rectObj->mLeft + width;
       rectObj->mBottom = rectObj->mTop + height;
    }

    if (mFormat == IImageFormat::YUY2) {
        // Make sure left and width are both even.
        width &= ~1;
        rectObj->mLeft &= ~1;
        rectObj->mRight = rectObj->mLeft + width;
    }
}

Boolean CYuvImage::NativeCompressToJpeg(
    /* [in] */ ArrayOf<Byte>* oriYuv,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ArrayOf<Int32>* offsets,
    /* [in] */ ArrayOf<Int32>* strides,
    /* [in] */ Int32 quality,
    /* [in] */ IOutputStream* stream,
    /* [out] */ ArrayOf<Byte>* tempStorage)
{
    Byte* yuv = oriYuv->GetPayload();
    SkWStream* strm = CreateOutputStreamAdaptor(stream, tempStorage);

    Int32* imgOffsets = offsets->GetPayload();
    Int32* imgStrides = strides->GetPayload();

    YuvToJpegEncoder* encoder = YuvToJpegEncoder::Create(format, imgStrides);
    if (encoder == NULL) {
        return FALSE;
    }

    encoder->Encode(strm, yuv, width, height, imgOffsets, quality);
    delete encoder;
    return TRUE;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
