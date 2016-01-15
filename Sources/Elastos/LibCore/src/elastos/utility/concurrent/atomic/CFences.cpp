
#include "CFences.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

Int32 CFences::sTheVolatile = 0;

CAR_INTERFACE_IMPL(CFences, Singleton, IFences)

CAR_SINGLETON_IMPL(CFences)

ECode CFences::OrderReads(
    /* [in] */ IInterface* ref,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    Int32 ignore = sTheVolatile;
    *outface = ref;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CFences::OrderWrites(
    /* [in] */ IInterface* ref,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    sTheVolatile = 0;
    *outface = ref;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CFences::OrderAccesses(
    /* [in] */ IInterface* ref,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    sTheVolatile = 0;
    *outface = ref;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CFences::ReachabilityFence(
    /* [in] */ IInterface* ref)
{
    if (ref != NULL) {
        assert(0 && "TODO");
        // synchronized (ref) {}
    }
    return NOERROR;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
