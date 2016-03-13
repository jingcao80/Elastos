
#include "elastos/droid/launcher2/AccessibleTabView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(AccessibleTabView, TextView, IAccessibleTabView);

ECode AccessibleTabView::constructor(
    /* [in] */ IContext* context)
{
    return TextView::constructor(context);
}

ECode AccessibleTabView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::constructor(context, attrs);
}

ECode AccessibleTabView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TextView::constructor(context, attrs, defStyle);
}

ECode AccessibleTabView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if(FocusHelper::HandleTabKeyEvent(this, keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    TextView::OnKeyDown(keyCode, event, &res);
    *result = res;
    return NOERROR;
}

ECode AccessibleTabView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (FocusHelper::HandleTabKeyEvent(this, keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    TextView::onKeyUp(keyCode, event, &res);
    *result = res;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos