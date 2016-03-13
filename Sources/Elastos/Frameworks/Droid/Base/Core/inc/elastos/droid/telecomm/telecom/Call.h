#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CALL_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CALL_H__

#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Represents an ongoing phone call that the in-call app should present to the user.
 *
 * {@hide}
 */
class Call
    : public Object
    , public ICall
{
public:
    class Details
        : public Object
        , public ICallDetails
    {
    public:
        CAR_INTERFACE_DECL()

        /** {@hide} */
        Details(
            /* [in] */ IUri* handle,
            /* [in] */ Int32 handlePresentation,
            /* [in] */ const String& callerDisplayName,
            /* [in] */ Int32 callerDisplayNamePresentation,
            /* [in] */ IPhoneAccountHandle* accountHandle,
            /* [in] */ Int32 capabilities,
            /* [in] */ Int32 properties,
            /* [in] */ IDisconnectCause* disconnectCause,
            /* [in] */ Int64 connectTimeMillis,
            /* [in] */ IGatewayInfo* gatewayInfo,
            /* [in] */ Int32 videoState,
            /* [in] */ IStatusHints* statusHints,
            /* [in] */ IBundle* extras);

        /**
         * @return The handle (e.g., phone number) to which the {@code Call} is currently
         * connected.
         */
        CARAPI GetHandle(
            /* [out] */ IUri** handle);

        /**
         * @return The presentation requirements for the handle. See
         * {@link TelecomManager} for valid values.
         */
        CARAPI GetHandlePresentation(
            /* [out] */ Int32* result);

        /**
         * @return The display name for the caller.
         */
        CARAPI GetCallerDisplayName(
            /* [out] */ String* disName);

        /**
         * @return The presentation requirements for the caller display name. See
         * {@link TelecomManager} for valid values.
         */
        CARAPI GetCallerDisplayNamePresentation(
            /* [out] */ Int32* result);

        /**
         * @return The {@code PhoneAccountHandle} whereby the {@code Call} is currently being
         * routed.
         */
        CARAPI GetAccountHandle(
            /* [out] */ IPhoneAccountHandle** result);

        /**
         * @return A bitmask of the capabilities of the {@code Call}, as defined in
         *         {@link PhoneCapabilities}.
         */
        CARAPI GetCallCapabilities(
            /* [out] */ Int32* result);

        /**
         * @return A bitmask of the properties of the {@code Call}, as defined in
         *         {@link CallProperties}.
         */
        CARAPI GetCallProperties(
            /* [out] */ Int32* result);

        /**
         * @return For a {@link #STATE_DISCONNECTED} {@code Call}, the disconnect cause expressed
         * by {@link android.telecom.DisconnectCause}.
         */
        CARAPI GetDisconnectCause(
            /* [out] */ IDisconnectCause** result);

        /**
         * @return The time the {@code Call} has been connected. This information is updated
         * periodically, but user interfaces should not rely on this to display any "call time
         * clock".
         */
        CARAPI GetConnectTimeMillis(
            /* [out] */ Int64* result);

        /**
         * @return Information about any calling gateway the {@code Call} may be using.
         */
        CARAPI GetGatewayInfo(
            /* [out] */ IGatewayInfo** result);

        /**
         * @return The video state of the {@code Call}.
         */
        CARAPI GetVideoState(
            /* [out] */ Int32* result);

        /**
         * @return The current {@link android.telecom.StatusHints}, or {@code null} if none
         * have been set.
         */
        CARAPI GetStatusHints(
            /* [out] */ IStatusHints** result);

        /**
         * @return A bundle extras to pass with the call
         */
        CARAPI GetExtras(
            /* [out] */ IBundle** result);

        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* result);

    private:
        AutoPtr<IUri> mHandle;
        Int32 mHandlePresentation;
        String mCallerDisplayName;
        Int32 mCallerDisplayNamePresentation;
        AutoPtr<IPhoneAccountHandle> mAccountHandle;
        Int32 mCallCapabilities;
        Int32 mCallProperties;
        AutoPtr<IDisconnectCause> mDisconnectCause;
        Int64 mConnectTimeMillis;
        AutoPtr<IGatewayInfo> mGatewayInfo;
        Int32 mVideoState;
        AutoPtr<IStatusHints> mStatusHints;
        AutoPtr<IBundle> mExtras;
    };

public:
    CAR_INTERFACE_DECL()

    Call();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IPhone* phone,
        /* [in] */ const String& telecomCallId,
        /* [in] */ IInCallAdapter* inCallAdapter);

    /**
     * Obtains the post-dial sequence remaining to be emitted by this {@code Call}, if any.
     *
     * @return The remaining post-dial sequence, or {@code null} if there is no post-dial sequence
     * remaining or this {@code Call} is not in a post-dial state.
     */
    CARAPI GetRemainingPostDialSequence(
        /* [out] */ String* result);

    /**
     * Instructs this {@link #STATE_RINGING} {@code Call} to answer.
     * @param videoState The video state in which to answer the call.
     */
    CARAPI Answer(
        /* [in] */ Int32 videoState);

    /**
     * Instructs this {@link #STATE_RINGING} {@code Call} to reject.
     *
     * @param rejectWithMessage Whether to reject with a text message.
     * @param textMessage An optional text message with which to respond.
     */
    CARAPI Reject(
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

    /**
     * Instructs this {@code Call} to disconnect.
     */
    CARAPI Disconnect();

    /**
     * Instructs this {@code Call} to go on hold.
     */
    CARAPI Hold();

    /**
     * Instructs this {@link #STATE_HOLDING} call to release from hold.
     */
    CARAPI Unhold();

    /**
     * Instructs this {@code Call} to play a dual-tone multi-frequency signaling (DTMF) tone.
     *
     * Any other currently playing DTMF tone in the specified call is immediately stopped.
     *
     * @param digit A character representing the DTMF digit for which to play the tone. This
     *         value must be one of {@code '0'} through {@code '9'}, {@code '*'} or {@code '#'}.
     */
    CARAPI PlayDtmfTone(
        /* [in] */ Char32 digit);

    /**
     * Instructs this {@code Call} to stop any dual-tone multi-frequency signaling (DTMF) tone
     * currently playing.
     *
     * DTMF tones are played by calling {@link #playDtmfTone(char)}. If no DTMF tone is
     * currently playing, this method will do nothing.
     */
    CARAPI StopDtmfTone();

    /**
     * Instructs this {@code Call} to continue playing a post-dial DTMF string.
     *
     * A post-dial DTMF string is a string of digits entered after a phone number, when dialed,
     * that are immediately sent as DTMF tones to the recipient as soon as the connection is made.
     *
     * If the DTMF string contains a {@link TelecomManager#DTMF_CHARACTER_PAUSE} symbol, this
     * {@code Call} will temporarily pause playing the tones for a pre-defined period of time.
     *
     * If the DTMF string contains a {@link TelecomManager#DTMF_CHARACTER_WAIT} symbol, this
     * {@code Call} will pause playing the tones and notify listeners via
     * {@link Listener#onPostDialWait(Call, String)}. At this point, the in-call app
     * should display to the user an indication of this state and an affordance to continue
     * the postdial sequence. When the user decides to continue the postdial sequence, the in-call
     * app should invoke the {@link #postDialContinue(boolean)} method.
     *
     * @param proceed Whether or not to continue with the post-dial sequence.
     */
    CARAPI PostDialContinue(
        /* [in] */ Boolean proceed);

    /**
     * Notifies this {@code Call} that an account has been selected and to proceed with placing
     * an outgoing call.
     */
    CARAPI PhoneAccountSelected(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * Instructs this {@code Call} to enter a conference.
     *
     * @param callToConferenceWith The other call with which to conference.
     */
    CARAPI Conference(
        /* [in] */ ICall* callToConferenceWith);

    /**
     * Instructs this {@code Call} to split from any conference call with which it may be
     * connected.
     */
    CARAPI SplitFromConference();

    /**
     * Merges the calls within this conference. See {@link PhoneCapabilities#MERGE_CONFERENCE}.
     */
    CARAPI MergeConference();

    /**
     * Swaps the calls within this conference. See {@link PhoneCapabilities#SWAP_CONFERENCE}.
     */
    CARAPI SwapConference();

    /**
     * Obtains the parent of this {@code Call} in a conference, if any.
     *
     * @return The parent {@code Call}, or {@code null} if this {@code Call} is not a
     * child of any conference {@code Call}s.
     */
    CARAPI GetParent(
        /* [out] */ ICall** result);

    /**
     * Obtains the children of this conference {@code Call}, if any.
     *
     * @return The children of this {@code Call} if this {@code Call} is a conference, or an empty
     * {@code List} otherwise.
     */
    CARAPI GetChildren(
        /* [out] */ IList** result);

    /**
     * Returns the list of {@code Call}s with which this {@code Call} is allowed to conference.
     *
     * @return The list of conferenceable {@code Call}s.
     */
    CARAPI GetConferenceableCalls(
        /* [out] */ IList** result);

    /**
     * Obtains the state of this {@code Call}.
     *
     * @return A state value, chosen from the {@code STATE_*} constants.
     */
    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
     * Obtains a list of canned, pre-configured message responses to present to the user as
     * ways of rejecting this {@code Call} using via a text message.
     *
     * @see #reject(boolean, String)
     *
     * @return A list of canned text message responses.
     */
    CARAPI GetCannedTextResponses(
        /* [out] */ IList** result);

    /**
     * Obtains an object that can be used to display video from this {@code Call}.
     *
     * @return An {@code Call.VideoCall}.
     * @hide
     */
    CARAPI GetVideoCall(
        /* [out] */ IInCallServiceVideoCall** result);

    /**
     * Obtains an object containing call details.
     *
     * @return A {@link Details} object. Depending on the state of the {@code Call}, the
     * result may be {@code null}.
     */
    CARAPI GetDetails(
        /* [out] */ ICallDetails** result);

    /**
     * Adds a listener to this {@code Call}.
     *
     * @param listener A {@code Listener}.
     */
    CARAPI AddListener(
        /* [in] */ ICallListener* listener);

    /**
     * Removes a listener from this {@code Call}.
     *
     * @param listener A {@code Listener}.
     */
    CARAPI RemoveListener(
        /* [in] */ ICallListener* listener);

    /** {@hide} */
    CARAPI InternalGetCallId(
        /* [out] */ String* result);

    /** {@hide} */
    CARAPI InternalUpdate(
        /* [in] */ IParcelableCall* parcelableCall,
        /* [in] */ IMap* callIdMap);

    /** {@hide} */
    CARAPI InternalSetPostDialWait(
        /* [in] */ const String& remaining);

    /** {@hide} */
    CARAPI InternalSetDisconnected();

private:
    CARAPI_(void) FireStateChanged(
        /* [in] */ Int32 newState);

    CARAPI_(void) FireParentChanged(
        /* [in] */ ICall* newParent);

    CARAPI_(void) FireChildrenChanged(
        /* [in] */ IList* children);

    CARAPI_(void) FireDetailsChanged(
        /* [in] */ ICallDetails* details);

    CARAPI_(void) FireCannedTextResponsesLoaded(
        /* [in] */ IList* cannedTextResponses);

    CARAPI_(void) FireVideoCallChanged(
        /* [in] */ IInCallServiceVideoCall* videoCall);

    CARAPI_(void) FirePostDialWait(
        /* [in] */ const String& remainingPostDialSequence);

    CARAPI_(void) FireCallDestroyed();

    CARAPI_(void) FireConferenceableCallsChanged();

    CARAPI_(Int32) StateFromParcelableCallState(
        /* [in] */ Int32 parcelableCallState);

private:
    AutoPtr<IPhone> mPhone;
    String mTelecomCallId;
    AutoPtr<IInCallAdapter> mInCallAdapter;
    AutoPtr<IList> mChildrenIds;
    AutoPtr<IList> mChildren;
    AutoPtr<IList> mUnmodifiableChildren;
    AutoPtr<IList> mListeners;
    AutoPtr<IList> mConferenceableCalls;
    AutoPtr<IList> mUnmodifiableConferenceableCalls;

    Boolean mChildrenCached;
    String mParentId;
    Int32 mState;
    AutoPtr<IList> mCannedTextResponses;
    String mRemainingPostDialSequence;
    AutoPtr<IInCallServiceVideoCall> mVideoCall;
    AutoPtr<ICallDetails> mDetails;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CALL_H__