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

#ifndef __ELASTOS_DROID_TELECOM_REMOTECONNECTIONMANAGER_H__
#define __ELASTOS_DROID_TELECOM_REMOTECONNECTIONMANAGER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Telecom::IIConnectionService;
using Elastos::Core::Object;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * @hide
 */
class RemoteConnectionManager
    : public Object
    , public IRemoteConnectionManager
{
public:
    CAR_INTERFACE_DECL()

    RemoteConnectionManager();

    CARAPI constructor(
        /* [in] */ IConnectionService* ourConnectionServiceImpl);

    CARAPI AddConnectionService(
        /* [in] */ IComponentName* componentName,
        /* [in] */ IIConnectionService* outgoingConnectionServiceRpc);

    CARAPI CreateRemoteConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ Boolean isIncoming,
        /* [out] */ IRemoteConnection** result);

    CARAPI ConferenceRemoteConnections(
        /* [in] */ IRemoteConnection* a,
        /* [in] */ IRemoteConnection* b);

private:
    AutoPtr<IMap> mRemoteConnectionServices;
    AutoPtr<IConnectionService> mOurConnectionServiceImpl;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_REMOTECONNECTIONMANAGER_H__