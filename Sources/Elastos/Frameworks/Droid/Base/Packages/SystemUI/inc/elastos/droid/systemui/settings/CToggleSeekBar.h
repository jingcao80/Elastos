
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_CTOGGLESEEKBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_CTOGGLESEEKBAR_H__

#include "_Elastos_Droid_SystemUI_Settings_CToggleSeekBar.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/widget/SeekBar.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::SeekBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

CarClass(CToggleSeekBar), public SeekBar
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_CTOGGLESEEKBAR_H__
