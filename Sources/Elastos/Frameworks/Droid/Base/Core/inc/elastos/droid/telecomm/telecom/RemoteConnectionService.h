#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONNECTIONSERVICE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONNECTIONSERVICE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/telecomm/telecom/ConnectionServiceAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Internal::IIConnectionService;
using Elastos::Droid::Telecomm::Internal::IRemoteServiceCallback;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::Telecomm::Internal::IIConnectionServiceAdapter;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Remote connection service which other connection services can use to place calls on their behalf.
 *
 * @hide
 */
class RemoteConnectionService
    : public Object
    , public IRemoteConnectionService
{
public:
    class MyConnectionServiceAdapter
        : public Object
        , public IIConnectionServiceAdapter
    {
    public:
        class RemoteConferenceCallback
            : public Object
            , public IRemoteConferenceCallback
        {
        public:
            CAR_INTERFACE_DECL()

            RemoteConferenceCallback(
                /* [in] */ const String& callId,
                /* [in] */ MyConnectionServiceAdapter* host);

            CARAPI OnStateChanged(
                /* [in] */ IRemoteConference* conference,
                /* [in] */ Int32 oldState,
                /* [in] */ Int32 newState);

            CARAPI OnDisconnected(
                /* [in] */ IRemoteConference* conference,
                /* [in] */ IDisconnectCause* disconnectCause);

            CARAPI OnConnectionAdded(
                /* [in] */ IRemoteConference* conference,
                /* [in] */ IRemoteConnection* connection);

            CARAPI OnConnectionRemoved(
                /* [in] */ IRemoteConference* conference,
                /* [in] */ IRemoteConnection* connection);

            CARAPI OnCapabilitiesChanged(
                /* [in] */ IRemoteConference* conference,
                /* [in] */ Int32 capabilities);

            CARAPI OnConferenceableConnectionsChanged(
                /* [in] */ IRemoteConference* conference,
                /* [in] */ IList* conferenceableConnections);

            CARAPI OnDestroyed(
                /* [in] */ IRemoteConference* conference);

        public:
            String mCallId;
            MyConnectionServiceAdapter* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        MyConnectionServiceAdapter(
            /* [in] */ RemoteConnectionService* host);

        CARAPI HandleCreateConnectionComplete(
            /* [in] */ const String& id,
            /* [in] */ IConnectionRequest* request,
            /* [in] */ IParcelableConnection* parcel);

        CARAPI SetActive(
            /* [in] */ const String& callId);

        CARAPI SetRinging(
            /* [in] */ const String& callId);

        CARAPI SetDialing(
            /* [in] */ const String& callId);

        CARAPI SetDisconnected(
            /* [in] */ const String& callId,
            /* [in] */ IDisconnectCause* disconnectCause);

        CARAPI SetOnHold(
            /* [in] */ const String& callId);

        CARAPI SetRingbackRequested(
            /* [in] */ const String& callId,
            /* [in] */ Boolean ringing);

        CARAPI SetCallCapabilities(
            /* [in] */ const String& callId,
            /* [in] */ Int32 callCapabilities);

        CARAPI SetIsConferenced(
            /* [in] */ const String& callId,
            /* [in] */ const String& conferenceCallId);

        CARAPI AddConferenceCall(
            /* [in] */ const String& callId,
            /* [in] */ IParcelableConference* parcel);

        CARAPI RemoveCall(
            /* [in] */ const String& callId);

        CARAPI OnPostDialWait(
            /* [in] */ const String& callId,
            /* [in] */ const String& remaining);

        CARAPI QueryRemoteConnectionServices(
            /* [in] */ IRemoteServiceCallback* callback);

        CARAPI SetVideoProvider(
            /* [in] */ const String& callId,
            /* [in] */ IIVideoProvider* videoProvider);

        CARAPI SetVideoState(
            /* [in] */ const String& callId,
            /* [in] */ Int32 videoState);

        CARAPI SetIsVoipAudioMode(
            /* [in] */ const String& callId,
            /* [in] */ Boolean isVoip);

        CARAPI SetStatusHints(
            /* [in] */ const String& callId,
            /* [in] */ IStatusHints* statusHints);

        CARAPI SetAddress(
            /* [in] */ const String& callId,
            /* [in] */ IUri* address,
            /* [in] */ Int32 presentation);

        CARAPI SetCallerDisplayName(
            /* [in] */ const String& callId,
            /* [in] */ const String& callerDisplayName,
            /* [in] */ Int32 presentation);

        CARAPI AsBinder(
            /* [out] */ IBinder** result);

        CARAPI SetConferenceableConnections(
            /* [in] */ const String& callId,
            /* [in] */ IList* conferenceableConnectionIds);

    public:
        RemoteConnectionService* mHost;
    };

    class ProxyDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ProxyDeathRecipient(
            /* [in] */ RemoteConnectionService* host);

        CARAPI ProxyDied();

    public:
        RemoteConnectionService* mHost;
    };

    class RemoteConnectionCallback
        : public Object
        , public IRemoteConnectionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        RemoteConnectionCallback(
            /* [in] */ const String& id,
            /* [in] */ RemoteConnectionService* host);

        CARAPI OnStateChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ Int32 state);

        CARAPI OnDisconnected(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ IDisconnectCause* disconnectCause);

        CARAPI OnRingbackRequested(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ Boolean ringback);

        CARAPI OnCallCapabilitiesChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ Int32 callCapabilities);

        CARAPI OnPostDialWait(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ const String& remainingPostDialSequence);

        CARAPI OnVoipAudioChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ Boolean isVoip);

        CARAPI OnStatusHintsChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ IStatusHints* statusHints);

        CARAPI OnAddressChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ IUri* address,
            /* [in] */ Int32 presentation);

        CARAPI OnCallerDisplayNameChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ const String& callerDisplayName,
            /* [in] */ Int32 presentation);

        CARAPI OnVideoStateChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ Int32 videoState);

        CARAPI OnDestroyed(
            /* [in] */ IRemoteConnection* connection);

        CARAPI OnConferenceableConnectionsChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ IList* conferenceableConnections);

        CARAPI OnVideoProviderChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ IRemoteConnectionVideoProvider* videoProvider);

        CARAPI OnConferenceChanged(
            /* [in] */ IRemoteConnection* connection,
            /* [in] */ IRemoteConference* conference);

    public:
        String mId;
        RemoteConnectionService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RemoteConnectionService();

    CARAPI constructor(
        /* [in] */ IIConnectionService* outgoingConnectionServiceRpc,
        /* [in] */ IConnectionService* ourConnectionServiceImpl);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI CreateRemoteConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ Boolean isIncoming,
        /* [out] */ IRemoteConnection** result);

    Boolean HasConnection(
        /* [in] */ const String& callId);

    AutoPtr<IRemoteConnection> FindConnectionForAction(
        /* [in] */ const String& callId,
        /* [in] */ const String& action);

    AutoPtr<IRemoteConference> FindConferenceForAction(
        /* [in] */ const String& callId,
        /* [in] */ const String& action);

    void MaybeDisconnectAdapter();

private:
    static AutoPtr<IRemoteConnection> NULL_CONNECTION;

    static AutoPtr<IRemoteConference> NULL_CONFERENCE;

    AutoPtr<IIConnectionServiceAdapter> mServantDelegate;

    AutoPtr<IConnectionServiceAdapterServant> mServant;

    AutoPtr<IProxyDeathRecipient> mDeathRecipient;

    AutoPtr<IIConnectionService> mOutgoingConnectionServiceRpc;
    AutoPtr<IConnectionService> mOurConnectionServiceImpl;
    AutoPtr<IMap> mConnectionById;
    AutoPtr<IMap> mConferenceById;
    AutoPtr<ISet> mPendingConnections;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_REMOTECONNECTIONSERVICE_H__