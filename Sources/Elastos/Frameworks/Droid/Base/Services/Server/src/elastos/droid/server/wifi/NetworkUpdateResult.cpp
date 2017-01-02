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
#include "elastos/droid/server/wifi/NetworkUpdateResult.h"

using Elastos::Droid::Wifi::IWifiConfiguration;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

NetworkUpdateResult::NetworkUpdateResult(
    /* [in] */ Int32 id)
    : mNetId(id)
    , mIpChanged(FALSE)
    , mProxyChanged(FALSE)
    , mIsNewNetwork(FALSE)
{}

NetworkUpdateResult::NetworkUpdateResult(
    /* [in] */ Boolean ip,
    /* [in] */ Boolean proxy)
    : mNetId(IWifiConfiguration::INVALID_NETWORK_ID)
    , mIpChanged(ip)
    , mProxyChanged(proxy)
    , mIsNewNetwork(FALSE)
{}

void NetworkUpdateResult::SetNetworkId(
    /* [in] */ Int32 id)
{
    mNetId = id;
}

Int32 NetworkUpdateResult::GetNetworkId()
{
    return mNetId;
}

void NetworkUpdateResult::SetIpChanged(
    /* [in] */ Boolean ip)
{
    mIpChanged = ip;
}

Boolean NetworkUpdateResult::HasIpChanged()
{
    return mIpChanged;
}

void NetworkUpdateResult::SetProxyChanged(
    /* [in] */ Boolean proxy)
{
    mProxyChanged = proxy;
}

Boolean NetworkUpdateResult::HasProxyChanged()
{
    return mProxyChanged;
}

Boolean NetworkUpdateResult::IsNewNetwork()
{
    return mIsNewNetwork;
}

void NetworkUpdateResult::SetIsNewNetwork(
    /* [in] */ Boolean isNew)
{
    mIsNewNetwork = isNew;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
