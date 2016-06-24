#include "elastos/droid/internal/telephony/gsm/CGsmCall.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCallTracker.h"
#include "elastos/droid/internal/telephony/gsm/CGsmConnection.h"
#include "elastos/droid/internal/telephony/gsm/CGSMPhone.h"
#include "elastos/droid/internal/telephony/gsm/CGsmServiceStateTracker.h"
#include "elastos/droid/internal/telephony/DriverCall.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::DriverCall;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CGsmConnection::TAG("GsmConnection");
const Boolean CGsmConnection::DBG;

CAR_INTERFACE_IMPL(CGsmConnection, Connection, IGsmConnection)

CAR_OBJECT_IMPL(CGsmConnection)

CGsmConnection::MyHandler::MyHandler(
    /* [in] */ ILooper* l,
    /* [in] */ CGsmConnection* host)
    : Handler(l)
    , mHost(host)
{
}

ECode CGsmConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_NEXT_POST_DIAL:
        case EVENT_DTMF_DONE:
        case EVENT_PAUSE_DONE:
            mHost->ProcessNextPostDialChar();
            break;
        case EVENT_WAKE_LOCK_TIMEOUT:
            mHost->ReleaseWakeLock();
            break;
    }
    return NOERROR;
}

CGsmConnection::CGsmConnection()
    : mDisconnected(FALSE)
    , mIndex(0)
    , mDisconnectTime(0)
    , mNextPostDialChar(0)
    , mCause(IDisconnectCause::NOT_DISCONNECTED)
    , mPostDialState(PostDialState_NOT_STARTED)
    , mPreciseCause(0)
{
}

ECode CGsmConnection::constructor()
{
    return NOERROR;
}

ECode CGsmConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDriverCall* dc,
    /* [in] */ IGsmCallTracker* ct,
    /* [in] */ Int32 index)
{
    CreateWakeLock(context);
    AcquireWakeLock();

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    mAddress = ((DriverCall*)dc)->mNumber;

    mIsIncoming = ((DriverCall*)dc)->mIsMT;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);
    mCnapName = ((DriverCall*)dc)->mName;
    mCnapNamePresentation = ((DriverCall*)dc)->mNamePresentation;
    mNumberPresentation = ((DriverCall*)dc)->mNumberPresentation;
    mUusInfo = ((DriverCall*)dc)->mUusInfo;

    mIndex = index;

    mParent = ParentFromDCState(((DriverCall*)dc)->mState);
    mParent->Attach(this, dc);
    return NOERROR;
}

ECode CGsmConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString,
    /* [in] */ IGsmCallTracker* ct,
    /* [in] */ IGsmCall* parent)
{
    CreateWakeLock(context);
    AcquireWakeLock();

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    mDialString = dialString;

    PhoneNumberUtils::ExtractNetworkPortionAlt(dialString, &mAddress);
    PhoneNumberUtils::ExtractPostDialPortion(dialString, &mPostDialString);

    mIndex = -1;

    mIsIncoming = FALSE;
    mCnapName = NULL;
    mCnapNamePresentation = IPhoneConstants::PRESENTATION_ALLOWED;
    mNumberPresentation = IPhoneConstants::PRESENTATION_ALLOWED;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);

    mParent = parent;
    parent->AttachFake(this, ICallState_DIALING);
    return NOERROR;
}

ECode CGsmConnection::Dispose()
{
    return NOERROR;
}

ECode CGsmConnection::EqualsHandlesNulls(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (a == NULL) {
        *result = b == NULL;
    }
    else {
        IObject::Probe(a)->Equals(b, result);
    }
    return NOERROR;
}

