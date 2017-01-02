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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLSMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLSMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/BlacklistCallNotifier.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallLogManager.h"
#include "elastos/droid/server/telecom/ConnectionServiceRepository.h"
#include "elastos/droid/server/telecom/DtmfLocalTonePlayer.h"
#include "elastos/droid/server/telecom/HeadsetMediaButton.h"
#include "elastos/droid/server/telecom/InCallController.h"
#include "elastos/droid/server/telecom/MissedCallNotifier.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "elastos/droid/server/telecom/PhoneStateBroadcaster.h"
#include "elastos/droid/server/telecom/ProximitySensorManager.h"
#include "elastos/droid/server/telecom/Ringer.h"
#include "elastos/droid/server/spam_filter/SpamBlocker.h"
#include "elastos/droid/server/telecom/TtyManager.h"
#include "elastos/droid/server/telecom/WiredHeadsetManager.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::IParcelableConference;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Singleton.
 *
 * NOTE: by design most APIs are package private, use the relevant adapter/s to allow
 * access from other packages specifically refraining from passing the CallsManager instance
 * beyond the com.android.server.telecom package boundary.
 */
class CallsManager
    : public Call::ListenerBase
{
private:
    class LchHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CallsManager::LchHandler")

        LchHandler(
            /* [in] */ CallsManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CallsManager* mHost;
    };

public:
    CallsManager();

    /** Singleton accessor. */
    static CARAPI_(AutoPtr<CallsManager>) GetInstance();

    /**
     * Sets the static singleton instance.
     *
     * @param instance The instance to set.
     */
    static CARAPI Initialize(
        /* [in] */ CallsManager* instance);

    /**
     * Initializes the required Telecom components.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ MissedCallNotifier* missedCallNotifier,
        /* [in] */ BlacklistCallNotifier* blacklistCallNotifier,
        /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar);

    // @Override
    CARAPI OnSuccessfulOutgoingCall(
        /* [in] */ ICall* call,
        /* [in] */ Int32 callState);

    // @Override
    CARAPI OnFailedOutgoingCall(
        /* [in] */ ICall* call,
        /* [in] */ IDisconnectCause* disconnectCause);

    // @Override
    CARAPI OnSuccessfulIncomingCall(
        /* [in] */ ICall* incomingCall);

    // @Override
    CARAPI OnFailedIncomingCall(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnSuccessfulUnknownCall(
        /* [in] */ ICall* call,
        /* [in] */ Int32 callState);

    // @Override
    CARAPI OnFailedUnknownCall(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnRingbackRequested(
        /* [in] */ ICall* call,
        /* [in] */ Boolean ringback);

    // @Override
    CARAPI OnPostDialWait(
        /* [in] */ ICall* call,
        /* [in] */ const String& remaining);

    // @Override
    CARAPI OnParentChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnChildrenChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnIsVoipAudioModeChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnVideoStateChanged(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallSubstateChanged(
        /* [in] */ ICall* call);

    CARAPI GetCalls(
        /* [out] */ IList** result); // ImmutableCollection

    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    CARAPI GetRinger(
        /* [out] */ Ringer** result);

    CARAPI GetInCallController(
        /* [out] */ InCallController** result);

    CARAPI HasEmergencyCall(
        /* [out] */ Boolean* result);

    CARAPI GetAudioState(
        /* [out] */ IAudioState** result);

    CARAPI IsTtySupported(
        /* [out] */ Boolean* result);

    CARAPI GetCurrentTtyMode(
        /* [out] */ Int32* result);

    CARAPI AddListener(
        /* [in] */ ICallsManagerListener* listener);

    CARAPI RemoveListener(
        /* [in] */ ICallsManagerListener* listener);

    /**
     * Starts the process to attach the call to a connection service.
     *
     * @param phoneAccountHandle The phone account which contains the component name of the
     *        connection service to use for this call.
     * @param extras The optional extras Bundle passed with the intent used for the incoming call.
     */
    CARAPI ProcessIncomingCallIntent(
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
        /* [in] */ IBundle* extras);

    CARAPI AddNewUnknownCall(
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
        /* [in] */ IBundle* extras);

    /**
     * Kicks off the first steps to creating an outgoing call so that InCallUI can launch.
     *
     * @param handle Handle to connect the call with.
     * @param phoneAccountHandle The phone account which contains the component name of the
     *        connection service to use for this call.
     * @param extras The optional extras Bundle passed with the intent used for the incoming call.
     */
    CARAPI StartOutgoingCall(
        /* [in] */ IUri* handle,
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
        /* [in] */ IBundle* extras,
        /* [out] */ ICall** result);

    /**
     * Attempts to issue/connect the specified call.
     *
     * @param handle Handle to connect the call with.
     * @param gatewayInfo Optional gateway information that can be used to route the call to the
     *        actual dialed handle via a gateway provider. May be null.
     * @param speakerphoneOn Whether or not to turn the speakerphone on once the call connects.
     * @param videoState The desired video state for the outgoing call.
     */
    CARAPI PlaceOutgoingCall(
        /* [in] */ ICall* call,
        /* [in] */ IUri* handle,
        /* [in] */ IGatewayInfo* gatewayInfo,
        /* [in] */ Boolean speakerphoneOn,
        /* [in] */ Int32 videoState);

    /**
     * Attempts to add participant in a call.
     *
     * @param handle Handle to connect the call with.
     * @param gatewayInfo Optional gateway information that can be used to route the call to the
     *        actual dialed handle via a gateway provider. May be null.
     * @param speakerphoneOn Whether or not to turn the speakerphone on once the call connects.
     * @param videoState The desired video state for the outgoing call.
     */
    CARAPI AddParticipant(
        /* [in] */ ICall* call,
        /* [in] */ IUri* handle,
        /* [in] */ IGatewayInfo* gatewayInfo,
        /* [in] */ Boolean speakerphoneOn,
        /* [in] */ Int32 videoState);

    /**
     * Attempts to start a conference call for the specified call.
     *
     * @param call The call to conference.
     * @param otherCall The other call to conference with.
     */
    CARAPI Conference(
        /* [in] */ ICall* call,
        /* [in] */ ICall* otherCall);

    /**
     * Instructs Telecom to answer the specified call. Intended to be invoked by the in-call
     * app through {@link InCallAdapter} after Telecom notifies it of an incoming call followed by
     * the user opting to answer said call.
     *
     * @param call The call to answer.
     * @param videoState The video state in which to answer the call.
     */
    CARAPI AnswerCall(
        /* [in] */ ICall* call,
        /* [in] */ Int32 videoState);

    /**
     * Instructs Telecomm to deflect the specified call. Intended to be invoked by the in-call
     * app through {@link InCallAdapter} after Telecomm notifies it of an incoming call followed by
     * the user opting to deflect said call.
     */
    CARAPI DeflectCall(
        /* [in] */ ICall* call,
        /* [in] */ const String& number);

    /**
     * Instructs Telecom to reject the specified call. Intended to be invoked by the in-call
     * app through {@link InCallAdapter} after Telecom notifies it of an incoming call followed by
     * the user opting to reject said call.
     */
    CARAPI RejectCall(
        /* [in] */ ICall* call,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& textMessage);

    /**
     * Instructs Telecom to play the specified DTMF tone within the specified call.
     *
     * @param digit The DTMF digit to play.
     */
    CARAPI PlayDtmfTone(
        /* [in] */ ICall* call,
        /* [in] */ Char32 digit);

    /**
     * Instructs Telecom to stop the currently playing DTMF tone, if any.
     */
    CARAPI StopDtmfTone(
        /* [in] */ ICall* call);

    /**
     * Instructs Telecom to continue (or not) the current post-dial DTMF string, if any.
     */
    CARAPI PostDialContinue(
        /* [in] */ ICall* call,
        /* [in] */ Boolean proceed);

    /**
     * Instructs Telecom to disconnect the specified call. Intended to be invoked by the
     * in-call app through {@link InCallAdapter} for an ongoing call. This is usually triggered by
     * the user hitting the end-call button.
     */
    CARAPI DisconnectCall(
        /* [in] */ ICall* call);

    /**
     * Instructs Telecom to disconnect all calls.
     */
    CARAPI DisconnectAllCalls();

    /**
     * Instructs Telecom to put the specified call on hold. Intended to be invoked by the
     * in-call app through {@link InCallAdapter} for an ongoing call. This is usually triggered by
     * the user hitting the hold button during an active call.
     */
    CARAPI HoldCall(
        /* [in] */ ICall* call);

    /**
     * Instructs Telecom to release the specified call from hold. Intended to be invoked by
     * the in-call app through {@link InCallAdapter} for an ongoing call. This is usually triggered
     * by the user hitting the hold button during a held call.
     */
    CARAPI UnholdCall(
        /* [in] */ ICall* call);

    /** Called by the in-call UI to change the mute state. */
    CARAPI Mute(
        /* [in] */ Boolean shouldMute);

    /**
      * Called by the in-call UI to change the audio route, for example to change from earpiece to
      * speaker phone.
      */
    CARAPI SetAudioRoute(
        /* [in] */ Int32 route);

    /** Called by the in-call UI to turn the proximity sensor on. */
    CARAPI TurnOnProximitySensor();

    /**
     * Called by the in-call UI to turn the proximity sensor off.
     * @param screenOnImmediately If true, the screen will be turned on immediately. Otherwise,
     *        the screen will be kept off until the proximity sensor goes negative.
     */
    CARAPI TurnOffProximitySensor(
        /* [in] */ Boolean screenOnImmediately);

    CARAPI PhoneAccountSelected(
        /* [in] */ ICall* call,
        /* [in] */ IPhoneAccountHandle* account);

    CARAPI PhoneAccountSelectedForMMI(
        /* [in] */ IUri* handle,
        /* [in] */ IPhoneAccountHandle* account);

    /** Called when the audio state changes. */
    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* oldAudioState,
        /* [in] */ IAudioState* newAudioState);

    CARAPI MarkCallAsRinging(
        /* [in] */ ICall* call);

    CARAPI MarkCallAsDialing(
        /* [in] */ ICall* call);

    CARAPI MarkCallAsActive(
        /* [in] */ ICall* call);

    CARAPI MarkCallAsOnHold(
        /* [in] */ ICall* call);

    /**
     * Marks the specified call as STATE_DISCONNECTED and notifies the in-call app. If this was the
     * last live call, then also disconnect from the in-call controller.
     *
     * @param disconnectCause The disconnect cause, see {@link android.telecomm.DisconnectCause}.
     */
    CARAPI MarkCallAsDisconnected(
        /* [in] */ ICall* call,
        /* [in] */ IDisconnectCause* disconnectCause);

    /**
     * Removes an existing disconnected call, and notifies the in-call app.
     */
    CARAPI MarkCallAsRemoved(
        /* [in] */ ICall* call);

    /**
     * Cleans up any calls currently associated with the specified connection service when the
     * service binder disconnects unexpectedly.
     *
     * @param service The connection service that disconnected.
     */
    CARAPI HandleConnectionServiceDeath(
        /* [in] */ IConnectionServiceWrapper* service);

    CARAPI HasAnyCalls(
        /* [out] */ Boolean* result);

    CARAPI HasActiveOrHoldingCall(
        /* [out] */ Boolean* result);

    CARAPI HasActiveOrHoldingCall(
        /* [in] */ const String& sub,
        /* [out] */ Boolean* result);

    CARAPI HasRingingCall(
        /* [out] */ Boolean* result);

    CARAPI OnMediaButton(
        /* [in] */ Int32 type,
        /* [out] */ Boolean* result);

    /**
     * Returns true if telecom supports adding another top-level call.
     */
    CARAPI CanAddCall(
        /* [out] */ Boolean* result);

    CARAPI GetRingingCall(
        /* [out] */ ICall** result);

    CARAPI GetActiveCall(
        /* [out] */ ICall** result);

    CARAPI GetDialingCall(
        /* [out] */ ICall** result);

    CARAPI GetHeldCall(
        /* [out] */ ICall** result);

    CARAPI GetNumHeldCalls(
        /* [out] */ Int32* result);

    CARAPI GetFirstCallWithState(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ ICall** result);

    CARAPI GetFirstCallWithHandle(
        /* [in] */ IUri* handle,
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ ICall** result);

    /**
     * Returns the first call that it finds with the given states. The states are treated as having
     * priority order so that any call with the first state will be returned before any call with
     * states listed later in the parameter list.
     *
     * @param callToSkip Call that this method should skip while searching
     */
    CARAPI GetFirstCallWithState(
        /* [in] */ ICall* callToSkip,
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ ICall** result);

    /**
     * Returns the first call that it finds with the given states for given subscription.
     * the states are treated as having priority order so that any call with the first
     * state will be returned before any call with states listed later in the parameter list.
     *
     * @param subId check calls only on this subscription
     * @param callToSkip Call that this method should skip while searching
     */
    CARAPI GetFirstCallWithStateUsingSubId(
        /* [in] */ const String& subId,
        /* [in] */ ICall* callToSkip,
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ ICall** result);

    CARAPI CreateConferenceCall(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ IParcelableConference* parcelableConference,
        /* [out] */ ICall** result);

    /**
     * @return the call state currently tracked by {@link PhoneStateBroadcaster}
     */
    CARAPI GetCallState(
        /* [out] */ Int32* result);

    /**
     * Retrieves the {@link PhoneAccountRegistrar}.
     *
     * @return The {@link PhoneAccountRegistrar}.
     */
    CARAPI GetPhoneAccountRegistrar(
        /* [out] */ PhoneAccountRegistrar** result);

    /**
     * Retrieves the {@link MissedCallNotifier}
     * @return The {@link MissedCallNotifier}.
     */
    CARAPI GetMissedCallNotifier(
        /* [out] */ MissedCallNotifier** result);

    /**
     * Retrieves the {@link MissedCallNotifier}
     * @return The {@link MissedCallNotifier}.
     */
    CARAPI GetBlacklistCallNotifier(
        /* [out] */ BlacklistCallNotifier** result);

    /**
     * Adds OEM extras from lower layers into Call's extras.
     *
     * @param call The call.
     * @param extras OEM call extras.
     */
    CARAPI SetCallExtras(
        /* [in] */ ICall* call,
        /* [in] */ IBundle* extras);

    /**
     * Creates a new call for an existing connection.
     *
     * @param callId The id of the new call.
     * @param connection The connection information.
     * @return The new call.
     */
    CARAPI CreateCallForExistingConnection(
        /* [in] */ const String& callId,
        /* [in] */ IParcelableConnection* connection,
        /* [out] */ ICall** result);

    /**
     * Dumps the state of the {@link CallsManager}.
     *
     * @param pw The {@code IndentingPrintWriter} to write the state to.
     */
    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw);

    CARAPI SwitchToOtherActiveSub(
        /* [in] */ const String& subId,
        /* [in] */ Boolean retainLch);

    CARAPI SetActiveSubscription(
        /* [in] */ const String& subId);

    CARAPI GetActiveSubscription(
        /* [out] */ String* result);

    CARAPI ManageMSimInCallTones(
        /* [in] */ Boolean isSubSwitch);

    /**
     * Returns the first call that it finds with the given states for given subscription.
     * The states are treated as having priority order so that any call with the first
     * state will be returned before any call with states listed later in the parameter list.
     */
    CARAPI GetFirstCallWithStateUsingSubId(
        /* [in] */ const String& sub,
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ ICall** result);

    CARAPI StopMSimInCallTones();

    CARAPI IsCallBlacklisted(
        /* [in] */ ICall* c,
        /* [out] */ Boolean* result);

private:
    /**
     *  Returns true if the ids are same or one of the ids is sip id.
     */
    CARAPI IsSameIdOrSipId(
        /* [in] */ const String& id1,
        /* [in] */ const String& id2,
        /* [out] */ Boolean* result);

    /**
     * Adds the specified call to the main list of live calls.
     *
     * @param call The call to add.
     */
    CARAPI AddCall(
        /* [in] */ ICall* call);

    CARAPI RemoveCall(
        /* [in] */ ICall* call);

    /**
     * Sets the specified state on the specified call.
     *
     * @param call The call.
     * @param newState The new state of the call.
     */
    CARAPI SetCallState(
        /* [in] */ ICall* call,
        /* [in] */ Int32 newState);

    /**
     * Checks which call should be visible to the user and have audio focus.
     */
    CARAPI UpdateForegroundCall();

    CARAPI UpdateCanAddCall();

    CARAPI UpdateCallsManagerState();

    CARAPI IsPotentialMMICode(
        /* [in] */ IUri* handle,
        /* [out] */ Boolean* result);

    /**
     * Determines if a dialed number is potentially an In-Call MMI code.  In-Call MMI codes are
     * MMI codes which can be dialed when one or more calls are in progress.
     * <P>
     * Checks for numbers formatted similar to the MMI codes defined in:
     * {@link com.android.internal.telephony.gsm.GSMPhone#handleInCallMmiCommands(String)}
     * and
     * {@link com.android.internal.telephony.imsphone.ImsPhone#handleInCallMmiCommands(String)}
     *
     * @param handle The URI to call.
     * @return {@code True} if the URI represents a number which could be an in-call MMI code.
     */
    CARAPI IsPotentialInCallMMICode(
        /* [in] */ IUri* handle,
        /* [out] */ Boolean* result);

    CARAPI GetNumCallsWithState(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ Int32* result);

    CARAPI GetNumCallsWithState(
        /* [in] */ const String& subId,
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ Int32* result);

    CARAPI HasMaximumLiveCalls(
        /* [out] */ Boolean* result);

    CARAPI HasMaximumLiveCalls(
        /* [in] */ const String& subId,
        /* [out] */ Boolean* result);

    CARAPI HasMaximumHoldingCalls(
        /* [out] */ Boolean* result);

    CARAPI HasMaximumHoldingCalls(
        /* [in] */ const String& subId,
        /* [out] */ Boolean* result);

    CARAPI HasMaximumRingingCalls(
        /* [out] */ Boolean* result);

    CARAPI HasMaximumRingingCalls(
        /* [in] */ const String& subId,
        /* [out] */ Boolean* result);

    CARAPI HasMaximumOutgoingCalls(
        /* [out] */ Boolean* result);

    CARAPI MakeRoomForOutgoingCall(
        /* [in] */ ICall* call,
        /* [in] */ Boolean isEmergency,
        /* [out] */ Boolean* result);

    CARAPI MakeRoomForOutgoingCallForDsda(
        /* [in] */ ICall* call,
        /* [in] */ Boolean isEmergency,
        /* [out] */ Boolean* result);

    CARAPI GetConversationSub(
        /* [out] */ String* result);

    CARAPI ReStartMSimInCallTones();

    CARAPI GetLchSub(
        /* [out] */ String* result);

    CARAPI PlayLchDtmf();

    CARAPI GetNonRingingLiveCall(
        /* [in] */ const String& subId,
        /* [out] */ ICall** result);

    CARAPI StopLchDtmf();

    CARAPI StartMSimInCallTones();

    CARAPI RemoveAnyPendingDtmfMsgs();

    /**
     * Update the local call hold state for all subscriptions
     * 1 -- if call on local hold, 0 -- if call is not on local hold
     *
     * @param subInConversation is the sub user is currently active in subsription.
     * so if this sub is in LCH, then bring that sub out of LCH.
     */
    CARAPI UpdateLchStatus(
        /* [in] */ const String& subInConversation);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitOUTGOING_CALL_STATES();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitLIVE_CALL_STATES();

    static CARAPI_(String) InitSupervisoryCallHoldToneConfig();

private:
    static AutoPtr<ArrayOf<Int32> > OUTGOING_CALL_STATES;
    static AutoPtr<ArrayOf<Int32> > LIVE_CALL_STATES;

    /**
     * Singleton instance of the {@link CallsManager}, initialized from {@link TelecomService}.
     */
    static AutoPtr<CallsManager> INSTANCE;

    static const String TAG;

    static const Int32 MAXIMUM_LIVE_CALLS;

    static const Int32 MAXIMUM_HOLD_CALLS;

    static const Int32 MAXIMUM_RINGING_CALLS;

    static const Int32 MAXIMUM_OUTGOING_CALLS;

    static const Int32 MAXIMUM_DSDA_LIVE_CALLS;

    static const Int32 MAXIMUM_DSDA_HOLD_CALLS;

    static const Int32 MAXIMUM_TOP_LEVEL_CALLS;

    /**
     * The main call repository. Keeps an instance of all live calls. New incoming and outgoing
     * calls are added to the map and removed when the calls move to the disconnected state.
    *
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mCalls;

    AutoPtr<ConnectionServiceRepository> mConnectionServiceRepository;

    AutoPtr<DtmfLocalTonePlayer> mDtmfLocalTonePlayer;

    AutoPtr<InCallController> mInCallController;

    AutoPtr<CallAudioManager> mCallAudioManager;

    AutoPtr<Ringer> mRinger;

    // For this set initial table size to 16 because we add 13 listeners in
    // the CallsManager constructor.
    AutoPtr<ISet> mListeners;

    AutoPtr<HeadsetMediaButton> mHeadsetMediaButton;

    AutoPtr<WiredHeadsetManager> mWiredHeadsetManager;

    AutoPtr<TtyManager> mTtyManager;

    AutoPtr<ProximitySensorManager> mProximitySensorManager;

    AutoPtr<PhoneStateBroadcaster> mPhoneStateBroadcaster;

    AutoPtr<CallLogManager> mCallLogManager;

    AutoPtr<IContext> mContext;

    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;

    AutoPtr<MissedCallNotifier> mMissedCallNotifier;

    AutoPtr<BlacklistCallNotifier> mBlacklistCallNotifier;

    AutoPtr<SpamBlocker> mCallerInfoBlocker;

    AutoPtr<ISet> mLocallyDisconnectingCalls;

    Boolean mCanAddCall;

    /**
     * The call the user is currently interacting with. This is the call that should have audio
     * focus and be visible in the in-call UI.
     */
    AutoPtr<ICall> mForegroundCall;

    AutoPtr<InCallTonePlayer::Factory> mPlayerFactory;

    static const Int32 LCH_PLAY_DTMF;

    static const Int32 LCH_STOP_DTMF;

    static const Int32 LCH_DTMF_PERIODICITY;

    static const Int32 LCH_DTMF_PERIOD;

    static const Int32 PHONE_START_MSIM_INCALL_TONE;

    static const String sSupervisoryCallHoldToneConfig;

    String mLchSub;

    AutoPtr<InCallTonePlayer> mLocalCallReminderTonePlayer;

    AutoPtr<InCallTonePlayer> mSupervisoryCallHoldTonePlayer;

    String mSubInConversation;

    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLSMANAGER_H__
