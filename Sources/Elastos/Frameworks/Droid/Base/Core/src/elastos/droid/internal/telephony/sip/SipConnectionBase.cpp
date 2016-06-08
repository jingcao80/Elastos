
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipConnectionBase.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                          SipConnectionBase
//=====================================================================
const String SipConnectionBase::LOGTAG("SipConnBase");
const Boolean SipConnectionBase::DBG = TRUE;
const Boolean SipConnectionBase::VDBG = FALSE;

SipConnectionBase::SipConnectionBase(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (DBG) log("SipConnectionBase: ctor dialString=" + dialString);
    // mPostDialString = PhoneNumberUtils.extractPostDialPortion(dialString);
    //
    // mCreateTime = System.currentTimeMillis();
}

ECode SipConnectionBase::GetCreateTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getCreateTime: ret=" + mCreateTime);
    // return mCreateTime;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetConnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getConnectTime: ret=" + mConnectTime);
    // return mConnectTime;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getDisconnectTime: ret=" + mDisconnectTime);
    // return mDisconnectTime;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // long dur;
    // if (mConnectTimeReal == 0) {
    //     dur = 0;
    // } else if (mDuration < 0) {
    //     dur = SystemClock.elapsedRealtime() - mConnectTimeReal;
    // } else {
    //     dur = mDuration;
    // }
    // if (VDBG) log("getDurationMillis: ret=" + dur);
    // return dur;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // long dur;
    // if (getState() != Call.State.HOLDING) {
    //     // If not holding, return 0
    //     dur = 0;
    // } else {
    //     dur = SystemClock.elapsedRealtime() - mHoldingStartTime;
    // }
    // if (VDBG) log("getHoldDurationMillis: ret=" + dur);
    // return dur;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getDisconnectCause: ret=" + mCause);
    // return mCause;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::SetDisconnectCause(
    /* [in] */ Int32 cause)
{
    // ==================before translated======================
    // if (DBG) log("setDisconnectCause: prev=" + mCause + " new=" + cause);
    // mCause = cause;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getPostDialState: ret=" + mPostDialState);
    // return mPostDialState;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::ProceedAfterWaitChar()
{
    // ==================before translated======================
    // if (DBG) log("proceedAfterWaitChar: ignore");
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // if (DBG) log("proceedAfterWildChar: ignore");
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::CancelPostDial()
{
    // ==================before translated======================
    // if (DBG) log("cancelPostDial: ignore");
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mPostDialState == PostDialState.CANCELLED
    //     || mPostDialState == PostDialState.COMPLETE
    //     || mPostDialString == null
    //     || mPostDialString.length() <= mNextPostDialChar) {
    //     if (DBG) log("getRemaingPostDialString: ret empty string");
    //     return "";
    // }
    //
    // return mPostDialString.substring(mNextPostDialChar);
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // TODO: add PRESENTATION_URL
    // if (VDBG) log("getNumberPresentation: ret=PRESENTATION_ALLOWED");
    // return PhoneConstants.PRESENTATION_ALLOWED;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: what's this for SIP?
    // if (VDBG) log("getUUSInfo: ? ret=null");
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return 0;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetHoldingStartTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHoldingStartTime;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetConnectTimeReal(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mConnectTimeReal;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipConnectionBase::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

void SipConnectionBase::SetState(
    /* [in] */ ICallState state)
{
    // ==================before translated======================
    // if (DBG) log("setState: state=" + state);
    // switch (state) {
    //     case ACTIVE:
    //         if (mConnectTime == 0) {
    //             mConnectTimeReal = SystemClock.elapsedRealtime();
    //             mConnectTime = System.currentTimeMillis();
    //         }
    //         break;
    //     case DISCONNECTED:
    //         mDuration = getDurationMillis();
    //         mDisconnectTime = System.currentTimeMillis();
    //         break;
    //     case HOLDING:
    //         mHoldingStartTime = SystemClock.elapsedRealtime();
    //         break;
    //     default:
    //         // Ignore
    //         break;
    // }
    assert(0);
}

void SipConnectionBase::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, msg);
    assert(0);
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
