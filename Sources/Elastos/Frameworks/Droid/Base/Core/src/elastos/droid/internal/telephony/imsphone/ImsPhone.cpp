
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/ims/CImsCallForwardInfo.h"
#include "elastos/droid/ims/CImsSsInfo.h"
#include "elastos/droid/internal/telephony/Call.h"
#include "elastos/droid/internal/telephony/imsphone/CImsPhoneCallTracker.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhone.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneMmiCode.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Ims::CImsCallForwardInfo;
using Elastos::Droid::Ims::CImsSsInfo;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Ims::IImsSsInfo;
using Elastos::Droid::Internal::Telephony::Call;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::CallForwardInfo;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

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
}

ECode ImsPhone::InnerRunnable::Run()
{
    return mOwner->ExitEmergencyCallbackMode();
}

//=====================================================================
//                             ImsPhone::Cf
//=====================================================================
ImsPhone::Cf::Cf(
    /* [in] */ const String& cfNumber,
    /* [in] */ Boolean isCfu,
    /* [in] */ IMessage* onComplete)
    : mSetCfNumber(cfNumber)
    , mOnComplete(onComplete)
    , mIsCfu(isCfu)
{
}

//=====================================================================
//                 ImsPhone::InnerImsEcbmStateListener
//=====================================================================
//TODO CAR_INTERFACE_IMPL(ImsPhone::InnerImsEcbmStateListener, Object, IImsEcbmStateListener);

ImsPhone::InnerImsEcbmStateListener::InnerImsEcbmStateListener(
    /* [in] */ ImsPhone* owner)
    : mOwner(owner)
{
}

ECode ImsPhone::InnerImsEcbmStateListener::OnECBMEntered()
{
    if (DBG) Logger::D(LOGTAG, "onECBMEntered");
    mOwner->HandleEnterEmergencyCallbackMode();
    return NOERROR;
}

ECode ImsPhone::InnerImsEcbmStateListener::OnECBMExited()
{
    if (DBG) Logger::D(LOGTAG, "onECBMExited");
    mOwner->HandleExitEmergencyCallbackMode();
    return NOERROR;
}

//=====================================================================
//                               ImsPhone
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhone, ImsPhoneBase, IImsPhone);

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
    : mIsPhoneInEcmState(FALSE)
    , mIsVideoCapable(FALSE)
{
    CArrayList::New((IArrayList**)&mPendingMMIs);
    CServiceState::New((IServiceState**)&mSS);
    mSsnRegistrants = new RegistrantList();
    mSilentRedialRegistrants = new RegistrantList();
    mExitEcmRunnable = new InnerRunnable(this);
}

ECode ImsPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ IPhone* defaultPhone)
{
    ImsPhoneBase::constructor(String("ImsPhone"), context, notifier);

    mDefaultPhone = IPhoneBase::Probe(defaultPhone);
    CImsPhoneCallTracker::New(this, (IImsPhoneCallTracker**)&mCT);
    mSS->SetStateOff();

    mDefaultPhone->GetPhoneId(&mPhoneId);

    // This is needed to handle phone process crashes
    // Same property is used for both CDMA & IMS phone.
    SystemProperties::GetBoolean(
            ITelephonyProperties::PROPERTY_INECM_MODE, FALSE, &mIsPhoneInEcmState);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, LOGTAG, (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(FALSE);
    return NOERROR;
}

ECode ImsPhone::UpdateParentPhone(
    /* [in] */ IPhoneBase* parentPhone)
{
    // synchronization is managed at the PhoneBase scope (which calls this function)
    mDefaultPhone = parentPhone;
    mDefaultPhone->GetPhoneId(&mPhoneId);

    // When the parent phone is updated, we need to notify listeners of the cached video
    // capability.
    Logger::D(LOGTAG, "updateParentPhone - Notify video capability changed %d", mIsVideoCapable);
    NotifyForVideoCapabilityChanged(mIsVideoCapable);
    return NOERROR;
}

ECode ImsPhone::Dispose()
{
    Logger::D(LOGTAG, "dispose");
    // Nothing to dispose in PhoneBase
    //super.dispose();
    mPendingMMIs->Clear();
    mCT->Dispose();

    //Force all referenced classes to unregister their former registered events
    return NOERROR;
}

ECode ImsPhone::RemoveReferences()
{
    Logger::D(LOGTAG, "removeReferences");
    ImsPhoneBase::RemoveReferences();

    mCT = NULL;
    mSS = NULL;
    return NOERROR;
}

ECode ImsPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSS;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::SetServiceState(
    /* [in] */ Int32 state)
{
    mSS->SetState(state);
    return NOERROR;
}

ECode ImsPhone::GetCallTracker(
    /* [out] */ ICallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICallTracker::Probe(mCT);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::GetPendingMmiCodes(
    /* [out] */ IList/*<? extends ImsPhoneMmiCode>*/** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IList::Probe(mPendingMMIs);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    return ((ImsPhoneCallTracker*)mCT.Get())->AcceptCall(videoState);
}

ECode ImsPhone::DeflectCall(
    /* [in] */ const String& number)
{
    return ((ImsPhoneCallTracker*)mCT.Get())->DeflectCall(number);
}

ECode ImsPhone::RejectCall()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->RejectCall();
}

ECode ImsPhone::SwitchHoldingAndActive()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
}

ECode ImsPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ((ImsPhoneCallTracker*)mCT.Get())->CanConference(result);
}

ECode ImsPhone::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ((ImsPhoneCallTracker*)mCT.Get())->CanDial(result);
}

ECode ImsPhone::Conference()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->Conference();
}

ECode ImsPhone::ClearDisconnected()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->ClearDisconnected();
}

ECode ImsPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ((ImsPhoneCallTracker*)mCT.Get())->CanTransfer(result);
}

ECode ImsPhone::ExplicitCallTransfer()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->ExplicitCallTransfer();
}

