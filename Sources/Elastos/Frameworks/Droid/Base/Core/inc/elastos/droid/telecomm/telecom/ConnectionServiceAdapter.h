#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICEADAPTER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICEADAPTER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecomm::Internal::IIConnectionServiceAdapter;
using Elastos::Droid::Telecomm::Internal::IRemoteServiceCallback;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Provides methods for IConnectionService implementations to interact with the system phone app.
 *
 * @hide
 */
class ConnectionServiceAdapter
    : public Object
    , public IConnectionServiceAdapter
    , public IProxyDeathRecipient
{
public:
    CAR_INTERFACE_DECL()

    ConnectionServiceAdapter();

    CARAPI AddAdapter(
        /* [in] */ IIConnectionServiceAdapter* adapter);

    CARAPI RemoveAdapter(
        /* [in] */ IIConnectionServiceAdapter* adapter);

    /** ${inheritDoc} */
    CARAPI ProxyDied();

    CARAPI HandleCreateConnectionComplete(
        /* [in] */ const String& id,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ IParcelableConnection* connection);

    /**
     * Sets a call's state to active (e.g., an ongoing call where two parties can actively
     * communicate).
     *
     * @param callId The unique ID of the call whose state is changing to active.
     */
    CARAPI SetActive(
        /* [in] */ const String& callId);

    /**
     * Sets a call's state to ringing (e.g., an inbound ringing call).
     *
     * @param callId The unique ID of the call whose state is changing to ringing.
     */
    CARAPI SetRinging(
        /* [in] */ const String& callId);

    /**
     * Sets a call's state to dialing (e.g., dialing an outbound call).
     *
     * @param callId The unique ID of the call whose state is changing to dialing.
     */
    CARAPI SetDialing(
        /* [in] */ const String& callId);

    /**
     * Sets a call's state to disconnected.
     *
     * @param callId The unique ID of the call whose state is changing to disconnected.
     * @param disconnectCause The reason for the disconnection, as described by
     *            {@link android.telecomm.DisconnectCause}.
     */
    CARAPI SetDisconnected(
        /* [in] */ const String& callId,
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * Sets a call's state to be on hold.
     *
     * @param callId - The unique ID of the call whose state is changing to be on hold.
     */
    CARAPI SetOnHold(
        /* [in] */ const String& callId);

    /**
     * Asks Telecom to start or stop a ringback tone for a call.
     *
     * @param callId The unique ID of the call whose ringback is being changed.
     * @param ringback Whether Telecom should start playing a ringback tone.
     */
    CARAPI SetRingbackRequested(
        /* [in] */ const String& callId,
        /* [in] */ Boolean ringback);

    CARAPI SetCallCapabilities(
        /* [in] */ const String& callId,
        /* [in] */ Int32 capabilities);

    /**
     * Indicates whether or not the specified call is currently conferenced into the specified
     * conference call.
     *
     * @param callId The unique ID of the call being conferenced.
     * @param conferenceCallId The unique ID of the conference call. Null if call is not
     *            conferenced.
     */
    CARAPI SetIsConferenced(
        /* [in] */ const String& callId,
        /* [in] */ const String& conferenceCallId);

    /**
     * Indicates that the call no longer exists. Can be used with either a call or a conference
     * call.
     *
     * @param callId The unique ID of the call.
     */
    CARAPI RemoveCall(
        /* [in] */ const String& callId);

    CARAPI OnPostDialWait(
        /* [in] */ const String& callId,
        /* [in] */ const String& remaining);

    /**
     * Indicates that a new conference call has been created.
     *
     * @param callId The unique ID of the conference call.
     */
    CARAPI AddConferenceCall(
        /* [in] */ const String& callId,
        /* [in] */ IParcelableConference* parcelableConference);

    /**
     * Retrieves a list of remote connection services usable to place calls.
     */
    CARAPI QueryRemoteConnectionServices(
        /* [in] */ IRemoteServiceCallback* clb);

    /**
     * Sets the call video provider for a call.
     *
     * @param callId The unique ID of the call to set with the given call video provider.
     * @param videoProvider The call video provider instance to set on the call.
     */
    CARAPI SetVideoProvider(
        /* [in] */ const String& callId,
        /* [in] */ IConnectionVideoProvider* videoProvider);

    /**
     * Requests that the framework use VOIP audio mode for this connection.
     *
     * @param callId The unique ID of the call to set with the given call video provider.
     * @param isVoip True if the audio mode is VOIP.
     */
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

    /**
     * Sets the video state associated with a call.
     *
     * Valid values: {@link VideoProfile.VideoState#AUDIO_ONLY},
     * {@link VideoProfile.VideoState#BIDIRECTIONAL},
     * {@link VideoProfile.VideoState#TX_ENABLED},
     * {@link VideoProfile.VideoState#RX_ENABLED}.
     *
     * @param callId The unique ID of the call to set the video state for.
     * @param videoState The video state.
     */
    CARAPI SetVideoState(
        /* [in] */ const String& callId,
        /* [in] */ Int32 videoState);

    CARAPI SetConferenceableConnections(
        /* [in] */ const String& callId,
        /* [in] */ IList* conferenceableCallIds);

private:
    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mAdapters;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICEADAPTER_H__