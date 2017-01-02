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

#ifndef __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPEER_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPEER_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Wifi_P2p_CWifiP2pPeer.h"
#include "elastos/droid/preference/Preference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pPeer)
    , public Elastos::Droid::Preference::Preference
    , public IWifiP2pPeer
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWifiP2pPeer();

    ~CWifiP2pPeer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IWifiP2pDevice* dev);

    //@Override
    CARAPI CompareTo(
        /* [in] */ IInterface* preference,
        /* [out] */ Int32* result);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetLevel();

private:
    CARAPI_(void) Refresh();

public:
    AutoPtr<IWifiP2pDevice> mDevice;

private:
    static const AutoPtr< ArrayOf<Int32> > STATE_SECURED;;

    Int32 mRssi;
    AutoPtr<IImageView> mSignal;

    static const Int32 SIGNAL_LEVELS;
};

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPEER_H__