ECode CGsmConnection::CompareTo(
    /* [in] */ IDriverCall* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // On mobile originated (MO) calls, the phone number may have changed
    // due to a SIM Toolkit call control modification.
    //
    // We assume we know when MO calls are created (since we created them)
    // and therefore don't need to compare the phone number anyway.
    if (! (mIsIncoming || ((DriverCall*)c)->mIsMT)) return true;

    // A new call appearing by SRVCC may have invalid number
    //  if IMS service is not tightly coupled with cellular modem stack.
    // Thus we prefer the preexisting handover connection instance.
    if (mOrigConnection != NULL) return true;

    // ... but we can compare phone numbers on MT calls, and we have
    // no control over when they begin, so we might as well

    String cAddress;
    PhoneNumberUtils::StringFromStringAndTOA(((DriverCall*)c)->mNumber,
            ((DriverCall*)c)->mTOA, &cAddress);
    Boolean b;
    *result = mIsIncoming == ((DriverCall*)c)->mIsMT &&
            (EqualsHandlesNulls(CoreUtils::Convert(mAddress), CoreUtils::Convert(cAddress), &b), b);
    return NOERROR;
}

ECode CGsmConnection::GetCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ICall::Probe(mParent);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmConnection::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectTime;
    return NOERROR;
}

ECode CGsmConnection::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    ICallState state;
    GetState(&state);
    if (state != ICallState_HOLDING) {
        // If not holding, return 0
        *result = 0;
    }
    else {
        *result = SystemClock::GetElapsedRealtime() - mHoldingStartTime;
    }
    return NOERROR;
}

ECode CGsmConnection::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCause;
    return NOERROR;
}

ECode CGsmConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result)
    if (mDisconnected) {
        *result = ICallState_DISCONNECTED;
    } else {
        Connection::GetState(result);
    }
    return NOERROR;
}

