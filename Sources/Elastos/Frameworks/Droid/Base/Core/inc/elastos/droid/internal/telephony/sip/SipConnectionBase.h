#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCONNECTIONBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCONNECTIONBASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/Connection.h"

// import com.android.internal.telephony.Call;
// import com.android.internal.telephony.PhoneConstants;
// import android.os.SystemClock;
// import android.telephony.DisconnectCause;
// import android.telephony.Rlog;
// import android.telephony.PhoneNumberUtils;

using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::Connection;
using Elastos::Droid::Internal::Telephony::IConnectionPostDialState;
using Elastos::Droid::Internal::Telephony::IPhone;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

class SipConnectionBase
    : public Connection
{
public:
    SipConnectionBase(
        /* [in] */ const String& dialString);

    // @Override
    CARAPI GetCreateTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetConnectTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetDisconnectTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetDurationMillis(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetHoldDurationMillis(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetDisconnectCause(
        /* [out] */ Int32* result);

    virtual CARAPI SetDisconnectCause(
        /* [in] */ Int32 cause);

    // @Override
    CARAPI GetPostDialState(
        /* [out] */ IConnectionPostDialState* result);

    // @Override
    CARAPI ProceedAfterWaitChar();

    // @Override
    CARAPI ProceedAfterWildChar(
        /* [in] */ const String& str);

    // @Override
    CARAPI CancelPostDial();

    // @Override
    CARAPI GetRemainingPostDialString(
        /* [out] */ String* result);

    // @Override
    CARAPI GetNumberPresentation(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetUUSInfo(
        /* [out] */ IUUSInfo** result);

    // @Override
    CARAPI GetPreciseDisconnectCause(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetHoldingStartTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetConnectTimeReal(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetOrigConnection(
        /* [out] */ IConnection** result);

    // @Override
    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

protected:
    virtual CARAPI_(void) SetState(
        /* [in] */ ICallState state);

    virtual CARAPI_(AutoPtr<IPhone>) GetPhone() = 0;

private:
    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String LOGTAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    // STOPSHIP if true
    String mPostDialString;
    // outgoing calls only
    Int32 mNextPostDialChar;
    // index into postDialString
    /*
      * These time/timespan values are based on System.currentTimeMillis(),
      * i.e., "wall clock" time.
      */
    Int64 mCreateTime;
    Int64 mConnectTime;
    Int64 mDisconnectTime;
    /*
      * These time/timespan values are based on SystemClock.elapsedRealTime(),
      * i.e., time since boot.  They are appropriate for comparison and
      * calculating deltas.
      */
    Int64 mConnectTimeReal;
    Int64 mDuration;
    Int64 mHoldingStartTime;
    // The time when the Connection last transitioned
    // into HOLDING
    Int32 mCause;
    IConnectionPostDialState mPostDialState;
};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_SIPCONNECTIONBASE_H__
