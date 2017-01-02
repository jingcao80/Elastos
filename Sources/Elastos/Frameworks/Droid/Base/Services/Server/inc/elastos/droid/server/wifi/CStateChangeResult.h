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

#ifndef __ELASTOS_DROID_SERVER_WIFI_CSTATECHANGERESULT_H__
#define __ELASTOS_DROID_SERVER_WIFI_CSTATECHANGERESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Wifi::IWifiSsid;
using Elastos::Droid::Wifi::SupplicantState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class CStateChangeResult
    : public Object
{
public:

    CStateChangeResult(
        /* [in] */ Int32 networkId,
        /* [in] */ IWifiSsid* wifiSsid,
        /* [in] */ const String& BSSID,
        /* [in] */ SupplicantState pState);

    CARAPI GetWifiSsid(
        /* [out] */ IWifiSsid** ssid);

    CARAPI SetWifiSsid(
        /* [in] */ IWifiSsid* ssid);

    CARAPI GetBSSID(
        /* [out] */ String* bssid);

    CARAPI SetBSSID(
        /* [in] */ const String& bssid);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkId);

    CARAPI SetNetworkId(
        /* [in] */ Int32 networkId);

    CARAPI GetSupplicantState(
        /* [out] */ SupplicantState* state);

    CARAPI SetSupplicantState(
        /* [in] */ SupplicantState state);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    Int32 mNetworkId;
    AutoPtr<IWifiSsid> mWifiSsid;
    String mBSSID;
    SupplicantState mState;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namepsace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_CSTATECHANGERESULT_H__