ECode CGsmConnection::Hangup()
{
    if (!mDisconnected) {
        ((CGsmCallTracker*)mOwner.Get())->Hangup(this);
    }
    else {
        // throw new CallStateException ("disconnected");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CGsmConnection::Separate()
{
    if (!mDisconnected) {
        ((CGsmCallTracker*)mOwner.Get())->Separate(this);
    }
    else {
        // throw new CallStateException ("disconnected");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CGsmConnection::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPostDialState;
    return NOERROR;
}

ECode CGsmConnection::ProceedAfterWaitChar()
{
    if (mPostDialState != PostDialState_WAIT) {
        Logger::W(TAG, "GsmConnection.proceedAfterWaitChar(): Expected getPostDialState() to be WAIT but was %d", mPostDialState);
        return NOERROR;
    }

    SetPostDialState(PostDialState_STARTED);

    ProcessNextPostDialChar();
    return NOERROR;
}

ECode CGsmConnection::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    if (mPostDialState != PostDialState_WILD) {
        Logger::W(TAG, "GsmConnection.proceedAfterWaitChar(): Expected  getPostDialState() to be WILD but was %d", mPostDialState);
        return NOERROR;
    }

    SetPostDialState(PostDialState_STARTED);

    // make a new postDialString, with the wild char replacement string
    // at the beginning, followed by the remaining postDialString.

    StringBuilder buf(str);
    buf.Append(mPostDialString.Substring(mNextPostDialChar));
    buf.ToString(&mPostDialString);
    mNextPostDialChar = 0;
    if (IPhone::DEBUG_PHONE) {
        Log(String("proceedAfterWildChar: new postDialString is ") +
                mPostDialString);
    }

    ProcessNextPostDialChar();
    return NOERROR;
}

ECode CGsmConnection::CancelPostDial()
{
    SetPostDialState(PostDialState_CANCELLED);
    return NOERROR;
}

ECode CGsmConnection::OnHangupLocal()
{
    mCause = IDisconnectCause::LOCAL;
    mPreciseCause = 0;
    return NOERROR;
}

ECode CGsmConnection::DisconnectCauseFromCode(
    /* [in] */ Int32 causeCode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    /**
     * See 22.001 Annex F.4 for mapping of cause codes
     * to local tones
     */

    switch (causeCode) {
        case ICallFailCause::USER_BUSY:
            *result = IDisconnectCause::BUSY;
            return NOERROR;

        case ICallFailCause::NO_CIRCUIT_AVAIL:
        case ICallFailCause::TEMPORARY_FAILURE:
        case ICallFailCause::SWITCHING_CONGESTION:
        case ICallFailCause::CHANNEL_NOT_AVAIL:
        case ICallFailCause::QOS_NOT_AVAIL:
        case ICallFailCause::BEARER_NOT_AVAIL:
            *result = IDisconnectCause::CONGESTION;
            return NOERROR;

        case ICallFailCause::ACM_LIMIT_EXCEEDED:
            *result = IDisconnectCause::LIMIT_EXCEEDED;
            return NOERROR;

        case ICallFailCause::CALL_BARRED:
            *result = IDisconnectCause::CALL_BARRED;
            return NOERROR;

        case ICallFailCause::FDN_BLOCKED:
            *result = IDisconnectCause::FDN_BLOCKED;
            return NOERROR;

        case ICallFailCause::UNOBTAINABLE_NUMBER:
            *result = IDisconnectCause::UNOBTAINABLE_NUMBER;
            return NOERROR;

        case ICallFailCause::DIAL_MODIFIED_TO_USSD:
            *result = IDisconnectCause::DIAL_MODIFIED_TO_USSD;
            return NOERROR;

        case ICallFailCause::DIAL_MODIFIED_TO_SS:
            *result = IDisconnectCause::DIAL_MODIFIED_TO_SS;
            return NOERROR;

        case ICallFailCause::DIAL_MODIFIED_TO_DIAL:
            *result = IDisconnectCause::DIAL_MODIFIED_TO_DIAL;
            return NOERROR;

        case ICallFailCause::ERROR_UNSPECIFIED:
        case ICallFailCause::NORMAL_CLEARING:
        default:
            AutoPtr<IGSMPhone> phone = ((CGsmCallTracker*)mOwner.Get())->mPhone;
            AutoPtr<IServiceState> ss;
            IPhone::Probe(phone)->GetServiceState((IServiceState**)&ss);
            Int32 serviceState;
            ss->GetState(&serviceState);
            AutoPtr<IUiccCardApplication> cardApp;
            cardApp = ((CGSMPhone*)phone.Get())->GetUiccCardApplication();
            AppState uiccAppState;
            if (cardApp != NULL) {
                cardApp->GetState(&uiccAppState);
            }
            else {
                uiccAppState = Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_UNKNOWN;
            }
            if (serviceState == IServiceState::STATE_POWER_OFF) {
                *result = IDisconnectCause::POWER_OFF;
                return NOERROR;
            }
            else if (serviceState == IServiceState::STATE_OUT_OF_SERVICE
                    || serviceState == IServiceState::STATE_EMERGENCY_ONLY ) {
                *result = IDisconnectCause::OUT_OF_SERVICE;
                return NOERROR;
            }
            else if (uiccAppState != Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_READY) {
                *result = IDisconnectCause::ICC_ERROR;
                return NOERROR;
            }
            else if (causeCode == ICallFailCause::ERROR_UNSPECIFIED) {
                AutoPtr<IRestrictedState> rs = ((CGsmServiceStateTracker*)((CGSMPhone*)phone.Get())->mSST.Get())->mRestrictedState;
                Boolean b;
                if (rs->IsCsRestricted(&b), b) {
                    *result = IDisconnectCause::CS_RESTRICTED;
                    return NOERROR;
                }
                else if (rs->IsCsEmergencyRestricted(&b), b) {
                    *result = IDisconnectCause::CS_RESTRICTED_EMERGENCY;
                    return NOERROR;
                }
                else if (rs->IsCsNormalRestricted(&b), b) {
                    *result = IDisconnectCause::CS_RESTRICTED_NORMAL;
                    return NOERROR;
                }
                else {
                    *result = IDisconnectCause::ERROR_UNSPECIFIED;
                    return NOERROR;
                }
            }
            else if (causeCode == ICallFailCause::NORMAL_CLEARING) {
                *result = IDisconnectCause::NORMAL;
                return NOERROR;
            }
            else {
                // If nothing else matches, report unknown call drop reason
                // to app, not NORMAL call end.
                *result = IDisconnectCause::ERROR_UNSPECIFIED;
                return NOERROR;
            }
    }
    return NOERROR;
}

ECode CGsmConnection::OnRemoteDisconnect(
/* [in] */ Int32 causeCode)
{
    this->mPreciseCause = causeCode;
    Int32 val;
    DisconnectCauseFromCode(causeCode, &val);
    Boolean b;
    OnDisconnect(val, &b);
    return NOERROR;
}

ECode CGsmConnection::OnDisconnect(
    /* [in] */ Int32 cause,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean changed = FALSE;

    mCause = cause;

    if (!mDisconnected) {
        mIndex = -1;

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mDisconnectTime);
        mDuration = SystemClock::GetElapsedRealtime() - mConnectTimeReal;
        mDisconnected = TRUE;

        if (DBG) Logger::D(TAG, "onDisconnect: cause=%d", cause);

        ((CGSMPhone*)((CGsmCallTracker*)mOwner.Get())->mPhone.Get())->NotifyDisconnect(this);

        if (mParent != NULL) {
            mParent->ConnectionDisconnected(this, &changed);
        }

        mOrigConnection = NULL;
    }
    ClearPostDialListeners();
    ReleaseWakeLock();
    *result = changed;
    return NOERROR;
}

ECode CGsmConnection::Update (
    /* [in] */ IDriverCall* dc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IGsmCall> newParent;
    Boolean changed = FALSE;
    Boolean wasConnectingInOrOut = IsConnectingInOrOut();
    ICallState state;
    GetState(&state);
    Boolean wasHolding = (state == ICallState_HOLDING);

    newParent = ParentFromDCState(((DriverCall*)dc)->mState);

    //Ignore dc->mNumber and dc->mName in case of a handover connection
    if (mOrigConnection != NULL) {
        if (IPhone::DEBUG_PHONE) Log(String("update: mOrigConnection is not null"));
    }
    else {
        Log(String(" mNumberConverted ") + mNumberConverted);
        Boolean b1, b2;
        if ((EqualsHandlesNulls(CoreUtils::Convert(mAddress), CoreUtils::Convert(((DriverCall*)dc)->mNumber), &b1), !b1) && (!mNumberConverted
                || (EqualsHandlesNulls(CoreUtils::Convert(mConvertedNumber), CoreUtils::Convert(((DriverCall*)dc)->mNumber), &b2), b2))) {
            if (IPhone::DEBUG_PHONE) Log(String("update: phone # changed!"));
            mAddress = ((DriverCall*)dc)->mNumber;
            changed = TRUE;
        }
    }

    // A null cnapName should be the same as ""
    if (((DriverCall*)dc)->mName.IsEmpty()) {
        if (!mCnapName.IsEmpty()) {
            changed = TRUE;
            mCnapName = String("");
        }
    }
    else if (!((DriverCall*)dc)->mName.Equals(mCnapName)) {
        changed = TRUE;
        mCnapName = ((DriverCall*)dc)->mName;
    }

    if (IPhone::DEBUG_PHONE) Log(String("--dssds----") + mCnapName);
    mCnapNamePresentation = ((DriverCall*)dc)->mNamePresentation;
    mNumberPresentation = ((DriverCall*)dc)->mNumberPresentation;

    if (newParent != mParent) {
        if (mParent != NULL) {
            mParent->Detach(this);
        }
        newParent->Attach(this, dc);
        mParent = newParent;
        changed = TRUE;
    }
    else {
        Boolean parentStateChange;
        mParent->Update(this, dc, &parentStateChange);
        changed = changed || parentStateChange;
    }

    /** Some state-transition events */

    // if (IPhone::DEBUG_PHONE) Log(
    //         "update: parent=" + mParent +
    //         ", hasNewParent=" + (newParent != mParent) +
    //         ", wasConnectingInOrOut=" + wasConnectingInOrOut +
    //         ", wasHolding=" + wasHolding +
    //         ", isConnectingInOrOut=" + isConnectingInOrOut() +
    //         ", changed=" + changed);

    if (wasConnectingInOrOut && !IsConnectingInOrOut()) {
        OnConnectedInOrOut();
    }

    ICallState s;
    GetState(&s);
    if (changed && !wasHolding && (s == ICallState_HOLDING)) {
        // We've transitioned into HOLDING
        OnStartedHolding();
    }

    *result = changed;
    return NOERROR;
}

ECode CGsmConnection::FakeHoldBeforeDial()
{
    if (mParent != NULL) {
        mParent->Detach(this);
    }

    mParent = ((CGsmCallTracker*)mOwner.Get())->mBackgroundCall;
    mParent->AttachFake(this, ICallState_HOLDING);

    OnStartedHolding();
    return NOERROR;
}

ECode CGsmConnection::GetGSMIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mIndex >= 0) {
        *result = mIndex + 1;
    } else {
        // throw new CallStateException ("GSM index not yet assigned");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CGsmConnection::OnConnectedInOrOut()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mConnectTime);
    mConnectTimeReal = SystemClock::GetElapsedRealtime();
    mDuration = 0;

    // bug #678474: incoming call interpreted as missed call, even though
    // it sounds like the user has picked up the call.

    if (IPhone::DEBUG_PHONE) {
        Log(String("onConnectedInOrOut: connectTime=") + mConnectTime);
    }

    if (!mIsIncoming) {
        // outgoing calls only
        ProcessNextPostDialChar();
    }
    ReleaseWakeLock();
    return NOERROR;
}

ECode CGsmConnection::OnStartedHolding()
{
    mHoldingStartTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode CGsmConnection::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mPostDialState == PostDialState_CANCELLED
        || mPostDialState == PostDialState_COMPLETE
        || mPostDialString.IsNull()
        || mPostDialString.GetLength() <= mNextPostDialChar)
    {
        *result = String("");
    }

    *result = mPostDialString.Substring(mNextPostDialChar);
    return NOERROR;
}

ECode CGsmConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumberPresentation;
    return NOERROR;
}

ECode CGsmConnection::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmConnection::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPreciseCause;
    return NOERROR;
}

ECode CGsmConnection::MigrateFrom(
    /* [in] */ IConnection* c)
{
    if (c == NULL) return NOERROR;

    Connection::MigrateFrom(c);

    ((CGsmConnection*)c)->GetUUSInfo((IUUSInfo**)&mUusInfo);

    AutoPtr<IInterface> obj;
    c->GetUserData((IInterface**)&obj);
    this->SetUserData(obj);
    return NOERROR;
}

ECode CGsmConnection::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOrigConnection;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmConnection::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mOrigConnection != NULL) {
        return mOrigConnection->IsMultiparty(result);
    }

    *result = FALSE;
    return NOERROR;
}

void CGsmConnection::Finalize()
{
    /**
     * It is understood that This finializer is not guaranteed
     * to be called and the release lock call is here just in
     * case there is some path that doesn't call onDisconnect
     * and or onConnectedInOrOut.
     */
    Boolean b;
    if (mPartialWakeLock->IsHeld(&b), b) {
        Logger::E(TAG, "[GSMConn] UNEXPECTED; mPartialWakeLock is held when finalizing.");
    }
    ClearPostDialListeners();
    ReleaseWakeLock();
}

