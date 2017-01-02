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

#ifndef __ELASTOS_DROID_APP_TRUST_TRUSTMANAGER_H__
#define __ELASTOS_DROID_APP_TRUST_TRUSTMANAGER_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Trust {

/**
 * See {@link com.android.server.trust.TrustManagerService}
 * @hide
 */
class TrustManager
    : public Object
    , public ITrustManager
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TrustManager::MyHandler")

        MyHandler();

        ////@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    class TrustManagerTrustListener
        : public Object
        , public IITrustListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ITrustListener* trustListener,
            /* [ini] */ IHandler* handle);

        //@Override
        CARAPI OnTrustChanged(
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean initiatedByUser);

        //@Override
        CARAPI OnTrustManagedChanged(
            /* [in] */ Boolean managed,
            /* [in] */ Int32 userId);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        AutoPtr<ITrustListener> mTrustListener;
        AutoPtr<IHandler> mHandler;
    };

public:
    CAR_INTERFACE_DECL()

    TrustManager();

    virtual ~TrustManager();

    CARAPI constructor(
        /* [in] */ IBinder* b);

    /**
     * Reports that user {@param userId} has tried to unlock the device.
     *
     * @param successful if true, the unlock attempt was successful.
     *
     * Requires the {@link android.Manifest.permission#ACCESS_KEYGUARD_SECURE_STORAGE} permission.
     */
    CARAPI ReportUnlockAttempt(
        /* [in] */ Boolean successful,
        /* [in] */ Int32 userId);

    /**
     * Reports that the list of enabled trust agents changed for user {@param userId}.
     *
     * Requires the {@link android.Manifest.permission#ACCESS_KEYGUARD_SECURE_STORAGE} permission.
     */
    CARAPI ReportEnabledTrustAgentsChanged(
        /* [in] */ Int32 userId);

    /**
     * Reports that trust is disabled until credentials have been entered for user {@param userId}.
     *
     * Requires the {@link android.Manifest.permission#ACCESS_KEYGUARD_SECURE_STORAGE} permission.
     *
     * @param userId either an explicit user id or {@link android.os.UserHandle#USER_ALL}
     */
    CARAPI ReportRequireCredentialEntry(
        /* [in] */ Int32 userId);

    /**
     * Registers a listener for trust events.
     *
     * Requires the {@link android.Manifest.permission#TRUST_LISTENER} permission.
     */
    CARAPI RegisterTrustListener(
        /* [in] */ ITrustListener* trustListener);

    /**
     * Unregisters a listener for trust events.
     *
     * Requires the {@link android.Manifest.permission#TRUST_LISTENER} permission.
     */
    CARAPI UnregisterTrustListener(
        /* [in] */ ITrustListener* trustListener);

private:
    void OnError(
        /* [in] */ ECode ec);

private:
    friend class MyHandler;

    static const Int32 MSG_TRUST_CHANGED;
    static const Int32 MSG_TRUST_MANAGED_CHANGED;

    static const String TAG;
    static const String DATA_INITIATED_BY_USER;

    AutoPtr<IITrustManager> mService;
    AutoPtr<IArrayMap> mTrustListeners;// ArrayMap<ITrustListener, IITrustListener>;

    AutoPtr<MyHandler> mHandler;
};

} // namespace Trust
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_TRUST_TRUSTMANAGER_H__