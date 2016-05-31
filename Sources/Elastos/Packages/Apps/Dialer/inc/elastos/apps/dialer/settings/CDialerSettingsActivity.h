#ifndef __ELASTOS_APPS_DIALER_SETTINGS_CDEFAULTSETTINGSACTIVITY_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_CDEFAULTSETTINGSACTIVITY_H__

#include "_Elastos_Apps_Dialer_Settings_CDialerSettingsActivity.h"
#include "DialerSettingsActivity.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Settings {

CarClass(CDialerSettingsActivity)
    , public DialerSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // Settings
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_CDEFAULTSETTINGSACTIVITY_H__
