#include "elastos/droid/systemui/recent/ColorDrawableWithDimensions.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

ColorDrawableWithDimensions::ColorDrawableWithDimensions(
    /* [in] */ Int32 color,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
    : ColorDrawable(color)
    , mWidth(width)
    , mHeight(height)
{}

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