ECode ImsPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(((ImsPhoneCallTracker*)mCT.Get())->mForegroundCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(((ImsPhoneCallTracker*)mCT.Get())->mBackgroundCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(((ImsPhoneCallTracker*)mCT.Get())->mRingingCall);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::NotifySuppSvcNotification(
    /* [in] */ ISuppServiceNotification* suppSvc)
{
    // Logger::D(LOGTAG, "notifySuppSvcNotification: suppSvc = " + suppSvc);

    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, suppSvc, NULL);
    mSsnRegistrants->NotifyRegistrants(ar);
    return NOERROR;
}

ECode ImsPhone::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (IsInCall(&b), !b) {
        *result = FALSE;
        return NOERROR;
    }

    if (dialString.IsEmpty()) {
        *result = FALSE;
        return NOERROR;
    }

    *result = FALSE;
    Char32 ch = dialString.GetChar(0);
    switch (ch) {
        case '0':
            *result = HandleCallDeflectionIncallSupplementaryService(
                    dialString);
            break;
        case '1':
            *result = HandleCallWaitingIncallSupplementaryService(
                    dialString);
            break;
        case '2':
            *result = HandleCallHoldIncallSupplementaryService(dialString);
            break;
        case '3':
            *result = HandleMultipartyIncallSupplementaryService(dialString);
            break;
        case '4':
            *result = HandleEctIncallSupplementaryService(dialString);
            break;
        case '5':
            *result = HandleCcbsIncallSupplementaryService(dialString);
            break;
        default:
            break;
    }

    return NOERROR;
}

ECode ImsPhone::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICall> fgc;
    GetForegroundCall((ICall**)&fgc);
    ICallState foregroundCallState;
    fgc->GetState(&foregroundCallState);

    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);
    ICallState backgroundCallState;
    bgc->GetState(&backgroundCallState);

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    ICallState ringingCallState;
    call->GetState(&ringingCallState);

// TODO: Need ICallState::isAlive
    // *result = (foregroundCallState.isAlive() ||
    //         backgroundCallState.isAlive() ||
    //         ringingCallState.isAlive());
    return NOERROR;
}

ECode ImsPhone::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    mDefaultPhone->NotifyNewRingingConnectionP(c);
    return NOERROR;
}

ECode ImsPhone::NotifyForVideoCapabilityChanged(
    /* [in] */ Boolean isVideoCapable)
{
    mIsVideoCapable = isVideoCapable;
    mDefaultPhone->NotifyForVideoCapabilityChanged(isVideoCapable);
    return NOERROR;
}

ECode ImsPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    return DialInternal(dialString, videoState, extras, result);
}

ECode ImsPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    return DialInternal(dialString, videoState, NULL, result);
}

ECode ImsPhone::AddParticipant(
    /* [in] */ const String& dialString)
{
    return mCT->AddParticipant(dialString);
}

ECode ImsPhone::SendDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), !b) {
        Logger::E(LOGTAG,
                "sendDtmf called with invalid character '%d'", c);
    }
    else {
        if (((ImsPhoneCallTracker*)mCT.Get())->mState ==  PhoneConstantsState_OFFHOOK) {
            ((ImsPhoneCallTracker*)mCT.Get())->SendDtmf(c);
        }
    }
    return NOERROR;
}

ECode ImsPhone::StartDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if ((PhoneNumberUtils::Is12Key(c, &b), !b) || (c >= 'A' && c <= 'D')) {
        Logger::E(LOGTAG,
                "startDtmf called with invalid character '%d'", c);
    }
    else {
        ((ImsPhoneCallTracker*)mCT.Get())->StartDtmf(c);
    }
    return NOERROR;
}

ECode ImsPhone::StopDtmf()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->StopDtmf();
}

ECode ImsPhone::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return CRegistrant::New(h, what, obj, (IRegistrant**)&mPostDialHandler);
}

ECode ImsPhone::NotifyIncomingRing()
{
    if (DBG) Logger::D(LOGTAG, "notifyIncomingRing");
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_CALL_RING, (IInterface*)(IObject*)ar, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode ImsPhone::SetMute(
    /* [in] */ Boolean muted)
{
    return ((ImsPhoneCallTracker*)mCT.Get())->SetMute(muted);
}

ECode ImsPhone::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    return ((ImsPhoneCallTracker*)mCT.Get())->SetUiTTYMode(uiTtyMode, onComplete);
}

ECode ImsPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ((ImsPhoneCallTracker*)mCT.Get())->GetMute(result);
}

ECode ImsPhone::GetState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((ImsPhoneCallTracker*)mCT.Get())->mState;
    return NOERROR;
}

ECode ImsPhone::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    if (DBG) Logger::D(LOGTAG, "getCallForwardingOption reason=%d", commandInterfaceCFReason);
    if (IsValidCommandInterfaceCFReason(commandInterfaceCFReason)) {
        if (DBG) Logger::D(LOGTAG, "requesting call forwarding query.");
        AutoPtr<IMessage> resp;
        ObtainMessage(EVENT_GET_CALL_FORWARD_DONE, onComplete, (IMessage**)&resp);

        // try {
// TODO: Need ImsUtInterface
        // ImsUtInterface ut = mCT->GetUtInterface();
        // ut->QueryCallForward(getConditionFromCFReason(commandInterfaceCFReason),NULL,resp);
        // } catch (ImsException e) {
        //     sendErrorResponse(onComplete, e);
        // }
    }
    else if (onComplete != NULL) {
        SendErrorResponse(onComplete);
    }
    return NOERROR;
}

ECode ImsPhone::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    if (DBG) Logger::D(LOGTAG, "setCallForwardingOption action=%d, reason=%d",
            commandInterfaceCFAction, commandInterfaceCFReason);
    if ((IsValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
            (IsValidCommandInterfaceCFReason(commandInterfaceCFReason))) {
        AutoPtr<IMessage> resp;
        AutoPtr<Cf> cf = new Cf(dialingNumber,
                (commandInterfaceCFReason == ICommandsInterface::CF_REASON_UNCONDITIONAL ? TRUE : FALSE),
                onComplete);
        ObtainMessage(EVENT_SET_CALL_FORWARD_DONE,
                IsCfEnable(commandInterfaceCFAction) ? 1 : 0, 0,
                (IInterface*)(IObject*)cf, (IMessage**)&resp);

        // try {
// TODO: Need ImsUtInterface
        // ImsUtInterface ut = mCT->GetUtInterface();
        // ut->UpdateCallForward(GetActionFromCFAction(commandInterfaceCFAction),
        //         GetConditionFromCFReason(commandInterfaceCFReason),
        //         dialingNumber,
        //         timerSeconds,
        //         onComplete);
         // } catch (ImsException e) {
         //    sendErrorResponse(onComplete, e);
         // }
    }
    else if (onComplete != NULL) {
        SendErrorResponse(onComplete);
    }
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
    if (DBG) Logger::D(LOGTAG, "setCallForwardingUncondTimerOption action=%d, reason=%d, startHour=%d, startMinute=%d, endHour=%d, endMinute=%d",
            commandInterfaceCFAction, commandInterfaceCFReason, startHour,
            startMinute, endHour, endMinute);

    if ((IsValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
            (IsValidCommandInterfaceCFReason(commandInterfaceCFReason))) {
        AutoPtr<IMessage> resp;
        AutoPtr<Cf> cf = new Cf(dialingNumber,
                (commandInterfaceCFReason == ICommandsInterface::CF_REASON_UNCONDITIONAL ? TRUE : FALSE),
                onComplete);
        ObtainMessage(EVENT_SET_CALL_FORWARD_TIMER_DONE,
                IsCfEnable(commandInterfaceCFAction) ? 1 : 0, 0,
                (IInterface*)(IObject*)cf, (IMessage**)&resp);

        // try {
// TODO: Need ImsUtInterface
        // ImsUtInterface ut = mCT.getUtInterface();
        // ut.updateCallForwardUncondTimer(startHour, startMinute, endHour,
        //         endMinute, getActionFromCFAction(commandInterfaceCFAction),
        //         getConditionFromCFReason(commandInterfaceCFReason),
        //         dialingNumber, resp);
         // } catch (ImsException e) {
         //    sendErrorResponse(onComplete, e);
         // }
    }
    else if (onComplete != NULL) {
        SendErrorResponse(onComplete);
    }
    return NOERROR;
}

ECode ImsPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    if (DBG) Logger::D(LOGTAG, "getCallWaiting");
    AutoPtr<IMessage> resp;
    ObtainMessage(EVENT_GET_CALL_WAITING_DONE, onComplete, (IMessage**)&resp);

    // try {
// TODO: Need ImsUtInterface
    // ImsUtInterface ut = mCT.getUtInterface();
    // ut.queryCallWaiting(resp);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    return NOERROR;
}

