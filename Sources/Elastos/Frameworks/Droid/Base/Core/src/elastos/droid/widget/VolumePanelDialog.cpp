#include "elastos/droid/widget/VolumePanelDialog.h"
#include "elastos/droid/widget/CVolumePanel.h"

namespace Elastos {
namespace Droid {
namespace Widget {

ECode VolumePanelDialog::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    event->GetAction(&action);

    if (action == IMotionEvent::ACTION_OUTSIDE) {
        CVolumePanel* panel = (CVolumePanel*)mVolumePanel.Get();
        if (IsShowing()) {
            panel->ForceTimeout();
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode VolumePanelDialog::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ IVolumePanel* panel)
{
    mVolumePanel = panel;
    return Dialog::Init(context, theme);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
