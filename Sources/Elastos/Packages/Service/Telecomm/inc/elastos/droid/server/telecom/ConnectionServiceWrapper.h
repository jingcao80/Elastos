
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CONNECTIONSERVICEWRAPPER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CONNECTIONSERVICEWRAPPER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/ConnectionServiceRepository.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "elastos/droid/server/telecom/ServiceBinder.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telecom::IIConnectionService;
using Elastos::Droid::Internal::Telecom::IIConnectionServiceAdapter;
using Elastos::Droid::Internal::Telecom::IIVideoProvider;
using Elastos::Droid::Internal::Telecom::IRemoteServiceCallback;
using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::Telecom::IConnectionRequest;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::IParcelableConnection;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::IStatusHints;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Wrapper for {@link IConnectionService}s, handles binding to {@link IConnectionService} and keeps
 * track of when the object can safely be unbound. Other classes should not use
 * {@link IConnectionService} directly and instead should use this class to invoke methods of
 * {@link IConnectionService}.
 */
class ConnectionServiceWrapper
    : public ServiceBinder
    , public IConnectionServiceWrapper
{
public:
    class Adapter
        : public Object
        , public IIConnectionServiceAdapter
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IInterface* host);

        // @Override
        CARAPI HandleCreateConnectionComplete(
            /* [in] */ const String& callId,
            /* [in] */ IConnectionRequest* request,
            /* [in] */ IParcelableConnection* connection);

        // @Override
        CARAPI SetActive(
            /* [in] */ const String& callId);

        // @Override
        CARAPI SetExtras(
            /* [in] */ const String& callId,
            /* [in] */ IBundle* extras);

        // @Override
        CARAPI SetRinging(
            /* [in] */ const String& callId);

        // @Override
        CARAPI SetVideoProvider(
            /* [in] */ const String& callId,
            /* [in] */ IIVideoProvider* videoProvider);

        // @Override
        CARAPI SetDialing(
            /* [in] */ const String& callId);

        // @Override
        CARAPI SetDisconnected(
            /* [in] */ const String& callId,
            /* [in] */ IDisconnectCause* disconnectCause);

        // @Override
        CARAPI SetOnHold(
            /* [in] */ const String& callId);

        // @Override
        CARAPI SetRingbackRequested(
            /* [in] */ const String& callId,
            /* [in] */ Boolean ringback);

        // @Override
        CARAPI RemoveCall(
            /* [in] */ const String& callId);

        // @Override
        CARAPI SetCallCapabilities(
            /* [in] */ const String& callId,
            /* [in] */ Int32 callCapabilities);

        // @Override
        CARAPI SetCallProperties(
            /* [in] */ const String& callId,
            /* [in] */ Int32 callProperties);

        // @Override
        CARAPI SetIsConferenced(
            /* [in] */ const String& callId,
            /* [in] */ const String& conferenceCallId);

        // @Override
        CARAPI AddConferenceCall(
            /* [in] */ const String& callId,
            /* [in] */ IParcelableConference* parcelableConference);

        // @Override
        CARAPI OnPostDialWait(
            /* [in] */ const String& callId,
            /* [in] */ const String& remaining);

        // @Override
        CARAPI QueryRemoteConnectionServices(
            /* [in] */ IRemoteServiceCallback* callback);

        // @Override
        CARAPI SetVideoState(
            /* [in] */ const String& callId,
            /* [in] */ Int32 videoState);

        // @Override
        CARAPI SetIsVoipAudioMode(
            /* [in] */ const String& callId,
            /* [in] */ Boolean isVoip);

        // @Override
        CARAPI SetStatusHints(
            /* [in] */ const String& callId,
            /* [in] */ IStatusHints* statusHints);

        // @Override
        CARAPI SetAddress(
            /* [in] */ const String& callId,
            /* [in] */ IUri* address,
            /* [in] */ Int32 presentation);

        // @Override
        CARAPI SetCallerDisplayName(
            /* [in] */ const String& callId,
            /* [in] */ const String& callerDisplayName,
            /* [in] */ Int32 presentation);

        // @Override
        CARAPI SetConferenceableConnections(
            /* [in] */ const String& callId,
            /* [in] */ IList* conferenceableCallIds);

        // @Override
        CARAPI SetPhoneAccountHandle(
            /* [in] */ const String& callId,
            /* [in] */ IPhoneAccountHandle* pHandle);

        // @Override
        CARAPI SetCallSubstate(
            /* [in] */ const String& callId,
            /* [in] */ Int32 callSubstate);

        // @Override
        CARAPI AddExistingConnection(
            /* [in] */ const String& callId,
            /* [in] */ IParcelableConnection* connection);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        ConnectionServiceWrapper* mHost;
    };

