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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_SERVICEBINDER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_SERVICEBINDER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Abstract class to perform the work of binding and unbinding to the specified service interface.
 * Subclasses supply the service intent and component name and this class will invoke protected
 * methods when the class is bound, unbound, or upon failure.
 */
class ServiceBinder
    : public Object
    , public IServiceBinder
{
public:
    /**
     * Helper class to perform on-demand binding.
     */
    class Binder
        : public Object
    {
    public:
        Binder(
            /* [in] */ ServiceBinder* host);

        /**
         * Performs an asynchronous bind to the service (only if not already bound) and executes the
         * specified callback.
         *
         * @param callback The callback to notify of the binding's success or failure.
         */
        CARAPI Bind(
            /* [in] */ IServiceBinderBindCallback* callback);

    private:
        ServiceBinder* mHost;
    };

private:
    class ServiceBinderConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceBinderConnection(
            /* [in] */ ServiceBinder* host);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* componentName,
            /* [in] */ IBinder* binder);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* componentName);

    private:
        ServiceBinder* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ServiceBinder();

    /**
     * Persists the specified parameters and initializes the new instance.
     *
     * @param serviceAction The intent-action used with {@link Context#bindService}.
     * @param componentName The component name of the service with which to bind.
     * @param context The context.
     */
    CARAPI constructor(
        /* [in] */ const String& serviceAction,
        /* [in] */ IComponentName* componentName,
        /* [in] */ IContext* context);

    CARAPI IncrementAssociatedCallCount();

    CARAPI DecrementAssociatedCallCount();

    CARAPI GetAssociatedCallCount(
        /* [out] */ Int32* result);

    /**
     * Unbinds from the service if already bound, no-op otherwise.
     */
    CARAPI Unbind();

    CARAPI GetComponentName(
        /* [out] */ IComponentName** result);

    CARAPI IsServiceValid(
        /* [in] */ const String& actionName,
        /* [out] */ Boolean* result);

    CARAPI AddListener(
        /* [in] */ IServiceBinderListener* listener);

    CARAPI RemoveListener(
        /* [in] */ IServiceBinderListener* listener);

    /**
     * Sets the service interface after the service is bound or unbound.
     *
     * @param binder The actual bound service implementation.
     */
    virtual CARAPI SetServiceInterface(
        /* [in] */ IBinder* binder) = 0;

private:
    /**
     * Logs a standard message upon service disconnection. This method exists because there is no
     * single method called whenever the service unbinds and we want to log the same string in all
     * instances where that occurs.  (Context.unbindService() does not cause onServiceDisconnected
     * to execute).
     *
     * @param sourceTag Tag to disambiguate
     */
    CARAPI LogServiceDisconnected(
        /* [in] */ const String& sourceTag);

    /**
     * Notifies all the outstanding callbacks that the service is successfully bound. The list of
     * outstanding callbacks is cleared afterwards.
     */
    CARAPI HandleSuccessfulConnection();

    /**
     * Notifies all the outstanding callbacks that the service failed to bind. The list of
     * outstanding callbacks is cleared afterwards.
     */
    CARAPI HandleFailedConnection();

    /**
     * Handles a service disconnection.
     */
    CARAPI HandleServiceDisconnected();

    CARAPI ClearAbort();

    /**
     * Sets the (private) binder and updates the child class.
     *
     * @param binder The new binder value.
     */
    CARAPI SetBinder(
        /* [in] */ IBinder* binder);

private:
    /** The application context. */
    AutoPtr<IContext> mContext;

    /** The intent action to use when binding through {@link Context#bindService}. */
    String mServiceAction;

    /** The component name of the service to bind to. */
    AutoPtr<IComponentName> mComponentName;

    /** The set of callbacks waiting for notification of the binding's success or failure. */
    AutoPtr<ISet> mCallbacks;

    /** Used to bind and unbind from the service. */
    AutoPtr<IServiceConnection> mServiceConnection;

    /** The binder provided by {@link ServiceConnection#onServiceConnected} */
    AutoPtr<IBinder> mBinder;

    Int32 mAssociatedCallCount;

    /**
     * Indicates that an unbind request was made when the service was not yet bound. If the service
     * successfully connects when this is true, it should be unbound immediately.
     */
    Boolean mIsBindingAborted;

    /**
     * Set of currently registered listeners.
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mListeners;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_SERVICEBINDER_H__
