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

#ifndef __ELASTOS_DROID_TELECOM_CONNECTIONSERVICEADAPTERSERVANT_H__
#define __ELASTOS_DROID_TELECOM_CONNECTIONSERVICEADAPTERSERVANT_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telecom::IIConnectionServiceAdapter;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * A component that provides an RPC servant implementation of {@link IConnectionServiceAdapter},
 * posting incoming messages on the main thread on a client-supplied delegate object.
 *
 * TODO: Generate this and similar classes using a compiler starting from AIDL interfaces.
 *
 * @hide
 */
class ConnectionServiceAdapterServant
    : public Object
    , public IConnectionServiceAdapterServant
{
public:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ConnectionServiceAdapterServant::MyHandler")

        MyHandler(
            /* [in] */ ConnectionServiceAdapterServant* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        // Internal method defined to centralize handling of RemoteException
        void InternalHandleMessage(
            /* [in] */ IMessage* msg);

    public:
        ConnectionServiceAdapterServant* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ConnectionServiceAdapterServant();

    CARAPI constructor(
        /* [in] */ IIConnectionServiceAdapter* delegate);

    CARAPI GetStub(
        /* [out] */ IIConnectionServiceAdapter** result);

private:
    AutoPtr<IIConnectionServiceAdapter> mDelegate;

    AutoPtr<MyHandler> mHandler;

    AutoPtr<IIConnectionServiceAdapter> mStub;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CONNECTIONSERVICEADAPTERSERVANT_H__