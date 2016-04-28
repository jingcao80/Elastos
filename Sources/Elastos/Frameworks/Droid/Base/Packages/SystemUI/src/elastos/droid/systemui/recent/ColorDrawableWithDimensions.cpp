#include "elastos/droid/systemui/recent/ColorDrawableWithDimensions.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

ColorDrawableWithDimensions::ColorDrawableWithDimensions()
    : mWidth(0)
    , mHeight(0)
{}

ECode ColorDrawableWithDimensions::constructor(
    /* [in] */ Int32 color,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(ColorDrawable::constructor(color));
    mWidth = width;
    mHeight = height;
    return NOERROR;
}

ECode ColorDrawableWithDimensions::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode ColorDrawableWithDimensions::GetIntrinsicHeight(
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