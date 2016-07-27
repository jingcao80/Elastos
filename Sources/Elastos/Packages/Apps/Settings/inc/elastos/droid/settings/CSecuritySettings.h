
#ifndef __ELASTOS_DROID_SETTINGS_CSECURITYSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CSECURITYSETTINGS_H__

#include "_Elastos_Droid_Settings_CSecuritySettings.h"
#include "elastos/droid/settings/SecuritySettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSecuritySettings)
    , public SecuritySettings
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSECURITYSETTINGS_H__
