
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICREFERENCE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICREFERENCE_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicReference.h"
#include "AtomicReference.h"

using Elastos::Utility::Concurrent::Atomic::AtomicReference;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicReference)
    , public AtomicReference
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICREFERENCE_H__
