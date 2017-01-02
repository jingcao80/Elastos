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

#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceRequestHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceRequest.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pServiceRequestHelper, Singleton, IWifiP2pServiceRequestHelper)

CAR_SINGLETON_IMPL(CWifiP2pServiceRequestHelper)

ECode CWifiP2pServiceRequestHelper::NewInstance(
    /* [in] */ Int32 protocolType,
    /* [in] */ const String& queryData,
    /* [out] */ IWifiP2pServiceRequest** instance)
{
    return CWifiP2pServiceRequest::NewInstance(protocolType, queryData, instance);
}

ECode CWifiP2pServiceRequestHelper::NewInstance(
    /* [in] */ Int32 protocolType,
    /* [out] */ IWifiP2pServiceRequest** instance)
{
    return CWifiP2pServiceRequest::NewInstance(protocolType, String(NULL), instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
