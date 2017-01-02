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

#ifndef __ELASTOS_DROID_TELECOM_REMOTECONNECTION_H__
#define __ELASTOS_DROID_TELECOM_REMOTECONNECTION_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Internal::Telecom::IIConnectionService;
using Elastos::Droid::Internal::Telecom::IIVideoCallback;
using Elastos::Droid::Internal::Telecom::IIVideoProvider;
using Elastos::Droid::Telecom::IVideoCallbackServant;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * A connection provided to a {@link ConnectionService} by another {@code ConnectionService}
 * running in a different process.
 *
 * @see ConnectionService#createRemoteOutgoingConnection(PhoneAccountHandle, ConnectionRequest)
 * @see ConnectionService#createRemoteIncomingConnection(PhoneAccountHandle, ConnectionRequest)
 * @hide
 */
class RemoteConnection
    : public Object
    , public IRemoteConnection
{
public:
    /** {@hide} */
    class VideoProvider
        : public Object
        , public IRemoteConnectionVideoProvider
    {
    private:
        class VideoCallback
            : public Object
        {
        public:
            VideoCallback(
                /* [in] */ VideoProvider* host);

            CARAPI ReceiveSessionModifyRequest(
                /* [in] */ IVideoProfile* videoProfile);

            CARAPI ReceiveSessionModifyResponse(
                /* [in] */ Int32 status,
                /* [in] */ IVideoProfile* requestedProfile,
                /* [in] */ IVideoProfile* responseProfile);

            CARAPI HandleCallSessionEvent(
                /* [in] */ Int32 event);

            CARAPI ChangePeerDimensions(
                /* [in] */ Int32 width,
                /* [in] */ Int32 height);

            CARAPI ChangeCallDataUsage(
                /* [in] */ Int32 dataUsage);

            CARAPI ChangeCameraCapabilities(
                /* [in] */ ICameraCapabilities* cameraCapabilities);

            CARAPI AsBinder(
                /* [out] */ IBinder** result);

        public:
            VideoProvider* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        VideoProvider();

        VideoProvider(
            /* [in] */ IIVideoProvider* videoProviderBinder);

        CARAPI AddListener(
            /* [in] */ IRemoteConnectionListener* l);

        CARAPI RemoveListener(
            /* [in] */ IRemoteConnectionListener* l);

        CARAPI SetCamera(
            /* [in] */ const String& cameraId);

        CARAPI SetPreviewSurface(
            /* [in] */ ISurface* surface);

        CARAPI SetDisplaySurface(
            /* [in] */ ISurface* surface);

        CARAPI SetDeviceOrientation(
            /* [in] */ Int32 rotation);

        CARAPI SetZoom(
            /* [in] */ Float value);

        CARAPI SendSessionModifyRequest(
            /* [in] */ IVideoProfile* reqProfile);

        CARAPI SendSessionModifyResponse(
            /* [in] */ IVideoProfile* responseProfile);

        CARAPI RequestCameraCapabilities();

        CARAPI RequestCallDataUsage();

        CARAPI SetPauseImage(
            /* [in] */ const String& uri);

    private:
        AutoPtr<IIVideoCallback> mVideoCallbackDelegate;

        AutoPtr<IVideoCallbackServant> mVideoCallbackServant;

        AutoPtr<IIVideoProvider> mVideoProviderBinder;

        /**
         * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
         * load factor before resizing, 1 means we only expect a single thread to
         * access the map so make only a single shard
         */
        AutoPtr<ISet> mListeners;
    };

public:
    CAR_INTERFACE_DECL()

    RemoteConnection();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& id,
        /* [in] */ IIConnectionService* connectionService,
        /* [in] */ IConnectionRequest* request);

    /**
     * Create a RemoteConnection which is used for failed connections. Note that using it for any
     * "real" purpose will almost certainly fail. Callers should note the failure and act
     * accordingly (moving on to another RemoteConnection, for example)
     *
     * @param disconnectCause The reason for the failed connection.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * Adds a callback to this {@code RemoteConnection}.
     *
     * @param callback A {@code Callback}.
     */
    CARAPI RegisterCallback(
        /* [in] */ IRemoteConnectionCallback* callback);

    /**
     * Removes a callback from this {@code RemoteConnection}.
     *
     * @param callback A {@code Callback}.
     */
    CARAPI UnregisterCallback(
        /* [in] */ IRemoteConnectionCallback* callback);

    /**
     * Obtains the state of this {@code RemoteConnection}.
     *
     * @return A state value, chosen from the {@code STATE_*} constants.
     */
    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
     * @return For a {@link Connection#STATE_DISCONNECTED} {@code RemoteConnection}, the
     * disconnect cause expressed as a code chosen from among those declared in
     * {@link DisconnectCause}.
     */
    CARAPI GetDisconnectCause(
        /* [out] */ IDisconnectCause** result);

    /**
     * @return A bitmask of the capabilities of the {@code RemoteConnection}, as defined in
     *         {@link PhoneCapabilities}.
     */
    CARAPI GetCallCapabilities(
        /* [out] */ Int32* result);

    /**
     * @return {@code true} if the {@code RemoteConnection}'s current audio mode is VOIP.
     */
    CARAPI IsVoipAudioMode(
        /* [out] */ Boolean* result);

    /**
     * @return The current {@link StatusHints} of this {@code RemoteConnection},
     * or {@code null} if none have been set.
     */
    CARAPI GetStatusHints(
        /* [out] */ IStatusHints** result);

    /**
     * @return The address (e.g., phone number) to which the {@code RemoteConnection} is currently
     * connected.
     */
    CARAPI GetAddress(
        /* [out] */ IUri** result);

    /**
     * @return The presentation requirements for the address. See {@link TelecomManager} for valid
     * values.
     */
    CARAPI GetAddressPresentation(
        /* [out] */ Int32* result);

    /**
     * @return The display name for the caller.
     */
    CARAPI GetCallerDisplayName(
        /* [out] */ ICharSequence** result);

    /**
     * @return The presentation requirements for the caller display name. See
     * {@link TelecomManager} for valid values.
     */
    CARAPI GetCallerDisplayNamePresentation(
        /* [out] */ Int32* result);

    /**
     * @return The video state of the {@code RemoteConnection}. See
     * {@link VideoProfile.VideoState}.
     * @hide
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    /**
     * @return The video provider associated with this {@code RemoteConnection}.
     * @hide
     */
    CARAPI GetVideoProvider(
        /* [out] */ IRemoteConnectionVideoProvider** result);

    /**
     * @return Whether the {@code RemoteConnection} is requesting that the framework play a
     * ringback tone on its behalf.
     */
    CARAPI IsRingbackRequested(
        /* [out] */ Boolean* result);

    /**
     * Instructs this {@code RemoteConnection} to abort.
     */
    CARAPI Abort();

    /**
     * Instructs this {@link Connection#STATE_RINGING} {@code RemoteConnection} to answer.
     */
    CARAPI Answer();

    /**
     * Instructs this {@link Connection#STATE_RINGING} {@code RemoteConnection} to answer.
     * @param videoState The video state in which to answer the call.
     * @hide
     */
    CARAPI Answer(
        /* [in] */ Int32 videoState);

    /**
     * Instructs this {@link Connection#STATE_RINGING} {@code RemoteConnection} to reject.
     */
    CARAPI Reject();

    /**
     * Instructs this {@code RemoteConnection} to go on hold.
     */
    CARAPI Hold();

    /**
     * Instructs this {@link Connection#STATE_HOLDING} call to release from hold.
     */
    CARAPI Unhold();

    /**
     * Instructs this {@code RemoteConnection} to disconnect.
     */
    CARAPI Disconnect();

    /**
     * Instructs this {@code RemoteConnection} to play a dual-tone multi-frequency signaling
     * (DTMF) tone.
     *
     * Any other currently playing DTMF tone in the specified call is immediately stopped.
     *
     * @param digit A character representing the DTMF digit for which to play the tone. This
     *         value must be one of {@code '0'} through {@code '9'}, {@code '*'} or {@code '#'}.
     */
    CARAPI PlayDtmfTone(
        /* [in] */ Char32 digit);

    /**
     * Instructs this {@code RemoteConnection} to stop any dual-tone multi-frequency signaling
     * (DTMF) tone currently playing.
     *
     * DTMF tones are played by calling {@link #playDtmfTone(char)}. If no DTMF tone is
     * currently playing, this method will do nothing.
     */
    CARAPI StopDtmfTone();

    /**
     * Instructs this {@code RemoteConnection} to continue playing a post-dial DTMF string.
     *
     * A post-dial DTMF string is a string of digits following the first instance of either
     * {@link TelecomManager#DTMF_CHARACTER_WAIT} or {@link TelecomManager#DTMF_CHARACTER_PAUSE}.
     * These digits are immediately sent as DTMF tones to the recipient as soon as the
     * connection is made.
     *
     * If the DTMF string contains a {@link TelecomManager#DTMF_CHARACTER_PAUSE} symbol, this
     * {@code RemoteConnection} will temporarily pause playing the tones for a pre-defined period
     * of time.
     *
     * If the DTMF string contains a {@link TelecomManager#DTMF_CHARACTER_WAIT} symbol, this
     * {@code RemoteConnection} will pause playing the tones and notify callbackss via
     * {@link Callback#onPostDialWait(RemoteConnection, String)}. At this point, the in-call app
     * should display to the user an indication of this state and an affordance to continue
     * the postdial sequence. When the user decides to continue the postdial sequence, the in-call
     * app should invoke the {@link #postDialContinue(boolean)} method.
     *
     * @param proceed Whether or not to continue with the post-dial sequence.
     */
    CARAPI PostDialContinue(
        /* [in] */ Boolean proceed);

    /**
     * Set the audio state of this {@code RemoteConnection}.
     *
     * @param state The audio state of this {@code RemoteConnection}.
     */
    CARAPI SetAudioState(
        /* [in] */ IAudioState* state);

    /**
     * Obtain the {@code RemoteConnection}s with which this {@code RemoteConnection} may be
     * successfully asked to create a conference with.
     *
     * @return The {@code RemoteConnection}s with which this {@code RemoteConnection} may be
     *         merged into a {@link RemoteConference}.
     */
    CARAPI GetConferenceableConnections(
        /* [out] */ IList** result);

    /**
     * Obtain the {@code RemoteConference} that this {@code RemoteConnection} may be a part
     * of, or {@code null} if there is no such {@code RemoteConference}.
     *
     * @return A {@code RemoteConference} or {@code null};
     */
    CARAPI GetConference(
        /* [out] */ IRemoteConference** result);

    /** {@hide} */
    CARAPI GetId(
        /* [out] */ String* result);

    /** {@hide} */
    CARAPI GetConnectionService(
        /* [out] */ IIConnectionService** result);

    /**
     * @hide
     */
    CARAPI SetState(
        /* [in] */ Int32 state);

    /**
     * @hide
     */
    CARAPI SetDisconnected(
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * @hide
     */
    CARAPI SetRingbackRequested(
        /* [in] */ Boolean ringback);

    /**
     * @hide
     */
    CARAPI SetCallCapabilities(
        /* [in] */ Int32 callCapabilities);

    /**
     * @hide
     */
    CARAPI SetDestroyed();

    /**
     * @hide
     */
    CARAPI SetPostDialWait(
        /* [in] */ const String& remainingDigits);

    /**
     * @hide
     */
    CARAPI SetVideoState(
        /* [in] */ Int32 videoState);

    /**
     * @hide
     */
    CARAPI SetVideoProvider(
        /* [in] */ IRemoteConnectionVideoProvider* videoProvider);

    /** @hide */
    CARAPI SetIsVoipAudioMode(
        /* [in] */ Boolean isVoip);

    /** @hide */
    CARAPI SetStatusHints(
        /* [in] */ IStatusHints* statusHints);

    /** @hide */
    CARAPI SetAddress(
        /* [in] */ IUri* address,
        /* [in] */ Int32 presentation);

    /** @hide */
    CARAPI SetCallerDisplayName(
        /* [in] */ const String& callerDisplayName,
        /* [in] */ Int32 presentation);

    /** @hide */
    CARAPI SetConferenceableConnections(
        /* [in] */ IList* conferenceableConnections);

    /** @hide */
    CARAPI SetConference(
        /* [in] */ IRemoteConference* conference);

    /**
     * Create a RemoteConnection represents a failure, and which will be in
     * {@link Connection#STATE_DISCONNECTED}. Attempting to use it for anything will almost
     * certainly result in bad things happening. Do not do this.
     *
     * @return a failed {@link RemoteConnection}
     *
     * @hide
     */
    static CARAPI Failure(
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [out] */ IRemoteConnection** result);

private:
    AutoPtr<IIConnectionService> mConnectionService;
    String mConnectionId;
    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mCallbacks;
    AutoPtr<IList> mConferenceableConnections;
    AutoPtr<IList> mUnmodifiableconferenceableConnections;

    Int32 mState;
    AutoPtr<IDisconnectCause> mDisconnectCause;
    Boolean mRingbackRequested;
    Boolean mConnected;
    Int32 mCallCapabilities;
    Int32 mVideoState;
    AutoPtr<IRemoteConnectionVideoProvider> mVideoProvider;
    Boolean mIsVoipAudioMode;
    AutoPtr<IStatusHints> mStatusHints;
    AutoPtr<IUri> mAddress;
    Int32 mAddressPresentation;
    String mCallerDisplayName;
    Int32 mCallerDisplayNamePresentation;
    AutoPtr<IRemoteConference> mConference;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_REMOTECONNECTION_H__