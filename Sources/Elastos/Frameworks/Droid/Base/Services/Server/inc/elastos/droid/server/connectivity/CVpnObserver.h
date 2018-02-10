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

#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_CVPNOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_CVPNOBSERVER_H__

#include "_Elastos_Droid_Server_Connectivity_CVpnObserver.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Server::Net::BaseNetworkObserver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

class Vpn;

CarClass(CVpnObserver)
    , public BaseNetworkObserver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE vpnHost);

    CARAPI InterfaceStatusChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI InterfaceRemoved(
        /* [in] */ const String& iface);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Vpn* mHost;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_CVPNOBSERVER_H__
