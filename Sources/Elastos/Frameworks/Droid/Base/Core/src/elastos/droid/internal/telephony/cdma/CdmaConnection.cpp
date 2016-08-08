#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCall.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallWaitingNotification.h"
#include "elastos/droid/internal/telephony/cdma/CdmaConnection.h"
#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"
#include "elastos/droid/internal/telephony/DriverCall.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::DriverCall;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_UNKNOWN;
using Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_READY;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                      CdmaConnection::MyHandler
//=====================================================================
CdmaConnection::MyHandler::MyHandler(
    /* [in] */ ILooper* l,
    /* [in] */ CdmaConnection* host)
    : Handler(l)
    , mHost(host)
{
}

ECode CdmaConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

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

//=====================================================================
//                            CdmaConnection
//=====================================================================
CAR_INTERFACE_IMPL(CdmaConnection, Connection, ICdmaConnection);
const String CdmaConnection::LOGTAG("CdmaConnection");
const Int32 CdmaConnection::EVENT_DTMF_DONE;
const Int32 CdmaConnection::EVENT_PAUSE_DONE;
const Int32 CdmaConnection::EVENT_NEXT_POST_DIAL;
const Int32 CdmaConnection::EVENT_WAKE_LOCK_TIMEOUT;
const Int32 CdmaConnection::WAKE_LOCK_TIMEOUT_MILLIS;
const Int32 CdmaConnection::PAUSE_DELAY_MILLIS;
const Boolean CdmaConnection::VDBG = FALSE;

CdmaConnection::CdmaConnection()
    : mDisconnected(FALSE)
    , mIndex(0)
    , mDisconnectTime(0)
    , mNextPostDialChar(0)
    , mCause(IDisconnectCause::NOT_DISCONNECTED)
    , mPostDialState(PostDialState_NOT_STARTED)
    , mPreciseCause(0)
{
}

ECode CdmaConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDriverCall* _dc,
    /* [in] */ ICdmaCallTracker* ct,
    /* [in] */ Int32 index)
{
    CreateWakeLock(context);
    AcquireWakeLock();

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    AutoPtr<DriverCall> dc = (DriverCall*)_dc;
    mAddress = dc->mNumber;

    mIsIncoming = dc->mIsMT;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);
    mCnapName = dc->mName;
    mCnapNamePresentation = dc->mNamePresentation;
    mNumberPresentation = dc->mNumberPresentation;

    mIndex = index;

    ParentFromDCState(dc->mState, (ICdmaCall**)&mParent);
    ((CdmaCall*)mParent.Get())->Attach(this, _dc);
    return NOERROR;
}

ECode CdmaConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& _dialString,
    /* [in] */ ICdmaCallTracker* ct,
    /* [in] */ ICdmaCall* parent)
{
    String dialString = _dialString;

    CreateWakeLock(context);
    AcquireWakeLock();

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    mDialString = dialString;
    Logger::D(LOGTAG, "[CDMAConn] CdmaConnection: dialString=%s", dialString.string());
    dialString = FormatDialString(dialString);
    Logger::D(LOGTAG, "[CDMAConn] CdmaConnection:formated dialString=%s", dialString.string());

    PhoneNumberUtils::ExtractNetworkPortionAlt(dialString, &mAddress);
    PhoneNumberUtils::ExtractPostDialPortion(dialString, &mPostDialString);

    mIndex = -1;

    mIsIncoming = FALSE;
    mCnapName = NULL;
    mCnapNamePresentation = IPhoneConstants::PRESENTATION_ALLOWED;
    mNumberPresentation = IPhoneConstants::PRESENTATION_ALLOWED;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);

    if (parent != NULL) {
        mParent = parent;

        //for the three way call case, not change parent state
        if (((CdmaCall*)parent)->mState == ICallState_ACTIVE) {
            ((CdmaCall*)parent)->AttachFake(this, ICallState_ACTIVE);
        }
        else {
            ((CdmaCall*)parent)->AttachFake(this, ICallState_DIALING);
        }
    }
    return NOERROR;
}

