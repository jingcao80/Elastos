
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_TOGGLESEEKBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_TOGGLESEEKBAR_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/widget/SeekBar.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::SeekBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

class ToggleSeekBar
    : public SeekBar
{
public:
    ToggleSeekBar(
        /* [in] */ IContext* context);

    ToggleSeekBar(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ToggleSeekBar(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_TOGGLESEEKBAR_H__