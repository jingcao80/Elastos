#ifndef __ELASTOS_DROID_OS_CSTRICTMODEVMPOLICYBUILDER_H__
#define __ELASTOS_DROID_OS_CSTRICTMODEVMPOLICYBUILDER_H__

#include "_Elastos_Droid_Os_CStrictModeVmPolicyBuilder.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStrictModeVmPolicyBuilder)
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IStrictModeVmPolicy* policy);

    /**
     * Set an upper bound on how many instances of a class can be in memory
     * at once.  Helps to prevent object leaks.
     */
    CARAPI SetClassInstanceLimit(
        /* [in] */ Handle32 klass,
        /* [in] */ Int32 instanceLimit);

    /**
     * Detect leaks of {@link android.app.Activity} subclasses.
     */
    CARAPI DetectActivityLeaks();

    /**
     * Detect everything that's potentially suspect.
     *
     * <p>In the Honeycomb release this includes leaks of
     * SQLite cursors, Activities, and other closable objects
     * but will likely expand in future releases.
     */
    CARAPI DetectAll();
    /**
     * Detect when an
     * {@link android.database.sqlite.SQLiteCursor} or other
     * SQLite object is finalized without having been closed.
     *
     * <p>You always want to explicitly close your SQLite
     * cursors to avoid unnecessary database contention and
     * temporary memory leaks.
     */
    CARAPI DetectLeakedSqlLiteObjects();

    /**
     * Detect when an {@link java.io.Closeable} or other
     * object with a explict termination method is finalized
     * without having been closed.
     *
     * <p>You always want to explicitly close such objects to
     * avoid unnecessary resources leaks.
     */
    CARAPI DetectLeakedClosableObjects();

    /**
     * Detect when a {@link BroadcastReceiver} or
     * {@link ServiceConnection} is leaked during {@link Context}
     * teardown.
     */
    CARAPI DetectLeakedRegistrationObjects();

    /**
     * Crashes the whole process on violation.  This penalty runs at
     * the end of all enabled penalties so yo you'll still get
     * your logging or other violations before the process dies.
     */
    CARAPI PenaltyDeath();

    /**
     * Log detected violations to the system log.
     */
    CARAPI PenaltyLog();
    /**
     * Enable detected violations log a stacktrace and timing data
     * to the {@link android.os.DropBoxManager DropBox} on policy
     * violation.  Intended mostly for platform integrators doing
     * beta user field data collection.
     */
    CARAPI PenaltyDropBox();


    /**
     * Construct the VmPolicy instance.
     *
     * <p>Note: if no penalties are enabled before calling
     * <code>build</code>, {@link #penaltyLog} is implicitly
     * set.
     */
    CARAPI Build(
        /* [out] */ IStrictModeVmPolicy** policy);

private:
    CARAPI Enable(
        /* [in] */ Int32 bit);

private:
    Int32 mMask;

    HashMap<Handle32, Int32 > mClassInstanceLimit;  // null until needed
    Boolean mClassInstanceLimitNeedCow;             // need copy-on-write
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODEVMPOLICYBUILDER_H__
