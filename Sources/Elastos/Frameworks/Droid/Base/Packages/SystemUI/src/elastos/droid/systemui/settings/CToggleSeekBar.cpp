
#include "elastos/droid/systemui/settings/CToggleSeekBar.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

CAR_OBJECT_IMPL(CToggleSeekBar);
ECode CToggleSeekBar::constructor(
    /* [in] */ IContext* context)
{
    return SeekBar::constructor(context);
}

ECode CToggleSeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return SeekBar::constructor(context, attrs);
}

ECode CToggleSeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return SeekBar::constructor(context, attrs, defStyleAttr);
}

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