ECode CdmaConnection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICdmaCallWaitingNotification* _cw,
    /* [in] */ ICdmaCallTracker* ct,
    /* [in] */ ICdmaCall* parent)
{
    CreateWakeLock(context);
    AcquireWakeLock();

    mOwner = ct;
    AutoPtr<ILooper> looper;
    IHandler::Probe(mOwner)->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    AutoPtr<CdmaCallWaitingNotification> cw = (CdmaCallWaitingNotification*)_cw;
    mAddress = cw->number;
    mNumberPresentation = cw->numberPresentation;
    mCnapName = cw->name;
    mCnapNamePresentation = cw->namePresentation;
    mIndex = -1;
    mIsIncoming = TRUE;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);
    mConnectTime = 0;
    mParent = parent;
    ((CdmaCall*)parent)->AttachFake(this, ICallState_WAITING);
    return NOERROR;
}

ECode CdmaConnection::Dispose()
{
    return NOERROR;
}

Boolean CdmaConnection::EqualsHandlesNulls(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    if (a == NULL) {
        return (b == NULL);
    }
    else {
        Boolean Boolean;
        IObject::Probe(a)->Equals(b, &Boolean);
        return Boolean;
    }
}

ECode CdmaConnection::CompareTo(
    /* [in] */ IDriverCall* _c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // On mobile originated (MO) calls, the phone number may have changed
    // due to a SIM Toolkit call control modification.
    //
    // We assume we know when MO calls are created (since we created them)
    // and therefore don't need to compare the phone number anyway.
    AutoPtr<DriverCall> c = (DriverCall*)_c;
    if (! (mIsIncoming || c->mIsMT)) {
        *result = TRUE;
        return NOERROR;
    }

    // ... but we can compare phone numbers on MT calls, and we have
    // no control over when they begin, so we might as well

    String cAddress;
    PhoneNumberUtils::StringFromStringAndTOA(c->mNumber, c->mTOA, &cAddress);
    *result = mIsIncoming == c->mIsMT &&
            EqualsHandlesNulls(CoreUtils::Convert(mAddress), CoreUtils::Convert(cAddress));
    return NOERROR;
}

ECode CdmaConnection::GetOrigDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDialString;
    return NOERROR;
}

ECode CdmaConnection::GetCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(mParent);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaConnection::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDisconnectTime;
    return NOERROR;
}

ECode CdmaConnection::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
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

ECode CdmaConnection::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCause;
    return NOERROR;
}

ECode CdmaConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    if (mDisconnected) {
        *result = ICallState_DISCONNECTED;
        return NOERROR;
    }
    else {
        return Connection::GetState(result);
    }
}

