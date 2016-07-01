#ifndef __ELASTOS_DROID_DIALER_SETTINGS_CDEFAULTSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_DIALER_SETTINGS_CDEFAULTSETTINGSACTIVITY_H__

#include "_Elastos_Droid_Dialer_Settings_CDialerSettingsActivity.h"
#include "elastos/apps/dialer/settings/DialerSettingsActivity.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

CarClass(CDialerSettingsActivity)
    , public DialerSettingsActivity
{
public:
    CAR_OBJECT_DECL();
};

} // Settings
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SETTINGS_CDEFAULTSETTINGSACTIVITY_H__
