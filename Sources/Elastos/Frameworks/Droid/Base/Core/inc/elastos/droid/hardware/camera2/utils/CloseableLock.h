//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CLOSEABLELOCK_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CLOSEABLELOCK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::Utility::Concurrent::Locks::ICondition;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class CloseableLock
    : public Object
    , public ICloseableLock
    , public ICloseable
{
public:
    /**
     * Helper class to release a lock at the end of a try-with-resources statement.
     */
    class ScopedLock
        : public Object
        , public ICloseableLockScopedLock
        , public ICloseable
    {
    public:
        CAR_INTERFACE_DECL()

        ScopedLock(
            /* [in] */ CloseableLock* host);

        /** Release the lock with {@link CloseableLock#releaseLock}. */
        //@Override
        CARAPI Close();

    private:
        ScopedLock() {}

    private:
        CloseableLock* mHost;
    };
public:
    CAR_INTERFACE_DECL()


    CloseableLock();

    virtual ~CloseableLock() {}

    /**
     * Create a new instance; starts out with 0 locks acquired.
     */
    CARAPI constructor();

    /**
     * Create a new instance; starts out with 0 locks acquired.
     *
     * @param name set an optional name for logging functionality
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Acquires the lock exclusively (blocking), marks it as closed, then releases the lock.
     *
     * <p>Marking a lock as closed will fail all further acquisition attempts;
     * it will also immediately unblock all other threads currently trying to acquire a lock.</p>
     *
     * <p>This operation is idempotent; calling it more than once has no effect.</p>
     *
     * @throws IllegalStateException
     *          if an attempt is made to {@code close} while this thread has a lock acquired
     */
    //@Override
    CARAPI Close();

    /**
     * Try to acquire the lock non-exclusively, blocking until the operation completes.
     *
     * <p>If the lock has already been closed, or being closed before this operation returns,
     * the call will immediately return {@code false}.</p>
     *
     * <p>If other threads hold a non-exclusive lock (and the lock is not yet closed),
     * this operation will return immediately. If another thread holds an exclusive lock,
     * this thread will block until the exclusive lock has been released.</p>
     *
     * <p>This lock is re-entrant; acquiring more than one non-exclusive lock per thread is
     * supported, and must be matched by an equal number of {@link #releaseLock} calls.</p>
     *
     * @return {@code ScopedLock} instance if the lock was acquired, or {@code null} if the lock
     *         was already closed.
     *
     * @throws IllegalStateException if this thread is already holding an exclusive lock
     */
    CARAPI AcquireLock(
        /* [out] */ ICloseableLockScopedLock** outsl);

    /**
     * Try to acquire the lock exclusively, blocking until all other threads release their locks.
     *
     * <p>If the lock has already been closed, or being closed before this operation returns,
     * the call will immediately return {@code false}.</p>
     *
     * <p>If any other threads are holding a lock, this thread will block until all
     * other locks are released.</p>
     *
     * <p>This lock is re-entrant; acquiring more than one exclusive lock per thread is supported,
     * and must be matched by an equal number of {@link #releaseLock} calls.</p>
     *
     * @return {@code ScopedLock} instance if the lock was acquired, or {@code null} if the lock
     *         was already closed.
     *
     * @throws IllegalStateException
     *          if an attempt is made to acquire an exclusive lock while already holding a lock
     */
    CARAPI AcquireExclusiveLock(
        /* [out] */ ICloseableLockScopedLock** outsl);

    /**
     * Release a single lock that was acquired.
     *
     * <p>Any other other that is blocked and trying to acquire a lock will get a chance
     * to acquire the lock.</p>
     *
     * @throws IllegalStateException if no locks were acquired, or if the lock was already closed
     */
    CARAPI ReleaseLock();

private:
    CARAPI Log(
        /* [in] */ const String& what);

public:
    /** How many times the current thread is holding the lock */
    // private final ThreadLocal<Integer> mLockCount =
    //     new ThreadLocal<Integer>() {
    //         @Override protected Integer initialValue() {
    //             return 0;
    //         }
    //     };
    static pthread_once_t sKeyOnce;
    static pthread_key_t sTlsKey;

private:
    static const Boolean VERBOSE;

    const String TAG;
    String mName;

    /*volatile*/ Boolean mClosed;

    /** If an exclusive lock is acquired by some thread. */
    Boolean mExclusive;
    /**
     * How many shared locks are acquired by any thread:
     *
     * <p>Reentrant locking increments this. If an exclusive lock is held,
     * this value will stay at 0.</p>
     */
    Int32 mSharedLocks;

    AutoPtr<IReentrantLock> mLock;
    /** This condition automatically releases mLock when waiting; re-acquiring it after notify */
    AutoPtr<ICondition> mCondition;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CLOSEABLELOCK_H__
