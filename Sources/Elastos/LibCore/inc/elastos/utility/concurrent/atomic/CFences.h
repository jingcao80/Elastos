
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CFENCES_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CFENCES_H__

#include "_Elastos_Utility_Concurrent_Atomic_CFences.h"
#include "Singleton.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CFences)
    , public Singleton
    , public IFences
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()
   /**
     * Informally: Ensures that a read of the given reference prior to
     * the invocation of this method occurs before a subsequent use of
     * the given reference with the effect of reading or writing a
     * field (or if an array, element) of the referenced object.  The
     * use of this method is sensible only when paired with other
     * invocations of {@link #orderWrites} and/or {@link
     * #orderAccesses} for the given reference. For details, see the
     * class documentation for this class.
     *
     * @param ref the reference. If null, this method has no effect.
     * @return the given ref, to simplify usage
     */
    CARAPI OrderReads(
        /* [in] */ IInterface* ref,
        /* [out] */ IInterface** outface);

    /**
     * Informally: Ensures that a use of the given reference with the
     * effect of reading or writing a field (or if an array, element)
     * of the referenced object, prior to the invocation of this
     * method occur before a subsequent write of the reference. For
     * details, see the class documentation for this class.
     *
     * @param ref the reference. If null, this method has no effect.
     * @return the given ref, to simplify usage
     */
    CARAPI OrderWrites(
        /* [in] */ IInterface* ref,
        /* [out] */ IInterface** outface);

    /**
     * Informally: Ensures that accesses (reads or writes) using the
     * given reference prior to the invocation of this method occur
     * before subsequent accesses.  For details, see the class
     * documentation for this class.
     *
     * @param ref the reference. If null, this method has no effect.
     * @return the given ref, to simplify usage
     */
    CARAPI OrderAccesses(
        /* [in] */ IInterface* ref,
        /* [out] */ IInterface** outface);

    /**
     * Ensures that the object referenced by the given reference
     * remains <em>strongly reachable</em> (as defined in the {@link
     * java.lang.ref} package documentation), regardless of any prior
     * actions of the program that might otherwise cause the object to
     * become unreachable; thus, the referenced object is not
     * reclaimable by garbage collection at least until after the
     * invocation of this method. Invocation of this method does not
     * itself initiate garbage collection or finalization.
     *
     * <p>See the class-level documentation for further explanation
     * and usage examples.
     *
     * @param ref the reference. If null, this method has no effect.
     */
    CARAPI ReachabilityFence(
        /* [in] */ IInterface* ref);

private:
    /*
     * The methods of this class are intended to be intrinisified by a
     * JVM. However, we provide correct but inefficient Java-level
     * code that simply reads and writes a static volatile
     * variable. Without JVM support, the consistency effects are
     * stronger than necessary, and the memory contention effects can
     * be a serious performance issue.
     */
    static /* volatile */ Int32 sTheVolatile;
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CFENCES_H__
