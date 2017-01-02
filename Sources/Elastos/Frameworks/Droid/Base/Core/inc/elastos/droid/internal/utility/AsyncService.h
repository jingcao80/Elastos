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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_ASYNCSERVICE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_ASYNCSERVICE_H__

#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class AsyncService
    : public Service
    , public IAsyncService
{
public:
    class AsyncServiceInfo
        : public Object
        , public IAsyncServiceInfo
    {
    public:
        CAR_INTERFACE_DECL()

        /** Message Handler that will receive messages */
        AutoPtr<IHandler> mHandler;

        /**
         * The flags returned by onStartCommand on how to restart.
         * For instance @see android.app.Service#START_STICKY
         */
        Int32 mRestartFlags;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Get the handler
     */
    CARAPI GetHandler(
        /* [out] */ IHandler** handler);

    /**
     * onCreate
     */
    CARAPI OnCreate();

    /**
     * Sends the CMD_ASYNC_SERVICE_ON_START_INTENT message.
     */
    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* res);

    /**
     * Called when service is destroyed. After returning the
     * service is dead and no more processing should be expected
     * to occur.
     */
    CARAPI OnDestroy();

    /**
     * Returns the Messenger's binder.
     */
    CARAPI OnBind(
        /* [in] */ Intent* intent,
        /* [out] */ IBinder** binder);

protected:
    static const Boolean DBG;

    /** Messenger transport */
    AutoPtr<IMessenger> mMessenger;

    /** Message Handler that will receive messages */
    AutoPtr<IHandler> mHandler;

    AutoPtr<AsyncServiceInfo> mAsyncServiceInfo;

private:
    static const String TAG;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_ASYNCSERVICE_H__
