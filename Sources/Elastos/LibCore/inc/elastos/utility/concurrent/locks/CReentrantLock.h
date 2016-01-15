
#ifndef __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CREENTRANTLOCK_H__
#define __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CREENTRANTLOCK_H__

#include "_Elastos_Utility_Concurrent_Locks_CReentrantLock.h"
#include "ReentrantLock.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

CarClass(CReentrantLock), public ReentrantLock
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CREENTRANTLOCK_H__