ECode ImsPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    if (DBG) Logger::D(LOGTAG, "setCallWaiting enable=%d", enable);
    AutoPtr<IMessage> resp;
    ObtainMessage(EVENT_SET_CALL_WAITING_DONE, onComplete, (IMessage**)&resp);

    // try {
// TODO: Need ImsUtInterface
    // ImsUtInterface ut = mCT.getUtInterface();
    // ut.updateCallWaiting(enable, resp);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    return NOERROR;
}

ECode ImsPhone::GetCallBarring(
    /* [in] */ const String& facility,
    /* [in] */ IMessage* onComplete)
{
    if (DBG) Logger::D(LOGTAG, "getCallBarring facility=%s", facility.string());
    AutoPtr<IMessage> resp;
    ObtainMessage(EVENT_GET_CALL_BARRING_DONE, onComplete, (IMessage**)&resp);

    // try {
// TODO: Need ImsUtInterface
    // ImsUtInterface ut = mCT.getUtInterface();
    // ut.queryCallBarring(getCBTypeFromFacility(facility), resp);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    return NOERROR;
}

ECode ImsPhone::SetCallBarring(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    if (DBG) Logger::D(LOGTAG, "setCallBarring facility=%s, lockState=%d",
            facility.string(), lockState);
    AutoPtr<IMessage> resp;
    ObtainMessage(EVENT_SET_CALL_BARRING_DONE, onComplete, (IMessage**)&resp);

    Int32 action;
    if (lockState) {
        action = ICommandsInterface::CF_ACTION_ENABLE;
    }
    else {
        action = ICommandsInterface::CF_ACTION_DISABLE;
    }

    // try {
// TODO: Need ImsUtInterface
    // ImsUtInterface ut = mCT.getUtInterface();
    // // password is not required with Ut interface
    // ut.updateCallBarring(getCBTypeFromFacility(facility), action, resp, NULL);
    // } catch (ImsException e) {
    //     sendErrorResponse(onComplete, e);
    // }
    return NOERROR;
}

ECode ImsPhone::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    Logger::D(LOGTAG, "sendUssdResponse");
    AutoPtr<IImsPhoneMmiCode> mmi = ImsPhoneMmiCode::NewFromUssdUserInput(ussdMessge, this);
    mPendingMMIs->Add(mmi);
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
    mMmiRegistrants->NotifyRegistrants(ar);
    ((ImsPhoneMmiCode*)mmi.Get())->SendUssd(ussdMessge);
    return NOERROR;
}

ECode ImsPhone::SendUSSD(
    /* [in] */ const String& ussdString,
    /* [in] */ IMessage* response)
{
    return ((ImsPhoneCallTracker*)mCT.Get())->SendUSSD(ussdString, response);
}

ECode ImsPhone::CancelUSSD()
{
    return ((ImsPhoneCallTracker*)mCT.Get())->CancelUSSD();
}

ECode ImsPhone::SendErrorResponse(
    /* [in] */ IMessage* onComplete)
{
    Logger::D(LOGTAG, "sendErrorResponse");
    if (onComplete != NULL) {
// TODO: Need CommandException
        // AsyncResult::ForMessage(onComplete, NULL,
        //         new CommandException(CommandException.Error.GENERIC_FAILURE));
        onComplete->SendToTarget();
    }
    return NOERROR;
}

ECode ImsPhone::SendErrorResponse(
   /* [in] */ IMessage* onComplete,
   /* [in] */ IThrowable* e)
{
   Logger::D(LOGTAG, "sendErrorResponse");
   if (onComplete != NULL) {
// TODO: Need CommandException
       // AsyncResult::ForMessage(onComplete, NULL, GetCommandException(e));
       onComplete->SendToTarget();
   }
   return NOERROR;
}

ECode ImsPhone::SendErrorResponse(
    /* [in] */ IMessage* onComplete,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    Int32 code;
    reasonInfo->GetCode(&code);
    Logger::D(LOGTAG, "sendErrorResponse reasonCode=%d", code);
    if (onComplete != NULL) {
// TODO: Need CommandException
        // AsyncResult::ForMessage(onComplete, NULL, getCommandException(reasonInfo.getCode()));
        onComplete->SendToTarget();
    }
    return NOERROR;
}

ECode ImsPhone::GetCommandException(
    /* [in] */ Int32 code,
    /* [out] */ ICommandException** result)
{
    VALIDATE_NOT_NULL(result);
    Logger::D(LOGTAG, "getCommandException code=%d", code);
// TODO: Need CommandExceptionError
    // CommandException.Error error = CommandException.Error.GENERIC_FAILURE;

    switch(code) {
        case IImsReasonInfo::CODE_UT_NOT_SUPPORTED:
// TODO: Need CommandExceptionError
            // error = CommandException.Error.REQUEST_NOT_SUPPORTED;
            break;
        case IImsReasonInfo::CODE_UT_CB_PASSWORD_MISMATCH:
// TODO: Need CommandExceptionError
            // error = CommandException.Error.PASSWORD_INCORRECT;
            break;
        default:
            break;
    }

// TODO: Need CommandException
    // *result = new CommandException(error);
    return NOERROR;
}

