
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICSTAMPEDREFRERNCE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICSTAMPEDREFRERNCE_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicStampedReference.h"
#include "Object.h"


namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicStampedReference)
    , public Object
    , public IAtomicStampedReference
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new {@code AtomicStampedReference} with the given
     * initial values.
     *
     * @param initialRef the initial reference
     * @param initialStamp the initial stamp
     */
    CARAPI constructor(
        /* [in] */ IInterface* initialRef,
        /* [in] */ Int32 initialStamp);

    /**
     * Returns the current value of the reference.
     *
     * @return the current value of the reference
     */
    CARAPI GetReference(
        /* [out] */ IInterface** outface);

    /**
     * Returns the current value of the stamp.
     *
     * @return the current value of the stamp
     */
    CARAPI GetStamp(
        /* [out] */ Int32* value);

    /**
     * Returns the current values of both the reference and the stamp.
     * Typical usage is {@code int[1] holder; ref = v.get(holder); }.
     *
     * @param stampHolder an array of size of at least one.  On return,
     * {@code stampholder[0]} will hold the value of the stamp.
     * @return the current value of the reference
     */
    CARAPI Get(
        /* [in] */ ArrayOf<Int32>* stampHolder,
        /* [out] */ IInterface** outface);

    /**
     * Atomically sets the value of both the reference and stamp
     * to the given update values if the
     * current reference is {@code ==} to the expected reference
     * and the current stamp is equal to the expected stamp.
     *
     * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
     * and does not provide ordering guarantees, so is only rarely an
     * appropriate alternative to {@code compareAndSet}.
     *
     * @param expectedReference the expected value of the reference
     * @param newReference the new value for the reference
     * @param expectedStamp the expected value of the stamp
     * @param newStamp the new value for the stamp
     * @return true if successful
     */
    CARAPI WeakCompareAndSet(
        /* [in] */ IInterface* expectedReference,
        /* [in] */ IInterface* newReference,
        /* [in] */ Int32 expectedStamp,
        /* [in] */ Int32 newStamp,
        /* [out] */ Boolean* value);

    /**
     * Atomically sets the value of both the reference and stamp
     * to the given update values if the
     * current reference is {@code ==} to the expected reference
     * and the current stamp is equal to the expected stamp.
     *
     * @param expectedReference the expected value of the reference
     * @param newReference the new value for the reference
     * @param expectedStamp the expected value of the stamp
     * @param newStamp the new value for the stamp
     * @return true if successful
     */
    CARAPI CompareAndSet(
        /* [in] */ IInterface* expectedReference,
        /* [in] */ IInterface* newReference,
        /* [in] */ Int32 expectedStamp,
        /* [in] */ Int32 newStamp,
        /* [out] */ Boolean* value);


    /**
     * Unconditionally sets the value of both the reference and stamp.
     *
     * @param newReference the new value for the reference
     * @param newStamp the new value for the stamp
     */
    CARAPI Set(
        /* [in] */ IInterface* newReference,
        /* [in] */ Int32 newStamp);

    /**
     * Atomically sets the value of the stamp to the given update value
     * if the current reference is {@code ==} to the expected
     * reference.  Any given invocation of this operation may fail
     * (return {@code false}) spuriously, but repeated invocation
     * when the current value holds the expected value and no other
     * thread is also attempting to set the value will eventually
     * succeed.
     *
     * @param expectedReference the expected value of the reference
     * @param newStamp the new value for the stamp
     * @return true if successful
     */
    CARAPI AttemptStamp(
        /* [in] */ IInterface* expectedReference,
        /* [in] */ Int32 newStamp,
        /* [out] */ Boolean* value);

private:
    class Pair {
    private:
        Pair(
            /* [in] */ IInterface* reference,
            /* [in] */ Int32 stamp)
        {
            mReference = reference;
            mStamp = stamp;
        }

    public:
        static Pair* Of(
            /* [in] */ IInterface* reference,
            /* [in] */ Int32 stamp)
        {
            Pair * res = new Pair(reference, stamp);
            return res;
        }

    public:
        AutoPtr<IInterface> mReference;
        Int32 mStamp;
    };

private:
    CARAPI_(Boolean) CasPair(
        /* [in] */ Pair* cmp,
        /* [in] */ Pair* val);

private:
    /* volatile */ Pair* mPair;
    // Unsafe mechanics

    // private static final sun.misc.Unsafe UNSAFE = sun.misc.Unsafe.getUnsafe();
    static const Int64 mPairOffset; // = objectFieldOffset(UNSAFE, "pair", AtomicStampedReference.class);

    // static long objectFieldOffset(sun.misc.Unsafe UNSAFE,
    //                               String field, Class<?> klazz) {
    //     try {
    //         return UNSAFE.objectFieldOffset(klazz.getDeclaredField(field));
    //     } catch (NoSuchFieldException e) {
    //         // Convert Exception to corresponding Error
    //         NoSuchFieldError error = new NoSuchFieldError(field);
    //         error.initCause(e);
    //         throw error;
    //     }
    // }
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICSTAMPEDREFRERNCE_H__
