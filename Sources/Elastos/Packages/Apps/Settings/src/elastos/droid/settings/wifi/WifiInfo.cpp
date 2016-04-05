
#include "elastos/droid/settings/wifi/WifiInfo.h"
#include "../R.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

ECode WifiInfo::OnCreate(
    /* [in] */ IBundle* savedInstanceState);
{
    PreferenceActivity::OnCreate(savedInstanceState);
    AddPreferencesFromResource(R::xml::testing_wifi_settings);
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
