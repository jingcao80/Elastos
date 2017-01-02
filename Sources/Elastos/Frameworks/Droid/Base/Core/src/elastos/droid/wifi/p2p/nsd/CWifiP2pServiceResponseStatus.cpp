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

#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponseStatus.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pServiceResponseStatus, Object, IWifiP2pServiceResponseStatus)

CAR_OBJECT_IMPL(CWifiP2pServiceResponseStatus)

ECode CWifiP2pServiceResponseStatus::constructor()
{
    return NOERROR;
}

ECode CWifiP2pServiceResponseStatus::ToString(
    /* [in] */ Int32 status,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    switch(status) {
        case IWifiP2pServiceResponseStatus::SUCCESS:
            *string = String("SUCCESS");
            break;
        case IWifiP2pServiceResponseStatus::SERVICE_PROTOCOL_NOT_AVAILABLE:
            *string = String("SERVICE_PROTOCOL_NOT_AVAILABLE");
            break;
        case IWifiP2pServiceResponseStatus::REQUESTED_INFORMATION_NOT_AVAILABLE:
            *string = String("REQUESTED_INFORMATION_NOT_AVAILABLE");
            break;
        case IWifiP2pServiceResponseStatus::BAD_REQUEST:
            *string = String("BAD_REQUEST");
            break;
        default:
            *string = String("UNKNOWN");
            break;
    }
    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
