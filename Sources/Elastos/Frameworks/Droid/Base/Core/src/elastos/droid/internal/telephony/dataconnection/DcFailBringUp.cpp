
#include "elastos/droid/internal/telephony/dataconnection/DcFailBringUp.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

const String DcFailBringUp::INTENT_BASE("Elastos.Droid.Internal.Telephony.DataConnection");
const String DcFailBringUp::LOG__TAG("DcFailBringUp");
const Boolean DcFailBringUp::DBG = true;
const String DcFailBringUp::ACTION_FAIL_BRINGUP("action_fail_bringup");
const String DcFailBringUp::COUNTER("counter");
const Int32 DcFailBringUp::DEFAULT_COUNTER = 1;
const String DcFailBringUp::FAIL_CAUSE("fail_cause");
const EnumDcFailCause DcFailBringUp::DEFAULT_FAIL_CAUSE = DcFailCause_ERROR_UNSPECIFIED;
const String DcFailBringUp::SUGGESTED_RETRY_TIME("suggested_retry_time");
const Int32 DcFailBringUp::DEFAULT_SUGGESTED_RETRY_TIME = -1;

ECode DcFailBringUp::SaveParameters(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log(s + ".saveParameters: action=" + intent.getAction());
        mCounter = intent.getIntExtra(COUNTER, DEFAULT_COUNTER);
        mFailCause = DcFailCause.fromInt(
                intent.getIntExtra(FAIL_CAUSE, DEFAULT_FAIL_CAUSE.getErrorCode()));
        mSuggestedRetryTime =
                intent.getIntExtra(SUGGESTED_RETRY_TIME, DEFAULT_SUGGESTED_RETRY_TIME);
        if (DBG) {
            log(s + ".saveParameters: " + this);
        }

#endif
}

ECode DcFailBringUp::SaveParameters(
    /* [in] */ Int32 counter,
    /* [in] */ Int32 failCause,
    /* [in] */ Int32 suggestedRetryTime)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCounter = counter;
        mFailCause = DcFailCause.fromInt(failCause);
        mSuggestedRetryTime = suggestedRetryTime;

#endif
}

ECode DcFailBringUp::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return "{mCounter=" + mCounter +
                " mFailCause=" + mFailCause +
                " mSuggestedRetryTime=" + mSuggestedRetryTime + "}";

#endif
}

ECode DcFailBringUp::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, s);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