ECode ImsPhone::GetCommandException(
    /* [in] */ IThrowable* e,
    /* [out] */ ICommandException** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICommandException> ex;

// TODO: Need ImsException, CommandException
    // if (e instanceof ImsException) {
    //     ex = getCommandException(((ImsException)e).getCode());
    // } else {
    //     Logger::D(LOGTAG, "getCommandException generic failure");
    //     ex = new CommandException(CommandException.Error.GENERIC_FAILURE);
    // }
    *result = ex;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::OnIncomingUSSD(
    /* [in] */ Int32 ussdMode,
    /* [in] */ const String& ussdMessage)
{
    if (DBG) Logger::D(LOGTAG, "onIncomingUSSD ussdMode=%d", ussdMode);

    Boolean isUssdError;
    Boolean isUssdRequest;

    isUssdRequest
        = (ussdMode == ICommandsInterface::USSD_MODE_REQUEST);

    isUssdError
        = (ussdMode != ICommandsInterface::USSD_MODE_NOTIFY
            && ussdMode != ICommandsInterface::USSD_MODE_REQUEST);

    AutoPtr<IImsPhoneMmiCode> found;
    Int32 size;
    mPendingMMIs->GetSize(&size);
    for (Int32 i = 0, s = size ; i < s; i++) {
        AutoPtr<IInterface> obj;
        mPendingMMIs->Get(i, (IInterface**)&obj);
        Boolean b;
        IImsPhoneMmiCode::Probe(obj)->IsPendingUSSD(&b);
        if(b) {
            found = IImsPhoneMmiCode::Probe(obj);
            break;
        }
    }

    if (found != NULL) {
        // Complete pending USSD
        if (isUssdError) {
            ((ImsPhoneMmiCode*)found.Get())->OnUssdFinishedError();
        }
        else {
            ((ImsPhoneMmiCode*)found.Get())->OnUssdFinished(ussdMessage, isUssdRequest);
        }
    }
    else { // pending USSD not found
        // The network may initiate its own USSD request

        // ignore everything that isnt a Notify or a Request
        // also, discard if there is no message to present
        if (!isUssdError && ussdMessage != NULL) {
            AutoPtr<IImsPhoneMmiCode> mmi;
            mmi = ImsPhoneMmiCode::NewNetworkInitiatedUssd(ussdMessage,
                    isUssdRequest,
                    IImsPhone::Probe(this));
            OnNetworkInitiatedUssd(mmi);
        }
    }
    return NOERROR;
}

ECode ImsPhone::OnMMIDone(
    /* [in] */ IImsPhoneMmiCode* mmi)
{
    /* Only notify complete if it's on the pending list.
     * Otherwise, it's already been handled (eg, previously canceled).
     * The exception is cancellation of an incoming USSD-REQUEST, which is
     * not on the list.
     */
    Boolean b1;
    mPendingMMIs->Remove(mmi, &b1);
    Boolean b2;
    ((ImsPhoneMmiCode*)mmi)->IsUssdRequest(&b2);
    if (b1 || b2) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiCompleteRegistrants->NotifyRegistrants(ar);
    }
    return NOERROR;
}

ECode ImsPhone::GetHandoverConnection(
    /* [out] */ IArrayList/*<IConnection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayList> connList;
    CArrayList::New((IArrayList**)&connList);

    AutoPtr<ICall> fgc;
    GetForegroundCall((ICall**)&fgc);
    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);
    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);

    // Add all foreground call connections
    connList->AddAll(ICollection::Probe(((Call*)fgc.Get())->mConnections));
    // Add all background call connections
    connList->AddAll(ICollection::Probe(((Call*)bgc.Get())->mConnections));
    // Add all background call connections
    connList->AddAll(ICollection::Probe(((Call*)call.Get())->mConnections));

    Int32 size;
    connList->GetSize(&size);
    if (size > 0) {
        *result = connList;
    }
    else {
        *result = NULL;
    }
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::NotifySrvccState(
    /* [in] */ ICallSrvccState state)
{
    return ((ImsPhoneCallTracker*)mCT.Get())->NotifySrvccState(state);
}

ECode ImsPhone::InitiateSilentRedial()
{
    String result = mLastDialString;
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, CoreUtils::Convert(result), NULL);
    if (ar != NULL) {
        mSilentRedialRegistrants->NotifyRegistrants(ar);
    }
    return NOERROR;
}

ECode ImsPhone::RegisterForSilentRedial(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSilentRedialRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode ImsPhone::UnregisterForSilentRedial(
    /* [in] */ IHandler* h)
{
    mSilentRedialRegistrants->Remove(h);
    return NOERROR;
}

ECode ImsPhone::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSsnRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode ImsPhone::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    mSsnRegistrants->Remove(h);
    return NOERROR;
}

ECode ImsPhone::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    mDefaultPhone->GetSubId(result);
    return NOERROR;
}

ECode ImsPhone::GetPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    mDefaultPhone->GetPhoneId(result);
    return NOERROR;
}

ECode ImsPhone::GetSubscriptionInfo(
    /* [out] */ ISubscription** result)
{
    VALIDATE_NOT_NULL(result);
    mDefaultPhone->GetSubscriptionInfo(result);
    return NOERROR;
}

