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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_INCALLCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_INCALLCONTROLLER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::Telecom::IParcelableCall;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Binds to {@link IInCallService} and provides the service to {@link CallsManager} through which it
 * can send updates to the in-call app. This class is created and owned by CallsManager and retains
 * a binding to the {@link IInCallService} (implemented by the in-call app).
 */
class InCallController
    : public CallsManagerListenerBase
{
private:
    /**
     * Used to bind to the in-call app and triggers the start of communication between
     * this class and in-call app.
     */
    class InCallServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InCallServiceConnection(
            /* [in] */ InCallController* host);

        /** {@inheritDoc} */
        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        /** {@inheritDoc} */
        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        InCallController* mHost;
    };

    class SubCallListenerBase
        : public Call::ListenerBase
    {
    public:
        SubCallListenerBase(
            /* [in] */ InCallController* host);

        // @Override
        CARAPI OnCallCapabilitiesChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallPropertiesChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCannedSmsResponsesLoaded(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnVideoCallProviderChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnStatusHintsChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnHandleChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallerDisplayNameChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnVideoStateChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnTargetPhoneAccountChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnConferenceableCallsChanged(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallSubstateChanged(
            /* [in] */ ICall* call);

    private:
        InCallController* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnCallAdded(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    // @Override
    CARAPI OnCallExtrasUpdated(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnConnectionServiceChanged(
        /* [in] */ ICall* call,
        /* [in] */ IConnectionServiceWrapper* oldService,
        /* [in] */ IConnectionServiceWrapper* newService);

    // @Override
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* oldAudioState,
        /* [in] */ IAudioState* newAudioState);

    // @Override
    CARAPI OnCanAddCallChanged(
        /* [in] */ Boolean canAddCall);

    CARAPI OnPostDialWait(
        /* [in] */ ICall* call,
        /* [in] */ const String& remaining);

    // @Override
    CARAPI OnIsConferencedChanged(
        /* [in] */ ICall* call);

    CARAPI BringToForeground(
        /* [in] */ Boolean showDialpad);

private:
    /**
     * Unbinds an existing bound connection to the in-call app.
     */
    CARAPI Unbind();

    /**
     * Binds to the in-call app if not already connected by binding directly to the saved
     * component name of the {@link IInCallService} implementation.
     */
    CARAPI Bind();

    /**
     * Persists the {@link IInCallService} instance and starts the communication between
     * this class and in-call app by sending the first update to in-call app. This method is
     * called after a successful binding connection is established.
     *
     * @param componentName The service {@link ComponentName}.
     * @param service The {@link IInCallService} implementation.
     */
    CARAPI OnConnected(
        /* [in] */ IComponentName* componentName,
        /* [in] */ IBinder* service);

    /**
     * Cleans up an instance of in-call app after the service has been unbound.
     *
     * @param disconnectedComponent The {@link ComponentName} of the service which disconnected.
     */
    CARAPI OnDisconnected(
        /* [in] */ IComponentName* disconnectedComponent);

    /**
     * Informs all {@link InCallService} instances of the updated call information.  Changes to the
     * video provider are only communicated to the default in-call UI.
     *
     * @param call The {@link Call}.
     */
    CARAPI UpdateCall(
        /* [in] */ ICall* call);

    /**
     * Parcels all information for a {@link Call} into a new {@link ParcelableCall} instance.
     *
     * @param call The {@link Call} to parcel.
     * @param includeVideoProvider When {@code true}, the {@link IVideoProvider} is included in the
     *      parcelled call.  When {@code false}, the {@link IVideoProvider} is not included.
     * @return The {@link ParcelableCall} containing all call information from the {@link Call}.
     */
    CARAPI ToParcelableCall(
        /* [in] */ ICall* call,
        /* [in] */ Boolean includeVideoProvider,
        /* [out] */ IParcelableCall** result);

    /**
     * Adds the call to the list of calls tracked by the {@link InCallController}.
     * @param call The call to add.
     */
    CARAPI AddCall(
        /* [in] */ ICall* call);

private:
    AutoPtr<ICallListener> mCallListener;

    /**
     * Maintains a binding connection to the in-call app(s).
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<IMap> mServiceConnections;

    /** The in-call app implementations, see {@link IInCallService}. */
    AutoPtr<IMap> mInCallServices;

    AutoPtr<ICallIdMapper> mCallIdMapper;

    /** The {@link ComponentName} of the default InCall UI. */
    AutoPtr<IComponentName> mInCallComponentName;

    AutoPtr<IContext> mContext;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_INCALLCONTROLLER_H__
