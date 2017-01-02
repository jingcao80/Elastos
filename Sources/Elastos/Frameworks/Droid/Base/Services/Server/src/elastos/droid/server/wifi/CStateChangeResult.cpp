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

#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/server/wifi/CStateChangeResult.h"
#include "elastos/core/StringBuffer.h"
#include <elastos/utility/logging/Logger.h>


using Elastos::Core::StringBuffer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//CStateChangeResult::CStateChangeResult()
//    : mNetworkId(0)
//    , mState(SupplicantState_INVALID)
//{}

CStateChangeResult::CStateChangeResult(
    /* [in] */ Int32 networkId,
    /* [in] */ IWifiSsid* wifiSsid,
    /* [in] */ const String& BSSID,
    /* [in] */ SupplicantState state)
{
    mNetworkId = networkId;
    mWifiSsid = wifiSsid;
    mBSSID = BSSID;
    mState = state;
}

ECode CStateChangeResult::GetWifiSsid(
    /* [out] */ IWifiSsid** ssid)
{
    VALIDATE_NOT_NULL(ssid);
    *ssid = mWifiSsid;
    REFCOUNT_ADD(*ssid);
    return NOERROR;
}

ECode CStateChangeResult::SetWifiSsid(
    /* [in] */ IWifiSsid* ssid)
{
    mWifiSsid = ssid;
    return NOERROR;
}

ECode CStateChangeResult::GetBSSID(
    /* [out] */ String* bssid)
{
    VALIDATE_NOT_NULL(bssid);
    *bssid = mBSSID;
    return NOERROR;
}

ECode CStateChangeResult::SetBSSID(
    /* [in] */ const String& bssid)
{
    mBSSID = bssid;
    return NOERROR;
}

ECode CStateChangeResult::GetNetworkId(
    /* [out] */ Int32* networkId)
{
    VALIDATE_NOT_NULL(networkId);
    *networkId = mNetworkId;
    return NOERROR;
}

ECode CStateChangeResult::SetNetworkId(
    /* [in] */ Int32 networkId)
{
    mNetworkId = networkId;
    return NOERROR;
}

ECode CStateChangeResult::GetSupplicantState(
    /* [out] */ SupplicantState* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CStateChangeResult::SetSupplicantState(
    /* [in] */ SupplicantState state)
{
    mState = state;
    return NOERROR;
}

ECode CStateChangeResult::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<StringBuffer> sb = new StringBuffer();
    sb->Append(" SSID: ");
    String wifissid;
    IObject::Probe(mWifiSsid)->ToString(&wifissid);
    sb->Append(wifissid);
    sb->Append(" BSSID: ");
    sb->Append(mBSSID);
    sb->Append(" nid: ");
    sb->Append(mNetworkId);
    sb->Append(" state: ");
    sb->Append(mState);
    *info = sb->ToString();
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namepsace Elastos
