
#include "CCountDownLatch.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CCountDownLatch::Sync::Sync(
    /* [in] */ Int32 count)
{
    SetState(count);
}

Int32 CCountDownLatch::Sync::GetCount()
{
    return GetState();
}

Int32 CCountDownLatch::Sync::TryAcquireShared(
    /* [in] */ Int32 acquires)
{
    return (GetState() == 0) ? 1 : -1;
}

Boolean CCountDownLatch::Sync::TryReleaseShared(
    /* [in] */ Int32 releases)
{
    // Decrement count; signal when transition to zero
    for (;;) {
        Int32 c = GetState();
        if (c == 0) {
            return FALSE;
        }
        Int32 nextc = c - 1;
        if (CompareAndSetState(c, nextc)) {
            return nextc == 0;
        }
    }
}

CAR_INTERFACE_IMPL(CCountDownLatch, Object, ICountDownLatch)

CAR_OBJECT_IMPL(CCountDownLatch)

ECode CCountDownLatch::constructor(
    /* [in] */ Int32 count)
{
    if (count < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSync = new Sync(count);
    return NOERROR;
}

ECode CCountDownLatch::Await()
{
    return mSync->AcquireSharedInterruptibly(1);
}

ECode CCountDownLatch::Await(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    Int64 nas;
    unit->ToNanos(timeout, &nas);
    return mSync->TryAcquireSharedNanos(1, nas, result);
}

ECode CCountDownLatch::CountDown()
{
    mSync->TryReleaseShared(1);
    return NOERROR;
}

ECode CCountDownLatch::GetCount(
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count);

    *count = mSync->GetCount();
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
