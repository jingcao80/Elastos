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

#ifndef __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPERSISTENTGROUP_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPERSISTENTGROUP_H__

#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Wifi_P2p_CWifiP2pPersistentGroup.h"
#include "elastos/droid/preference/Preference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pPersistentGroup)
    , public Elastos::Droid::Preference::Preference
    , public IWifiP2pPersistentGroup
{
    friend class CWifiP2pSettings;
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWifiP2pPersistentGroup();

    ~CWifiP2pPersistentGroup();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IWifiP2pGroup* group);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetNetworkId();

    CARAPI_(String) GetGroupName();

public:
    AutoPtr<IWifiP2pGroup> mGroup;
};

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPERSISTENTGROUP_H__