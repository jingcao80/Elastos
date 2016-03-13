#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICE_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * A {@link android.app.Service} that provides telephone connections to processes running on an
 * Android device.
 * @hide
 */
// Service
class ConnectionService
    : public Object
    , public IConnectionService
{
public:
    class MyHandler
        : public Handler
    {
    public:
        class MyRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            MyRunnable(
                /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
                /* [in] */ const String& id,
                /* [in] */ IConnectionRequest* request,
                /* [in] */ Boolean isIncoming,
                /* [in] */ Boolean isUnknown,
                /* [in] */ MyHandler* host);

            CARAPI Run();

        public:
            AutoPtr<IPhoneAccountHandle> mConnectionManagerPhoneAccount;
            String mId;
            AutoPtr<IConnectionRequest> mRequest;
            Boolean mIsIncoming;
            Boolean mIsUnknown;
            MyHandler* mHost;
        };

    public:
        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ ConnectionService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        ConnectionService* mHost;
    };

    class ConferenceListener
        : public Object
        , public IConferenceListener
    {
    public:
        CAR_INTERFACE_DECL()

        ConferenceListener(
            /* [in] */ ConnectionService* host);

        CARAPI OnStateChanged(
            /* [in] */ IConference* conference,
            /* [in] */ Int32 oldState,
            /* [in] */ Int32 newState);

        CARAPI OnDisconnected(
            /* [in] */ IConference* conference,
            /* [in] */ IDisconnectCause* disconnectCause);

        CARAPI OnConnectionAdded(
            /* [in] */ IConference* conference,
            /* [in] */ IConnection* connection);

        CARAPI OnConnectionRemoved(
            /* [in] */ IConference* conference,
            /* [in] */ IConnection* connection);

        CARAPI OnConferenceableConnectionsChanged(
            /* [in] */ IConference* conference,
            /* [in] */ IList* conferenceableConnections);

        CARAPI OnDestroyed(
            /* [in] */ IConference* conference);

        CARAPI OnCapabilitiesChanged(
            /* [in] */ IConference* conference,
            /* [in] */ Int32 capabilities);

    public:
        ConnectionService* mHost;
    };

    class ConnectionListener
        : public Object
        , public IConnectionListener
    {
    public:
        CAR_INTERFACE_DECL()

        ConnectionListener(
            /* [in] */ ConnectionService* host);

        CARAPI OnStateChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 state);

        CARAPI OnDisconnected(
            /* [in] */ IConnection* c,
            /* [in] */ IDisconnectCause* disconnectCause);

        CARAPI OnVideoStateChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 videoState);

        CARAPI OnAddressChanged(
            /* [in] */ IConnection* c,
            /* [in] */ IUri* address,
            /* [in] */ Int32 presentation);

        CARAPI OnCallerDisplayNameChanged(
            /* [in] */ IConnection* c,
            /* [in] */ const String& callerDisplayName,
            /* [in] */ Int32 presentation);

        CARAPI OnDestroyed(
            /* [in] */ IConnection* c);

        CARAPI OnPostDialWait(
            /* [in] */ IConnection* c,
            /* [in] */ const String& remaining);

        CARAPI OnRingbackRequested(
            /* [in] */ IConnection* c,
            /* [in] */ Boolean ringback);

        CARAPI OnCallCapabilitiesChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 capabilities);

        CARAPI OnVideoProviderChanged(
            /* [in] */ IConnection* c,
            /* [in] */ IConnectionVideoProvider* videoProvider);

        CARAPI OnAudioModeIsVoipChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Boolean isVoip);

        CARAPI OnStatusHintsChanged(
            /* [in] */ IConnection* c,
            /* [in] */ IStatusHints* statusHints);

        CARAPI OnConferenceableConnectionsChanged(
            /* [in] */ IConnection* connection,
            /* [in] */ IList* conferenceableConnections);

        CARAPI OnConferenceChanged(
            /* [in] */ IConnection* connection,
            /* [in] */ IConference* conference);

    public:
        ConnectionService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ConnectionService();

    /** {@inheritDoc} */
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

    /** {@inheritDoc} */
    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Ask some other {@code ConnectionService} to create a {@code RemoteConnection} given an
     * incoming request. This is used to attach to existing incoming calls.
     *
     * @param connectionManagerPhoneAccount See description at
     *         {@link #onCreateOutgoingConnection(PhoneAccountHandle, ConnectionRequest)}.
     * @param request Details about the incoming call.
     * @return The {@code Connection} object to satisfy this call, or {@code null} to
     *         not handle the call.
     */
    CARAPI CreateRemoteIncomingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IRemoteConnection** result);

    /**
     * Ask some other {@code ConnectionService} to create a {@code RemoteConnection} given an
     * outgoing request. This is used to initiate new outgoing calls.
     *
     * @param connectionManagerPhoneAccount See description at
     *         {@link #onCreateOutgoingConnection(PhoneAccountHandle, ConnectionRequest)}.
     * @param request Details about the incoming call.
     * @return The {@code Connection} object to satisfy this call, or {@code null} to
     *         not handle the call.
     */
    CARAPI CreateRemoteOutgoingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IRemoteConnection** result);

    /**
     * Adds two {@code RemoteConnection}s to some {@code RemoteConference}.
     */
    CARAPI ConferenceRemoteConnections(
        /* [in] */ IRemoteConnection* a,
        /* [in] */ IRemoteConnection* b);

    /**
     * Adds a new conference call. When a conference call is created either as a result of an
     * explicit request via {@link #onConference} or otherwise, the connection service should supply
     * an instance of {@link Conference} by invoking this method. A conference call provided by this
     * method will persist until {@link Conference#destroy} is invoked on the conference instance.
     *
     * @param conference The new conference object.
     */
    CARAPI AddConference(
        /* [in] */ IConference* conference);

    /**
     * Returns all the active {@code Connection}s for which this {@code ConnectionService}
     * has taken responsibility.
     *
     * @return A collection of {@code Connection}s created by this {@code ConnectionService}.
     */
    CARAPI GetAllConnections(
        /* [out] */ ICollection** result);

    /**
     * Create a {@code Connection} given an incoming request. This is used to attach to existing
     * incoming calls.
     *
     * @param connectionManagerPhoneAccount See description at
     *         {@link #onCreateOutgoingConnection(PhoneAccountHandle, ConnectionRequest)}.
     * @param request Details about the incoming call.
     * @return The {@code Connection} object to satisfy this call, or {@code null} to
     *         not handle the call.
     */
    CARAPI OnCreateIncomingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IConnection** result);

    /**
     * Create a {@code Connection} given an outgoing request. This is used to initiate new
     * outgoing calls.
     *
     * @param connectionManagerPhoneAccount The connection manager account to use for managing
     *         this call.
     *         <p>
     *         If this parameter is not {@code null}, it means that this {@code ConnectionService}
     *         has registered one or more {@code PhoneAccount}s having
     *         {@link PhoneAccount#CAPABILITY_CONNECTION_MANAGER}. This parameter will contain
     *         one of these {@code PhoneAccount}s, while the {@code request} will contain another
     *         (usually but not always distinct) {@code PhoneAccount} to be used for actually
     *         making the connection.
     *         <p>
     *         If this parameter is {@code null}, it means that this {@code ConnectionService} is
     *         being asked to make a direct connection. The
     *         {@link ConnectionRequest#getAccountHandle()} of parameter {@code request} will be
     *         a {@code PhoneAccount} registered by this {@code ConnectionService} to use for
     *         making the connection.
     * @param request Details about the outgoing call.
     * @return The {@code Connection} object to satisfy this call, or the result of an invocation
     *         of {@link Connection#createFailedConnection(DisconnectCause)} to not handle the call.
     */
    CARAPI OnCreateOutgoingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IConnection** result);

    /**
     * Create a {@code Connection} for a new unknown call. An unknown call is a call originating
     * from the ConnectionService that was neither a user-initiated outgoing call, nor an incoming
     * call created using
     * {@code TelecomManager#addNewIncomingCall(PhoneAccountHandle, android.os.Bundle)}.
     *
     * @param connectionManagerPhoneAccount
     * @param request
     * @return
     *
     * @hide
     */
    CARAPI OnCreateUnknownConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IConnection** result);

    /**
     * Conference two specified connections. Invoked when the user has made a request to merge the
     * specified connections into a conference call. In response, the connection service should
     * create an instance of {@link Conference} and pass it into {@link #addConference}.
     *
     * @param connection1 A connection to merge into a conference call.
     * @param connection2 A connection to merge into a conference call.
     */
    CARAPI OnConference(
        /* [in] */ IConnection* connection1,
        /* [in] */ IConnection* connection2);

    CARAPI OnRemoteConferenceAdded(
        /* [in] */ IRemoteConference* conference);

    /**
     * @hide
     */
    CARAPI ContainsConference(
        /* [in] */ IConference* conference,
        /* [out] */ Boolean* result);

    /** {@hide} */
    CARAPI AddRemoteConference(
        /* [in] */ IRemoteConference* remoteConference);

    CARAPI GetHandler(
        /* [out] */ IHandler** handler);

    /**
     * This can be used by telecom to either create a new outgoing call or attach to an existing
     * incoming call. In either case, telecom will cycle through a set of services and call
     * createConnection util a connection service cancels the process or completes it successfully.
     */
    CARAPI_(void) CreateConnection(
        /* [in] */ IPhoneAccountHandle* callManagerAccount,
        /* [in] */ const String& callId,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ Boolean isIncoming,
        /* [in] */ Boolean isUnknown);

    CARAPI_(void) Abort(
        /* [in] */ const String& callId);

    CARAPI_(void) AnswerVideo(
        /* [in] */ const String& callId,
        /* [in] */ Int32 videoState);

    CARAPI_(void) Answer(
        /* [in] */ const String& callId);

    CARAPI_(void) Reject(
        /* [in] */ const String& callId);

    CARAPI_(void) Disconnect(
        /* [in] */ const String& callId);

    CARAPI_(void) Hold(
        /* [in] */ const String& callId);

    CARAPI_(void) Unhold(
        /* [in] */ const String& callId);

    CARAPI_(void) OnAudioStateChanged(
        /* [in] */ const String& callId,
        /* [in] */ IAudioState* audioState);

    CARAPI_(void) PlayDtmfTone(
        /* [in] */ const String& callId,
        /* [in] */ Char32 digit);

    CARAPI_(void) StopDtmfTone(
        /* [in] */ const String& callId);

    CARAPI_(void) Conference(
        /* [in] */ const String& callId1,
        /* [in] */ const String& callId2);

    CARAPI_(void) SplitFromConference(
        /* [in] */ const String& callId);

    CARAPI_(void) MergeConference(
        /* [in] */ const String& callId);

    CARAPI_(void) SwapConference(
        /* [in] */ const String& callId);

    CARAPI_(void) OnPostDialContinue(
        /* [in] */ const String& callId,
        /* [in] */ Boolean proceed);

    CARAPI_(void) OnAdapterAttached();

    CARAPI_(void) OnAccountsInitialized();

    CARAPI_(void) AddConnection(
        /* [in] */ const String& callId,
        /* [in] */ IConnection* connection);

    CARAPI_(void) RemoveConnection(
        /* [in] */ IConnection* connection);

    CARAPI_(String) AddConferenceInternal(
        /* [in] */ IConference* conference);

    CARAPI_(void) RemoveConference(
        /* [in] */ IConference* conference);

    CARAPI_(AutoPtr<IConnection>) FindConnectionForAction(
        /* [in] */ const String& callId,
        /* [in] */ const String& action);

    static CARAPI_(AutoPtr<IConnection>) GetNullConnection();

    CARAPI_(AutoPtr<IConference>) FindConferenceForAction(
        /* [in] */ const String& conferenceId,
        /* [in] */ const String& action);

    CARAPI_(AutoPtr<IList>) CreateConnectionIdList(
        /* [in] */ IList* connections);

    CARAPI_(AutoPtr<IConference>) GetNullConference();

    CARAPI_(void) EndAllConnections();

public:
    // Flag controlling whether PII is emitted into the logs
    static const Boolean PII_DEBUG;

    static AutoPtr<IConnection> sNullConnection;

    AutoPtr<IMap> mConnectionById;
    AutoPtr<IMap> mIdByConnection;
    AutoPtr<IMap> mConferenceById;
    AutoPtr<IMap> mIdByConference;
    AutoPtr<IRemoteConnectionManager> mRemoteConnectionManager;
    AutoPtr<IList> mPreInitializationConnectionRequests;
    AutoPtr<IConnectionServiceAdapter> mAdapter;

    Boolean mAreAccountsInitialized;
    AutoPtr<IConference> sNullConference;

    AutoPtr<IBinder> mBinder;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IConferenceListener> mConferenceListener;

    AutoPtr<IConnectionListener> mConnectionListener;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICE_H__