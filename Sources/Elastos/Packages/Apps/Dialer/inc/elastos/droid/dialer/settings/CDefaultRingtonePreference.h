#ifndef __ELASTOS_DROID_DIALER_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__
#define __ELASTOS_DROID_DIALER_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__

#include "_Elastos_Droid_Dialer_Settings_CDefaultRingtonePreference.h"
#include "elastos/apps/dialer/settings/DefaultRingtonePreference.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SETTINGS_CDEFAULTRINGTONEPREFERENCE_H__
