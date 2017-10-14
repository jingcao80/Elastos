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

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/hardware/camera2/utils/CloseableLock.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuffer;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

const Boolean CloseableLock::VERBOSE = FALSE;

pthread_key_t CloseableLock::sTlsKey;
pthread_once_t CloseableLock::sKeyOnce = PTHREAD_ONCE_INIT;

CAR_INTERFACE_IMPL_2(CloseableLock::ScopedLock, Object, ICloseableLockScopedLock, ICloseable)

static void MakeKey()
{
    ASSERT_TRUE(pthread_key_create(&CloseableLock::sTlsKey, NULL) == 0)
}

CloseableLock::ScopedLock::ScopedLock(
    /* [in] */ CloseableLock* host)
    : mHost(host)
{
}

ECode CloseableLock::ScopedLock::Close()
{
    return mHost->ReleaseLock();
}

CAR_INTERFACE_IMPL_2(CloseableLock, Object, ICloseableLock, ICloseable)

CloseableLock::CloseableLock()
    : TAG("CloseableLock")
    , mName(NULL)
    , mClosed(FALSE)
    , mExclusive(FALSE)
    , mSharedLocks(0)
{
    CReentrantLock::New((IReentrantLock**)&mLock);
    ILock::Probe(mLock)->NewCondition((ICondition**)&mCondition);
}

ECode CloseableLock::constructor()
{
    mName = "";
    return NOERROR;
}

