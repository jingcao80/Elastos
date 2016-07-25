#ifndef __ELASTOS_DROID_TELECOM_CALL_H__
#define __ELASTOS_DROID_TELECOM_CALL_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Represents an ongoing phone call that the in-call app should present to the user.
 *
 * {@hide}
 */
class ECO_PUBLIC Call
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

    class Listener
        : public Object
        , public ICallListener
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * Invoked when the state of this {@code Call} has changed. See {@link #getState()}.
         *
         * @param call The {@code Call} invoking this method.
         * @param state The new state of the {@code Call}.
         */
        CARAPI OnStateChanged(
            /* [in] */ ICall* call,
            /* [in] */ Int32 state);

        /**
         * Invoked when the parent of this {@code Call} has changed. See {@link #getParent()}.
         *
         * @param call The {@code Call} invoking this method.
         * @param parent The new parent of the {@code Call}.
         */
        CARAPI OnParentChanged(
            /* [in] */ ICall* call,
            /* [in] */ ICall* parent);

        /**
         * Invoked when the children of this {@code Call} have changed. See {@link #getChildren()}.
         *
         * @param call The {@code Call} invoking this method.
         * @param children The new children of the {@code Call}.
         */
        CARAPI OnChildrenChanged(
            /* [in] */ ICall* call,
            /* [in] */ IList* children);

        /**
         * Invoked when the details of this {@code Call} have changed. See {@link #getDetails()}.
         *
         * @param call The {@code Call} invoking this method.
         * @param details A {@code Details} object describing the {@code Call}.
         */
        CARAPI OnDetailsChanged(
            /* [in] */ ICall* call,
            /* [in] */ ICallDetails* details);

        /**
         * Invoked when the text messages that can be used as responses to the incoming
         * {@code Call} are loaded from the relevant database.
         * See {@link #getCannedTextResponses()}.
         *
         * @param call The {@code Call} invoking this method.
         * @param cannedTextResponses The text messages useable as responses.
         */
        CARAPI OnCannedTextResponsesLoaded(
            /* [in] */ ICall* call,
            /* [in] */ IList* cannedTextResponses);

        /**
         * Invoked when the post-dial sequence in the outgoing {@code Call} has reached a pause
         * character. This causes the post-dial signals to stop pending user confirmation. An
         * implementation should present this choice to the user and invoke
         * {@link #postDialContinue(boolean)} when the user makes the choice.
         *
         * @param call The {@code Call} invoking this method.
         * @param remainingPostDialSequence The post-dial characters that remain to be sent.
         */
        CARAPI OnPostDialWait(
            /* [in] */ ICall* call,
            /* [in] */ const String& remainingPostDialSequence);

        /**
         * Invoked when the {@code Call.VideoCall} of the {@code Call} has changed.
         *
         * @param call The {@code Call} invoking this method.
         * @param videoCall The {@code Call.VideoCall} associated with the {@code Call}.
         * @hide
         */
        CARAPI OnVideoCallChanged(
            /* [in] */ ICall* call,
            /* [in] */ IInCallServiceVideoCall* videoCall);

        /**
         * Invoked when the {@code Call} is destroyed. Clients should refrain from cleaning
         * up their UI for the {@code Call} in response to state transitions. Specifically,
         * clients should not assume that a {@link #onStateChanged(Call, int)} with a state of
         * {@link #STATE_DISCONNECTED} is the final notification the {@code Call} will send. Rather,
         * clients should wait for this method to be invoked.
         *
         * @param call The {@code Call} being destroyed.
         */
        CARAPI OnCallDestroyed(
            /* [in] */ ICall* call);

        /**
         * Invoked upon changes to the set of {@code Call}s with which this {@code Call} can be
         * conferenced.
         *
         * @param call The {@code Call} being updated.
         * @param conferenceableCalls The {@code Call}s with which this {@code Call} can be
         *          conferenced.
         */
        CARAPI OnConferenceableCallsChanged(
            /* [in] */ ICall* call,
            /* [in] */ IList* conferenceableCalls);
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
    ECO_LOCAL CARAPI_(void) FireStateChanged(
        /* [in] */ Int32 newState);

    ECO_LOCAL CARAPI_(void) FireParentChanged(
        /* [in] */ ICall* newParent);

    ECO_LOCAL CARAPI_(void) FireChildrenChanged(
        /* [in] */ IList* children);

    ECO_LOCAL CARAPI_(void) FireDetailsChanged(
        /* [in] */ ICallDetails* details);

    ECO_LOCAL CARAPI_(void) FireCannedTextResponsesLoaded(
        /* [in] */ IList* cannedTextResponses);

    ECO_LOCAL CARAPI_(void) FireVideoCallChanged(
        /* [in] */ IInCallServiceVideoCall* videoCall);

    ECO_LOCAL CARAPI_(void) FirePostDialWait(
        /* [in] */ const String& remainingPostDialSequence);

    ECO_LOCAL CARAPI_(void) FireCallDestroyed();

    ECO_LOCAL CARAPI_(void) FireConferenceableCallsChanged();

    ECO_LOCAL CARAPI_(Int32) StateFromParcelableCallState(
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
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CALL_H__