private:
    class SubCreateBindCallback
        : public Object
        , public IServiceBinderBindCallback
    {
    public:
        CAR_INTERFACE_DECL()

        SubCreateBindCallback(
            /* [in] */ ConnectionServiceWrapper* host,
            /* [in] */ ICall* call,
            /* [in] */ ICreateConnectionResponse* response);

        // @Override
        CARAPI OnSuccess();

        // @Override
        CARAPI OnFailure();

    private:
        ConnectionServiceWrapper* mHost;
        AutoPtr<ICall> mCall;
        AutoPtr<ICreateConnectionResponse> mResponse;
    };

    class SubHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ConnectionServiceWrapper::SubHandler")

        SubHandler(
            /* [in] */ ConnectionServiceWrapper* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<ConnectionServiceWrapper> mHost;// life is longer
    };

    class SubQueryBindCallback
        : public Object
        , public IServiceBinderBindCallback
    {
    public:
        CAR_INTERFACE_DECL()

        SubQueryBindCallback(
            /* [in] */ ConnectionServiceWrapper* host,
            /* [in] */ IConnectionServiceWrapper* currentSimService,
            /* [in] */ IList* simServiceComponentNames,
            /* [in] */ IList* simServiceBinders,
            /* [in] */ IRemoteServiceCallback* callback,
            /* [in] */ ISet* simServices);

        // @Override
        CARAPI OnSuccess();

        // @Override
        CARAPI OnFailure();

    private:
        CARAPI MaybeComplete();

    private:
        ConnectionServiceWrapper* mHost;
        AutoPtr<IConnectionServiceWrapper> mCurrentSimService;
        AutoPtr<IList> mSimServiceComponentNames;
        AutoPtr<IList> mSimServiceBinders;
        AutoPtr<IRemoteServiceCallback> mCallback;
        AutoPtr<ISet> mSimServices;
    };


public:
    CAR_INTERFACE_DECL()

    ConnectionServiceWrapper();

    /**
     * Creates a connection service.
     *
     * @param componentName The component name of the service with which to bind.
     * @param connectionServiceRepository Connection service repository.
     * @param phoneAccountRegistrar Phone account registrar
     * @param context The context.
     */
    CARAPI constructor(
        /* [in] */ IComponentName* componentName,
        /* [in] */ ConnectionServiceRepository* connectionServiceRepository,
        /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
        /* [in] */ IContext* context);

    /**
     * Creates a new connection for a new outgoing call or to attach to an existing incoming call.
     */
    CARAPI CreateConnection(
        /* [in] */ ICall* call,
        /* [in] */ ICreateConnectionResponse* response);

    /** @see ConnectionService#abort(String) */
    CARAPI Abort(
        /* [in] */ ICall* call);

    /** @see ConnectionService#hold(String) */
    CARAPI Hold(
        /* [in] */ ICall* call);

    /** @see ConnectionService#unhold(String) */
    CARAPI Unhold(
        /* [in] */ ICall* call);

    /** @see ConnectionService#onAudioStateChanged(String,AudioState) */
    CARAPI OnAudioStateChanged(
        /* [in] */ ICall* activeCall,
        /* [in] */ IAudioState* audioState);

    /** @see ConnectionService#disconnect(String) */
    CARAPI Disconnect(
        /* [in] */ ICall* call);

    /** @see ConnectionService#answer(String,int) */
    CARAPI Answer(
        /* [in] */ ICall* call,
        /* [in] */ Int32 videoState);

    /** @see ConnectionService#deflect(String, String) */
    CARAPI Deflect(
        /* [in] */ ICall* call,
        /* [in] */ const String& number);

    /** @see ConnectionService#reject(String) */
    CARAPI Reject(
        /* [in] */ ICall* call);

    /** @see ConnectionService#playDtmfTone(String,char) */
    CARAPI PlayDtmfTone(
        /* [in] */ ICall* call,
        /* [in] */ Char32 digit);

    /** @see ConnectionService#setLocalCallHold(String,int) */
    CARAPI SetLocalCallHold(
        /* [in] */ ICall* call,
        /* [in] */ Int32 lchStatus);

    /** @see ConnectionService#setLocalCallHold(String,int) */
    CARAPI SetActiveSubscription(
        /* [in] */ ICall* call);

    /** @see ConnectionService#stopDtmfTone(String) */
    CARAPI StopDtmfTone(
        /* [in] */ ICall* call);

    CARAPI AddCall(
        /* [in] */ ICall* call);

    /**
     * Associates newCall with this connection service by replacing callToReplace.
     */
    CARAPI ReplaceCall(
        /* [in] */ ICall* newCall,
        /* [in] */ ICall* callToReplace);

    CARAPI RemoveCall(
        /* [in] */ ICall* call);

    CARAPI RemoveCall(
        /* [in] */ const String& callId,
        /* [in] */ IDisconnectCause* disconnectCause);

    CARAPI RemoveCall(
        /* [in] */ ICall* call,
        /* [in] */ IDisconnectCause* disconnectCause);

    CARAPI OnPostDialContinue(
        /* [in] */ ICall* call,
        /* [in] */ Boolean proceed);

    CARAPI Conference(
        /* [in] */ ICall* call,
        /* [in] */ ICall* otherCall);

    CARAPI SplitFromConference(
        /* [in] */ ICall* call);

    CARAPI MergeConference(
        /* [in] */ ICall* call);

    CARAPI SwapConference(
        /* [in] */ ICall* call);

    /** {@inheritDoc} */
    // @Override
    CARAPI SetServiceInterface(
        /* [in] */ IBinder* binder);

