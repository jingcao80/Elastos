
#include "elastos/droid/launcher2/CAccessibleTabView.h"
#include "elastos/droid/launcher2/FocusHelper.h"
#include "Elastos.Droid.Service.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CAccessibleTabView, TextView, IAccessibleTabView);

CAR_OBJECT_IMPL(CAccessibleTabView);

ECode CAccessibleTabView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if(FocusHelper::HandleTabKeyEvent(IAccessibleTabView::Probe(this), keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    TextView::OnKeyDown(keyCode, event, &res);
    *result = res;
    return NOERROR;
}

ECode CAccessibleTabView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (FocusHelper::HandleTabKeyEvent(IAccessibleTabView::Probe(this), keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    TextView::OnKeyUp(keyCode, event, &res);
    *result = res;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos