#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBARSAVEDSTATE_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBARSAVEDSTATE_H__

#include "_Elastos_Droid_Settings_Widget_CSwitchBarSavedState.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CSwitchBarSavedState), public CSwitchBar::SavedState
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Settings
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBARSAVEDSTATE_H__
