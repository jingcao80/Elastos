
#include "LockSupport.h"
#include <Thread.h>

using Elastos::Core::Thread;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

const Int64 LockSupport::mParkBlockerOffset = 0;

ECode LockSupport::Unpark(
    /* [in] */ IThread* thread)
{
    if (thread != NULL) {
        return thread->Unpark();
    }
    return NOERROR;
}

ECode LockSupport::Park(
    /* [in] */ IInterface* blocker)
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    // TODO:
    // setBlocker(t, blocker);
    return t->ParkFor(0);
    // setBlocker(t, null);
}

ECode LockSupport::ParkNanos(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 nanos)
{
    if (nanos > 0) {
        AutoPtr<IThread> t = Thread::GetCurrentThread();
        // TODO:
        // setBlocker(t, blocker);
        return t->ParkFor(nanos);
        // setBlocker(t, null);
    }
    return NOERROR;
}

ECode LockSupport::ParkUntil(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 deadline)
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    SetBlocker(t, blocker);
    assert(0 && "TODO");
    // unsafe.park(true, deadline);
    SetBlocker(t, NULL);
    return NOERROR;
}

AutoPtr<IInterface> LockSupport::GetBlocker(
    /* [in] */ IThread* t)
{
    if (t == NULL) {
        // throw new NullPointerException();
        return NULL;
    }
    assert(0 && "TODO");
    return NULL;
    // return unsafe.getObjectVolatile(t, parkBlockerOffset);
}

ECode LockSupport::Park()
{
    assert(0 && "TODO");
    // unsafe.park(false, 0L);
    return NOERROR;
}

ECode LockSupport::ParkNanos(
    /* [in] */ Int64 nanos)
{
    assert(0 && "TODO");
    // if (nanos > 0)
    //     unsafe.park(false, nanos);
    return NOERROR;
}

ECode LockSupport::ParkUntil(
    /* [in] */ Int64 deadline)
{
    // unsafe.park(true, deadline);
    assert(0 && "TODO");
    return NOERROR;
}

LockSupport::LockSupport()
{
}

ECode LockSupport::SetBlocker(
    /* [in] */ IThread* t,
    /* [in] */ IInterface* arg)
{
    // // Even though volatile, hotspot doesn't need a write barrier here.
    // unsafe.putObject(t, parkBlockerOffset, arg);
    assert(0 && "TODO");
    return NOERROR;
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