Boolean CGsmConnection::ProcessPostDialChar(
    /* [in] */ Char32 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), b) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_DTMF_DONE, (IMessage**)&msg);
        ((CGsmCallTracker*)mOwner.Get())->mCi->SendDtmf(c, msg);
    }
    else if (c == IPhoneNumberUtils::PAUSE) {
        // From TS 22.101:
        // It continues...
        // Upon the called party answering the UE shall send the DTMF digits
        // automatically to the network after a delay of 3 seconds( 20 ).
        // The digits shall be sent according to the procedures and timing
        // specified in 3GPP TS 24.008 [13]. The first occurrence of the
        // "DTMF Control Digits Separator" shall be used by the ME to
        // distinguish between the addressing digits (i.e. the phone number)
        // and the DTMF digits. Upon subsequent occurrences of the
        // separator,
        // the UE shall pause again for 3 seconds ( 20 ) before sending
        // any further DTMF digits.
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_PAUSE_DONE, (IMessage**)&msg);
        mHandler->SendMessageDelayed(msg, PAUSE_DELAY_MILLIS, &b);
    }
    else if (c == IPhoneNumberUtils::WAIT) {
        SetPostDialState(PostDialState_WAIT);
    }
    else if (c == IPhoneNumberUtils::WILD) {
        SetPostDialState(PostDialState_WILD);
    }
    else {
        return FALSE;
    }

    return TRUE;
}

