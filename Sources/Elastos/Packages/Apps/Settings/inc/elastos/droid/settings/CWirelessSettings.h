
#ifndef __ELASTOS_DROID_SETTINGS_CWIRELESSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CWIRELESSSETTINGS_H__

#include "_Elastos_Droid_Settings_CWirelessSettings.h"
#include "elastos/droid/settings/WirelessSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CWirelessSettings)
    , public WirelessSettings
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CWIRELESSSETTINGS_H__
