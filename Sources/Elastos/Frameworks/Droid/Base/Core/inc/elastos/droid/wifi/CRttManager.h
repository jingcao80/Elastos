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

#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGER_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Wifi_CRttManager.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::Concurrent::ICountDownLatch;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManager)
    , public Object
    , public IRttManager
{
private:
    class ServiceHandler : public Handler
    {
    public:
        TO_STRING_IMPL("CRttManager::ServiceHandler")

        ServiceHandler(
            /* [in] */ ILooper* looper);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ReportSuccess(
            /* [in] */ IObject* listener,
            /* [in] */ IMessage* msg);

        CARAPI ReportFailure(
            /* [in] */ IObject* listener,
            /* [in] */ IMessage* msg);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIRttManager* service);

    CARAPI GetCapabilities(
        /* [out] */ IRttManagerCapabilities** result);

    CARAPI StartRanging(
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params,
        /* [in] */ IRttManagerRttListener* listener);

    CARAPI StopRanging(
        /* [in] */ IRttManagerRttListener* listener);

private:
    CARAPI_(void) Init();

    CARAPI ValidateChannel();

    static CARAPI_(Int32) PutListener(
        /* [in] */ IObject* listener);

    static CARAPI_(AutoPtr<IObject>) GetListener(
        /* [in] */ Int32 key);

    static CARAPI_(Int32) GetListenerKey(
        /* [in] */ IObject* listener);

    static CARAPI_(AutoPtr<IObject>) RemoveListener(
        /* [in] */ Int32 key);

    static CARAPI_(Int32) RemoveListener(
        /* [in] */ IObject* listener);

private:
    static const Boolean DBG;
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IIRttManager> mService;

    static const Int32 INVALID_KEY = 0;
    static Int32 sListenerKey;

    static AutoPtr<ISparseArray> sListenerMap;
    static Object sListenerMapLock;

    static AutoPtr<IAsyncChannel> sAsyncChannel;
    static AutoPtr<ICountDownLatch> sConnected;

    static Object sThreadRefLock;
    static Int32 sThreadRefCount;
    static AutoPtr<IHandlerThread> sHandlerThread;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGER_H__
