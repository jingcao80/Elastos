#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__

#include "_Elastos_Droid_Settings_Widget_CToggleSwitch.h"
#include "elastos/droid/settings/widget/ToggleSwitch.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CToggleSwitch), public ToggleSwitch
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Settings
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__
