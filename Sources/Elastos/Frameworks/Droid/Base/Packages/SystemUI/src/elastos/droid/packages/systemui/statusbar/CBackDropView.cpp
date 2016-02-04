
#include "elastos/droid/packages/systemui/statusbar/CBackDropView.h"

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CBackDropView);
CAR_INTERFACE_IMPL(CBackDropView, FrameLayout, IBackDropView);
ECode CBackDropView::constructor(
    /* [in] */ IContext* context)
{
    return FrameLayout::constructor(context);
}

ECode CBackDropView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode CBackDropView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return FrameLayout::constructor(context, attrs, defStyleAttr);
}

ECode CBackDropView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CBackDropView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CBackDropView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    FrameLayout::OnVisibilityChanged(changedView, visibility);
    if (changedView == this && mOnVisibilityChangedRunnable != NULL) {
        mOnVisibilityChangedRunnable->Run();
    }
    return NOERROR;
}

ECode CBackDropView::SetOnVisibilityChangedRunnable(
    /* [in] */ IRunnable* runnable)
{
    mOnVisibilityChangedRunnable = runnable;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos
