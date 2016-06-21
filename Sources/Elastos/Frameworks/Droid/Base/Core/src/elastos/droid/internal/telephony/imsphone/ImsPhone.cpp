
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhone.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                       ImsPhone::InnerRunnable
//=====================================================================
ImsPhone::InnerRunnable::InnerRunnable(
    /* [in] */ ImsPhone* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhone::InnerRunnable::Run()
{
    // ==================before translated======================
    // exitEmergencyCallbackMode();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                             ImsPhone::Cf
//=====================================================================
ImsPhone::Cf::Cf(
    /* [in] */ const String& cfNumber,
    /* [in] */ Boolean isCfu,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // mSetCfNumber = cfNumber;
    // mIsCfu = isCfu;
    // mOnComplete = onComplete;
}

//=====================================================================
//                 ImsPhone::InnerImsEcbmStateListener
//=====================================================================
//TODO CAR_INTERFACE_IMPL(ImsPhone::InnerImsEcbmStateListener, Object, IImsEcbmStateListener);

ImsPhone::InnerImsEcbmStateListener::InnerImsEcbmStateListener(
    /* [in] */ ImsPhone* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhone::InnerImsEcbmStateListener::OnECBMEntered()
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "onECBMEntered");
    // handleEnterEmergencyCallbackMode();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::InnerImsEcbmStateListener::OnECBMExited()
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "onECBMExited");
    // handleExitEmergencyCallbackMode();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                               ImsPhone
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhone, ImsPhoneBase, IImsPhone);

//const String ImsPhone::CS_FALLBACK("cs_fallback");
const Int32 ImsPhone::RESTART_ECM_TIMER;
const Int32 ImsPhone::CANCEL_ECM_TIMER;
const Int32 ImsPhone::EVENT_SET_CALL_BARRING_DONE;
const Int32 ImsPhone::EVENT_GET_CALL_BARRING_DONE;
const Int32 ImsPhone::EVENT_SET_CALL_WAITING_DONE;
const Int32 ImsPhone::EVENT_GET_CALL_WAITING_DONE;
const String ImsPhone::LOGTAG("ImsPhone");
const Boolean ImsPhone::DBG = TRUE;
const Boolean ImsPhone::VDBG = FALSE;
const Int32 ImsPhone::DEFAULT_ECM_EXIT_TIMER_VALUE;

ImsPhone::ImsPhone()
{
}

ECode ImsPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ IPhone* defaultPhone)
{
    ImsPhoneBase::constructor(String("ImsPhone"), context, notifier);
    // ==================before translated======================
    //
    // mDefaultPhone = (PhoneBase) defaultPhone;
    // mCT = new ImsPhoneCallTracker(this);
    // mSS.setStateOff();
    //
    // mPhoneId = mDefaultPhone.getPhoneId();
    //
    // // This is needed to handle phone process crashes
    // // Same property is used for both CDMA & IMS phone.
    // mIsPhoneInEcmState = SystemProperties.getBoolean(
    //         TelephonyProperties.PROPERTY_INECM_MODE, false);
    //
    // PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
    // mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOGTAG);
    // mWakeLock.setReferenceCounted(false);
    return NOERROR;
}

ECode ImsPhone::UpdateParentPhone(
    /* [in] */ IPhoneBase* parentPhone)
{
    // ==================before translated======================
    // // synchronization is managed at the PhoneBase scope (which calls this function)
    // mDefaultPhone = parentPhone;
    // mPhoneId = mDefaultPhone.getPhoneId();
    //
    // // When the parent phone is updated, we need to notify listeners of the cached video
    // // capability.
    // Rlog.d(LOGTAG, "updateParentPhone - Notify video capability changed " + mIsVideoCapable);
    // notifyForVideoCapabilityChanged(mIsVideoCapable);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::Dispose()
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "dispose");
    // // Nothing to dispose in PhoneBase
    // //super.dispose();
    // mPendingMMIs.clear();
    // mCT.dispose();
    //
    // //Force all referenced classes to unregister their former registered events
    assert(0);
    return NOERROR;
}

ECode ImsPhone::RemoveReferences()
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "removeReferences");
    // super.removeReferences();
    //
    // mCT = null;
    // mSS = null;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSS;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetServiceState(
    /* [in] */ Int32 state)
{
    // ==================before translated======================
    // mSS.setState(state);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetCallTracker(
    /* [out] */ ICallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetPendingMmiCodes(
    /* [out] */ IList/*<? extends ImsPhoneMmiCode>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPendingMMIs;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    // ==================before translated======================
    // mCT.acceptCall(videoState);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::DeflectCall(
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // mCT.deflectCall(number);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::RejectCall()
{
    // ==================before translated======================
    // mCT.rejectCall();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SwitchHoldingAndActive()
{
    // ==================before translated======================
    // mCT.switchWaitingOrHoldingAndActive();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.canConference();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.canDial();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::Conference()
{
    // ==================before translated======================
    // mCT.conference();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::ClearDisconnected()
{
    // ==================before translated======================
    // mCT.clearDisconnected();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.canTransfer();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::ExplicitCallTransfer()
{
    // ==================before translated======================
    // mCT.explicitCallTransfer();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.mForegroundCall;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.mBackgroundCall;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.mRingingCall;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::NotifySuppSvcNotification(
    /* [in] */ ISuppServiceNotification* suppSvc)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "notifySuppSvcNotification: suppSvc = " + suppSvc);
    //
    // AsyncResult ar = new AsyncResult(null, suppSvc, null);
    // mSsnRegistrants.notifyRegistrants(ar);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!isInCall()) {
    //     return false;
    // }
    //
    // if (TextUtils.isEmpty(dialString)) {
    //     return false;
    // }
    //
    // boolean result = false;
    // char ch = dialString.charAt(0);
    // switch (ch) {
    //     case '0':
    //         result = handleCallDeflectionIncallSupplementaryService(
    //                 dialString);
    //         break;
    //     case '1':
    //         result = handleCallWaitingIncallSupplementaryService(
    //                 dialString);
    //         break;
    //     case '2':
    //         result = handleCallHoldIncallSupplementaryService(dialString);
    //         break;
    //     case '3':
    //         result = handleMultipartyIncallSupplementaryService(dialString);
    //         break;
    //     case '4':
    //         result = handleEctIncallSupplementaryService(dialString);
    //         break;
    //     case '5':
    //         result = handleCcbsIncallSupplementaryService(dialString);
    //         break;
    //     default:
    //         break;
    // }
    //
    // return result;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //  ImsPhoneCall.State foregroundCallState = getForegroundCall().getState();
    //  ImsPhoneCall.State backgroundCallState = getBackgroundCall().getState();
    //  ImsPhoneCall.State ringingCallState = getRingingCall().getState();
    //
    // return (foregroundCallState.isAlive() ||
    //         backgroundCallState.isAlive() ||
    //         ringingCallState.isAlive());
    assert(0);
    return NOERROR;
}