ECode ImsPhone::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    ((PhoneBase*)mDefaultPhone.Get())->mIccRecords->Get((IInterface**)&obj);
    *result = IIccRecords::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject*)obj.Get();
    AutoPtr<IMessage> onComplete;

    Int32 what;
    msg->GetWhat(&what);
    if (DBG) Logger::D(LOGTAG, "handleMessage what=%d", what);
    switch (what) {
        case EVENT_SET_CALL_FORWARD_DONE: {
            AutoPtr<IIccRecords> r;
            GetIccRecords((IIccRecords**)&r);
            AutoPtr<Cf> cf = (Cf*)IObject::Probe(ar->mUserObj);
            if (cf->mIsCfu && ar->mException == NULL && r != NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                r->SetVoiceCallForwardingFlag(1, arg1 == 1, cf->mSetCfNumber);
            }
            SendResponse(cf->mOnComplete, NULL, ar->mException);
            break;
        }
        case EVENT_GET_CALL_FORWARD_DONE: {
            AutoPtr<ArrayOf<ICallForwardInfo*> > cfInfos;
            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
                Int32 size;
                iArray->GetLength(&size);
                AutoPtr<ArrayOf<IImsCallForwardInfo* > > array =
                        ArrayOf<IImsCallForwardInfo* >::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    iArray->Get(i, (IInterface**)&obj);
                    array->Set(i, IImsCallForwardInfo::Probe(obj));
                }

                cfInfos = HandleCfQueryResult(array);
            }

            AutoPtr<IArrayOf> iArray;
            CArrayOf::New(EIID_ICallForwardInfo, cfInfos->GetLength(), (IArrayOf**)&iArray);
            for (Int32 i = 0; i < cfInfos->GetLength(); i++) {
                iArray->Set(i, (*cfInfos)[i]);
            }
            SendResponse(IMessage::Probe(ar->mUserObj), iArray, ar->mException);
            break;
        }
        case EVENT_SET_CALL_FORWARD_TIMER_DONE: {
           AutoPtr<Cf> cft = (Cf*)IObject::Probe(ar->mUserObj);
           SendResponse(cft->mOnComplete, NULL, ar->mException);
           break;
        }
        case EVENT_GET_CALL_BARRING_DONE:
        case EVENT_GET_CALL_WAITING_DONE: {
            AutoPtr<ArrayOf<Int32> > ssInfos;
            if (ar->mException == NULL) {
                if (what == EVENT_GET_CALL_BARRING_DONE) {
                    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
                    Int32 size;
                    iArray->GetLength(&size);
                    AutoPtr<ArrayOf<IImsSsInfo* > > array =
                            ArrayOf<IImsSsInfo* >::Alloc(size);
                    for (Int32 i = 0; i < size; i++) {
                        AutoPtr<IInterface> obj;
                        iArray->Get(i, (IInterface**)&obj);
                        array->Set(i, IImsSsInfo::Probe(obj));
                    }

                    ssInfos = HandleCbQueryResult(array);
                }
                else if (what == EVENT_GET_CALL_WAITING_DONE) {
                    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
                    Int32 size;
                    iArray->GetLength(&size);
                    AutoPtr<ArrayOf<IImsSsInfo* > > array =
                            ArrayOf<IImsSsInfo* >::Alloc(size);
                    for (Int32 i = 0; i < size; i++) {
                        AutoPtr<IInterface> obj;
                        iArray->Get(i, (IInterface**)&obj);
                        array->Set(i, IImsSsInfo::Probe(obj));
                    }

                    ssInfos = HandleCwQueryResult(array);
                }
            }

            AutoPtr<IArrayOf> iArray;
            CArrayOf::New(EIID_IInteger32, ssInfos->GetLength(), (IArrayOf**)&iArray);
            for (Int32 i = 0; i < ssInfos->GetLength(); i++) {
                iArray->Set(i, CoreUtils::Convert((*ssInfos)[i]));
            }
            SendResponse(IMessage::Probe(ar->mUserObj), iArray, ar->mException);
            break;
        }
        case EVENT_SET_CALL_BARRING_DONE:
        case EVENT_SET_CALL_WAITING_DONE:
            SendResponse(IMessage::Probe(ar->mUserObj), NULL, ar->mException);
            break;

        default:
            ImsPhoneBase::HandleMessage(msg);
            break;
    }
    return NOERROR;
}

ECode ImsPhone::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mCT->IsInEmergencyCall(result);
}

ECode ImsPhone::IsInEcm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsPhoneInEcmState;
    return NOERROR;
}

ECode ImsPhone::SendEmergencyCallbackModeChange()
{
    // Send an Intent
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED, (IIntent**)&intent);
    intent->PutExtra(IPhoneConstants::PHONE_IN_ECM_STATE, mIsPhoneInEcmState);
    Int32 id;
    GetPhoneId(&id);
    SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, id);
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
    if (DBG) Logger::D(LOGTAG, "sendEmergencyCallbackModeChange");
    return NOERROR;
}

ECode ImsPhone::ExitEmergencyCallbackMode()
{
    Boolean b;
    if (mWakeLock->IsHeld(&b), b) {
        mWakeLock->ReleaseLock();
    }
    if (DBG) Logger::D(LOGTAG, "exitEmergencyCallbackMode()");

    // Send a message which will invoke handleExitEmergencyCallbackMode
// TODO: Need IImsEcbm
    // AutoPtr<IImsEcbm> ecbm;
    // try {
    // ecbm = mCT->GetEcbmInterface();
    // ecbm->ExitEmergencyCallbackMode();
    // } catch (ImsException e) {
    //     e.printStackTrace();
    // }
    return NOERROR;
}

ECode ImsPhone::HandleTimerInEmergencyCallbackMode(
    /* [in] */ Int32 action)
{
    switch (action) {
        case CANCEL_ECM_TIMER: {
            RemoveCallbacks(mExitEcmRunnable);
            Int32 type;
            IPhone::Probe(mDefaultPhone)->GetPhoneType(&type);
            if (type == IPhoneConstants::PHONE_TYPE_GSM) {
                IGSMPhone::Probe(mDefaultPhone)->NotifyEcbmTimerReset(TRUE);
            }
            else { // Should be CDMA - also go here by default
                ICDMAPhone::Probe(mDefaultPhone)->NotifyEcbmTimerReset(TRUE);
            }
            break;
        }
        case RESTART_ECM_TIMER: {
            Int64 delayInMillis;
            SystemProperties::GetInt64(
                    ITelephonyProperties::PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE, &delayInMillis);
            Boolean b;
            PostDelayed(mExitEcmRunnable, delayInMillis, &b);
            Int32 type;
            IPhone::Probe(mDefaultPhone)->GetPhoneType(&type);
            if (type == IPhoneConstants::PHONE_TYPE_GSM) {
                IGSMPhone::Probe(mDefaultPhone)->NotifyEcbmTimerReset(FALSE);
            }
            else { // Should be CDMA - also go here by default
                ICDMAPhone::Probe(mDefaultPhone)->NotifyEcbmTimerReset(FALSE);
            }
            break;
        }
        default:
            Logger::E(LOGTAG, "handleTimerInEmergencyCallbackMode, unsupported action %d", action);
    }
    return NOERROR;
}

ECode ImsPhone::SetOnEcbModeExitResponse(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return CRegistrant::New(h, what, obj, (IRegistrant**)&mEcmExitRespRegistrant);
}

ECode ImsPhone::UnsetOnEcbModeExitResponse(
    /* [in] */ IHandler* h)
{
    mEcmExitRespRegistrant->Clear();
    return NOERROR;
}

ECode ImsPhone::IsVolteEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mCT->IsVolteEnabled(result);
}

ECode ImsPhone::IsVtEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mCT->IsVtEnabled(result);
}

