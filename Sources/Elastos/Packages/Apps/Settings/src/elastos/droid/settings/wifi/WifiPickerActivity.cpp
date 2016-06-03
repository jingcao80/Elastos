
#include "elastos/droid/settings/wifi/WifiPickerActivity.h"
#include "../R.h"

using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

CAR_INTERFACE_IMPL(WifiPickerActivity, SettingsActivity, IWifiPickerActivity);

ECode WifiPickerActivity::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    AutoPtr<IIntent> baseIntent;
    SettingsActivity::GetIntent((IIntent**)&baseIntent);
    AutoPtr<IIntent> modIntent;
    CIntent::New(baseIntent, (IIntent**)&modIntent);
    Boolean hasExtra;
    if (modIntent->HasExtra(EXTRA_SHOW_FRAGMENT, &hasExtra), !hasExtra) {
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT,
                String("Elastos.Droid.Settings.Wifi.CWifiSettings")/*GetWifiSettingsClass()->GetName()*/);
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT_TITLE_RESID, R::string::wifi_select_network);
    }
    *intent = modIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

Boolean WifiPickerActivity::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    if (String("Elastos.Droid.Settings.Wifi.CWifiSettings").Equals(fragmentName)
            || String("Elastos.Droid.Settings.Wifi.P2p.WifiP2pSettings").Equals(fragmentName)
            || String("Elastos.Droid.Settings.Wifi.SavedAccessPointsWifiSettings").Equals(fragmentName)
            || String("Elastos.Droid.Settings.Wifi.AdvancedWifiSettings").Equals(fragmentName)) return TRUE;
    return FALSE;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
