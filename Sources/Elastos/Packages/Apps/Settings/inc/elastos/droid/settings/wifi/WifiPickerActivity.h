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