ECode ImsPhone::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    // ==================before translated======================
    // mDefaultPhone.notifyNewRingingConnectionP(c);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::NotifyForVideoCapabilityChanged(
    /* [in] */ Boolean isVideoCapable)
{
    // ==================before translated======================
    // mIsVideoCapable = isVideoCapable;
    // mDefaultPhone.notifyForVideoCapabilityChanged(isVideoCapable);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return dialInternal(dialString, videoState, extras);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return dialInternal(dialString, videoState, null);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::AddParticipant(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // mCT.addParticipant(dialString);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SendDtmf(
    /* [in] */ Char32 c)
{
    // ==================before translated======================
    // if (!PhoneNumberUtils.is12Key(c)) {
    //     Rlog.e(LOGTAG,
    //             "sendDtmf called with invalid character '" + c + "'");
    // } else {
    //     if (mCT.mState ==  PhoneConstants.State.OFFHOOK) {
    //         mCT.sendDtmf(c);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::StartDtmf(
    /* [in] */ Char32 c)
{
    // ==================before translated======================
    // if (!(PhoneNumberUtils.is12Key(c) || (c >= 'A' && c <= 'D'))) {
    //     Rlog.e(LOGTAG,
    //             "startDtmf called with invalid character '" + c + "'");
    // } else {
    //     mCT.startDtmf(c);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::StopDtmf()
{
    // ==================before translated======================
    // mCT.stopDtmf();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mPostDialHandler = new Registrant(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::NotifyIncomingRing()
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "notifyIncomingRing");
    // AsyncResult ar = new AsyncResult(null, null, null);
    // sendMessage(obtainMessage(EVENT_CALL_RING, ar));
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetMute(
    /* [in] */ Boolean muted)
{
    // ==================before translated======================
    // mCT.setMute(muted);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // mCT.setUiTTYMode(uiTtyMode, onComplete);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.getMute();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.mState;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "getCallForwardingOption reason=" + commandInterfaceCFReason);
    // if (isValidCommandInterfaceCFReason(commandInterfaceCFReason)) {
    //     if (DBG) Rlog.d(LOGTAG, "requesting call forwarding query.");
    //     Message resp;
    //     resp = obtainMessage(EVENT_GET_CALL_FORWARD_DONE, onComplete);
    //
    //     try {
    //         ImsUtInterface ut = mCT.getUtInterface();
    //         ut.queryCallForward(getConditionFromCFReason(commandInterfaceCFReason),null,resp);
    //     } catch (ImsException e) {
    //         sendErrorResponse(onComplete, e);
    //     }
    // } else if (onComplete != null) {
    //     sendErrorResponse(onComplete);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "setCallForwardingOption action=" + commandInterfaceCFAction
    //         + ", reason=" + commandInterfaceCFReason);
    // if ((isValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
    //         (isValidCommandInterfaceCFReason(commandInterfaceCFReason))) {
    //     Message resp;
    //     Cf cf = new Cf(dialingNumber,
    //             (commandInterfaceCFReason == CF_REASON_UNCONDITIONAL ? true : false),
    //             onComplete);
    //     resp = obtainMessage(EVENT_SET_CALL_FORWARD_DONE,
    //             isCfEnable(commandInterfaceCFAction) ? 1 : 0, 0, cf);
    //
    //     try {
    //         ImsUtInterface ut = mCT.getUtInterface();
    //         ut.updateCallForward(getActionFromCFAction(commandInterfaceCFAction),
    //                 getConditionFromCFReason(commandInterfaceCFReason),
    //                 dialingNumber,
    //                 timerSeconds,
    //                 onComplete);
    //      } catch (ImsException e) {
    //         sendErrorResponse(onComplete, e);
    //      }
    // } else if (onComplete != null) {
    //     sendErrorResponse(onComplete);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetCallForwardingUncondTimerOption(
    /* [in] */ Int32 startHour,
    /* [in] */ Int32 startMinute,
    /* [in] */ Int32 endHour,
    /* [in] */ Int32 endMinute,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "setCallForwardingUncondTimerOption action="
    //         + commandInterfaceCFAction + ", reason=" + commandInterfaceCFReason
    //         + ", startHour=" + startHour + ", startMinute=" + startMinute
    //         + ", endHour=" + endHour + ", endMinute=" + endMinute);
    // if ((isValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
    //         (isValidCommandInterfaceCFReason(commandInterfaceCFReason))) {
    //     Message resp;
    //     Cf cf = new Cf(dialingNumber,
    //             (commandInterfaceCFReason == CF_REASON_UNCONDITIONAL ? true : false),
    //             onComplete);
    //     resp = obtainMessage(EVENT_SET_CALL_FORWARD_TIMER_DONE,
    //             isCfEnable(commandInterfaceCFAction) ? 1 : 0, 0, cf);
    //
    //     try {
    //         ImsUtInterface ut = mCT.getUtInterface();
    //         ut.updateCallForwardUncondTimer(startHour, startMinute, endHour,
    //                 endMinute, getActionFromCFAction(commandInterfaceCFAction),
    //                 getConditionFromCFReason(commandInterfaceCFReason),
    //                 dialingNumber, resp);
    //      } catch (ImsException e) {
    //         sendErrorResponse(onComplete, e);
    //      }
    // } else if (onComplete != null) {
    //     sendErrorResponse(onComplete);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "getCallWaiting");
    // Message resp;
    // resp = obtainMessage(EVENT_GET_CALL_WAITING_DONE, onComplete);
    //
    // try {
    //     ImsUtInterface ut = mCT.getUtInterface();
    //     ut.queryCallWaiting(resp);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "setCallWaiting enable=" + enable);
    // Message resp;
    // resp = obtainMessage(EVENT_SET_CALL_WAITING_DONE, onComplete);
    //
    // try {
    //     ImsUtInterface ut = mCT.getUtInterface();
    //     ut.updateCallWaiting(enable, resp);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetCallBarring(
    /* [in] */ const String& facility,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "getCallBarring facility=" + facility);
    // Message resp;
    // resp = obtainMessage(EVENT_GET_CALL_BARRING_DONE, onComplete);
    //
    // try {
    //     ImsUtInterface ut = mCT.getUtInterface();
    //     ut.queryCallBarring(getCBTypeFromFacility(facility), resp);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetCallBarring(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "setCallBarring facility=" + facility
    //         + ", lockState=" + lockState);
    // Message resp;
    // resp = obtainMessage(EVENT_SET_CALL_BARRING_DONE, onComplete);
    //
    // int action;
    // if (lockState) {
    //     action = CommandsInterface.CF_ACTION_ENABLE;
    // }
    // else {
    //     action = CommandsInterface.CF_ACTION_DISABLE;
    // }
    //
    // try {
    //     ImsUtInterface ut = mCT.getUtInterface();
    //     // password is not required with Ut interface
    //     ut.updateCallBarring(getCBTypeFromFacility(facility), action, resp, null);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "sendUssdResponse");
    // ImsPhoneMmiCode mmi = ImsPhoneMmiCode.newFromUssdUserInput(ussdMessge, this);
    // mPendingMMIs.add(mmi);
    // mMmiRegistrants.notifyRegistrants(new AsyncResult(null, mmi, null));
    // mmi.sendUssd(ussdMessge);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SendUSSD(
    /* [in] */ const String& ussdString,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCT.sendUSSD(ussdString, response);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::CancelUSSD()
{
    // ==================before translated======================
    // mCT.cancelUSSD();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SendErrorResponse(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "sendErrorResponse");
    // if (onComplete != null) {
    //     AsyncResult.forMessage(onComplete, null,
    //             new CommandException(CommandException.Error.GENERIC_FAILURE));
    //     onComplete.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

//ECode ImsPhone::SendErrorResponse(
//    /* [in] */ IMessage* onComplete,
//    /* [in] */ Throwable* e)
//{
//    // ==================before translated======================
//    // Rlog.d(LOGTAG, "sendErrorResponse");
//    // if (onComplete != null) {
//    //     AsyncResult.forMessage(onComplete, null, getCommandException(e));
//    //     onComplete.sendToTarget();
//    // }
//    assert(0);
//    return NOERROR;
//}

ECode ImsPhone::SendErrorResponse(
    /* [in] */ IMessage* onComplete,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "sendErrorResponse reasonCode=" + reasonInfo.getCode());
    // if (onComplete != null) {
    //     AsyncResult.forMessage(onComplete, null, getCommandException(reasonInfo.getCode()));
    //     onComplete.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetCommandException(
    /* [in] */ Int32 code,
    /* [out] */ ICommandException** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.d(LOGTAG, "getCommandException code=" + code);
    // CommandException.Error error = CommandException.Error.GENERIC_FAILURE;
    //
    // switch(code) {
    //     case ImsReasonInfo.CODE_UT_NOT_SUPPORTED:
    //         error = CommandException.Error.REQUEST_NOT_SUPPORTED;
    //         break;
    //     case ImsReasonInfo.CODE_UT_CB_PASSWORD_MISMATCH:
    //         error = CommandException.Error.PASSWORD_INCORRECT;
    //         break;
    //     default:
    //         break;
    // }
    //
    // return new CommandException(error);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetCommandException()
    ///* [in] */ Throwable* e,
    ///* [out] */ ICommandException** result)
{
    //VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // CommandException ex = null;
    //
    // if (e instanceof ImsException) {
    //     ex = getCommandException(((ImsException)e).getCode());
    // } else {
    //     Rlog.d(LOGTAG, "getCommandException generic failure");
    //     ex = new CommandException(CommandException.Error.GENERIC_FAILURE);
    // }
    // return ex;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::OnIncomingUSSD(
    /* [in] */ Int32 ussdMode,
    /* [in] */ const String& ussdMessage)
{
    // ==================before translated======================
    // if (DBG) Rlog.d(LOGTAG, "onIncomingUSSD ussdMode=" + ussdMode);
    //
    // boolean isUssdError;
    // boolean isUssdRequest;
    //
    // isUssdRequest
    //     = (ussdMode == CommandsInterface.USSD_MODE_REQUEST);
    //
    // isUssdError
    //     = (ussdMode != CommandsInterface.USSD_MODE_NOTIFY
    //         && ussdMode != CommandsInterface.USSD_MODE_REQUEST);
    //
    // ImsPhoneMmiCode found = null;
    // for (int i = 0, s = mPendingMMIs.size() ; i < s; i++) {
    //     if(mPendingMMIs.get(i).isPendingUSSD()) {
    //         found = mPendingMMIs.get(i);
    //         break;
    //     }
    // }
    //
    // if (found != null) {
    //     // Complete pending USSD
    //     if (isUssdError) {
    //         found.onUssdFinishedError();
    //     } else {
    //         found.onUssdFinished(ussdMessage, isUssdRequest);
    //     }
    // } else { // pending USSD not found
    //     // The network may initiate its own USSD request
    //
    //     // ignore everything that isnt a Notify or a Request
    //     // also, discard if there is no message to present
    //     if (!isUssdError && ussdMessage != null) {
    //         ImsPhoneMmiCode mmi;
    //         mmi = ImsPhoneMmiCode.newNetworkInitiatedUssd(ussdMessage,
    //                 isUssdRequest,
    //                 ImsPhone.this);
    //         onNetworkInitiatedUssd(mmi);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::OnMMIDone(
    /* [in] */ IImsPhoneMmiCode* mmi)
{
    // ==================before translated======================
    // /* Only notify complete if it's on the pending list.
    //  * Otherwise, it's already been handled (eg, previously canceled).
    //  * The exception is cancellation of an incoming USSD-REQUEST, which is
    //  * not on the list.
    //  */
    // if (mPendingMMIs.remove(mmi) || mmi.isUssdRequest()) {
    //     mMmiCompleteRegistrants.notifyRegistrants(
    //             new AsyncResult(null, mmi, null));
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetHandoverConnection(
    /* [out] */ IArrayList/*<IConnection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ArrayList<Connection> connList = new ArrayList<Connection>();
    // // Add all foreground call connections
    // connList.addAll(getForegroundCall().mConnections);
    // // Add all background call connections
    // connList.addAll(getBackgroundCall().mConnections);
    // // Add all background call connections
    // connList.addAll(getRingingCall().mConnections);
    // if (connList.size() > 0) {
    //     return connList;
    // } else {
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::NotifySrvccState(
    /* [in] */ ICallSrvccState state)
{
    // ==================before translated======================
    // mCT.notifySrvccState(state);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::InitiateSilentRedial()
{
    // ==================before translated======================
    // String result = mLastDialString;
    // AsyncResult ar = new AsyncResult(null, result, null);
    // if (ar != null) {
    //     mSilentRedialRegistrants.notifyRegistrants(ar);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::RegisterForSilentRedial(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mSilentRedialRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::UnregisterForSilentRedial(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mSilentRedialRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mSsnRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mSsnRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDefaultPhone.getSubId();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDefaultPhone.getPhoneId();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetSubscriptionInfo(
    /* [out] */ ISubscription** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDefaultPhone.getSubscriptionInfo();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDefaultPhone.mIccRecords.get();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar = (AsyncResult) msg.obj;
    // Message onComplete;
    //
    // if (DBG) Rlog.d(LOGTAG, "handleMessage what=" + msg.what);
    // switch (msg.what) {
    //     case EVENT_SET_CALL_FORWARD_DONE:
    //         IccRecords r = getIccRecords();
    //         Cf cf = (Cf) ar.userObj;
    //         if (cf.mIsCfu && ar.exception == null && r != null) {
    //             r.setVoiceCallForwardingFlag(1, msg.arg1 == 1, cf.mSetCfNumber);
    //         }
    //         sendResponse(cf.mOnComplete, null, ar.exception);
    //         break;
    //
    //     case EVENT_GET_CALL_FORWARD_DONE:
    //         CallForwardInfo[] cfInfos = null;
    //         if (ar.exception == null) {
    //             cfInfos = handleCfQueryResult((ImsCallForwardInfo[])ar.result);
    //         }
    //         sendResponse((Message) ar.userObj, cfInfos, ar.exception);
    //         break;
    //
    //      case EVENT_SET_CALL_FORWARD_TIMER_DONE:
    //         Cf cft = (Cf) ar.userObj;
    //         sendResponse(cft.mOnComplete, null, ar.exception);
    //         break;
    //
    //      case EVENT_GET_CALL_BARRING_DONE:
    //      case EVENT_GET_CALL_WAITING_DONE:
    //         int[] ssInfos = null;
    //         if (ar.exception == null) {
    //             if (msg.what == EVENT_GET_CALL_BARRING_DONE) {
    //                 ssInfos = handleCbQueryResult((ImsSsInfo[])ar.result);
    //             } else if (msg.what == EVENT_GET_CALL_WAITING_DONE) {
    //                 ssInfos = handleCwQueryResult((ImsSsInfo[])ar.result);
    //             }
    //         }
    //         sendResponse((Message) ar.userObj, ssInfos, ar.exception);
    //         break;
    //
    //      case EVENT_SET_CALL_BARRING_DONE:
    //      case EVENT_SET_CALL_WAITING_DONE:
    //         sendResponse((Message) ar.userObj, null, ar.exception);
    //         break;
    //
    //      default:
    //          super.handleMessage(msg);
    //          break;
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.isInEmergencyCall();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::IsInEcm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsPhoneInEcmState;
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SendEmergencyCallbackModeChange()
{
    // ==================before translated======================
    // // Send an Intent
    // Intent intent = new Intent(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    // intent.putExtra(PhoneConstants.PHONE_IN_ECM_STATE, mIsPhoneInEcmState);
    // SubscriptionManager.putPhoneIdAndSubIdExtra(intent, getPhoneId());
    // ActivityManagerNative.broadcastStickyIntent(intent, null, UserHandle.USER_ALL);
    // if (DBG) Rlog.d(LOGTAG, "sendEmergencyCallbackModeChange");
    assert(0);
    return NOERROR;
}

ECode ImsPhone::ExitEmergencyCallbackMode()
{
    // ==================before translated======================
    // if (mWakeLock.isHeld()) {
    //     mWakeLock.release();
    // }
    // if (DBG) Rlog.d(LOGTAG, "exitEmergencyCallbackMode()");
    //
    // // Send a message which will invoke handleExitEmergencyCallbackMode
    // ImsEcbm ecbm;
    // try {
    //     ecbm = mCT.getEcbmInterface();
    //     ecbm.exitEmergencyCallbackMode();
    // } catch (ImsException e) {
    //     e.printStackTrace();
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::HandleTimerInEmergencyCallbackMode(
    /* [in] */ Int32 action)
{
    // ==================before translated======================
    // switch (action) {
    //     case CANCEL_ECM_TIMER:
    //         removeCallbacks(mExitEcmRunnable);
    //         if (mDefaultPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
    //             ((GSMPhone) mDefaultPhone).notifyEcbmTimerReset(Boolean.TRUE);
    //         } else { // Should be CDMA - also go here by default
    //             ((CDMAPhone) mDefaultPhone).notifyEcbmTimerReset(Boolean.TRUE);
    //         }
    //         break;
    //     case RESTART_ECM_TIMER:
    //         long delayInMillis = SystemProperties.getLong(
    //                 TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
    //         postDelayed(mExitEcmRunnable, delayInMillis);
    //         if (mDefaultPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
    //             ((GSMPhone) mDefaultPhone).notifyEcbmTimerReset(Boolean.FALSE);
    //         } else { // Should be CDMA - also go here by default
    //             ((CDMAPhone) mDefaultPhone).notifyEcbmTimerReset(Boolean.FALSE);
    //         }
    //         break;
    //     default:
    //         Rlog.e(LOGTAG, "handleTimerInEmergencyCallbackMode, unsupported action " + action);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhone::SetOnEcbModeExitResponse(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mEcmExitRespRegistrant = new Registrant(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode ImsPhone::UnsetOnEcbModeExitResponse(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mEcmExitRespRegistrant.clear();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::IsVolteEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.isVolteEnabled();
    assert(0);
    return NOERROR;
}

ECode ImsPhone::IsVtEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.isVtEnabled();
    assert(0);
    return NOERROR;
}

AutoPtr<IConnection> ImsPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras)
{
    // ==================before translated======================
    // boolean isConferenceUri = false;
    // boolean isSkipSchemaParsing = false;
    // if (extras != null) {
    //     isConferenceUri = extras.getBoolean(TelephonyProperties.EXTRA_DIAL_CONFERENCE_URI,
    //             false);
    //     isSkipSchemaParsing = extras.getBoolean(TelephonyProperties.EXTRA_SKIP_SCHEMA_PARSING,
    //             false);
    // }
    // String newDialString = dialString;
    // // Need to make sure dialString gets parsed properly
    // if (!isConferenceUri && !isSkipSchemaParsing) {
    //     newDialString = PhoneNumberUtils.stripSeparators(dialString);
    // }
    //
    // // handle in-call MMI first if applicable
    // if (handleInCallMmiCommands(newDialString)) {
    //     return null;
    // }
    //
    // String networkPortion = newDialString;
    // if (!ImsPhoneMmiCode.isScMatchesSuppServType(newDialString)) {
    //     // Only look at the Network portion for mmi
    //     networkPortion = PhoneNumberUtils.extractNetworkPortionAlt(newDialString);
    // }
    //
    // ImsPhoneMmiCode mmi =
    //         ImsPhoneMmiCode.newFromDialString(networkPortion, this);
    // if (DBG) Rlog.d(LOGTAG,
    //         "dialing w/ mmi '" + mmi + "'...");
    //
    // if (mmi == null) {
    //     return mCT.dial(dialString, videoState, extras);
    // } else if (mmi.isTemporaryModeCLIR()) {
    //     return mCT.dial(mmi.getDialingNumber(), mmi.getCLIRMode(), videoState, extras);
    // } else if (!mmi.isSupportedOverImsPhone()) {
    //     // If the mmi is not supported by IMS service,
    //     // try to initiate dialing with default phone
    //     throw new CallStateException(CS_FALLBACK);
    // } else {
    //     mPendingMMIs.add(mmi);
    //     mMmiRegistrants.notifyRegistrants(new AsyncResult(null, mmi, null));
    //     mmi.processCode();
    //
    //     return null;
    // }
    assert(0);
    AutoPtr<IConnection> empty;
    return empty;
}

Boolean ImsPhone::HandleCallDeflectionIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (dialString.length() > 1) {
    //     return false;
    // }
    //
    // if (getRingingCall().getState() != ImsPhoneCall.State.IDLE) {
    //     if (DBG) Rlog.d(LOGTAG, "MmiCode 0: rejectCall");
    //     try {
    //         mCT.rejectCall();
    //     } catch (CallStateException e) {
    //         if (DBG) Rlog.d(LOGTAG, "reject failed", e);
    //         notifySuppServiceFailed(Phone.SuppService.REJECT);
    //     }
    // } else if (getBackgroundCall().getState() != ImsPhoneCall.State.IDLE) {
    //     if (DBG) Rlog.d(LOGTAG, "MmiCode 0: hangupWaitingOrBackground");
    //     try {
    //         mCT.hangup(getBackgroundCall());
    //     } catch (CallStateException e) {
    //         if (DBG) Rlog.d(LOGTAG, "hangup failed", e);
    //     }
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

Boolean ImsPhone::HandleCallWaitingIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // int len = dialString.length();
    //
    // if (len > 2) {
    //     return false;
    // }
    //
    // ImsPhoneCall call = getForegroundCall();
    //
    // try {
    //     if (len > 1) {
    //         if (DBG) Rlog.d(LOGTAG, "not support 1X SEND");
    //         notifySuppServiceFailed(Phone.SuppService.HANGUP);
    //     } else {
    //         if (call.getState() != ImsPhoneCall.State.IDLE) {
    //             if (DBG) Rlog.d(LOGTAG, "MmiCode 1: hangup foreground");
    //             mCT.hangup(call);
    //         } else {
    //             if (DBG) Rlog.d(LOGTAG, "MmiCode 1: switchWaitingOrHoldingAndActive");
    //             mCT.switchWaitingOrHoldingAndActive();
    //         }
    //     }
    // } catch (CallStateException e) {
    //     if (DBG) Rlog.d(LOGTAG, "hangup failed", e);
    //     notifySuppServiceFailed(Phone.SuppService.HANGUP);
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

Boolean ImsPhone::HandleCallHoldIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // int len = dialString.length();
    //
    // if (len > 2) {
    //     return false;
    // }
    //
    // ImsPhoneCall call = getForegroundCall();
    //
    // if (len > 1) {
    //     if (DBG) Rlog.d(LOGTAG, "separate not supported");
    //     notifySuppServiceFailed(Phone.SuppService.SEPARATE);
    // } else {
    //     try {
    //         if (getRingingCall().getState() != ImsPhoneCall.State.IDLE) {
    //             if (DBG) Rlog.d(LOGTAG, "MmiCode 2: accept ringing call");
    //             mCT.acceptCall(ImsCallProfile.CALL_TYPE_VOICE);
    //         } else {
    //             if (DBG) Rlog.d(LOGTAG, "MmiCode 2: switchWaitingOrHoldingAndActive");
    //             mCT.switchWaitingOrHoldingAndActive();
    //         }
    //     } catch (CallStateException e) {
    //         if (DBG) Rlog.d(LOGTAG, "switch failed", e);
    //         notifySuppServiceFailed(Phone.SuppService.SWITCH);
    //     }
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

Boolean ImsPhone::HandleMultipartyIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (dialString.length() > 1) {
    //     return false;
    // }
    //
    // if (DBG) Rlog.d(LOGTAG, "MmiCode 3: merge calls");
    // conference();
    // return true;
    assert(0);
    return FALSE;
}

Boolean ImsPhone::HandleEctIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    //
    // int len = dialString.length();
    //
    // if (len != 1) {
    //     return false;
    // }
    //
    // if (DBG) Rlog.d(LOGTAG, "MmiCode 4: not support explicit call transfer");
    // notifySuppServiceFailed(Phone.SuppService.TRANSFER);
    // return true;
    assert(0);
    return FALSE;
}

Boolean ImsPhone::HandleCcbsIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (dialString.length() > 1) {
    //     return false;
    // }
    //
    // Rlog.i(LOGTAG, "MmiCode 5: CCBS not supported!");
    // // Treat it as an "unknown" service.
    // notifySuppServiceFailed(Phone.SuppService.UNKNOWN);
    // return true;
    assert(0);
    return FALSE;
}

Boolean ImsPhone::IsValidCommandInterfaceCFReason(
    /* [in] */ Int32 commandInterfaceCFReason)
{
    // ==================before translated======================
    // switch (commandInterfaceCFReason) {
    // case CF_REASON_UNCONDITIONAL:
    // case CF_REASON_BUSY:
    // case CF_REASON_NO_REPLY:
    // case CF_REASON_NOT_REACHABLE:
    // case CF_REASON_ALL:
    // case CF_REASON_ALL_CONDITIONAL:
    //     return true;
    // default:
    //     return false;
    // }
    assert(0);
    return FALSE;
}

Boolean ImsPhone::IsValidCommandInterfaceCFAction(
    /* [in] */ Int32 commandInterfaceCFAction)
{
    // ==================before translated======================
    // switch (commandInterfaceCFAction) {
    // case CF_ACTION_DISABLE:
    // case CF_ACTION_ENABLE:
    // case CF_ACTION_REGISTRATION:
    // case CF_ACTION_ERASURE:
    //     return true;
    // default:
    //     return false;
    // }
    assert(0);
    return FALSE;
}

Boolean ImsPhone::IsCfEnable(
    /* [in] */ Int32 action)
{
    // ==================before translated======================
    // return (action == CF_ACTION_ENABLE) || (action == CF_ACTION_REGISTRATION);
    assert(0);
    return FALSE;
}

Int32 ImsPhone::GetConditionFromCFReason(
    /* [in] */ Int32 reason)
{
    // ==================before translated======================
    // switch(reason) {
    //     case CF_REASON_UNCONDITIONAL: return ImsUtInterface.CDIV_CF_UNCONDITIONAL;
    //     case CF_REASON_BUSY: return ImsUtInterface.CDIV_CF_BUSY;
    //     case CF_REASON_NO_REPLY: return ImsUtInterface.CDIV_CF_NO_REPLY;
    //     case CF_REASON_NOT_REACHABLE: return ImsUtInterface.CDIV_CF_NOT_REACHABLE;
    //     case CF_REASON_ALL: return ImsUtInterface.CDIV_CF_ALL;
    //     case CF_REASON_ALL_CONDITIONAL: return ImsUtInterface.CDIV_CF_ALL_CONDITIONAL;
    //     default:
    //         break;
    // }
    //
    // return ImsUtInterface.INVALID;
    assert(0);
    return 0;
}

Int32 ImsPhone::GetCFReasonFromCondition(
    /* [in] */ Int32 condition)
{
    // ==================before translated======================
    // switch(condition) {
    //     case ImsUtInterface.CDIV_CF_UNCONDITIONAL: return CF_REASON_UNCONDITIONAL;
    //     case ImsUtInterface.CDIV_CF_BUSY: return CF_REASON_BUSY;
    //     case ImsUtInterface.CDIV_CF_NO_REPLY: return CF_REASON_NO_REPLY;
    //     case ImsUtInterface.CDIV_CF_NOT_REACHABLE: return CF_REASON_NOT_REACHABLE;
    //     case ImsUtInterface.CDIV_CF_ALL: return CF_REASON_ALL;
    //     case ImsUtInterface.CDIV_CF_ALL_CONDITIONAL: return CF_REASON_ALL_CONDITIONAL;
    //     default:
    //         break;
    // }
    //
    // return CF_REASON_NOT_REACHABLE;
    assert(0);
    return 0;
}

Int32 ImsPhone::GetActionFromCFAction(
    /* [in] */ Int32 action)
{
    // ==================before translated======================
    // switch(action) {
    //     case CF_ACTION_DISABLE: return ImsUtInterface.ACTION_DEACTIVATION;
    //     case CF_ACTION_ENABLE: return ImsUtInterface.ACTION_ACTIVATION;
    //     case CF_ACTION_ERASURE: return ImsUtInterface.ACTION_ERASURE;
    //     case CF_ACTION_REGISTRATION: return ImsUtInterface.ACTION_REGISTRATION;
    //     default:
    //         break;
    // }
    //
    // return ImsUtInterface.INVALID;
    assert(0);
    return 0;
}

Int32 ImsPhone::GetCBTypeFromFacility(
    /* [in] */ const String& facility)
{
    // ==================before translated======================
    // if (CB_FACILITY_BAOC.equals(facility)) {
    //     return ImsUtInterface.CB_BAOC;
    // } else if (CB_FACILITY_BAOIC.equals(facility)) {
    //     return ImsUtInterface.CB_BOIC;
    // } else if (CB_FACILITY_BAOICxH.equals(facility)) {
    //     return ImsUtInterface.CB_BOIC_EXHC;
    // } else if (CB_FACILITY_BAIC.equals(facility)) {
    //     return ImsUtInterface.CB_BAIC;
    // } else if (CB_FACILITY_BAICr.equals(facility)) {
    //     return ImsUtInterface.CB_BIC_WR;
    // } else if (CB_FACILITY_BA_ALL.equals(facility)) {
    //     return ImsUtInterface.CB_BA_ALL;
    // } else if (CB_FACILITY_BA_MO.equals(facility)) {
    //     return ImsUtInterface.CB_BA_MO;
    // } else if (CB_FACILITY_BA_MT.equals(facility)) {
    //     return ImsUtInterface.CB_BA_MT;
    // }
    //
    // return 0;
    assert(0);
    return 0;
}

void ImsPhone::OnNetworkInitiatedUssd(
    /* [in] */ IImsPhoneMmiCode* mmi)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "onNetworkInitiatedUssd");
    // mMmiCompleteRegistrants.notifyRegistrants(
    //     new AsyncResult(null, mmi, null));
    assert(0);
}

AutoPtr<ICallForwardInfo> ImsPhone::GetCallForwardInfo(
    /* [in] */ IImsCallForwardInfo* info)
{
    // ==================before translated======================
    // CallForwardInfo cfInfo = new CallForwardInfo();
    // cfInfo.status = info.mStatus;
    // cfInfo.reason = getCFReasonFromCondition(info.mCondition);
    // cfInfo.serviceClass = SERVICE_CLASS_VOICE;
    // cfInfo.toa = info.mToA;
    // cfInfo.number = info.mNumber;
    // cfInfo.timeSeconds = info.mTimeSeconds;
    // cfInfo.startHour = info.mStartHour;
    // cfInfo.startMinute = info.mStartMinute;
    // cfInfo.endHour = info.mEndHour;
    // cfInfo.endMinute = info.mEndMinute;
    // return cfInfo;
    assert(0);
    AutoPtr<ICallForwardInfo> empty;
    return empty;
}

AutoPtr<ArrayOf<ICallForwardInfo*> > ImsPhone::HandleCfQueryResult(
    /* [in] */ ArrayOf<IImsCallForwardInfo*>* infos)
{
    // ==================before translated======================
    // CallForwardInfo[] cfInfos = null;
    //
    // if (infos != null && infos.length != 0) {
    //     cfInfos = new CallForwardInfo[infos.length];
    // }
    //
    // IccRecords r = getIccRecords();
    // if (infos == null || infos.length == 0) {
    //     if (r != null) {
    //         // Assume the default is not active
    //         // Set unconditional CFF in SIM to false
    //         r.setVoiceCallForwardingFlag(1, false, null);
    //     }
    // } else {
    //     for (int i = 0, s = infos.length; i < s; i++) {
    //         if (infos[i].mCondition == ImsUtInterface.CDIV_CF_UNCONDITIONAL) {
    //             if (r != null) {
    //                 r.setVoiceCallForwardingFlag(1, (infos[i].mStatus == 1),
    //                     infos[i].mNumber);
    //             }
    //         }
    //         cfInfos[i] = getCallForwardInfo(infos[i]);
    //     }
    // }
    //
    // return cfInfos;
    assert(0);
    AutoPtr<ArrayOf<ICallForwardInfo*> > empty;
    return empty;
}

AutoPtr<ArrayOf<Int32> > ImsPhone::HandleCbQueryResult(
    /* [in] */ ArrayOf<IImsSsInfo*>* infos)
{
    // ==================before translated======================
    // int[] cbInfos = new int[1];
    // cbInfos[0] = SERVICE_CLASS_NONE;
    //
    // if (infos[0].mStatus == 1) {
    //     cbInfos[0] = SERVICE_CLASS_VOICE;
    // }
    //
    // return cbInfos;
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr<ArrayOf<Int32> > ImsPhone::HandleCwQueryResult(
    /* [in] */ ArrayOf<IImsSsInfo*>* infos)
{
    // ==================before translated======================
    // int[] cwInfos = new int[2];
    // cwInfos[0] = 0;
    //
    // if (infos[0].mStatus == 1) {
    //     cwInfos[0] = 1;
    //     cwInfos[1] = SERVICE_CLASS_VOICE;
    // }
    //
    // return cwInfos;
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

void ImsPhone::SendResponse(
    /* [in] */ IMessage* onComplete,
    /* [in] */ IInterface* result)
    ///* [in] */ Throwable* e)
{
    // ==================before translated======================
    // if (onComplete != null) {
    //     CommandException ex = null;
    //     ImsException imsEx = null;
    //     if (e != null) {
    //         if (e instanceof ImsException) {
    //             imsEx = (ImsException) e;
    //             AsyncResult.forMessage(onComplete, result, imsEx);
    //         } else {
    //             ex = getCommandException(e);
    //             AsyncResult.forMessage(onComplete, result, ex);
    //         }
    //     } else {
    //         AsyncResult.forMessage(onComplete, result, null);
    //     }
    //     onComplete.sendToTarget();
    // }
    assert(0);
}

void ImsPhone::HandleEnterEmergencyCallbackMode()
{
    // ==================before translated======================
    // if (DBG) {
    //     Rlog.d(LOGTAG, "handleEnterEmergencyCallbackMode,mIsPhoneInEcmState= "
    //             + mIsPhoneInEcmState);
    // }
    // // if phone is not in Ecm mode, and it's changed to Ecm mode
    // if (mIsPhoneInEcmState == false) {
    //     mIsPhoneInEcmState = true;
    //     // notify change
    //     sendEmergencyCallbackModeChange();
    //     setSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "true");
    //
    //     // Post this runnable so we will automatically exit
    //     // if no one invokes exitEmergencyCallbackMode() directly.
    //     long delayInMillis = SystemProperties.getLong(
    //             TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
    //     postDelayed(mExitEcmRunnable, delayInMillis);
    //     // We don't want to go to sleep while in Ecm
    //     mWakeLock.acquire();
    // }
    assert(0);
}

void ImsPhone::HandleExitEmergencyCallbackMode()
{
    // ==================before translated======================
    // if (DBG) {
    //     Rlog.d(LOGTAG, "handleExitEmergencyCallbackMode: mIsPhoneInEcmState = "
    //             + mIsPhoneInEcmState);
    // }
    // // Remove pending exit Ecm runnable, if any
    // removeCallbacks(mExitEcmRunnable);
    //
    // if (mEcmExitRespRegistrant != null) {
    //     mEcmExitRespRegistrant.notifyResult(Boolean.TRUE);
    // }
    //     if (mIsPhoneInEcmState) {
    //         mIsPhoneInEcmState = false;
    //         setSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    //     }
    //     // send an Intent
    //     sendEmergencyCallbackModeChange();
    assert(0);
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
