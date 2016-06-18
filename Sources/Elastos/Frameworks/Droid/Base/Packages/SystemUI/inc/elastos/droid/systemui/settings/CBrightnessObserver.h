
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_CBRIGHTNESS_OBSERVER_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_CBRIGHTNESS_OBSERVER_H__

#include "_Elastos_Droid_SystemUI_Settings_CBrightnessObserver.h"
#include "elastos/droid/systemui/settings/BrightnessController.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

CarClass(CBrightnessObserver)
    , public BrightnessController::BrightnessObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_CBRIGHTNESS_OBSERVER_H__
