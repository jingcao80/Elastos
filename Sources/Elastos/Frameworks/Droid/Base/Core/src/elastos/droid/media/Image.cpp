#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/media/Image.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::IO::EIID_IAutoCloseable;

namespace Elastos {
namespace Droid {
namespace Media {

//================================================================================
//  Image::Plane
//================================================================================

CAR_INTERFACE_IMPL(Image::Plane, Object, IImagePlane)

Image::Plane::Plane()
{
}

Image::Plane::~Plane()
{
}

ECode Image::Plane::GetRowStride(
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode Image::Plane::GetPixelStride(
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode Image::Plane::GetBuffer(
    /* [out] */ IByteBuffer** result)
{
    return NOERROR;
}

//================================================================================
//  Image
//================================================================================

CAR_INTERFACE_IMPL_2(Image, Object, IImage, IAutoCloseable)

Image::Image()
{
}

Image::~Image()
{
}

ECode Image::constructor()
{
    return NOERROR;
}

ECode Image::GetFormat(
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode Image::GetWidth(
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode Image::GetHeight(
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode Image::GetTimestamp(
    /* [out] */ Int64* result)
{
    return NOERROR;
}

ECode Image::GetCropRect(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result)
    if (mCropRect == NULL) {
        Int32 width, height;
        GetWidth(&width);
        GetHeight(&height);
        CRect::New(0, 0, width, height, result);
    }
    else {
        CRect::New(mCropRect, result); // return a copy
    }
    return NOERROR;
}

ECode Image::SetCropRect(
    /* [in] */ IRect* _cropRect)
{
    AutoPtr<IRect> cropRect;
    CRect::New(_cropRect, (IRect**)&cropRect);  // make a copy
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    Boolean b;
    cropRect->Intersect(0, 0, width, height, &b);
    mCropRect = cropRect;
    return NOERROR;
}

ECode Image::GetPlanes(
    /* [out, callee] */ ArrayOf<IImagePlane*>** result)
{
    return NOERROR;
}

ECode Image::Close()
{
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
