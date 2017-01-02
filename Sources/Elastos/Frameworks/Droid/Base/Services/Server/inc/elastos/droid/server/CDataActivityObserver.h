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

#ifndef __ELASTOS_DROID_SERVER_CDATAACTIVITYOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CDATAACTIVITYOBSERVER_H__

#include "_Elastos_Droid_Server_CDataActivityObserver.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Server::Net::BaseNetworkObserver;

namespace Elastos {
namespace Droid {
namespace Server {

class CConnectivityService;

CarClass(CDataActivityObserver), public BaseNetworkObserver
{
public:
    CAR_OBJECT_DECL()

    /**
     * Interface data activity status is changed.
     *
     * @param iface The interface.
     * @param active  True if the interface is actively transmitting data, false if it is idle.
     */
    CARAPI InterfaceClassDataActivityChanged(
        /* [in] */ const String& label,
        /* [in] */ Boolean active,
        /* [in] */ Int64 tsNanos);

    CARAPI constructor(
        /* [in] */ IIConnectivityManager* host);

private:
    CConnectivityService* mHost;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CDATAACTIVITYOBSERVER_H__
