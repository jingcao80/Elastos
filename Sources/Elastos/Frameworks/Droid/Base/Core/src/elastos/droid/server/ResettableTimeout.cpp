
#include "elastos/droid/server/ResettableTimeout.h"
#include "elastos/droid/os/ConditionVariable.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::ConditionVariable;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Server {

//=====================================
//ResettableTimeout::T
//=====================================

ResettableTimeout::T::T(
    /* [in] */ ResettableTimeout* host)
    : mHost(host)
{
}

ECode ResettableTimeout::T::Run()
{
    mHost->mLock->Open();
    while (TRUE) {
        Int64 diff;
        synchronized(this) {
            diff = mHost->mOffAt - SystemClock::GetUptimeMillis();
            if (diff <= 0) {
                mHost->mOffCalled = TRUE;
                mHost->Off();
                mHost->mThread = NULL;
                break;
            }
        }
        ECode ec = this->Sleep(diff);
        if (FAILED(ec)) {
            return E_INTERRUPTED_EXCEPTION;
        }
    }
    return NOERROR;
}

//=====================================
//ResettableTimeout
//=====================================

CAR_INTERFACE_IMPL(ResettableTimeout, Object, IResettableTimeout)

ResettableTimeout::ResettableTimeout()
    : mOffAt(0L)
    , mOffCalled(FALSE)
{
    AutoPtr<ConditionVariable> _mLock = new ConditionVariable();
    mLock = (IConditionVariable*)_mLock.Get();
}

ECode ResettableTimeout::Go(
    /* [in] */ Int64 milliseconds)
{
    synchronized(this) {
        mOffAt = SystemClock::GetUptimeMillis() + milliseconds;

        Boolean alreadyOn;

        // By starting the thread first and waiting, we ensure that if the
        // thread to stop it can't start, we don't turn the vibrator on
        // forever.  This still isn't really sufficient, because we don't
        // have another processor watching us.  We really should have a
        // service for this in case our process crashes.
        if (mThread == NULL) {
            alreadyOn = FALSE;
            mLock->Close();
            AutoPtr<T> _mThread = new T(this);
            mThread = (Thread*)_mThread.Get();
            mThread->Start();
            mLock->Block();
            mOffCalled = FALSE;
        } else {
            alreadyOn = TRUE;
            // poke the thread so it gets the new timeout.
            mThread->Interrupt();
        }
        On(alreadyOn);
    }
    return NOERROR;
}

ECode ResettableTimeout::Cancel()
{
    synchronized(this) {
        mOffAt = 0;
        if (mThread != NULL) {
            mThread->Interrupt();
            mThread = NULL;
        }
        if (!mOffCalled) {
            mOffCalled = TRUE;
            Off();
        }
    }
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos