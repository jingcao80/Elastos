#include "elastos/droid/systemui/settings/ToggleSeekBar.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/widget/AbsSeekBar.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::AbsSeekBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

ToggleSeekBar::ToggleSeekBar(
    /* [in] */ IContext* context)
{
    SeekBar::constructor(context);
}

ToggleSeekBar::ToggleSeekBar(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    SeekBar::constructor(context, attrs);
}

ToggleSeekBar::ToggleSeekBar(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    SeekBar::constructor(context, attrs, defStyleAttr);
}

ECode ToggleSeekBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isEnabled;
    (IView*)this->IsEnabled(&isEnabled);
    if (!isEnabled) {
        (IView*)this->SetEnabled(TRUE);
    }
    return AbsSeekBar::OnTouchEvent(event, result);
}

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