private:
    /** See {@link IConnectionService#addConnectionServiceAdapter}. */
    CARAPI AddConnectionServiceAdapter(
        /* [in] */ IIConnectionServiceAdapter* adapter);

    CARAPI HandleCreateConnectionComplete(
        /* [in] */ const String& callId,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ IParcelableConnection* connection);

    /**
     * Called when the associated connection service dies.
     */
    CARAPI HandleConnectionServiceDeath();

    CARAPI LogIncoming(
    /* [in] */ const char * fmt, ...);

    CARAPI LogOutgoing(
    /* [in] */ const char * fmt, ...);

    CARAPI QueryRemoteConnectionServices(
        /* [in] */ IRemoteServiceCallback* callback);

    CARAPI SetRemoteServices(
        /* [in] */ IRemoteServiceCallback* callback,
        /* [in] */ IList* componentNames,
        /* [in] */ IList* binders);

    CARAPI NoRemoteServices(
        /* [in] */ IRemoteServiceCallback* callback);

private:
    static const Int32 MSG_HANDLE_CREATE_CONNECTION_COMPLETE;

    static const Int32 MSG_SET_ACTIVE;

    static const Int32 MSG_SET_RINGING;

    static const Int32 MSG_SET_DIALING;

    static const Int32 MSG_SET_DISCONNECTED;

    static const Int32 MSG_SET_ON_HOLD;

    static const Int32 MSG_SET_RINGBACK_REQUESTED;

    static const Int32 MSG_SET_CALL_CAPABILITIES;

    static const Int32 MSG_SET_IS_CONFERENCED;

    static const Int32 MSG_ADD_CONFERENCE_CALL;

    static const Int32 MSG_REMOVE_CALL;

    static const Int32 MSG_ON_POST_DIAL_WAIT;

    static const Int32 MSG_QUERY_REMOTE_CALL_SERVICES;

    static const Int32 MSG_SET_VIDEO_PROVIDER;

    static const Int32 MSG_SET_IS_VOIP_AUDIO_MODE;

    static const Int32 MSG_SET_STATUS_HINTS;

    static const Int32 MSG_SET_ADDRESS;

    static const Int32 MSG_SET_CALLER_DISPLAY_NAME;

    static const Int32 MSG_SET_VIDEO_STATE;

    static const Int32 MSG_SET_CONFERENCEABLE_CONNECTIONS;

    static const Int32 MSG_SET_EXTRAS;

    static const Int32 MSG_SET_DISCONNECTED_WITH_SUPP_NOTIFICATION;

    static const Int32 MSG_SET_PHONE_ACCOUNT;

    static const Int32 MSG_SET_CALL_SUBSTATE;

    static const Int32 MSG_ADD_EXISTING_CONNECTION;

    static const Int32 MSG_SET_CALL_PROPERTIES;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IIConnectionServiceAdapter> mAdapter;
    AutoPtr<CallsManager> mCallsManager;
    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mPendingConferenceCalls;
    AutoPtr<ICallIdMapper> mCallIdMapper;
    AutoPtr<IMap> mPendingResponses;

    AutoPtr<ServiceBinder::Binder> mBinder;

    AutoPtr<IIConnectionService> mServiceInterface;

    AutoPtr<ConnectionServiceRepository> mConnectionServiceRepository;

    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CONNECTIONSERVICEWRAPPER_H__
