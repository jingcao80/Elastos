
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                    ImsPhoneConnection::MyHandler
//=====================================================================
ImsPhoneConnection::MyHandler::MyHandler(
    /* [in] */ ILooper* l)
{
    // ==================before translated======================
    // super(l);
}

ECode ImsPhoneConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    //
    // switch (msg.what) {
    //     case EVENT_NEXT_POST_DIAL:
    //     case EVENT_DTMF_DONE:
    //     case EVENT_PAUSE_DONE:
    //         processNextPostDialChar();
    //         break;
    //     case EVENT_WAKE_LOCK_TIMEOUT:
    //         releaseWakeLock();
    //         break;
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                          ImsPhoneConnection
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhoneConnection, Connection, IImsPhoneConnection);

const String ImsPhoneConnection::LOGTAG("ImsPhoneConnection");
const Boolean ImsPhoneConnection::DBG = TRUE;
const Int32 ImsPhoneConnection::EVENT_DTMF_DONE;
const Int32 ImsPhoneConnection::EVENT_PAUSE_DONE;
const Int32 ImsPhoneConnection::EVENT_NEXT_POST_DIAL;
const Int32 ImsPhoneConnection::EVENT_WAKE_LOCK_TIMEOUT;
const Int32 ImsPhoneConnection::PAUSE_DELAY_MILLIS;
const Int32 ImsPhoneConnection::WAKE_LOCK_TIMEOUT_MILLIS;

ECode ImsPhoneConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsPhoneCallTracker* ct,
    /* [in] */ IImsPhoneCall* parent)
{
    // ==================before translated======================
    // createWakeLock(context);
    // acquireWakeLock();
    //
    // mOwner = ct;
    // mHandler = new MyHandler(mOwner.getLooper());
    // mImsCall = imsCall;
    //
    // if ((imsCall != null) && (imsCall.getCallProfile() != null)) {
    //     mAddress = imsCall.getCallProfile().getCallExtra(ImsCallProfile.EXTRA_OI);
    //     mCnapName = imsCall.getCallProfile().getCallExtra(ImsCallProfile.EXTRA_CNA);
    //     mNumberPresentation = ImsCallProfile.OIRToPresentation(
    //             imsCall.getCallProfile().getCallExtraInt(ImsCallProfile.EXTRA_OIR));
    //     mCnapNamePresentation = ImsCallProfile.OIRToPresentation(
    //             imsCall.getCallProfile().getCallExtraInt(ImsCallProfile.EXTRA_CNAP));
    //
    //     ImsCallProfile imsCallProfile = imsCall.getCallProfile();
    //     if (imsCallProfile != null) {
    //         setVideoState(ImsCallProfile.getVideoStateFromImsCallProfile(imsCallProfile));
    //
    //         ImsStreamMediaProfile mediaProfile = imsCallProfile.mMediaProfile;
    //         if (mediaProfile != null) {
    //             setAudioQuality(getAudioQualityFromMediaProfile(mediaProfile));
    //         }
    //     }
    //
    //     // Determine if the current call have video capabilities.
    //     try {
    //         ImsCallProfile localCallProfile = imsCall.getLocalCallProfile();
    //         if (localCallProfile != null) {
    //             boolean isLocalVideoCapable = localCallProfile.mCallType
    //                     == ImsCallProfile.CALL_TYPE_VT;
    //
    //             setLocalVideoCapable(isLocalVideoCapable);
    //         }
    //         ImsCallProfile remoteCallProfile = imsCall.getRemoteCallProfile();
    //         if (remoteCallProfile != null) {
    //             boolean isRemoteVideoCapable = remoteCallProfile.mCallType
    //                     == ImsCallProfile.CALL_TYPE_VT;
    //
    //             setRemoteVideoCapable(isRemoteVideoCapable);
    //         }
    //     } catch (ImsException e) {
    //         // No session, so cannot get local capabilities.
    //     }
    // } else {
    //     mNumberPresentation = PhoneConstants.PRESENTATION_UNKNOWN;
    //     mCnapNamePresentation = PhoneConstants.PRESENTATION_UNKNOWN;
    // }
    //
    // mIsIncoming = true;
    // mCreateTime = System.currentTimeMillis();
    // mUusInfo = null;
    //
    // //mIndex = index;
    //
    // mParent = parent;
    // mParent.attach(this, ImsPhoneCall.State.INCOMING);
    return NOERROR;
}

