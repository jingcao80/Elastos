
#include "elastos/droid/packages/systemui/statusbar/CAlphaOptimizedFrameLayout.h"

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CAlphaOptimizedFrameLayout);
CAR_INTERFACE_IMPL(CAlphaOptimizedFrameLayout, FrameLayout, IAlphaOptimizedFrameLayout);
ECode CAlphaOptimizedFrameLayout::constructor(
    /* [in] */ IContext* context)
{
    return FrameLayout::constructor(context);
}

ECode CAlphaOptimizedFrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode CAlphaOptimizedFrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return FrameLayout::constructor(context, attrs, defStyleAttr);
}

ECode CAlphaOptimizedFrameLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CAlphaOptimizedFrameLayout::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos
