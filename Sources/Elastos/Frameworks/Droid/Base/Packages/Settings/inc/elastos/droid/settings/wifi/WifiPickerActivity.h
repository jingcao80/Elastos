
package com.android.settings.wifi;

using Elastos::Droid::Settings::IButtonBarHandler;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Wifi::P2p::IWifiP2pSettings;
using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::IPreferenceFragment;

using Elastos::Lang::IClass;

public class WifiPickerActivity extends SettingsActivity implements ButtonBarHandler {

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        if (!modIntent->HasExtra(EXTRA_SHOW_FRAGMENT)) {
            modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, GetWifiSettingsClass()->GetName());
            modIntent->PutExtra(EXTRA_SHOW_FRAGMENT_TITLE_RESID, R::string::wifi_select_network);
        }
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        if (WifiSettings.class->GetName()->Equals(fragmentName)
                || WifiP2pSettings.class->GetName()->Equals(fragmentName)
                || SavedAccessPointsWifiSettings.class->GetName()->Equals(fragmentName)
                || AdvancedWifiSettings.class->GetName()->Equals(fragmentName)) return TRUE;
        return FALSE;
    }

    /* package */ Class<? extends PreferenceFragment> GetWifiSettingsClass() {
        return WifiSettings.class;
    }
}