ECode CloseableLock::constructor(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CloseableLock::Close()
{
    if (mClosed) {
        if (VERBOSE) {
            Logger::D(TAG, "close - already closed; ignoring");
        }
        return NOERROR;
    }

    AutoPtr<ICloseableLockScopedLock> scoper;
    AcquireExclusiveLock((ICloseableLockScopedLock**)&scoper);
    // Already closed by another thread?
    if (scoper == NULL) {
        return NOERROR;
    }
    else {
        pthread_once(&sKeyOnce, MakeKey);
        Int32 lockCount = (Int32)pthread_getspecific(sTlsKey);

        if (lockCount != 1) {
            // Future: may want to add a #releaseAndClose to allow this.
            // throw new IllegalStateException(
            //         "Cannot close while one or more acquired locks are being held by this " +
            //          "thread; release all other locks first");
            Logger::E(TAG, "Cannot close while one or more acquired locks are being held by this thread; release all other locks first");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    //try {
    ILock::Probe(mLock)->Lock();

    mClosed = TRUE;
    mExclusive = FALSE;
    mSharedLocks = 0;
    assert(0);
    // /mLockCount->Remove();

    // Notify all threads that are waiting to unblock and return immediately
    mCondition->SignalAll();
    //} finally {
    ILock::Probe(mLock)->UnLock();
    //}

    if (VERBOSE) {
        Logger::D(TAG, "close - completed");
    }
    return NOERROR;
}

ECode CloseableLock::AcquireLock(
    /* [out] */ ICloseableLockScopedLock** outsl)
{
    VALIDATE_NOT_NULL(outsl);
    *outsl = NULL;

    Int32 ownedLocks;

    //try {
    ILock::Probe(mLock)->Lock();

    // Lock is already closed, all further acquisitions will fail
    if (mClosed) {
        if (VERBOSE) {
            Logger::D(TAG, "acquire lock early aborted (already closed)");
        }
        *outsl = NULL;
        ILock::Probe(mLock)->UnLock();
        return NOERROR;
    }

    pthread_once(&sKeyOnce, MakeKey);
    ownedLocks = (Int32)pthread_getspecific(sTlsKey);

    // This thread is already holding an exclusive lock
    if (mExclusive && ownedLocks > 0) {
        // throw new IllegalStateException(
        //         "Cannot acquire shared lock while holding exclusive lock");
        Logger::E(TAG, "Cannot acquire shared lock while holding exclusive lock");
        ILock::Probe(mLock)->UnLock();
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Is another thread holding the exclusive lock? Block until we can get in.
    while (mExclusive) {
        mCondition->AwaitUninterruptibly();

        // Did another thread #close while we were waiting? Unblock immediately.
        if (mClosed) {
            if (VERBOSE) {
                Logger::D(TAG, "acquire lock unblocked aborted (already closed)");
            }
            *outsl = NULL;
            ILock::Probe(mLock)->UnLock();
            return NOERROR;
        }
    }

    mSharedLocks++;

    pthread_once(&sKeyOnce, MakeKey);
    ownedLocks = (Int32)pthread_getspecific(sTlsKey) + 1;
    ASSERT_TRUE(pthread_setspecific(sTlsKey, (void*)ownedLocks) == 0)

    //} finally {
    ILock::Probe(mLock)->UnLock();
    //}

    if (VERBOSE) {
        Logger::D(TAG, "acquired lock (local own count = %d )", ownedLocks);
    }

    AutoPtr<ICloseableLockScopedLock> _lock = new ScopedLock(this);
    *outsl = _lock;
    REFCOUNT_ADD(*outsl);
    return NOERROR;
}

ECode CloseableLock::AcquireExclusiveLock(
    /* [out] */ ICloseableLockScopedLock** outsl)
{
    VALIDATE_NOT_NULL(outsl);
    *outsl = NULL;

    Int32 ownedLocks;

    //try {
    ILock::Probe(mLock)->Lock();

    // Lock is already closed, all further acquisitions will fail
    if (mClosed) {
        if (VERBOSE) {
            Logger::D(TAG, "acquire exclusive lock early aborted (already closed)");
        }
        *outsl = NULL;
        ILock::Probe(mLock)->UnLock();
        return NOERROR;
    }

    pthread_once(&sKeyOnce, MakeKey);
    ownedLocks = (Int32)pthread_getspecific(sTlsKey);
    // This thread is already holding a shared lock
    if (!mExclusive && ownedLocks > 0) {
        // throw new IllegalStateException(
        //         "Cannot acquire exclusive lock while holding shared lock");
        Logger::E(TAG, "Cannot acquire exclusive lock while holding shared lock");
        ILock::Probe(mLock)->UnLock();
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    /*
     * Is another thread holding the lock? Block until we can get in.
     *
     * If we are already holding the lock, always let it through since
     * we are just reentering the exclusive lock.
     */
    while (ownedLocks == 0 && (mExclusive || mSharedLocks > 0)) {
        mCondition->AwaitUninterruptibly();

     // Did another thread #close while we were waiting? Unblock immediately.
        if (mClosed) {
            if (VERBOSE) {
                Logger::D(TAG, "acquire exclusive lock unblocked aborted (already closed)");
            }
            *outsl = NULL;
            ILock::Probe(mLock)->UnLock();
            return NOERROR;
        }
    }

    mExclusive = TRUE;

    pthread_once(&sKeyOnce, MakeKey);
    ownedLocks = (Int32)pthread_getspecific(sTlsKey) + 1;
    ASSERT_TRUE(pthread_setspecific(sTlsKey, (void*)ownedLocks) == 0)
    //} finally {
    ILock::Probe(mLock)->UnLock();
    //}

    if (VERBOSE) {
        Logger::D(TAG, "acquired exclusive lock (local own count = %d )", ownedLocks);
    }

    AutoPtr<ICloseableLockScopedLock> _lock = new ScopedLock(this);
    *outsl = _lock;
    REFCOUNT_ADD(*outsl);
    return NOERROR;
}

ECode CloseableLock::ReleaseLock()
{
    pthread_once(&sKeyOnce, MakeKey);
    Int32 lockCount1 = (Int32)pthread_getspecific(sTlsKey);

    if (lockCount1 <= 0) {
        // throw new IllegalStateException(
        //         "Cannot release lock that was not acquired by this thread");
        Logger::E(TAG, "Cannot release lock that was not acquired by this thread");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 ownedLocks;

    //try {
    ILock::Probe(mLock)->Lock();

    // Lock is already closed, it couldn't have been acquired in the first place
    if (mClosed) {
        //throw new IllegalStateException("Do not release after the lock has been closed");
        Logger::E(TAG, "Do not release after the lock has been closed");
        ILock::Probe(mLock)->UnLock();
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (!mExclusive) {
        mSharedLocks--;
    }
    else {
        if (mSharedLocks != 0) {
            //throw new AssertionError("Too many shared locks " + mSharedLocks);
            Logger::E(TAG, "Too many shared locks %d", mSharedLocks);
            ILock::Probe(mLock)->UnLock();
            return E_ASSERTION_ERROR;;
        }
    }

    pthread_once(&sKeyOnce, MakeKey);
    ownedLocks = (Int32)pthread_getspecific(sTlsKey) - 1;
    ASSERT_TRUE(pthread_setspecific(sTlsKey, (void*)ownedLocks) == 0)


    if (ownedLocks == 0 && mExclusive) {
        // Wake up any threads that might be waiting for the exclusive lock to be released
        mExclusive = FALSE;
        mCondition->SignalAll();
    }
    else if (ownedLocks == 0 && mSharedLocks == 0) {
        // Wake up any threads that might be trying to get the exclusive lock
        mCondition->SignalAll();
    }
    //} finally {
    ILock::Probe(mLock)->UnLock();
    //}

    if (VERBOSE) {
        Logger::D(TAG, "released lock (local lock count %d)", ownedLocks);
    }
    return NOERROR;
}

ECode CloseableLock::Log(
    /* [in] */ const String& what)
{
    StringBuffer sb;
    sb += TAG;
    sb += "[";
    sb += mName;
    sb += "]";
    Logger::V(sb.ToString(), what);
    return NOERROR;
}


} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
