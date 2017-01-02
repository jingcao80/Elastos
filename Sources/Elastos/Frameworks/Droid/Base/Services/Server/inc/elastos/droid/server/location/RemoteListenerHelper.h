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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_REMOTELISTENERHELPER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_REMOTELISTENERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A helper class, that handles operations in remote listeners, and tracks for remote process death.
 */
class RemoteListenerHelper
    : public Object
    , public IRemoteListenerHelper
{
private:
    class LinkedListener
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        LinkedListener(
            /* [in] */ IInterface* listener,
            /* [in] */ RemoteListenerHelper* host);

        // @NonNull
        CARAPI GetUnderlyingListener(
            /* [out] */ IInterface** listener);

        // @Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IInterface> mListener;
        RemoteListenerHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RemoteListenerHelper(
        /* [in] */ const String& name);

    CARAPI AddListener(
        /* [in] */ IInterface* listener,
        /* [out] */ Boolean* result);

    CARAPI RemoveListener(
        /* [in] */ IInterface* listener,
        /* [out] */ Boolean* result);

    virtual CARAPI IsSupported(
        /* [out] */ Boolean* isSupported) = 0;

    virtual CARAPI RegisterWithService(
        /* [out] */ Boolean* isRegistered) = 0;

    virtual CARAPI UnregisterFromService() = 0;

    CARAPI Foreach(
        /* [in] */ IListenerOperation* operation);

private:
    String mTag;
    AutoPtr<IHashMap> mListenerMap;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_REMOTELISTENERHELPER_H__