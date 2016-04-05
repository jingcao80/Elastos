
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFIINFO_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFIINFO_H__

#include "elastos/droid/preference/PreferenceActivity.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::PreferenceActivity;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * Wifi information menu item on the diagnostic screen
 */
class WifiInfo
    : public PreferenceActivity
{
protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFIINFO_H__

