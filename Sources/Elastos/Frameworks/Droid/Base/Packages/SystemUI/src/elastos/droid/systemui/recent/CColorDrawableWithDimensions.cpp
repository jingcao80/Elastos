#include "elastos/droid/systemui/recent/CColorDrawableWithDimensions.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CAR_OBJECT_IMPL(CColorDrawableWithDimensions)

CColorDrawableWithDimensions::CColorDrawableWithDimensions()
    : mWidth(0)
    , mHeight(0)
{}

ECode CColorDrawableWithDimensions::constructor(
    /* [in] */ Int32 color,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mWidth = width;
    mHeight = height;
    return ColorDrawable::constructor(color);
}

ECode CColorDrawableWithDimensions::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode CColorDrawableWithDimensions::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = mHeight;
    return NOERROR;
}

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos