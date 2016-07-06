
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipConnectionBase.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

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

SipConnectionBase::SipConnectionBase()
    : mNextPostDialChar(0)
    , mCreateTime(0)
    , mConnectTime(0)
    , mDisconnectTime(0)
    , mConnectTimeReal(0)
    , mDuration(-1)
    , mHoldingStartTime(0)
    , mCause(IDisconnectCause::NOT_DISCONNECTED)
    , mPostDialState(PostDialState_NOT_STARTED)
{
}

ECode SipConnectionBase::constructor(
    /* [in] */ const String& dialString)
{
    if (DBG) Log(String("SipConnectionBase: ctor dialString=") + dialString);
    PhoneNumberUtils::ExtractPostDialPortion(dialString, &mPostDialString);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);
    return NOERROR;
}

ECode SipConnectionBase::GetCreateTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Log(String("getCreateTime: ret=") + StringUtils::ToString(mCreateTime));
    *result = mCreateTime;
    return NOERROR;
}

ECode SipConnectionBase::GetConnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Log(String("getConnectTime: ret=") + StringUtils::ToString(mConnectTime));
    *result = mConnectTime;
    return NOERROR;
}

ECode SipConnectionBase::GetDisconnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Log(String("getDisconnectTime: ret=") + StringUtils::ToString(mDisconnectTime));
    *result = mDisconnectTime;
    return NOERROR;
}

ECode SipConnectionBase::GetDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 dur;
    if (mConnectTimeReal == 0) {
        dur = 0;
    }
    else if (mDuration < 0) {
        dur = SystemClock::GetElapsedRealtime() - mConnectTimeReal;
    }
    else {
        dur = mDuration;
    }
    if (VDBG) Log(String("getDurationMillis: ret=") + StringUtils::ToString(dur));
    *result = dur;
    return NOERROR;
}

ECode SipConnectionBase::GetHoldDurationMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 dur;
    ICallState state;
    GetState(&state);
    if (state != ICallState_HOLDING) {
        // If not holding, return 0
        dur = 0;
    }
    else {
        dur = SystemClock::GetElapsedRealtime() - mHoldingStartTime;
    }
    if (VDBG) Log(String("getHoldDurationMillis: ret=") + StringUtils::ToString(dur));
    *result = dur;
    return NOERROR;
}

ECode SipConnectionBase::GetDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Log(String("getDisconnectCause: ret=") + StringUtils::ToString(mCause));
    *result = mCause;
    return NOERROR;
}

ECode SipConnectionBase::SetDisconnectCause(
    /* [in] */ Int32 cause)
{
    if (DBG) Log(String("setDisconnectCause: prev=") + StringUtils::ToString(mCause) +
            " new=" + StringUtils::ToString(cause));
    mCause = cause;
    return NOERROR;
}

ECode SipConnectionBase::GetPostDialState(
    /* [out] */ IConnectionPostDialState* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Log(String("getPostDialState: ret=") + StringUtils::ToString(mPostDialState));
    *result = mPostDialState;
    return NOERROR;
}

ECode SipConnectionBase::ProceedAfterWaitChar()
{
    if (DBG) Log(String("proceedAfterWaitChar: ignore"));
    return NOERROR;
}

ECode SipConnectionBase::ProceedAfterWildChar(
    /* [in] */ const String& str)
{
    if (DBG) Log(String("proceedAfterWildChar: ignore"));
    return NOERROR;
}

ECode SipConnectionBase::CancelPostDial()
{
    if (DBG) Log(String("cancelPostDial: ignore"));
    return NOERROR;
}

ECode SipConnectionBase::GetRemainingPostDialString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mPostDialState == PostDialState_CANCELLED
        || mPostDialState == PostDialState_COMPLETE
        || mPostDialString == NULL
        || mPostDialString.GetLength() <= mNextPostDialChar) {
        if (DBG) Log(String("getRemaingPostDialString: ret empty string"));
        *result = String("");
    }

    *result = mPostDialString.Substring(mNextPostDialChar);
    return NOERROR;
}

ECode SipConnectionBase::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO: add PRESENTATION_URL
    if (VDBG) Log(String("getNumberPresentation: ret=PRESENTATION_ALLOWED"));
    *result = IPhoneConstants::PRESENTATION_ALLOWED;
    return NOERROR;
}

ECode SipConnectionBase::GetUUSInfo(
    /* [out] */ IUUSInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: what's this for SIP?
    if (VDBG) Log(String("getUUSInfo: ? ret=null"));
    *result = NULL;
    return NOERROR;
}

ECode SipConnectionBase::GetPreciseDisconnectCause(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode SipConnectionBase::GetHoldingStartTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHoldingStartTime;
    return NOERROR;
}

ECode SipConnectionBase::GetConnectTimeReal(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mConnectTimeReal;
    return NOERROR;
}

ECode SipConnectionBase::GetOrigConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipConnectionBase::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

void SipConnectionBase::SetState(
    /* [in] */ ICallState state)
{
    if (DBG) Log(String("setState: state=") + StringUtils::ToString(state));
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    switch (state) {
        case ICallState_ACTIVE:
            if (mConnectTime == 0) {
                mConnectTimeReal = SystemClock::GetElapsedRealtime();
                system->GetCurrentTimeMillis(&mConnectTime);
            }
            break;
        case ICallState_DISCONNECTED:
            GetDurationMillis(&mDuration);
            system->GetCurrentTimeMillis(&mDisconnectTime);
            break;
        case ICallState_HOLDING:
            mHoldingStartTime = SystemClock::GetElapsedRealtime();
            break;
        default:
            // Ignore
            break;
    }
}

void SipConnectionBase::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
