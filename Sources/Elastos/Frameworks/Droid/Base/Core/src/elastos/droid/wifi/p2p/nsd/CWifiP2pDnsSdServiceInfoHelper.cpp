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

#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceInfoHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pDnsSdServiceInfoHelper, Singleton, IWifiP2pDnsSdServiceInfoHelper)

CAR_SINGLETON_IMPL(CWifiP2pDnsSdServiceInfoHelper)

ECode CWifiP2pDnsSdServiceInfoHelper::NewInstance(
    /* [in] */ const String& instanceName,
    /* [in] */ const String& serviceType,
    /* [in] */ IMap* txtMap,
    /* [out] */ IWifiP2pDnsSdServiceInfo** instance)
{
    return CWifiP2pDnsSdServiceInfo::NewInstance(
        instanceName, serviceType, txtMap, instance);
}

ECode CWifiP2pDnsSdServiceInfoHelper::CreateRequest(
    /* [in] */ const String& dnsName,
    /* [in] */ const Int32 dnsType,
    /* [in] */ const Int32 versionNumber,
    /* [out] */ String* request)
{
    return CWifiP2pDnsSdServiceInfo::CreateRequest(
        dnsName, dnsType, versionNumber, request);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
