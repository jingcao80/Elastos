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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/wifi/p2p/WifiP2pService.h"
#include "elastos/droid/server/wifi/p2p/CWifiP2pServiceImpl.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

//=====================================================================
//                            WifiP2pService
//=====================================================================
const String WifiP2pService::TAG("WifiP2pService");

WifiP2pService::WifiP2pService()
{
}

ECode WifiP2pService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    AutoPtr<IIWifiP2pManager> wifip2pm;
    CWifiP2pServiceImpl::New(context, (IIWifiP2pManager**)&wifip2pm);
    mImpl = (WifiP2pServiceImpl*)wifip2pm.Get();
    return NOERROR;
}

ECode WifiP2pService::OnStart()
{
    Logger::I(TAG, "Registering %s, mImpl: %p, Interface: %p", IContext::WIFI_P2P_SERVICE.string(), mImpl.Get(), TO_IINTERFACE(mImpl));
    PublishBinderService(IContext::WIFI_P2P_SERVICE, mImpl);
    return NOERROR;
}

ECode WifiP2pService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        mImpl->ConnectivityServiceReady();
    }
    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
