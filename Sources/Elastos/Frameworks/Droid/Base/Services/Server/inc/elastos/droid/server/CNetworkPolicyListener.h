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

#ifndef __ELASTOS_DROID_SERVER_CNETWORKPOLICYLISTENER_H__
#define __ELASTOS_DROID_SERVER_CNETWORKPOLICYLISTENER_H__

#include "_Elastos_Droid_Server_CNetworkPolicyListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IINetworkPolicyListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {

class CConnectivityService;

CarClass(CNetworkPolicyListener)
    , public Object
    , public IINetworkPolicyListener
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIConnectivityManager* owner);

    CARAPI OnUidRulesChanged(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 uidRules);

    CARAPI OnMeteredIfacesChanged(
        /* [in] */ ArrayOf<String>* meteredIfaces);

    CARAPI OnRestrictBackgroundChanged(
        /* [in] */ Boolean restrictBackground);

    CARAPI ToString(
        /* [out] */ String* str);
private:
    CConnectivityService* mOwner;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CNETWORKPOLICYLISTENER_H__