ECode CdmaConnection::Hangup()
{
    if (!mDisconnected) {
        ((CdmaCallTracker*)mOwner.Get())->Hangup(this);
    }
    else {
        // throw new CallStateException ("disconnected");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CdmaConnection::Separate()
{
    if (!mDisconnected) {
        ((CdmaCallTracker*)mOwner.Get())->Separate(this);
    }
    else {
        // throw new CallStateException ("disconnected");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CdmaConnection::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPostDialState;
    return NOERROR;
}

ECode CdmaConnection::ProceedAfterWaitChar()
{
    if (mPostDialState != PostDialState_WAIT) {
        Logger::W(LOGTAG, "CdmaConnection.proceedAfterWaitChar(): Expected getPostDialState() to be WAIT but was %d", mPostDialState);
        return NOERROR;
    }

    SetPostDialState(PostDialState_STARTED);

    ProcessNextPostDialChar();
    return NOERROR;
}

ECode CdmaConnection::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    if (mPostDialState != PostDialState_WILD) {
        Logger::W(LOGTAG, "CdmaConnection.proceedAfterWaitChar(): Expected getPostDialState() to be WILD but was %d", mPostDialState);
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

ECode CdmaConnection::CancelPostDial()
{
    SetPostDialState(PostDialState_CANCELLED);
    return NOERROR;
}

ECode CdmaConnection::OnHangupLocal()
{
    mCause = IDisconnectCause::LOCAL;
    mPreciseCause = 0;
    return NOERROR;
}

ECode CdmaConnection::DisconnectCauseFromCode(
    /* [in] */ Int32 causeCode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    /**
     * See 22.001 Annex F.4 for mapping of cause codes
     * to local tones
     */

    switch (causeCode) {
        case ICallFailCause::USER_BUSY:
            *result = IDisconnectCause::BUSY;
            return NOERROR;
        case ICallFailCause::NO_CIRCUIT_AVAIL:
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
        case ICallFailCause::DIAL_MODIFIED_TO_USSD:
            *result = IDisconnectCause::DIAL_MODIFIED_TO_USSD;
            return NOERROR;
        case ICallFailCause::DIAL_MODIFIED_TO_SS:
            *result = IDisconnectCause::DIAL_MODIFIED_TO_SS;
            return NOERROR;
        case ICallFailCause::DIAL_MODIFIED_TO_DIAL:
            *result = IDisconnectCause::DIAL_MODIFIED_TO_DIAL;
            return NOERROR;
        case ICallFailCause::CDMA_LOCKED_UNTIL_POWER_CYCLE:
            *result = IDisconnectCause::CDMA_LOCKED_UNTIL_POWER_CYCLE;
            return NOERROR;
        case ICallFailCause::CDMA_DROP:
            *result = IDisconnectCause::CDMA_DROP;
            return NOERROR;
        case ICallFailCause::CDMA_INTERCEPT:
            *result = IDisconnectCause::CDMA_INTERCEPT;
            return NOERROR;
        case ICallFailCause::CDMA_REORDER:
            *result = IDisconnectCause::CDMA_REORDER;
            return NOERROR;
        case ICallFailCause::CDMA_SO_REJECT:
            *result = IDisconnectCause::CDMA_SO_REJECT;
            return NOERROR;
        case ICallFailCause::CDMA_RETRY_ORDER:
            *result = IDisconnectCause::CDMA_RETRY_ORDER;
            return NOERROR;
        case ICallFailCause::CDMA_ACCESS_FAILURE:
            *result = IDisconnectCause::CDMA_ACCESS_FAILURE;
            return NOERROR;
        case ICallFailCause::CDMA_PREEMPTED:
            *result = IDisconnectCause::CDMA_PREEMPTED;
            return NOERROR;
        case ICallFailCause::CDMA_NOT_EMERGENCY:
            *result = IDisconnectCause::CDMA_NOT_EMERGENCY;
            return NOERROR;
        case ICallFailCause::CDMA_ACCESS_BLOCKED:
            *result = IDisconnectCause::CDMA_ACCESS_BLOCKED;
            return NOERROR;
        case ICallFailCause::ERROR_UNSPECIFIED:
        case ICallFailCause::NORMAL_CLEARING:
        default: {
            AutoPtr<ICDMAPhone> phone = ((CdmaCallTracker*)mOwner.Get())->mPhone;
            AutoPtr<IServiceState> ss;
            IPhone::Probe(phone)->GetServiceState((IServiceState**)&ss);
            Int32 serviceState;
            ss->GetState(&serviceState);
            AutoPtr<IUiccCardApplication> app;
            UiccController::GetInstance()->GetUiccCardApplication(
                    IUiccController::APP_FAM_3GPP2, (IUiccCardApplication**)&app);
            AppState uiccAppState;
            if (app != NULL) {
                app->GetState(&uiccAppState);
            }
            else {
                uiccAppState = APPSTATE_UNKNOWN;
            }

            if (serviceState == IServiceState::STATE_POWER_OFF) {
                *result = IDisconnectCause::POWER_OFF;
                return NOERROR;
            }
            else if (serviceState == IServiceState::STATE_OUT_OF_SERVICE
                    || serviceState == IServiceState::STATE_EMERGENCY_ONLY) {
                *result = IDisconnectCause::OUT_OF_SERVICE;
            return NOERROR;
            }
            else if (((CDMAPhone*)phone.Get())->mCdmaSubscriptionSource ==
                    ICdmaSubscriptionSourceManager::SUBSCRIPTION_FROM_RUIM
                    && uiccAppState != APPSTATE_READY) {
                *result = IDisconnectCause::ICC_ERROR;
            return NOERROR;
            }
            else if (causeCode == ICallFailCause::NORMAL_CLEARING) {
                *result = IDisconnectCause::NORMAL;
                return NOERROR;
            }
            else {
                *result = IDisconnectCause::ERROR_UNSPECIFIED;
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CdmaConnection::OnRemoteDisconnect(
    /* [in] */ Int32 causeCode)
{
    mPreciseCause = causeCode;
    Int32 val;
    DisconnectCauseFromCode(causeCode, &val);
    Boolean b;
    OnDisconnect(val, &b);
    return NOERROR;
}

ECode CdmaConnection::OnDisconnect(
    /* [in] */ Int32 cause,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean changed = FALSE;

    mCause = cause;

    if (!mDisconnected) {
        DoDisconnect();
        if (VDBG) Logger::D(LOGTAG, "onDisconnect: cause=%d", cause);

        ((CDMAPhone*)((CdmaCallTracker*)mOwner.Get())->mPhone.Get())->NotifyDisconnect(this);

        if (mParent != NULL) {
            ((CdmaCall*)mParent.Get())->ConnectionDisconnected(this, &changed);
        }
    }
    ReleaseWakeLock();
    *result = changed;
    return NOERROR;
}

ECode CdmaConnection::OnLocalDisconnect()
{
    if (!mDisconnected) {
        DoDisconnect();
        if (VDBG) Logger::D(LOGTAG, "onLoalDisconnect" );

        if (mParent != NULL) {
            ((CdmaCall*)mParent.Get())->Detach(this);
        }
    }
    ReleaseWakeLock();
    return NOERROR;
}

ECode CdmaConnection::Update(
    /* [in] */ IDriverCall* _dc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaCall> newParent;
    Boolean changed = FALSE;
    Boolean wasConnectingInOrOut = IsConnectingInOrOut();
    ICallState state;
    GetState(&state);
    Boolean wasHolding = (state == ICallState_HOLDING);

    AutoPtr<DriverCall> dc = (DriverCall*)_dc;
    ParentFromDCState(dc->mState, (ICdmaCall**)&newParent);

    // if (IPhone::DEBUG_PHONE) Log(String("parent= ") + mParent +", newParent= " + newParent);

    Log(String(" mNumberConverted ") + StringUtils::BooleanToString(mNumberConverted));
    if (!EqualsHandlesNulls(CoreUtils::Convert(mAddress),
            CoreUtils::Convert(dc->mNumber))
            && (!mNumberConverted
            || !EqualsHandlesNulls(CoreUtils::Convert(mConvertedNumber),
            CoreUtils::Convert(dc->mNumber))))  {
        if (IPhone::DEBUG_PHONE) Log(String("update: phone # changed!"));
        mAddress = dc->mNumber;
        changed = TRUE;
    }

    // A NULL cnapName should be the same as ""
    if (dc->mName.IsEmpty()) {
        if (!mCnapName.IsEmpty()) {
            changed = TRUE;
            mCnapName = String("");
        }
    }
    else if (!dc->mName.Equals(mCnapName)) {
        changed = TRUE;
        mCnapName = dc->mName;
    }

    if (IPhone::DEBUG_PHONE) Log(String("--dssds----") + mCnapName);
    mCnapNamePresentation = dc->mNamePresentation;
    mNumberPresentation = dc->mNumberPresentation;

    if (newParent != mParent) {
        if (mParent != NULL) {
            ((CdmaCall*)mParent.Get())->Detach(this);
        }
        ((CdmaCall*)newParent.Get())->Attach(this, dc);
        mParent = newParent;
        changed = TRUE;
    }
    else {
        Boolean parentStateChange;
        ((CdmaCall*)mParent.Get())->Update(this, dc, &parentStateChange);
        changed = changed || parentStateChange;
    }

    /** Some state-transition events */

    Boolean b = IsConnectingInOrOut();
    if (IPhone::DEBUG_PHONE) Log(
            String("Update, wasConnectingInOrOut=") +
            StringUtils::BooleanToString(wasConnectingInOrOut) +
            ", wasHolding=" + StringUtils::BooleanToString(wasHolding) +
            ", isConnectingInOrOut=" + StringUtils::BooleanToString(b) +
            ", changed=" + StringUtils::BooleanToString(changed));


    if (wasConnectingInOrOut && !b) {
        OnConnectedInOrOut();
    }

    GetState(&state);
    if (changed && !wasHolding && (state == ICallState_HOLDING)) {
        // We've transitioned into HOLDING
        OnStartedHolding();
    }

    *result = changed;
    return NOERROR;
}

ECode CdmaConnection::FakeHoldBeforeDial()
{
    if (mParent != NULL) {
        ((CdmaCall*)mParent.Get())->Detach(this);
    }

    mParent = ((CdmaCallTracker*)mOwner.Get())->mBackgroundCall;
    ((CdmaCall*)mParent.Get())->AttachFake(this, ICallState_HOLDING);

    OnStartedHolding();
    return NOERROR;
}

ECode CdmaConnection::GetCDMAIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (mIndex >= 0) {
        *result = mIndex + 1;
        return NOERROR;
    }
    else {
        // throw new CallStateException ("CDMA connection index not assigned");
        return E_CALL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CdmaConnection::OnConnectedInOrOut()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mConnectTime);

    mConnectTimeReal = SystemClock::GetElapsedRealtime();
    mDuration = 0;

    // bug #678474: incoming call interpreted as missed call, even though
    // it sounds like the user has picked up the call.
    if (IPhone::DEBUG_PHONE) {
        Log(String("onConnectedInOrOut: connectTime=") + StringUtils::ToString(mConnectTime));
    }

    if (!mIsIncoming) {
        // outgoing calls only
        ProcessNextPostDialChar();
    }
    else {
        // Only release wake lock for incoming calls, for outgoing calls the wake lock
        // will be released after any pause-dial is completed
        ReleaseWakeLock();
    }
    return NOERROR;
}

ECode CdmaConnection::OnStartedHolding()
{
    mHoldingStartTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode CdmaConnection::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    if (mPostDialState == PostDialState_CANCELLED
            || mPostDialState == PostDialState_COMPLETE
            || mPostDialString.IsNull()
            || mPostDialString.GetLength() <= mNextPostDialChar) {
        *result = String("");
        return NOERROR;
    }

    String subStr = mPostDialString.Substring(mNextPostDialChar);
    if (subStr != NULL) {
        Int32 wIndex = subStr.IndexOf(IPhoneNumberUtils::WAIT);
        Int32 pIndex = subStr.IndexOf(IPhoneNumberUtils::PAUSE);

        if (wIndex > 0 && (wIndex < pIndex || pIndex <= 0)) {
            subStr = subStr.Substring(0, wIndex);
        }
        else if (pIndex > 0) {
            subStr = subStr.Substring(0, pIndex);
        }
    }
    *result = subStr;
    return NOERROR;
}

ECode CdmaConnection::UpdateParent(
    /* [in] */ ICdmaCall* oldParent,
    /* [in] */ ICdmaCall* newParent)
{
    if (newParent != oldParent) {
        if (oldParent != NULL) {
            ((CdmaCall*)oldParent)->Detach(this);
        }
        ((CdmaCall*)newParent)->AttachFake(this, ICallState_ACTIVE);
        mParent = newParent;
    }
    return NOERROR;
}

ECode CdmaConnection::ProcessNextPostDialChar()
{
    Char32 c = 0;
    AutoPtr<IRegistrant> postDialHandler;

    if (mPostDialState == PostDialState_CANCELLED) {
        ReleaseWakeLock();
        //Logger::V("CDMA", "##### processNextPostDialChar: postDialState == CANCELLED, bail");
        return NOERROR;
    }

    if (mPostDialString.IsNull() ||
            mPostDialString.GetLength() <= mNextPostDialChar) {
        SetPostDialState(PostDialState_COMPLETE);

        // We were holding a wake lock until pause-dial was complete, so give it up now
        ReleaseWakeLock();

        // notifyMessage.arg1 is 0 on complete
        c = 0;
    }
    else {
        Boolean isValid;

        SetPostDialState(PostDialState_STARTED);

        c = mPostDialString.GetChar(mNextPostDialChar++);

        isValid = ProcessPostDialChar(c);

        if (!isValid) {
            // Will call processNextPostDialChar
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(EVENT_NEXT_POST_DIAL, (IMessage**)&msg);
            msg->SendToTarget();
            // Don't notify application
            Logger::E("CDMA", "processNextPostDialChar: c=%d isn't valid!", c);
            return NOERROR;
        }
    }

    postDialHandler = ((CDMAPhone*)((CdmaCallTracker*)mOwner.Get())->mPhone.Get())->mPostDialHandler;

    AutoPtr<IMessage> notifyMessage;

    if (postDialHandler != NULL &&
            (postDialHandler->MessageForRegistrant((IMessage**)&notifyMessage), notifyMessage) != NULL) {
        // The AsyncResult.result is the Connection object
        IConnectionPostDialState state = mPostDialState;
        AutoPtr<AsyncResult> ar = AsyncResult::ForMessage(notifyMessage);
        ar->mResult = TO_IINTERFACE(this);
        ar->mUserObj = CoreUtils::Convert(state);

        // arg1 is the character that was/is being processed
        notifyMessage->SetArg1(c);

        notifyMessage->SendToTarget();
    }
    return NOERROR;
}

String CdmaConnection::FormatDialString(
    /* [in] */ const String& phoneNumber)
{
    /**
     * TODO(cleanup): This function should move to PhoneNumberUtils, and
     * tests should be added.
     */

    if (phoneNumber.IsNull()) {
        return String(NULL);
    }
    Int32 length = phoneNumber.GetLength();
    StringBuilder ret;
    Char32 c;
    Int32 currIndex = 0;

    while (currIndex < length) {
        c = phoneNumber.GetChar(currIndex);
        if (IsPause(c) || IsWait(c)) {
            if (currIndex < length - 1) {
                // if PW not at the end
                Int32 nextIndex = FindNextPCharOrNonPOrNonWCharIndex(phoneNumber, currIndex);
                // If there is non PW char following PW sequence
                if (nextIndex < length) {
                    Char32 pC = FindPOrWCharToAppend(phoneNumber, currIndex, nextIndex);
                    ret.Append((Int32)pC);
                    // If PW char sequence has more than 2 PW characters,
                    // skip to the last PW character since the sequence already be
                    // converted to WAIT character
                    if (nextIndex > (currIndex + 1)) {
                        currIndex = nextIndex - 1;
                    }
                }
                else if (nextIndex == length) {
                    // It means PW characters at the end, ignore
                    currIndex = length - 1;
                }
            }
        }
        else {
            ret.Append((Int32)c);
        }
        currIndex++;
    }

    String str1, str2;
    ret.ToString(&str1);
    PhoneNumberUtils::CdmaCheckAndProcessPlusCode(str1, &str2);
    return str2;
}

ECode CdmaConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumberPresentation;
    return NOERROR;
}

ECode CdmaConnection::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // UUS information not supported in CDMA
    *result = NULL;
    return NOERROR;
}

ECode CdmaConnection::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPreciseCause;
    return NOERROR;
}

ECode CdmaConnection::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CdmaConnection::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

void CdmaConnection::Finalize()
{
    /**
     * It is understood that This finializer is not guaranteed
     * to be called and the release lock call is here just in
     * case there is some path that doesn't call onDisconnect
     * and or onConnectedInOrOut.
     */
    Boolean b;
    if (mPartialWakeLock->IsHeld(&b), b) {
        Logger::E(LOGTAG, "[CdmaConn] UNEXPECTED; mPartialWakeLock is held when finalizing.");
    }
    ReleaseWakeLock();
}

void CdmaConnection::DoDisconnect()
{
    mIndex = -1;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mDisconnectTime);

    mDuration = SystemClock::GetElapsedRealtime() - mConnectTimeReal;
    mDisconnected = TRUE;
    ClearPostDialListeners();
}

Boolean CdmaConnection::ProcessPostDialChar(
    /* [in] */ Byte c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), b) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_DTMF_DONE, (IMessage**)&msg);
        ((CdmaCallTracker*)mOwner.Get())->mCi->SendDtmf(c, msg);
    }
    else if (c == IPhoneNumberUtils::PAUSE) {
        SetPostDialState(PostDialState_PAUSE);

        // Upon occurrences of the separator, the UE shall
        // pause again for 2 seconds before sending any
        // further DTMF digits.
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

Boolean CdmaConnection::IsConnectingInOrOut()
{
    return mParent == NULL || mParent == ((CdmaCallTracker*)mOwner.Get())->mRingingCall
        || ((CdmaCall*)mParent.Get())->mState == ICallState_DIALING
        || ((CdmaCall*)mParent.Get())->mState == ICallState_ALERTING;
    return FALSE;
}

ECode CdmaConnection::ParentFromDCState(
    /* [in] */  IDriverCallState state,
    /* [out] */ ICdmaCall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    switch (state) {
        case DriverCallState_ACTIVE:
        case DriverCallState_DIALING:
        case DriverCallState_ALERTING:
            *result = ((CdmaCallTracker*)mOwner.Get())->mForegroundCall;
            REFCOUNT_ADD(*result)
            return NOERROR;
        //break;

        case DriverCallState_HOLDING:
            *result = ((CdmaCallTracker*)mOwner.Get())->mBackgroundCall;
            REFCOUNT_ADD(*result)
            return NOERROR;
        //break;

        case DriverCallState_INCOMING:
        case DriverCallState_WAITING:
            *result = ((CdmaCallTracker*)mOwner.Get())->mRingingCall;
            REFCOUNT_ADD(*result)
            return NOERROR;
        //break;

        default:
            // throw new RuntimeException("illegal call state: " + state);
            return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

void CdmaConnection::SetPostDialState(
    /* [in] */ IConnectionPostDialState s)
{
    if (s == PostDialState_STARTED ||
            s == PostDialState_PAUSE) {
        {
            AutoLock lock(mPartialWakeLock);
            Boolean b;
            if (mPartialWakeLock->IsHeld(&b), b) {
                mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
            }
            else {
                AcquireWakeLock();
            }
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT, (IMessage**)&msg);
            mHandler->SendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MILLIS, &b);
        }
    }
    else {
        mHandler->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
        ReleaseWakeLock();
    }
    mPostDialState = s;
    NotifyPostDialListeners();
}

void CdmaConnection::CreateWakeLock(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, LOGTAG,
            (IPowerManagerWakeLock**)&mPartialWakeLock);
}

