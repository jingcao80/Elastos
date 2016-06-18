
#include "elastos/droid/systemui/settings/CToggleSeekBar.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

CAR_OBJECT_IMPL(CToggleSeekBar)

ECode CToggleSeekBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isEnabled;
    IsEnabled(&isEnabled);
    if (!isEnabled) {
        SetEnabled(TRUE);
    }
    return SeekBar::OnTouchEvent(event, result);
}

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
