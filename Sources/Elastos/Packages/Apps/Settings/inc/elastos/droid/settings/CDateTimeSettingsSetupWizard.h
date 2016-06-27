
#ifndef __ELASTOS_DROID_SETTINGS_CDATETIMESETTINGSSETUPWIZARD_H__
#define __ELASTOS_DROID_SETTINGS_CDATETIMESETTINGSSETUPWIZARD_H__

#include "_Elastos_Droid_Settings_CDateTimeSettingsSetupWizard.h"
#include "elastos/droid/settings/DateTimeSettingsSetupWizard.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDateTimeSettingsSetupWizard)
    , public DateTimeSettingsSetupWizard
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDATETIMESETTINGSSETUPWIZARD_H__