ECode ImsPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Boolean isConferenceUri = FALSE;
    Boolean isSkipSchemaParsing = FALSE;
    if (extras != NULL) {
        extras->GetBoolean(ITelephonyProperties::EXTRA_DIAL_CONFERENCE_URI,
                FALSE, &isConferenceUri);
        extras->GetBoolean(ITelephonyProperties::EXTRA_SKIP_SCHEMA_PARSING,
                FALSE, &isSkipSchemaParsing);
    }
    String newDialString = dialString;
    // Need to make sure dialString gets parsed properly
    if (!isConferenceUri && !isSkipSchemaParsing) {
        PhoneNumberUtils::StripSeparators(dialString, &newDialString);
    }

    // handle in-call MMI first if applicable
    Boolean b;
    if (HandleInCallMmiCommands(newDialString, &b), b) {
        *result = NULL;
        return NOERROR;
    }

    String networkPortion = newDialString;
    if (!ImsPhoneMmiCode::IsScMatchesSuppServType(newDialString)) {
        // Only look at the Network portion for mmi
        PhoneNumberUtils::ExtractNetworkPortionAlt(newDialString, &networkPortion);
    }

    AutoPtr<IImsPhoneMmiCode> mmi = ImsPhoneMmiCode::NewFromDialString(networkPortion, this);
    // if (DBG) Logger::D(LOGTAG,
    //         "dialing w/ mmi '" + mmi + "'...");

    if (mmi == NULL) {
        return ((ImsPhoneCallTracker*)mCT.Get())->Dial(dialString, videoState, extras, result);
    }
    else if (((ImsPhoneMmiCode*)mmi.Get())->IsTemporaryModeCLIR(&b), b) {
        String str;
        ((ImsPhoneMmiCode*)mmi.Get())->GetDialingNumber(&str);
        Int32 mode;
        ((ImsPhoneMmiCode*)mmi.Get())->GetCLIRMode(&mode);
        return ((ImsPhoneCallTracker*)mCT.Get())->Dial(str, mode, videoState, extras, result);
    }
    else if (((ImsPhoneMmiCode*)mmi.Get())->IsSupportedOverImsPhone(&b), !b) {
        // If the mmi is not supported by IMS service,
        // try to initiate dialing with default phone
        // throw new CallStateException(CS_FALLBACK);
        return E_CALL_STATE_EXCEPTION;
    }
    else {
        mPendingMMIs->Add(mmi);
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
        mMmiRegistrants->NotifyRegistrants(ar);
        ((ImsPhoneMmiCode*)mmi.Get())->ProcessCode();

        *result = NULL;
    }
    return NOERROR;
}

Boolean ImsPhone::HandleCallDeflectionIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    if (dialString.GetLength() > 1) {
        return FALSE;
    }

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    ICallState ringingCallState;
    call->GetState(&ringingCallState);

    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);
    ICallState backgroundCallState;
    bgc->GetState(&backgroundCallState);

    if (ringingCallState != ICallState_IDLE) {
        if (DBG) Logger::D(LOGTAG, "MmiCode 0: rejectCall");
        // try {
        ((ImsPhoneCallTracker*)mCT.Get())->RejectCall();
        // } catch (CallStateException e) {
        //     if (DBG) Logger::D(LOGTAG, "reject failed", e);
        //     notifySuppServiceFailed(IPhoneSuppService_REJECT);
        // }
    }
    else if (backgroundCallState != ICallState_IDLE) {
        if (DBG) Logger::D(LOGTAG, "MmiCode 0: hangupWaitingOrBackground");
        // try {
        ((ImsPhoneCallTracker*)mCT.Get())->Hangup(IImsPhoneCall::Probe(bgc));
        // } catch (CallStateException e) {
        //     if (DBG) Logger::D(LOGTAG, "hangup failed", e);
        // }
    }

    return TRUE;
}

Boolean ImsPhone::HandleCallWaitingIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    Int32 len = dialString.GetLength();

    if (len > 2) {
        return FALSE;
    }

    AutoPtr<ICall> c;
    GetForegroundCall((ICall**)&c);
    AutoPtr<IImsPhoneCall> call = IImsPhoneCall::Probe(c);

    // try {
    if (len > 1) {
        if (DBG) Logger::D(LOGTAG, "not support 1X SEND");
        NotifySuppServiceFailed(IPhoneSuppService_HANGUP);
    }
    else {
        ICallState callState;
        ICall::Probe(call)->GetState(&callState);
        if (callState != ICallState_IDLE) {
            if (DBG) Logger::D(LOGTAG, "MmiCode 1: hangup foreground");
            ((ImsPhoneCallTracker*)mCT.Get())->Hangup(call);
        }
        else {
            if (DBG) Logger::D(LOGTAG, "MmiCode 1: switchWaitingOrHoldingAndActive");
            ((ImsPhoneCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
        }
    }
    // } catch (CallStateException e) {
    //     if (DBG) Logger::D(LOGTAG, "hangup failed", e);
    //     notifySuppServiceFailed(IPhoneSuppService_HANGUP);
    // }

    return TRUE;
}

Boolean ImsPhone::HandleCallHoldIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    Int32 len = dialString.GetLength();

    if (len > 2) {
        return FALSE;
    }

    AutoPtr<ICall> c;
    GetForegroundCall((ICall**)&c);
    AutoPtr<IImsPhoneCall> call = IImsPhoneCall::Probe(c);

    if (len > 1) {
        if (DBG) Logger::D(LOGTAG, "separate not supported");
        NotifySuppServiceFailed(IPhoneSuppService_SEPARATE);
    }
    else {
        // try {
        AutoPtr<ICall> ringCall;
        GetRingingCall((ICall**)&ringCall);
        ICallState ringingCallState;
        ringCall->GetState(&ringingCallState);

        if (ringingCallState != ICallState_IDLE) {
            if (DBG) Logger::D(LOGTAG, "MmiCode 2: accept ringing call");
            ((ImsPhoneCallTracker*)mCT.Get())->AcceptCall(IImsCallProfile::CALL_TYPE_VOICE);
        }
        else {
            if (DBG) Logger::D(LOGTAG, "MmiCode 2: switchWaitingOrHoldingAndActive");
            ((ImsPhoneCallTracker*)mCT.Get())->SwitchWaitingOrHoldingAndActive();
        }
        // } catch (CallStateException e) {
        //     if (DBG) Logger::D(LOGTAG, "switch failed", e);
        //     notifySuppServiceFailed(IPhoneSuppService_SWITCH);
        // }
    }

    return TRUE;
}

Boolean ImsPhone::HandleMultipartyIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    if (dialString.GetLength() > 1) {
        return FALSE;
    }

    if (DBG) Logger::D(LOGTAG, "MmiCode 3: merge calls");
    Conference();
    return TRUE;
}