ECode ImsPhoneConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhoneCallTracker* ct,
    /* [in] */ IImsPhoneCall* parent,
    /* [in] */ IBundle* extras)
{
    // ==================before translated======================
    // createWakeLock(context);
    // acquireWakeLock();
    // boolean isConferenceUri = false;
    // boolean isSkipSchemaParsing = false;
    // if (extras != null) {
    //     isConferenceUri = extras.getBoolean(
    //             TelephonyProperties.EXTRA_DIAL_CONFERENCE_URI, false);
    //     isSkipSchemaParsing = extras.getBoolean(
    //             TelephonyProperties.EXTRA_SKIP_SCHEMA_PARSING, false);
    // }
    //
    // mOwner = ct;
    // mHandler = new MyHandler(mOwner.getLooper());
    //
    // mDialString = dialString;
    //
    // if (isConferenceUri || isSkipSchemaParsing) {
    //     mAddress = dialString;
    //     mPostDialString = "";
    // } else {
    //     mAddress = PhoneNumberUtils.extractNetworkPortionAlt(dialString);
    //     mPostDialString = PhoneNumberUtils.extractPostDialPortion(dialString);
    // }
    //
    // //mIndex = -1;
    //
    // mIsIncoming = false;
    // mCnapName = null;
    // mCnapNamePresentation = PhoneConstants.PRESENTATION_ALLOWED;
    // mNumberPresentation = PhoneConstants.PRESENTATION_ALLOWED;
    // mCreateTime = System.currentTimeMillis();
    //
    // if (extras != null) {
    //     mCallExtras = extras;
    // }
    //
    // mParent = parent;
    // parent.attachFake(this, ImsPhoneCall.State.DIALING);
    return NOERROR;
}

ECode ImsPhoneConnection::Dispose()
{
    assert(0);
    return NOERROR;
}

Boolean ImsPhoneConnection::EqualsHandlesNulls(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    // ==================before translated======================
    // return (a == null) ? (b == null) : a.equals (b);
    assert(0);
    return FALSE;
}

