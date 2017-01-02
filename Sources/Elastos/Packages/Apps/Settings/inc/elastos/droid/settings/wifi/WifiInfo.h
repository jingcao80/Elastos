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

