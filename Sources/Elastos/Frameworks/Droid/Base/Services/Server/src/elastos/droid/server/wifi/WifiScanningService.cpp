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

#include "elastos/droid/server/wifi/WifiScanningService.h"
#include "elastos/droid/server/wifi/CWifiScanningServiceImpl.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                         WifiScanningService
//=====================================================================
const String WifiScanningService::TAG("WifiScanningService");

WifiScanningService::WifiScanningService()
{
}

ECode WifiScanningService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    Logger::I(TAG, "Creating %s", IContext::WIFI_SCANNING_SERVICE.string());
    return NOERROR;
}

ECode WifiScanningService::OnStart()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIWifiScanner> wifiScanner;
    CWifiScanningServiceImpl::New(context, (IIWifiScanner**)&wifiScanner);
    mImpl = (WifiScanningServiceImpl*)wifiScanner.Get();

    Logger::I(TAG, "Starting %s", IContext::WIFI_SCANNING_SERVICE.string());
    PublishBinderService(IContext::WIFI_SCANNING_SERVICE, mImpl);
    return NOERROR;
}

ECode WifiScanningService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        Logger::I(TAG, "Registering %s", IContext::WIFI_SCANNING_SERVICE.string());
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        if (mImpl == NULL) {
            AutoPtr<IIWifiScanner> wifiScanner;
            CWifiScanningServiceImpl::New(context, (IIWifiScanner**)&wifiScanner);
            mImpl = (WifiScanningServiceImpl*)wifiScanner.Get();
        }
        mImpl->StartService(context);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