void CGsmConnection::ProcessNextPostDialChar()
{
    Char32 c = 0;
    AutoPtr<IRegistrant> postDialHandler;

    if (mPostDialState == PostDialState_CANCELLED) {
        //Logger::V("GSM", "##### ProcessNextPostDialChar: postDialState == CANCELLED, bail");
        return;
    }

    if (mPostDialString == NULL ||
            mPostDialString.GetLength() <= mNextPostDialChar) {
        SetPostDialState(PostDialState_COMPLETE);

        // notifyMessage.arg1 is 0 on complete
        c = 0;
    }
    else {
        Boolean isValid;

        SetPostDialState(PostDialState_STARTED);

        c = mPostDialString.GetChar(mNextPostDialChar++);

        isValid = ProcessPostDialChar(c);

        if (!isValid) {
            // Will call ProcessNextPostDialChar
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(EVENT_NEXT_POST_DIAL, (IMessage**)&msg);
            msg->SendToTarget();
            // Don't notify application
            Logger::E("GSM", String("ProcessNextPostDialChar: c=") + c + " isn't valid!");
            return;
        }
    }

    postDialHandler = ((CGSMPhone*)((CGsmCallTracker*)mOwner.Get())->mPhone.Get())->mPostDialHandler;

    AutoPtr<IMessage> notifyMessage;

    if (postDialHandler != NULL
            && ((postDialHandler->MessageForRegistrant((IMessage**)&notifyMessage), notifyMessage)) != NULL) {
        // The AsyncResult.result is the Connection object
        IConnectionPostDialState state = mPostDialState;
        AutoPtr<AsyncResult> ar = AsyncResult::ForMessage(notifyMessage);
        ar->mResult = TO_IINTERFACE(this);
        ar->mUserObj = CoreUtils::Convert(state);

        // arg1 is the character that was/is being processed
        notifyMessage->SetArg1(c);

        //Logger::V("GSM", "##### ProcessNextPostDialChar: send msg to postDialHandler, arg1=" + c);
        notifyMessage->SendToTarget();
    }
}

Boolean CGsmConnection::IsConnectingInOrOut()
{
    return mParent == NULL || mParent == ((CGsmCallTracker*)mOwner.Get())->mRingingCall
        || ((CGsmCall*)mParent.Get())->mState == ICallState_DIALING
        || ((CGsmCall*)mParent.Get())->mState == ICallState_ALERTING;
}

AutoPtr<IGsmCall> CGsmConnection::ParentFromDCState (
    /* [in] */ IDriverCallState state)
{
    switch (state) {
        case ICallState_ACTIVE:
        case ICallState_DIALING:
        case ICallState_ALERTING:
            return ((CGsmCallTracker*)mOwner.Get())->mForegroundCall;
        //break;

        case ICallState_HOLDING:
            return ((CGsmCallTracker*)mOwner.Get())->mBackgroundCall;
        //break;

        case ICallState_INCOMING:
        case ICallState_WAITING:
            return ((CGsmCallTracker*)mOwner.Get())->mRingingCall;
        //break;

        default:
            // throw new RuntimeException("illegal call state: " + state);
            return NULL;
    }
}

void CGsmConnection::SetPostDialState(
    /* [in] */ IConnectionPostDialState s)
{
    if (mPostDialState != PostDialState_STARTED
            && s == PostDialState_STARTED) {
        AcquireWakeLock();
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT, (IMessage**)&msg);
        Boolean b;
        mHandler->SendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS, &b);
    }
    else if (mPostDialState == PostDialState_STARTED
            && s != PostDialState_STARTED) {
        mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
        ReleaseWakeLock();
    }
    mPostDialState = s;
    NotifyPostDialListeners();
}

void CGsmConnection::CreateWakeLock(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mPartialWakeLock);
}

void CGsmConnection::AcquireWakeLock()
{
    Log(String("AcquireWakeLock"));
    mPartialWakeLock->AcquireLock();
}

void CGsmConnection::ReleaseWakeLock()
{
    AutoLock lock(mPartialWakeLock);
    Boolean b;
    if (mPartialWakeLock->IsHeld(&b), b) {
        Log(String("ReleaseWakeLock"));
        mPartialWakeLock->Release();
    }
}

void CGsmConnection::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, "[GSMConn] %s", msg.string());
}


} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos