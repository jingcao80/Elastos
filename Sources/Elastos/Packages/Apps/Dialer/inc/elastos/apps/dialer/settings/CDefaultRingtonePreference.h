#ifndef __ELASTOS_APPS_DIALER_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__

#include "_Elastos_Apps_Dialer_Settings_CDefaultRingtonePreference.h"
#include "elastos/apps/dialer/settings/DefaultRingtonePreference.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Settings {

CarClass(CDefaultRingtonePreference)
    , public DefaultRingtonePreference
{
public:
    CAR_OBJECT_DECL();
};

} // Settings
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__