void CdmaConnection::AcquireWakeLock()
{
    Log(String("AcquireWakeLock"));
    mPartialWakeLock->AcquireLock();
}

void CdmaConnection::ReleaseWakeLock()
{
    AutoLock lock(mPartialWakeLock);
    Boolean b;
    if (mPartialWakeLock->IsHeld(&b), b) {
        Log(String("releaseWakeLock"));
        mPartialWakeLock->ReleaseLock();
    }
}

Boolean CdmaConnection::IsPause(
    /* [in] */ Byte c)
{
    return c == IPhoneNumberUtils::PAUSE;
}

Boolean CdmaConnection::IsWait(
    /* [in] */ Byte c)
{
    return c == IPhoneNumberUtils::WAIT;
}

Int32 CdmaConnection::FindNextPCharOrNonPOrNonWCharIndex(
    /* [in] */ const String& phoneNumber,
    /* [in] */ Int32 currIndex)
{
    Boolean wMatched = IsWait(phoneNumber.GetChar(currIndex));
    Int32 index = currIndex + 1;
    Int32 length = phoneNumber.GetLength();

    while (index < length) {
        Char32 cNext = phoneNumber.GetChar(index);
        // if there is any W inside P/W sequence,mark it
        if (IsWait(cNext)) {
            wMatched = TRUE;
        }
        // if any characters other than P/W chars after P/W sequence
        // we break out the loop and append the correct
        if (!IsWait(cNext) && !IsPause(cNext)) {
            break;
        }
        index++;
    }

    // It means the PAUSE character(s) is in the middle of dial string
    // and it needs to be handled one by one.
    if ((index < length) && (index > (currIndex + 1))  &&
        ((wMatched == FALSE) && IsPause(phoneNumber.GetChar(currIndex)))) {
        return (currIndex + 1);
    }
    return index;
}

Byte CdmaConnection::FindPOrWCharToAppend(
    /* [in] */ const String& phoneNumber,
    /* [in] */ Int32 currPwIndex,
    /* [in] */ Int32 nextNonPwCharIndex)
{
    Char32 c = phoneNumber.GetChar(currPwIndex);
    Char32 ret;

    // Append the PW char
    ret = (IsPause(c)) ? IPhoneNumberUtils::PAUSE : IPhoneNumberUtils::WAIT;

    // If the nextNonPwCharIndex is greater than currPwIndex + 1,
    // it means the PW sequence contains not only P characters.
    // Since for the sequence that only contains P character,
    // the P character is handled one by one, the nextNonPwCharIndex
    // equals to currPwIndex + 1.
    // In this case, skip P, append W.
    if (nextNonPwCharIndex > (currPwIndex + 1)) {
        ret = IPhoneNumberUtils::WAIT;
    }
    return ret;
}

void CdmaConnection::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, "[CDMAConn] %s", msg.string());
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
