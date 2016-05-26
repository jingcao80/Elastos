
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER32_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER32_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicInteger32Array.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicInteger32Array)
    , public Object
    , public IAtomicInteger32Array
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new AtomicIntegerArray of the given length, with all
     * elements initially zero.
     *
     * @param length the length of the array
     */
    CARAPI constructor(
        /* [in] */ Int32 length);

    /**
     * Creates a new AtomicIntegerArray with the same length as, and
     * all elements copied from, the given array.
     *
     * @param array the array to copy elements from
     * @throws NullPointerException if array is null
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Int32>& other);

    /**
     * Returns the length of the array.
     *
     * @return the length of the array
     */
    CARAPI GetLength(
        /* [out] */ Int32* value);

    /**
     * Gets the current value at position {@code i}.
     *
     * @param i the index
     * @return the current value
     */
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
     * Sets the element at position {@code i} to the given value.
     *
     * @param i the index
     * @param newValue the new value
     */
    CARAPI Set(
        /* [in] */ Int32 index,
        /* [in] */ Int32 newValue);

    /**
     * Eventually sets the element at position {@code i} to the given value.
     *
     * @param i the index
     * @param newValue the new value
     * @since 1.6
     */
    CARAPI LazySet(
        /* [in] */ Int32 index,
        /* [in] */ Int32 newValue);

    /**
     * Atomically sets the element at position {@code i} to the given
     * value and returns the old value.
     *
     * @param i the index
     * @param newValue the new value
     * @return the previous value
     */
    CARAPI GetAndSet(
        /* [in] */ Int32 index,
        /* [in] */ Int32 newValue,
        /* [out] */ Int32* value);

    /**
     * Atomically sets the element at position {@code i} to the given
     * updated value if the current value {@code ==} the expected value.
     *
     * @param i the index
     * @param expect the expected value
     * @param update the new value
     * @return true if successful. False return indicates that
     * the actual value was not equal to the expected value.
     */
    CARAPI CompareAndSet(
        /* [in] */ Int32 index,
        /* [in] */ Int32 expect,
        /* [in] */ Int32 update,
        /* [out] */ Boolean* result);

    /**
     * Atomically sets the element at position {@code i} to the given
     * updated value if the current value {@code ==} the expected value.
     *
     * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
     * and does not provide ordering guarantees, so is only rarely an
     * appropriate alternative to {@code compareAndSet}.
     *
     * @param i the index
     * @param expect the expected value
     * @param update the new value
     * @return true if successful.
     */
    CARAPI WeakCompareAndSet(
        /* [in] */ Int32 index,
        /* [in] */ Int32 expect,
        /* [in] */ Int32 update,
        /* [out] */ Boolean* result);

    /**
     * Atomically increments by one the element at index {@code i}.
     *
     * @param i the index
     * @return the previous value
     */
    CARAPI GetAndIncrement(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
     * Atomically decrements by one the element at index {@code i}.
     *
     * @param i the index
     * @return the previous value
     */
    CARAPI GetAndDecrement(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
     * Atomically adds the given value to the element at index {@code i}.
     *
     * @param i the index
     * @param delta the value to add
     * @return the previous value
     */
    CARAPI GetAndAdd(
        /* [in] */ Int32 index,
        /* [in] */ Int32 delta,
        /* [out] */ Int32* value);

    /**
     * Atomically increments by one the element at index {@code i}.
     *
     * @param i the index
     * @return the updated value
     */
    CARAPI IncrementAndGet(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
     * Atomically decrements by one the element at index {@code i}.
     *
     * @param i the index
     * @return the updated value
     */
    CARAPI DecrementAndGet(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value);

    /**
     * Atomically adds the given value to the element at index {@code i}.
     *
     * @param i the index
     * @param delta the value to add
     * @return the updated value
     */
    CARAPI AddAndGet(
        /* [in] */ Int32 index,
        /* [in] */ Int32 delta,
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(Int32) GetRaw(
        /* [in] */ Int32 index);

    CARAPI CheckedByteOffset(
        /* [in] */ Int32 index);

    CARAPI_(Boolean) CompareAndSetRaw(
        /* [in] */ Int32 index,
        /* [in] */ Int32 expect,
        /* [in] */ Int32 update);

private:
    AutoPtr<ArrayOf<Int32> > mArray;
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER32_H__
