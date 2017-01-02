//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    if (fragmentName.Equals("Elastos.Droid.Settings.Wifi.CWifiSettings")
            || fragmentName.Equals("Elastos.Droid.Settings.Wifi.P2p.CWifiP2pSettings")
            || fragmentName.Equals("Elastos.Droid.Settings.Wifi.CSavedAccessPointsWifiSettings")
            || fragmentName.Equals("Elastos.Droid.Settings.Wifi.CAdvancedWifiSettings")) return TRUE;
    return FALSE;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