ECode ImsPhoneConnection::GetOrigDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDialString;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParent;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDisconnectTime;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetHoldingStartTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHoldingStartTime;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (getState() != ImsPhoneCall.State.HOLDING) {
    //     // If not holding, return 0
    //     return 0;
    // } else {
    //     return SystemClock.elapsedRealtime() - mHoldingStartTime;
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCause;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::SetDisconnectCause(
    /* [in] */ Int32 cause)
{
    // ==================before translated======================
    // mCause = cause;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetOwner(
    /* [out] */ IImsPhoneCallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOwner;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mDisconnected) {
    //     return ImsPhoneCall.State.DISCONNECTED;
    // } else {
    //     return super.getState();
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::Hangup()
{
    // ==================before translated======================
    // if (!mDisconnected) {
    //     mOwner.hangup(this);
    // } else {
    //     throw new CallStateException ("disconnected");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::Separate()
{
    // ==================before translated======================
    // throw new CallStateException ("not supported");
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPostDialState;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::ProceedAfterWaitChar()
{
    // ==================before translated======================
    // if (mPostDialState != PostDialState.WAIT) {
    //     Rlog.w(LOGTAG, "ImsPhoneConnection.proceedAfterWaitChar(): Expected "
    //             + "getPostDialState() to be WAIT but was " + mPostDialState);
    //     return;
    // }
    //
    // setPostDialState(PostDialState.STARTED);
    //
    // processNextPostDialChar();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // if (mPostDialState != PostDialState.WILD) {
    //     Rlog.w(LOGTAG, "ImsPhoneConnection.proceedAfterWaitChar(): Expected "
    //             + "getPostDialState() to be WILD but was " + mPostDialState);
    //     return;
    // }
    //
    // setPostDialState(PostDialState.STARTED);
    //
    // // make a new postDialString, with the wild char replacement string
    // // at the beginning, followed by the remaining postDialString.
    //
    // StringBuilder buf = new StringBuilder(str);
    // buf.append(mPostDialString.substring(mNextPostDialChar));
    // mPostDialString = buf.toString();
    // mNextPostDialChar = 0;
    // if (Phone.DEBUG_PHONE) {
    //     Rlog.d(LOGTAG, "proceedAfterWildChar: new postDialString is " +
    //             mPostDialString);
    // }
    //
    // processNextPostDialChar();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::CancelPostDial()
{
    // ==================before translated======================
    // setPostDialState(PostDialState.CANCELLED);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::OnHangupLocal()
{
    // ==================before translated======================
    // mCause = DisconnectCause.LOCAL;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::OnDisconnect(
    /* [in] */ Int32 cause,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.d(LOGTAG, "onDisconnect: cause=" + cause);
    // if (mCause != DisconnectCause.LOCAL) mCause = cause;
    // return onDisconnect();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::OnDisconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean changed = false;
    //
    // if (!mDisconnected) {
    //     //mIndex = -1;
    //
    //     mDisconnectTime = System.currentTimeMillis();
    //     mDuration = SystemClock.elapsedRealtime() - mConnectTimeReal;
    //     mDisconnected = true;
    //
    //     mOwner.mPhone.notifyDisconnect(this);
    //
    //     if (mParent != null) {
    //         changed = mParent.connectionDisconnected(this);
    //     } else {
    //         Rlog.d(LOGTAG, "onDisconnect: no parent");
    //     }
    //     if (mImsCall != null) mImsCall.close();
    //     mImsCall = null;
    // }
    // releaseWakeLock();
    // return changed;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::OnConnectedInOrOut()
{
    // ==================before translated======================
    // mConnectTime = System.currentTimeMillis();
    // mConnectTimeReal = SystemClock.elapsedRealtime();
    // mDuration = 0;
    //
    // if (Phone.DEBUG_PHONE) {
    //     Rlog.d(LOGTAG, "onConnectedInOrOut: connectTime=" + mConnectTime);
    // }
    //
    // if (!mIsIncoming) {
    //     // outgoing calls only
    //     processNextPostDialChar();
    // }
    // releaseWakeLock();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::OnStartedHolding()
{
    // ==================before translated======================
    // mHoldingStartTime = SystemClock.elapsedRealtime();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mPostDialState == PostDialState.CANCELLED
    //     || mPostDialState == PostDialState.COMPLETE
    //     || mPostDialString == null
    //     || mPostDialString.length() <= mNextPostDialChar
    // ) {
    //     return "";
    // }
    //
    // return mPostDialString.substring(mNextPostDialChar);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::ReleaseWakeLock()
{
    // ==================before translated======================
    // synchronized(mPartialWakeLock) {
    //     if (mPartialWakeLock.isHeld()) {
    //         Rlog.d(LOGTAG, "releaseWakeLock");
    //         mPartialWakeLock.release();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mNumberPresentation;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUusInfo;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mImsCall != null && mImsCall.isMultiparty();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetImsCall(
    /* [out] */ /*TODO IImsCall*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mImsCall;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::SetImsCall(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // mImsCall = imsCall;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::ChangeParent(
    /* [in] */ IImsPhoneCall* parent)
{
    // ==================before translated======================
    // mParent = parent;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::Update(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */  ICallState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean changed = false;
    //
    // if (state == ImsPhoneCall.State.ACTIVE) {
    //     if (mParent.getState().isRinging()
    //             || mParent.getState().isDialing()) {
    //         onConnectedInOrOut();
    //     }
    //
    //     if (mParent.getState().isRinging()
    //             || mParent == mOwner.mBackgroundCall) {
    //         //mForegroundCall should be IDLE
    //         //when accepting WAITING call
    //         //before accept WAITING call,
    //         //the ACTIVE call should be held ahead
    //         mParent.detach(this);
    //         mParent = mOwner.mForegroundCall;
    //         mParent.attach(this);
    //     }
    // } else if (state == ImsPhoneCall.State.HOLDING) {
    //     onStartedHolding();
    // }
    //
    // changed = mParent.update(this, imsCall, state);
    //
    // if (imsCall != null) {
    //     // Check for a change in the video capabilities for the call and update the
    //     // {@link ImsPhoneConnection} with this information.
    //     try {
    //         // Get the current local VT capabilities (i.e. even if currentCallType above is
    //         // audio-only, the local capability could support bi-directional video).
    //         ImsCallProfile localCallProfile = imsCall.getLocalCallProfile();
    //         Rlog.d(LOGTAG, " update localCallProfile=" + localCallProfile
    //                 + "isLocalVideoCapable()= " + isLocalVideoCapable());
    //         if (localCallProfile != null) {
    //             boolean newLocalVideoCapable = localCallProfile.mCallType
    //                     == ImsCallProfile.CALL_TYPE_VT;
    //
    //             if (isLocalVideoCapable() != newLocalVideoCapable) {
    //                 setLocalVideoCapable(newLocalVideoCapable);
    //                 changed = true;
    //             }
    //         }
    //
    //         ImsCallProfile remoteCallProfile = imsCall.getRemoteCallProfile();
    //         Rlog.d(LOGTAG, " update remoteCallProfile=" + remoteCallProfile
    //                 + "isRemoteVideoCapable()= " + isRemoteVideoCapable());
    //         if (remoteCallProfile != null) {
    //             boolean newRemoteVideoCapable = remoteCallProfile.mCallType
    //                     == ImsCallProfile.CALL_TYPE_VT;
    //
    //             if (isRemoteVideoCapable() != newRemoteVideoCapable) {
    //                 setRemoteVideoCapable(newRemoteVideoCapable);
    //                 changed = true;
    //             }
    //         }
    //
    //         // Check if call substate has changed. If so notify listeners of call state changed.
    //         int callSubstate = getCallSubstate();
    //         int newCallSubstate = imsCall.getCallSubstate();
    //
    //         if (callSubstate != newCallSubstate) {
    //             setCallSubstate(newCallSubstate);
    //             changed = true;
    //         }
    //
    //     } catch (ImsException e) {
    //         // No session in place -- no change
    //     }
    //
    //     // Check for a change in the call type / video state, or audio quality of the
    //     // {@link ImsCall} and update the {@link ImsPhoneConnection} with this information.
    //     ImsCallProfile callProfile = imsCall.getCallProfile();
    //     if (callProfile != null) {
    //         int oldVideoState = getVideoState();
    //         int newVideoState = ImsCallProfile.getVideoStateFromImsCallProfile(callProfile);
    //
    //         if (oldVideoState != newVideoState) {
    //             setVideoState(newVideoState);
    //             changed = true;
    //         }
    //
    //         ImsStreamMediaProfile mediaProfile = callProfile.mMediaProfile;
    //         if (mediaProfile != null) {
    //             int oldAudioQuality = getAudioQuality();
    //             int newAudioQuality = getAudioQualityFromMediaProfile(mediaProfile);
    //
    //             if (oldAudioQuality != newAudioQuality) {
    //                 setAudioQuality(newAudioQuality);
    //                 changed = true;
    //             }
    //         }
    //     }
    // }
    // return changed;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return 0;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::GetCallExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCallExtras;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::OnDisconnectConferenceParticipant(
    /* [in] */ IUri* endpoint)
{
    // ==================before translated======================
    // ImsCall imsCall = getImsCall();
    // if (imsCall == null) {
    //     return;
    // }
    // try {
    //     imsCall.removeParticipants(new String[]{endpoint.toString()});
    // } catch (ImsException e) {
    //     // No session in place -- no change
    //     Rlog.e(LOGTAG, "onDisconnectConferenceParticipant: no session in place. "+
    //             "Failed to disconnect endpoint = " + endpoint);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder();
    // sb.append("[ImsPhoneConnection objId: ");
    // sb.append(System.identityHashCode(this));
    // sb.append(" address:");
    // sb.append(Log.pii(getAddress()));
    // sb.append(" ImsCall:");
    // if (mImsCall == null) {
    //     sb.append("null");
    // } else {
    //     sb.append(mImsCall);
    // }
    // sb.append("]");
    // return sb.toString();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneConnection::Finalize()
{
    // ==================before translated======================
    // releaseWakeLock();
    assert(0);
    return NOERROR;
}

Int32 ImsPhoneConnection::GetAudioQualityFromMediaProfile(
    /* [in] */ /*TODO IImsStreamMediaProfile*/IInterface* mediaProfile)
{
    // ==================before translated======================
    // int audioQuality;
    //
    // // The Adaptive Multi-Rate Wideband codec is used for high definition audio calls.
    // if (mediaProfile.mAudioQuality == ImsStreamMediaProfile.AUDIO_QUALITY_AMR_WB) {
    //     audioQuality = AUDIO_QUALITY_HIGH_DEFINITION;
    // } else {
    //     audioQuality = AUDIO_QUALITY_STANDARD;
    // }
    //
    // return audioQuality;
    assert(0);
    return 0;
}

Boolean ImsPhoneConnection::ProcessPostDialChar(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (PhoneNumberUtils.is12Key(c)) {
    //     mOwner.mCi.sendDtmf(c, mHandler.obtainMessage(EVENT_DTMF_DONE));
    // } else if (c == PhoneNumberUtils.PAUSE) {
    //     // From TS 22.101:
    //     // It continues...
    //     // Upon the called party answering the UE shall send the DTMF digits
    //     // automatically to the network after a delay of 3 seconds( 20 ).
    //     // The digits shall be sent according to the procedures and timing
    //     // specified in 3GPP TS 24.008 [13]. The first occurrence of the
    //     // "DTMF Control Digits Separator" shall be used by the ME to
    //     // distinguish between the addressing digits (i.e. the phone number)
    //     // and the DTMF digits. Upon subsequent occurrences of the
    //     // separator,
    //     // the UE shall pause again for 3 seconds ( 20 ) before sending
    //     // any further DTMF digits.
    //     mHandler.sendMessageDelayed(mHandler.obtainMessage(EVENT_PAUSE_DONE),
    //             PAUSE_DELAY_MILLIS);
    // } else if (c == PhoneNumberUtils.WAIT) {
    //     setPostDialState(PostDialState.WAIT);
    // } else if (c == PhoneNumberUtils.WILD) {
    //     setPostDialState(PostDialState.WILD);
    // } else {
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

void ImsPhoneConnection::ProcessNextPostDialChar()
{
    // ==================before translated======================
    // char c = 0;
    // Registrant postDialHandler;
    //
    // if (mPostDialState == PostDialState.CANCELLED) {
    //     //Rlog.d(LOGTAG, "##### processNextPostDialChar: postDialState == CANCELLED, bail");
    //     return;
    // }
    //
    // if (mPostDialString == null ||
    //         mPostDialString.length() <= mNextPostDialChar) {
    //     setPostDialState(PostDialState.COMPLETE);
    //
    //     // notifyMessage.arg1 is 0 on complete
    //     c = 0;
    // } else {
    //     boolean isValid;
    //
    //     setPostDialState(PostDialState.STARTED);
    //
    //     c = mPostDialString.charAt(mNextPostDialChar++);
    //
    //     isValid = processPostDialChar(c);
    //
    //     if (!isValid) {
    //         // Will call processNextPostDialChar
    //         mHandler.obtainMessage(EVENT_NEXT_POST_DIAL).sendToTarget();
    //         // Don't notify application
    //         Rlog.e(LOGTAG, "processNextPostDialChar: c=" + c + " isn't valid!");
    //         return;
    //     }
    // }
    //
    // postDialHandler = mOwner.mPhone.mPostDialHandler;
    //
    // Message notifyMessage;
    //
    // if (postDialHandler != null
    //         && (notifyMessage = postDialHandler.messageForRegistrant()) != null) {
    //     // The AsyncResult.result is the Connection object
    //     PostDialState state = mPostDialState;
    //     AsyncResult ar = AsyncResult.forMessage(notifyMessage);
    //     ar.result = this;
    //     ar.userObj = state;
    //
    //     // arg1 is the character that was/is being processed
    //     notifyMessage.arg1 = c;
    //
    //     //Rlog.v(LOGTAG, "##### processNextPostDialChar: send msg to postDialHandler, arg1=" + c);
    //     notifyMessage.sendToTarget();
    // }
    assert(0);
}

void ImsPhoneConnection::SetPostDialState(
    /* [in] */ IConnectionPostDialState s)
{
    // ==================before translated======================
    // if (mPostDialState != PostDialState.STARTED
    //         && s == PostDialState.STARTED) {
    //     acquireWakeLock();
    //     Message msg = mHandler.obtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
    //     mHandler.sendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS);
    // } else if (mPostDialState == PostDialState.STARTED
    //         && s != PostDialState.STARTED) {
    //     mHandler.removeMessages(EVENT_WAKE_LOCK_TIMEOUT);
    //     releaseWakeLock();
    // }
    // mPostDialState = s;
    assert(0);
}

void ImsPhoneConnection::CreateWakeLock(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
    // mPartialWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOGTAG);
    assert(0);
}

void ImsPhoneConnection::AcquireWakeLock()
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "acquireWakeLock");
    // mPartialWakeLock.acquire();
    assert(0);
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
