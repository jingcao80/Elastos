
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSPAYMENTSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSPAYMENTSETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsPaymentSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsPaymentSettingsActivity)
    , public CSettings::PaymentSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSPAYMENTSETTINGSACTIVITY_H__
