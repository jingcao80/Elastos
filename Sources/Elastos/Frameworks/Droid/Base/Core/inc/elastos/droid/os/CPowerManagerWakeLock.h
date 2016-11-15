
#ifndef __ELASTOS_DROID_OS_CPOWERMANAGERWAKELOCK_H__
#define __ELASTOS_DROID_OS_CPOWERMANAGERWAKELOCK_H__

#include "_Elastos_Droid_Os_CPowerManagerWakeLock.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Os {

class CPowerManager;

/**
 * A wake lock is a mechanism to indicate that your application needs
 * to have the device stay on.
 * <p>
 * Any application using a WakeLock must request the {@code android.permission.WAKE_LOCK}
 * permission in an {@code &lt;uses-permission&gt;} element of the application's manifest.
 * Obtain a wake lock by calling {@link PowerManager#newWakeLock(int, String)}.
 * </p><p>
 * Call {@link #acquire()} to acquire the wake lock and force the device to stay
 * on at the level that was requested when the wake lock was created.
 * </p><p>
 * Call {@link #release()} when you are done and don't need the lock anymore.
 * It is very important to do this as soon as possible to avoid running down the
 * device's battery excessively.
 * </p>
 */
CarClass(CPowerManagerWakeLock)
    , public Object
    , public IPowerManagerWakeLock
{
private:
    class ReleaseRunnable
        : public Runnable
    {
    public:
        ReleaseRunnable(
            /* [in] */ CPowerManagerWakeLock* host);

        CARAPI Run();

    private:
        CPowerManagerWakeLock* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPowerManagerWakeLock();

    ~CPowerManagerWakeLock();
    /**
     * Sets whether this WakeLock is reference counted.
     * <p>
     * Wake locks are reference counted by default.  If a wake lock is
     * reference counted, then each call to {@link #acquire()} must be
     * balanced by an equal number of calls to {@link #release()}.  If a wake
     * lock is not reference counted, then one call to {@link #release()} is
     * sufficient to undo the effect of all previous calls to {@link #acquire()}.
     * </p>
     *
     * @param value True to make the wake lock reference counted, false to
     * make the wake lock non-reference counted.
     */
    CARAPI SetReferenceCounted(
        /* [in] */ Boolean value);

    /**
     * Acquires the wake lock.
     * <p>
     * Ensures that the device is on at the level requested when
     * the wake lock was created.
     * </p>
     */
    CARAPI AcquireLock();

    /**
     * Acquires the wake lock with a timeout.
     * <p>
     * Ensures that the device is on at the level requested when
     * the wake lock was created.  The lock will be released after the given timeout
     * expires.
     * </p>
     *
     * @param timeout The timeout after which to release the wake lock, in milliseconds.
     */
    CARAPI AcquireLock(
        /* [in] */ Int64 timeout);

    /**
     * Releases the wake lock.
     * <p>
     * This method releases your claim to the CPU or screen being on.
     * The screen may turn off shortly after you release the wake lock, or it may
     * not if there are other wake locks still held.
     * </p>
     */
    CARAPI ReleaseLock();

    /**
     * Releases the wake lock with flags to modify the release behavior.
     * <p>
     * This method releases your claim to the CPU or screen being on.
     * The screen may turn off shortly after you release the wake lock, or it may
     * not if there are other wake locks still held.
     * </p>
     *
     * @param flags Combination of flag values to modify the release behavior.
     * Currently only {@link #WAIT_FOR_PROXIMITY_NEGATIVE} is supported.
     *
     * {@hide}
     */
     CARAPI ReleaseLock(
        /* [in] */ Int32 flags);

    /**
     * Returns true if the wake lock has been acquired but not yet released.
     *
     * @return True if the wake lock is held.
     */
    CARAPI IsHeld(
        /* [out] */ Boolean* isHeld);

    /**
     * Sets the work source associated with the wake lock.
     * <p>
     * The work source is used to determine on behalf of which application
     * the wake lock is being held.  This is useful in the case where a
     * service is performing work on behalf of an application so that the
     * cost of that work can be accounted to the application.
     * </p>
     *
     * @param ws The work source, or null if none.
     */
    CARAPI SetWorkSource(
        /* [in] */ IWorkSource* ws);

    CARAPI ToString(
        /* [out] */ String* s);

    /** @hide */
    CARAPI SetTag(
        /* [in] */ const String& tag);

    /** @hide */
    CARAPI SetHistoryTag(
        /* [in] */ const String& tag);

    /** @hide */
    CARAPI SetUnimportantForLogging(
        /* [in] */ Boolean state);

    CARAPI constructor(
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ IPowerManager* host);

private:
    CARAPI_(void) AcquireLocked();

private:
    Int32 mFlags;
    String mTag;
    String mPackageName;
    AutoPtr<IBinder> mToken;
    Object mTokenLock;
    Int32 mCount;
    Boolean mRefCounted;
    Boolean mHeld;
    AutoPtr<IWorkSource> mWorkSource;
    AutoPtr<IRunnable> mReleaser;
    AutoPtr<CPowerManager> mPm;   // hold powermanager
    String mHistoryTag;
    String mTraceName;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPOWERMANAGERWAKELOCK_H__
