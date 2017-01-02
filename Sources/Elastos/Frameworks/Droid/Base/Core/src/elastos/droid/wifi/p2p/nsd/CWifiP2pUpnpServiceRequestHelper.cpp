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

#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceRequestHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceRequest.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pUpnpServiceRequestHelper, Singleton, IWifiP2pUpnpServiceRequestHelper)

CAR_SINGLETON_IMPL(CWifiP2pUpnpServiceRequestHelper)

ECode CWifiP2pUpnpServiceRequestHelper::NewInstance(
    /* [out] */ IWifiP2pUpnpServiceRequest** instance)
{
    return CWifiP2pUpnpServiceRequest::NewInstance(instance);
}

ECode CWifiP2pUpnpServiceRequestHelper::NewInstance(
    /* [in] */ const String& st,
    /* [out] */ IWifiP2pUpnpServiceRequest** instance)
{
    return CWifiP2pUpnpServiceRequest::NewInstance(st, instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