Boolean ImsPhone::HandleEctIncallSupplementaryService(
    /* [in] */ const String& dialString)
{

    Int32 len = dialString.GetLength();

    if (len != 1) {
        return FALSE;
    }

    if (DBG) Logger::D(LOGTAG, "MmiCode 4: not support explicit call transfer");
    NotifySuppServiceFailed(IPhoneSuppService_TRANSFER);
    return TRUE;
}

Boolean ImsPhone::HandleCcbsIncallSupplementaryService(
    /* [in] */ const String& dialString)
{
    if (dialString.GetLength() > 1) {
        return FALSE;
    }

    Logger::I(LOGTAG, "MmiCode 5: CCBS not supported!");
    // Treat it as an "unknown" service.
    NotifySuppServiceFailed(IPhoneSuppService_UNKNOWN);
    return TRUE;
}

Boolean ImsPhone::IsValidCommandInterfaceCFReason(
    /* [in] */ Int32 commandInterfaceCFReason)
{
    switch (commandInterfaceCFReason) {
    case ICommandsInterface::CF_REASON_UNCONDITIONAL:
    case ICommandsInterface::CF_REASON_BUSY:
    case ICommandsInterface::CF_REASON_NO_REPLY:
    case ICommandsInterface::CF_REASON_NOT_REACHABLE:
    case ICommandsInterface::CF_REASON_ALL:
    case ICommandsInterface::CF_REASON_ALL_CONDITIONAL:
        return TRUE;
    default:
        return FALSE;
    }
}

Boolean ImsPhone::IsValidCommandInterfaceCFAction(
    /* [in] */ Int32 commandInterfaceCFAction)
{
    switch (commandInterfaceCFAction) {
    case ICommandsInterface::CF_ACTION_DISABLE:
    case ICommandsInterface::CF_ACTION_ENABLE:
    case ICommandsInterface::CF_ACTION_REGISTRATION:
    case ICommandsInterface::CF_ACTION_ERASURE:
        return TRUE;
    default:
        return FALSE;
    }
}

Boolean ImsPhone::IsCfEnable(
    /* [in] */ Int32 action)
{
    return (action == ICommandsInterface::CF_ACTION_ENABLE) || (action == ICommandsInterface::CF_ACTION_REGISTRATION);
}

Int32 ImsPhone::GetConditionFromCFReason(
    /* [in] */ Int32 reason)
{
// TODO: Need IImsUtInterface
    // switch(reason) {
    //     case ICommandsInterface::CF_REASON_UNCONDITIONAL: return IImsUtInterface::CDIV_CF_UNCONDITIONAL;
    //     case ICommandsInterface::CF_REASON_BUSY: return IImsUtInterface::CDIV_CF_BUSY;
    //     case ICommandsInterface::CF_REASON_NO_REPLY: return IImsUtInterface::CDIV_CF_NO_REPLY;
    //     case ICommandsInterface::CF_REASON_NOT_REACHABLE: return IImsUtInterface::CDIV_CF_NOT_REACHABLE;
    //     case ICommandsInterface::CF_REASON_ALL: return IImsUtInterface::CDIV_CF_ALL;
    //     case ICommandsInterface::CF_REASON_ALL_CONDITIONAL: return IImsUtInterface::CDIV_CF_ALL_CONDITIONAL;
    //     default:
    //         break;
    // }
    //
    // return IImsUtInterface::INVALID;
    return 0;
}

Int32 ImsPhone::GetCFReasonFromCondition(
    /* [in] */ Int32 condition)
{
// TODO: Need IImsUtInterface
    // switch(condition) {
    //     case IImsUtInterface::CDIV_CF_UNCONDITIONAL: return ICommandsInterface::CF_REASON_UNCONDITIONAL;
    //     case IImsUtInterface::CDIV_CF_BUSY: return ICommandsInterface::CF_REASON_BUSY;
    //     case IImsUtInterface::CDIV_CF_NO_REPLY: return ICommandsInterface::CF_REASON_NO_REPLY;
    //     case IImsUtInterface::CDIV_CF_NOT_REACHABLE: return ICommandsInterface::CF_REASON_NOT_REACHABLE;
    //     case IImsUtInterface::CDIV_CF_ALL: return ICommandsInterface::CF_REASON_ALL;
    //     case IImsUtInterface::CDIV_CF_ALL_CONDITIONAL: return ICommandsInterface::CF_REASON_ALL_CONDITIONAL;
    //     default:
    //         break;
    // }

    // return ICommandsInterface::CF_REASON_NOT_REACHABLE;
    return 0;
}

Int32 ImsPhone::GetActionFromCFAction(
    /* [in] */ Int32 action)
{
// TODO: Need IImsUtInterface
    // switch(action) {
    //     case ICommandsInterface::CF_ACTION_DISABLE: return IImsUtInterface::ACTION_DEACTIVATION;
    //     case ICommandsInterface::CF_ACTION_ENABLE: return IImsUtInterface::ACTION_ACTIVATION;
    //     case ICommandsInterface::CF_ACTION_ERASURE: return IImsUtInterface::ACTION_ERASURE;
    //     case ICommandsInterface::CF_ACTION_REGISTRATION: return IImsUtInterface::ACTION_REGISTRATION;
    //     default:
    //         break;
    // }
    //
    // return IImsUtInterface::INVALID;
    return 0;
}

Int32 ImsPhone::GetCBTypeFromFacility(
    /* [in] */ const String& facility)
{
// TODO: Need IImsUtInterface
    // if (CB_FACILITY_BAOC.equals(facility)) {
    //     return IImsUtInterface::CB_BAOC;
    // } else if (CB_FACILITY_BAOIC.equals(facility)) {
    //     return IImsUtInterface::CB_BOIC;
    // } else if (CB_FACILITY_BAOICxH.equals(facility)) {
    //     return IImsUtInterface::CB_BOIC_EXHC;
    // } else if (CB_FACILITY_BAIC.equals(facility)) {
    //     return IImsUtInterface::CB_BAIC;
    // } else if (CB_FACILITY_BAICr.equals(facility)) {
    //     return IImsUtInterface::CB_BIC_WR;
    // } else if (CB_FACILITY_BA_ALL.equals(facility)) {
    //     return IImsUtInterface::CB_BA_ALL;
    // } else if (CB_FACILITY_BA_MO.equals(facility)) {
    //     return IImsUtInterface::CB_BA_MO;
    // } else if (CB_FACILITY_BA_MT.equals(facility)) {
    //     return IImsUtInterface::CB_BA_MT;
    // }
    //
    // return 0;
    return 0;
}

