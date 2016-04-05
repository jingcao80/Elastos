#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFIPICKERACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFIPICKERACTIVITY_H__

#include "elastos/droid/settings/SettingsActivity.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

class WifiPickerActivity
    : public SettingsActivity
    , public IWifiPickerActivity
{
public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

protected:
    //@Override
    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

    // /* package */ Class<? extends PreferenceFragment> GetWifiSettingsClass()
    // {
    //     return WifiSettings.class;
    // }
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFIPICKERACTIVITY_H__

