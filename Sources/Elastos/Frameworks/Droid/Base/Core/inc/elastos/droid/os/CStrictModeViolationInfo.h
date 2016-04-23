
#ifndef __ELASTOS_DROID_OS_CSTRICTMODEVOILATIONINFO_H__
#define __ELASTOS_DROID_OS_CSTRICTMODEVOILATIONINFO_H__

#include "_Elastos_Droid_Os_CStrictModeViolationInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.App.h>

using Elastos::Droid::App::IApplicationErrorReportCrashInfo;
using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStrictModeViolationInfo)
    , public Object
    , public IStrictModeViolationInfo
{
public:
    CStrictModeViolationInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        // /* [in] */ IThrowable* tr,
        /* [in] */ Int32 policy);

    CARAPI constructor(
        /* [in] */ IParcel* pin);

    CARAPI constructor(
        /* [in] */ IParcel* pin,
        /* [in] */ Boolean unsetGatheringBit);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    /**
     * Save a ViolationInfo instance to a parcel.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    /**
     * Dump a ViolationInfo instance to a Printer.
     */
    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI GetViolationUptimeMillis(
        /* [out] */ Int64* millis);

    CARAPI GetViolationNumThisLoop(
        /* [out] */ Int32* num);

    CARAPI GetNumAnimationsRunning(
        /* [out] */ Int32* num);

    CARAPI GetBroadcastIntentAction(
        /* [out] */ String* action);

    CARAPI GetDurationMillis(
        /* [out] */ Int32* millis);

    CARAPI GetNumInstances(
        /* [out] */ Int64* num);

    CARAPI GetTags(
        /* [out, callee] */ ArrayOf<String>** tags);

    CARAPI GetCrashInfo(
        /* [out] */ IApplicationErrorReportCrashInfo** crashInfo);

public:
    /**
     * Stack and other stuff info.
     */
    AutoPtr<IApplicationErrorReportCrashInfo> mCrashInfo;

    /**
     * The strict mode policy mask at the time of violation.
     */
    Int32 mPolicy;

    /**
     * The wall time duration of the violation, when known.  -1 when
     * not known.
     */
    Int32 mDurationMillis;

    /**
     * The number of animations currently running.
     */
    Int32 mNumAnimationsRunning;

    /**
     * List of tags from active Span instances during this
     * violation, or null for none.
     */
    AutoPtr<ArrayOf<String> > mTags;

    /**
     * Which violation number this was (1-based) since the last Looper loop,
     * from the perspective of the root caller (if it crossed any processes
     * via Binder calls).  The value is 0 if the root caller wasn't on a Looper
     * thread.
     */
    Int32 mViolationNumThisLoop;

    /**
     * The time (in terms of SystemClock.uptimeMillis()) that the
     * violation occurred.
     */
    Int64 mViolationUptimeMillis;

    /**
     * The action of the Intent being broadcast to somebody's onReceive
     * on this thread right now, or null.
     */
    String mBroadcastIntentAction;

    /**
     * If this is a instance count violation, the number of instances in memory,
     * else -1.
     */
    Int64 mNumInstances;

    Object mStateLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODEVOILATIONINFO_H__