void ImsPhone::OnNetworkInitiatedUssd(
    /* [in] */ IImsPhoneMmiCode* mmi)
{
    Logger::D(LOGTAG, "onNetworkInitiatedUssd");
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mmi, NULL);
    mMmiCompleteRegistrants->NotifyRegistrants(ar);
}

AutoPtr<ICallForwardInfo> ImsPhone::GetCallForwardInfo(
    /* [in] */ IImsCallForwardInfo* _info)
{
    AutoPtr<CallForwardInfo> cfInfo = new CallForwardInfo();

    AutoPtr<CImsCallForwardInfo> info = (CImsCallForwardInfo*)_info;

    cfInfo->mStatus = info->mStatus;
    cfInfo->mReason = GetCFReasonFromCondition(info->mCondition);
    cfInfo->mServiceClass = ICommandsInterface::SERVICE_CLASS_VOICE;
    cfInfo->mToa = info->mToA;
    cfInfo->mNumber = info->mNumber;
    cfInfo->mTimeSeconds = info->mTimeSeconds;
    cfInfo->mStartHour = info->mStartHour;
    cfInfo->mStartMinute = info->mStartMinute;
    cfInfo->mEndHour = info->mEndHour;
    cfInfo->mEndMinute = info->mEndMinute;

    return ICallForwardInfo::Probe(cfInfo);
}

AutoPtr<ArrayOf<ICallForwardInfo*> > ImsPhone::HandleCfQueryResult(
    /* [in] */ ArrayOf<IImsCallForwardInfo*>* infos)
{
    AutoPtr<ArrayOf<ICallForwardInfo*> > cfInfos;

    if (infos != NULL && infos->GetLength() != 0) {
        cfInfos = ArrayOf<ICallForwardInfo*>::Alloc(infos->GetLength());
    }

    AutoPtr<IIccRecords> r;
    GetIccRecords((IIccRecords**)&r);
    if (infos == NULL || infos->GetLength() == 0) {
        if (r != NULL) {
            // Assume the default is not active
            // Set unconditional CFF in SIM to FALSE
            r->SetVoiceCallForwardingFlag(1, FALSE, String(NULL));
        }
    }
    else {
        for (Int32 i = 0, s = infos->GetLength(); i < s; i++) {
            AutoPtr<CImsCallForwardInfo> info = (CImsCallForwardInfo*)(*infos)[i];
// TODO: Need IImsUtInterface
            // if (info->mCondition == IImsUtInterface::CDIV_CF_UNCONDITIONAL) {
            //     if (r != NULL) {
            //         r->SetVoiceCallForwardingFlag(1, (info->mStatus == 1),
            //             info->mNumber);
            //     }
            // }
            (*cfInfos)[i] = GetCallForwardInfo((*infos)[i]);
        }
    }

    return cfInfos;
}

AutoPtr<ArrayOf<Int32> > ImsPhone::HandleCbQueryResult(
    /* [in] */ ArrayOf<IImsSsInfo*>* infos)
{
    AutoPtr< ArrayOf<Int32> > cbInfos = ArrayOf<Int32>::Alloc(1);
    (*cbInfos)[0] = ICommandsInterface::SERVICE_CLASS_NONE;

    AutoPtr<CImsSsInfo> info = (CImsSsInfo*)(*infos)[0];
    if (info->mStatus == 1) {
        (*cbInfos)[0] = ICommandsInterface::SERVICE_CLASS_VOICE;
    }

    return cbInfos;
}

AutoPtr<ArrayOf<Int32> > ImsPhone::HandleCwQueryResult(
    /* [in] */ ArrayOf<IImsSsInfo*>* infos)
{
    AutoPtr< ArrayOf<Int32> > cwInfos = ArrayOf<Int32>::Alloc(2);
    (*cwInfos)[0] = 0;

    AutoPtr<CImsSsInfo> info = (CImsSsInfo*)(*infos)[0];
    if (info->mStatus == 1) {
        (*cwInfos)[0] = 1;
        (*cwInfos)[1] = ICommandsInterface::SERVICE_CLASS_VOICE;
    }

    return cwInfos;
}

void ImsPhone::SendResponse(
    /* [in] */ IMessage* onComplete,
    /* [in] */ IInterface* result,
    /* [in] */ IThrowable* e)
{
    if (onComplete != NULL) {
        AutoPtr<ICommandException> ex;
// TODO: Need IImsException
        // AutoPtr<IImsException> imsEx;
        if (e != NULL) {
            // if (e instanceof ImsException) {
            //     imsEx = (ImsException) e;
            //     AsyncResult::ForMessage(onComplete, result, imsEx);
            // }
            // else {
            //     GetCommandException(e, (ICommandException**)&ex);
            //     AsyncResult::ForMessage(onComplete, result, ex);
            // }
        }
        else {
            AsyncResult::ForMessage(onComplete, result, NULL);
        }
        onComplete->SendToTarget();
    }
}

void ImsPhone::HandleEnterEmergencyCallbackMode()
{
    if (DBG) {
        Logger::D(LOGTAG, "handleEnterEmergencyCallbackMode,mIsPhoneInEcmState= %d"
                , mIsPhoneInEcmState);
    }
    // if phone is not in Ecm mode, and it's changed to Ecm mode
    if (mIsPhoneInEcmState == FALSE) {
        mIsPhoneInEcmState = TRUE;
        // notify change
        SendEmergencyCallbackModeChange();
        SetSystemProperty(ITelephonyProperties::PROPERTY_INECM_MODE, String("TRUE"));

        // Post this runnable so we will automatically exit
        // if no one invokes exitEmergencyCallbackMode() directly.
        Int64 delayInMillis;
        SystemProperties::GetInt64(
                ITelephonyProperties::PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE,
                &delayInMillis);
        Boolean b;
        PostDelayed(mExitEcmRunnable, delayInMillis, &b);
        // We don't want to go to sleep while in Ecm
        mWakeLock->AcquireLock();
    }
}

void ImsPhone::HandleExitEmergencyCallbackMode()
{
    if (DBG) {
        Logger::D(LOGTAG, "handleExitEmergencyCallbackMode: mIsPhoneInEcmState = %d"
                , mIsPhoneInEcmState);
    }
    // Remove pending exit Ecm runnable, if any
    RemoveCallbacks(mExitEcmRunnable);

    if (mEcmExitRespRegistrant != NULL) {
        mEcmExitRespRegistrant->NotifyResult(CoreUtils::Convert(TRUE));
    }
        if (mIsPhoneInEcmState) {
            mIsPhoneInEcmState = FALSE;
            SetSystemProperty(ITelephonyProperties::PROPERTY_INECM_MODE, String("FALSE"));
        }
        // send an Intent
        SendEmergencyCallbackModeChange();
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
