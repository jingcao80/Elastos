
#include "CLockSupport.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

CAR_INTERFACE_IMPL(CLockSupport, Singleton, ILockSupport)

CAR_SINGLETON_IMPL(CLockSupport)

ECode CLockSupport::Unpark(
    /* [in] */ IThread* thread)
{
    return LockSupport::Unpark(thread);
}

ECode CLockSupport::Park(
    /* [in] */ IInterface* blocker)
{
    return LockSupport::Park(blocker);
}

ECode CLockSupport::ParkNanos(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 nanos)
{
    return LockSupport::ParkNanos(blocker, nanos);
}

ECode CLockSupport::ParkUntil(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 deadline)
{
    return LockSupport::ParkUntil(blocker, deadline);
}

ECode CLockSupport::Park()
{
    return LockSupport::Park();
}

ECode CLockSupport::ParkNanos(
    /* [in] */ Int64 nanos)
{
    return LockSupport::ParkNanos(nanos);
}

ECode CLockSupport::ParkUntil(
    /* [in] */ Int64 deadline)
{
    return LockSupport::ParkUntil(deadline);
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
