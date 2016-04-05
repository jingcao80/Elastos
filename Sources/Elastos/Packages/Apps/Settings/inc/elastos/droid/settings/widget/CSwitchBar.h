#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBAR_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBAR_H__

#include "_Elastos_Droid_Settings_Widget_CSwitchBar.h"
#include "elastos/droid/settings/widget/SwitchBar.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CSwitchBar), public SwitchBar
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Settings
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBAR_H__
