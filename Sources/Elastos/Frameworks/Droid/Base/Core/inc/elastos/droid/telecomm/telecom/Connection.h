#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTION_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTION_H__

#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::Telecomm::Internal::IIVideoCallback;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Represents a connection to a remote endpoint that carries voice traffic.
 * <p>
 * Implementations create a custom subclass of {@code Connection} and return it to the framework
 * as the return value of
 * {@link ConnectionService#onCreateIncomingConnection(PhoneAccountHandle, ConnectionRequest)}
 * or
 * {@link ConnectionService#onCreateOutgoingConnection(PhoneAccountHandle, ConnectionRequest)}.
 * Implementations are then responsible for updating the state of the {@code Connection}, and
 * must call {@link #destroy()} to signal to the framework that the {@code Connection} is no
 * longer used and associated resources may be recovered.
 * @hide
 */
class Connection
    : public Object
    , public IConnection
{
public:
    class Listener
        : public Object
        , public IConnectionListener
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnStateChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 state);

        CARAPI OnAddressChanged(
            /* [in] */ IConnection* c,
            /* [in] */ IUri* newAddress,
            /* [in] */ Int32 presentation);

        CARAPI OnCallerDisplayNameChanged(
            /* [in] */ IConnection* c,
            /* [in] */ const String& callerDisplayName,
            /* [in] */ Int32 presentation);

        CARAPI OnVideoStateChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 videoState);

        CARAPI OnDisconnected(
            /* [in] */ IConnection* c,
            /* [in] */ IDisconnectCause* disconnectCause);

        CARAPI OnPostDialWait(
            /* [in] */ IConnection* c,
            /* [in] */ const String& remaining);

        CARAPI OnRingbackRequested(
            /* [in] */ IConnection* c,
            /* [in] */ Boolean ringback);

        CARAPI OnDestroyed(
            /* [in] */ IConnection* c);

        CARAPI OnCallCapabilitiesChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 callCapabilities);

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
            /* [in] */ IConnection* c,
            /* [in] */ IList* conferenceableConnections);

        CARAPI OnConferenceChanged(
            /* [in] */ IConnection* c,
            /* [in] */ IConference* conference);
    };

    /** @hide */
    class VideoProvider
        : public Object
        , public IConnectionVideoProvider
    {
    private:
        /**
         * Default handler used to consolidate binder method calls onto a single thread.
         */
        class VideoProviderHandler
            : public Handler
        {
        public:
            VideoProviderHandler(
                /* [in] */ VideoProvider* host);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        public:
            VideoProvider* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        VideoProvider();

        /**
         * Returns binder object which can be used across IPC methods.
         * @hide
         */
        CARAPI GetInterface(
            /* [out] */ IIVideoProvider** result);

        /**
         * Invokes callback method defined in In-Call UI.
         *
         * @param videoProfile The requested video call profile.
         */
        CARAPI ReceiveSessionModifyRequest(
            /* [in] */ IVideoProfile* videoProfile);

        /**
         * Invokes callback method defined in In-Call UI.
         *
         * @param status Status of the session modify request.  Valid values are
         *               {@link VideoProvider#SESSION_MODIFY_REQUEST_SUCCESS},
         *               {@link VideoProvider#SESSION_MODIFY_REQUEST_FAIL},
         *               {@link VideoProvider#SESSION_MODIFY_REQUEST_INVALID}
         * @param requestedProfile The original request which was sent to the remote device.
         * @param responseProfile The actual profile changes made by the remote device.
         */
        CARAPI ReceiveSessionModifyResponse(
            /* [in] */ Int32 status,
            /* [in] */ IVideoProfile* requestedProfile,
            /* [in] */ IVideoProfile* responseProfile);

        /**
         * Invokes callback method defined in In-Call UI.
         *
         * Valid values are: {@link VideoProvider#SESSION_EVENT_RX_PAUSE},
         * {@link VideoProvider#SESSION_EVENT_RX_RESUME},
         * {@link VideoProvider#SESSION_EVENT_TX_START},
         * {@link VideoProvider#SESSION_EVENT_TX_STOP}
         *
         * @param event The event.
         */
        CARAPI HandleCallSessionEvent(
            /* [in] */ Int32 event);

        /**
         * Invokes callback method defined in In-Call UI.
         *
         * @param width  The updated peer video width.
         * @param height The updated peer video height.
         */
        CARAPI ChangePeerDimensions(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Invokes callback method defined in In-Call UI.
         *
         * @param dataUsage The updated data usage.
         */
        CARAPI ChangeCallDataUsage(
            /* [in] */ Int32 dataUsage);

        /**
         * Invokes callback method defined in In-Call UI.
         *
         * @param cameraCapabilities The changed camera capabilities.
         */
        CARAPI ChangeCameraCapabilities(
            /* [in] */ ICameraCapabilities* cameraCapabilities);

    private:
        AutoPtr<VideoProviderHandler> mMessageHandler;
        // AutoPtr<IVideoProviderBinder> mBinder;
        AutoPtr<IIVideoCallback> mVideoCallback;
    };

private:
    class MyListener
        : public Listener
    {
    public:
        MyListener(
            /* [in] */ Connection* host);

        CARAPI OnDestroyed(
            /* [in] */ IConnection* c);

    public:
        Connection* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Create a new Connection.
     */
    Connection();

    /**
     * @return The address (e.g., phone number) to which this Connection is currently communicating.
     */
    CARAPI GetAddress(
        /* [out] */ IUri** result);

    /**
     * @return The presentation requirements for the address.
     *         See {@link TelecomManager} for valid values.
     */
    CARAPI GetAddressPresentation(
        /* [out] */ Int32* presentation);

    /**
     * @return The caller display name (CNAP).
     */
    CARAPI GetCallerDisplayName(
        /* [out] */ String* disName);

    /**
     * @return The presentation requirements for the handle.
     *         See {@link TelecomManager} for valid values.
     */
    CARAPI GetCallerDisplayNamePresentation(
        /* [out] */ Int32* presentation);

    /**
     * @return The state of this Connection.
     */
    CARAPI GetState(
        /* [out] */ Int32* state);

    /**
     * Returns the video state of the call.
     * Valid values: {@link VideoProfile.VideoState#AUDIO_ONLY},
     * {@link VideoProfile.VideoState#BIDIRECTIONAL},
     * {@link VideoProfile.VideoState#TX_ENABLED},
     * {@link VideoProfile.VideoState#RX_ENABLED}.
     *
     * @return The video state of the call.
     * @hide
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* state);

    /**
     * @return The audio state of the call, describing how its audio is currently
     *         being routed by the system. This is {@code null} if this Connection
     *         does not directly know about its audio state.
     */
    CARAPI GetAudioState(
        /* [out] */ IAudioState** result);

    /**
     * @return The conference that this connection is a part of.  Null if it is not part of any
     *         conference.
     */
    CARAPI GetConference(
        /* [out] */ IConference** result);

    /**
     * Returns whether this connection is requesting that the system play a ringback tone
     * on its behalf.
     */
    CARAPI IsRingbackRequested(
        /* [out] */ Boolean* result);

    /**
     * @return True if the connection's audio mode is VOIP.
     */
    CARAPI GetAudioModeIsVoip(
        /* [out] */ Boolean* result);

    /**
     * @return The status hints for this connection.
     */
    CARAPI GetStatusHints(
        /* [out] */ IStatusHints** result);

    /**
     * Assign a listener to be notified of state changes.
     *
     * @param l A listener.
     * @return This Connection.
     *
     * @hide
     */
    CARAPI AddConnectionListener(
        /* [in] */ IConnectionListener* l);

    /**
     * Remove a previously assigned listener that was being notified of state changes.
     *
     * @param l A Listener.
     * @return This Connection.
     *
     * @hide
     */
    CARAPI RemoveConnectionListener(
        /* [in] */ IConnectionListener* l);

    /**
     * @return The {@link DisconnectCause} for this connection.
     */
    CARAPI GetDisconnectCause(
        /* [out] */ IDisconnectCause** result);

    /**
     * Inform this Connection that the state of its audio output has been changed externally.
     *
     * @param state The new audio state.
     * @hide
     */
    CARAPI SetAudioState(
        /* [in] */ IAudioState* state);

    /**
     * @param state An integer value of a {@code STATE_*} constant.
     * @return A string representation of the value.
     */
    static CARAPI StateToString(
        /* [in] */ Int32 state,
        /* [out] */ String* result);

    /**
     * Returns the connection's {@link PhoneCapabilities}
     */
    CARAPI GetCallCapabilities(
        /* [out] */ Int32* result);

    /**
     * Sets the value of the {@link #getAddress()} property.
     *
     * @param address The new address.
     * @param presentation The presentation requirements for the address.
     *        See {@link TelecomManager} for valid values.
     */
    CARAPI SetAddress(
        /* [in] */ IUri* address,
        /* [in] */ Int32 presentation);

    /**
     * Sets the caller display name (CNAP).
     *
     * @param callerDisplayName The new display name.
     * @param presentation The presentation requirements for the handle.
     *        See {@link TelecomManager} for valid values.
     */
    CARAPI SetCallerDisplayName(
        /* [in] */ const String& callerDisplayName,
        /* [in] */ Int32 presentation);

    /**
     * Set the video state for the connection.
     * Valid values: {@link VideoProfile.VideoState#AUDIO_ONLY},
     * {@link VideoProfile.VideoState#BIDIRECTIONAL},
     * {@link VideoProfile.VideoState#TX_ENABLED},
     * {@link VideoProfile.VideoState#RX_ENABLED}.
     *
     * @param videoState The new video state.
     * @hide
     */
    CARAPI SetVideoState(
        /* [in] */ Int32 videoState);

    /**
     * Sets state to active (e.g., an ongoing call where two or more parties can actively
     * communicate).
     */
    CARAPI SetActive();

    /**
     * Sets state to ringing (e.g., an inbound ringing call).
     */
    CARAPI SetRinging();

    /**
     * Sets state to initializing (this Connection is not yet ready to be used).
     */
    CARAPI SetInitializing();

    /**
     * Sets state to initialized (the Connection has been set up and is now ready to be used).
     */
    CARAPI SetInitialized();

    /**
     * Sets state to dialing (e.g., dialing an outbound call).
     */
    CARAPI SetDialing();

    /**
     * Sets state to be on hold.
     */
    CARAPI SetOnHold();

    /**
     * Sets the video call provider.
     * @param videoProvider The video provider.
     * @hide
     */
    CARAPI SetVideoProvider(
        /* [in] */ IConnectionVideoProvider* videoProvider);

    /** @hide */
    CARAPI GetVideoProvider(
        /* [out] */ IConnectionVideoProvider** result);

    /**
     * Sets state to disconnected.
     *
     * @param disconnectCause The reason for the disconnection, as specified by
     *         {@link DisconnectCause}.
     */
    CARAPI SetDisconnected(
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * TODO: Needs documentation.
     */
    CARAPI SetPostDialWait(
        /* [in] */ const String& remaining);

    /**
     * Requests that the framework play a ringback tone. This is to be invoked by implementations
     * that do not play a ringback tone themselves in the call's audio stream.
     *
     * @param ringback Whether the ringback tone is to be played.
     */
    CARAPI SetRingbackRequested(
        /* [in] */ Boolean ringback);

    /**
     * Sets the connection's {@link PhoneCapabilities}.
     *
     * @param callCapabilities The new call capabilities.
     */
    CARAPI SetCallCapabilities(
        /* [in] */ Int32 callCapabilities);

    /**
     * Tears down the Connection object.
     */
    CARAPI Destroy();

    /**
     * Requests that the framework use VOIP audio mode for this connection.
     *
     * @param isVoip True if the audio mode is VOIP.
     */
    CARAPI SetAudioModeIsVoip(
        /* [in] */ Boolean isVoip);

    /**
     * Sets the label and icon status to display in the in-call UI.
     *
     * @param statusHints The status label and icon to set.
     */
    CARAPI SetStatusHints(
        /* [in] */ IStatusHints* statusHints);

    /**
     * Sets the connections with which this connection can be conferenced.
     *
     * @param conferenceableConnections The set of connections this connection can conference with.
     */
    CARAPI SetConferenceableConnections(
        /* [in] */ IList* conferenceableConnections);

    /**
     * Returns the connections with which this connection can be conferenced.
     */
    CARAPI GetConferenceableConnections(
        /* [out] */ IList** result);

    /**
     * @hide
     */
    CARAPI SetConnectionService(
        /* [in] */ IConnectionService* connectionService);

    /**
     * @hide
     */
    CARAPI UnsetConnectionService(
        /* [in] */ IConnectionService* connectionService);

    /**
     * @hide
     */
    CARAPI GetConnectionService(
        /* [out] */ IConnectionService** result);

    /**
     * Sets the conference that this connection is a part of. This will fail if the connection is
     * already part of a conference call. {@link #resetConference} to un-set the conference first.
     *
     * @param conference The conference.
     * @return {@code true} if the conference was successfully set.
     * @hide
     */
    CARAPI SetConference(
        /* [in] */ IConference* conference,
        /* [out] */ Boolean* result);

    /**
     * Resets the conference that this connection is a part of.
     * @hide
     */
    CARAPI ResetConference();

    /**
     * Notifies this Connection that the {@link #getAudioState()} property has a new value.
     *
     * @param state The new call audio state.
     */
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* state);

    /**
     * Notifies this Connection of an internal state change. This method is called after the
     * state is changed.
     *
     * @param state The new state, one of the {@code STATE_*} constants.
     */
    CARAPI OnStateChanged(
        /* [in] */ Int32 state);

    /**
     * Notifies this Connection of a request to play a DTMF tone.
     *
     * @param c A DTMF character.
     */
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    /**
     * Notifies this Connection of a request to stop any currently playing DTMF tones.
     */
    CARAPI OnStopDtmfTone();

    /**
     * Notifies this Connection of a request to disconnect.
     */
    CARAPI OnDisconnect();

    /**
     * Notifies this Connection of a request to separate from its parent conference.
     */
    CARAPI OnSeparate();

    /**
     * Notifies this Connection of a request to abort.
     */
    CARAPI OnAbort();

    /**
     * Notifies this Connection of a request to hold.
     */
    CARAPI OnHold();

    /**
     * Notifies this Connection of a request to exit a hold state.
     */
    CARAPI OnUnhold();

    /**
     * Notifies this Connection, which is in {@link #STATE_RINGING}, of
     * a request to accept.
     *
     * @param videoState The video state in which to answer the call.
     * @hide
     */
    CARAPI OnAnswer(
        /* [in] */ Int32 videoState);

    /**
     * Notifies this Connection, which is in {@link #STATE_RINGING}, of
     * a request to accept.
     */
    CARAPI OnAnswer();

    /**
     * Notifies this Connection, which is in {@link #STATE_RINGING}, of
     * a request to reject.
     */
    CARAPI OnReject();

    /**
     * Notifies this Connection whether the user wishes to proceed with the post-dial DTMF codes.
     */
    CARAPI OnPostDialContinue(
        /* [in] */ Boolean proceed);

    /**
     * Merge this connection and the specified connection into a conference call.  Once the
     * connections are merged, the calls should be added to the an existing or new
     * {@code Conference} instance. For new {@code Conference} instances, use
     * {@code ConnectionService#addConference}.
     *
     * @param otherConnection The connection with which this connection should be conferenced.
     */
    CARAPI OnConferenceWith(
        /* [in] */ IConnection* otherConnection);

    static CARAPI ToLogSafePhoneNumber(
        /* [in] */ const String& number,
        /* [out] */ String* result);

    /**
     * Return a {@code Connection} which represents a failed connection attempt. The returned
     * {@code Connection} will have a {@link android.telecom.DisconnectCause} and as specified,
     * and a {@link #getState()} of {@link #STATE_DISCONNECTED}.
     * <p>
     * The returned {@code Connection} can be assumed to {@link #destroy()} itself when appropriate,
     * so users of this method need not maintain a reference to its return value to destroy it.
     *
     * @param disconnectCause The disconnect cause, ({@see android.telecomm.DisconnectCause}).
     * @return A {@code Connection} which indicates failure.
     */
    static CARAPI CreateFailedConnection(
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [out] */ IConnection** result);

    /**
     * Return a {@code Connection} which represents a canceled connection attempt. The returned
     * {@code Connection} will have state {@link #STATE_DISCONNECTED}, and cannot be moved out of
     * that state. This connection should not be used for anything, and no other
     * {@code Connection}s should be attempted.
     * <p>
     * The returned {@code Connection} can be assumed to {@link #destroy()} itself when appropriate,
     * so users of this method need not maintain a reference to its return value to destroy it.
     *
     * @return A {@code Connection} which indicates that the underlying call should be canceled.
     */
    static CARAPI CreateCanceledConnection(
        /* [out] */ IConnection** result);

private:
    CARAPI_(void) FireOnConferenceableConnectionsChanged();

    CARAPI_(void) FireConferenceChanged();

    CARAPI_(void) ClearConferenceableList();

    CARAPI_(void) SetState(
        /* [in] */ Int32 state);

private:
    // Flag controlling whether PII is emitted into the logs
    static Boolean PII_DEBUG;

    AutoPtr<IConnectionListener> mConnectionDeathListener;

    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mListeners;
    AutoPtr<IList> mConferenceableConnections;
    AutoPtr<IList> mUnmodifiableConferenceableConnections;

    Int32 mState;
    AutoPtr<IAudioState> mAudioState;
    AutoPtr<IUri> mAddress;
    Int32 mAddressPresentation;
    String mCallerDisplayName;
    Int32 mCallerDisplayNamePresentation;
    Boolean mRingbackRequested;
    Int32 mCallCapabilities;
    AutoPtr<IConnectionVideoProvider> mVideoProvider;
    Boolean mAudioModeIsVoip;
    AutoPtr<IStatusHints> mStatusHints;
    Int32 mVideoState;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    AutoPtr<IConference> mConference;
    AutoPtr<IConnectionService> mConnectionService;
};

class FailureSignalingConnection
    : public Connection
{
public:
    FailureSignalingConnection(
        /* [in] */ IDisconnectCause* disconnectCause);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CONNECTION_H__