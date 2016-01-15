
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CLargeBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/AutoDecoderCancel.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/NBitmapFactory.h"
#include <skia/core/SkTemplates.h>
#include <skia/core/SkPixelRef.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CLargeBitmap);
CAR_INTERFACE_IMPL(CLargeBitmap, Object, ILargeBitmap);
CLargeBitmap::CLargeBitmap()
    : mNativeLargeBitmap(0)
    , mRecycled(FALSE)
{}

CLargeBitmap::~CLargeBitmap()
{
    Recycle();
}

ECode CLargeBitmap::constructor(
    /* [in] */ Int64 lbm)
{
    mNativeLargeBitmap = lbm;
    mRecycled = FALSE;
    return NOERROR;
}

ECode CLargeBitmap::DecodeRegion(
    /* [in] */ IRect* _rect,
    /* [in] */ IBitmapFactoryOptions* options,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    FAIL_RETURN(CheckRecycled(String("decodeRegion called on recycled large bitmap")));

    CRect* rect = (CRect*)_rect;
    Int32 width;
    Int32 height;
    GetWidth(&width);
    GetHeight(&height);
    if (rect->mLeft < 0 || rect->mTop < 0 ||
        rect->mRight > width || rect->mBottom > height) {
        // throw new IllegalArgumentException("rectangle is not inside the image");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NativeDecodeRegion(mNativeLargeBitmap,
            rect->mLeft, rect->mTop, rect->mRight - rect->mLeft,
            rect->mBottom - rect->mTop, options, bitmap);
}

ECode CLargeBitmap::GetWidth(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    FAIL_RETURN(CheckRecycled(String("getWidth called on recycled large bitmap")));
    *length = NativeGetWidth(mNativeLargeBitmap);
    return NOERROR;
}

ECode CLargeBitmap::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    FAIL_RETURN(CheckRecycled(String("getHeight called on recycled large bitmap")));
    *height =  NativeGetHeight(mNativeLargeBitmap);
    return NOERROR;
}

ECode CLargeBitmap::Recycle()
{
    if (!mRecycled) {
        NativeClean(mNativeLargeBitmap);
        mRecycled = TRUE;
    }
    return NOERROR;
}

ECode CLargeBitmap::IsRecycled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRecycled;
    return NOERROR;
}

ECode CLargeBitmap::CheckRecycled(
    /* [in] */ const String& errorMessage)
{
    if (mRecycled) {
        // throw new IllegalStateException(errorMessage);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CLargeBitmap::NativeDecodeRegion(
    /* [in] */ Int64 lbm,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IBitmapFactoryOptions* options,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    assert(0 && "TODO: Not find jni codes.");
    return NOERROR;
}

Int32 CLargeBitmap::NativeGetWidth(
    /* [in] */ Int64 brdHandle)
{
    assert(0 && "TODO: Not find jni codes.");
    return 0;
}

Int32 CLargeBitmap::NativeGetHeight(
    /* [in] */ Int64 brdHandle)
{
    assert(0 && "TODO: Not find jni codes.");
    return 0;
}

void CLargeBitmap::NativeClean(
    /* [in] */ Int64 brdHandle)
{
    assert(0 && "TODO: Not find jni codes.");
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
