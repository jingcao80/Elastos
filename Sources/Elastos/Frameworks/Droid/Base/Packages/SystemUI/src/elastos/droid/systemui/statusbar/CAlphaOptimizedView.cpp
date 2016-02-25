
#include "elastos/droid/systemui/statusbar/CAlphaOptimizedView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CAlphaOptimizedView);
CAR_INTERFACE_IMPL(CAlphaOptimizedView, View, IAlphaOptimizedView);
ECode CAlphaOptimizedView::constructor(
    /* [in] */ IContext* context)
{
    return View::constructor(context);
}

ECode CAlphaOptimizedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return View::constructor(context, attrs);
}

ECode CAlphaOptimizedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return View::constructor(context, attrs, defStyleAttr);
}

ECode CAlphaOptimizedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return View::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CAlphaOptimizedView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
