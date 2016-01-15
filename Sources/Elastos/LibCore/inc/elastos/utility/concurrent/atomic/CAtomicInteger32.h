
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER32_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER32_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicInteger32.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicInteger32)
    , public Object
    , public IAtomicInteger32
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new AtomicInteger with the given initial value.
     *
     * @param initialValue the initial value
     */
    CARAPI constructor(
        /* [in] */ Int32 initialValue);

    /**
     * Creates a new AtomicInteger with initial value {@code 0}.
     */
    CARAPI constructor();

    /**
     * Gets the current value.
     *
     * @return the current value
     */
    CARAPI Get(
        /* [out] */ Int32* value);

    /**
     * Sets to the given value.
     *
     * @param newValue the new value
     */
    CARAPI Set(
        /* [in] */ Int32 newValue);

    /**
     * Eventually sets to the given value.
     *
     * @param newValue the new value
     * @since 1.6
     */
    CARAPI LazySet(
        /* [in] */ Int32 newValue);

    /**
     * Atomically sets to the given value and returns the old value.
     *
     * @param newValue the new value
     * @return the previous value
     */
    CARAPI GetAndSet(
        /* [in] */ Int32 newValue,
        /* [out] */ Int32* value);

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     *
     * @param expect the expected value
     * @param update the new value
     * @return true if successful. False return indicates that
     * the actual value was not equal to the expected value.
     */
    CARAPI CompareAndSet(
        /* [in] */ Int32 expect,
        /* [in] */ Int32 update,
        /* [out] */ Boolean* result);

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     *
     * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
     * and does not provide ordering guarantees, so is only rarely an
     * appropriate alternative to {@code compareAndSet}.
     *
     * @param expect the expected value
     * @param update the new value
     * @return true if successful.
     */
    CARAPI WeakCompareAndSet(
        /* [in] */ Int32 expect,
        /* [in] */ Int32 update,
        /* [out] */ Boolean* result);

    /**
     * Atomically increments by one the current value.
     *
     * @return the previous value
     */
    CARAPI GetAndIncrement(
        /* [out] */ Int32* value);

    /**
     * Atomically decrements by one the current value.
     *
     * @return the previous value
     */
    CARAPI GetAndDecrement(
        /* [out] */ Int32* value);

    /**
     * Atomically adds the given value to the current value.
     *
     * @param delta the value to add
     * @return the previous value
     */
    CARAPI GetAndAdd(
        /* [in] */ Int32 delta,
        /* [out] */ Int32* value);

    /**
     * Atomically increments by one the current value.
     *
     * @return the updated value
     */
    CARAPI IncrementAndGet(
        /* [out] */ Int32* value);

    /**
     * Atomically decrements by one the current value.
     *
     * @return the updated value
     */
    CARAPI DecrementAndGet(
        /* [out] */ Int32* value);

    /**
     * Atomically adds the given value to the current value.
     *
     * @param delta the value to add
     * @return the updated value
     */
    CARAPI AddAndGet(
        /* [in] */ Int32 delta,
        /* [out] */ Int32* value);

    /**
     * Returns the value of this {@code AtomicInteger} as an {@code int}.
     */
    CARAPI Int32Value(
        /* [out] */ Int32* value);

    /**
     * Returns the value of this {@code AtomicInteger} as a {@code long}
     * after a widening primitive conversion.
     */
    CARAPI Int64Value(
        /* [out] */ Int64* value);

    /**
     * Returns the value of this {@code AtomicInteger} as a {@code float}
     * after a widening primitive conversion.
     */
    CARAPI FloatValue(
        /* [out] */ Float* value);

    /**
     * Returns the value of this {@code AtomicInteger} as a {@code double}
     * after a widening primitive conversion.
     */
    CARAPI DoubleValue(
        /* [out] */ Double* value);

private:
    static Int64 mSerialVersionUID;// = 6214790243416807050L;

    // setup to use Unsafe.compareAndSwapInt for updates
//    private static final Unsafe unsafe = Unsafe.getUnsafe();
    static Int64 mValueOffset;

    // static {
    //     try {
    //         valueOffset = unsafe.objectFieldOffset
    //             (AtomicInteger.class.getDeclaredField("value"));
    //     } catch (Exception ex) { throw new Error(ex); }
    // }

    volatile Int32 mValue;
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER32_H__
