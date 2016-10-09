
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSPOWERUSAGESUMMARYACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSPOWERUSAGESUMMARYACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsPowerUsageSummaryActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsPowerUsageSummaryActivity)
    , public CSettings::PowerUsageSummaryActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSPOWERUSAGESUMMARYACTIVITY_H__
