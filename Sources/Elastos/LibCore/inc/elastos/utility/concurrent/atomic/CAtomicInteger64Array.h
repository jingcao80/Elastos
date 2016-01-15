
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER64_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER64_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicInteger64Array.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicInteger64Array)
    , public Object
    , public IAtomicInteger64Array
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
        /* [in] */ const ArrayOf<Int64>& other);

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
        /* [in] */ Int32 i,
        /* [out] */ Int64* value);

    /**
     * Sets the element at position {@code i} to the given value.
     *
     * @param i the index
     * @param newValue the new value
     */
    CARAPI Set(
        /* [in] */ Int32 i,
        /* [in] */ Int64 newValue);

    /**
     * Eventually sets the element at position {@code i} to the given value.
     *
     * @param i the index
     * @param newValue the new value
     * @since 1.6
     */
    CARAPI LazySet(
        /* [in] */ Int32 i,
        /* [in] */ Int64 newValue);


    /**
     * Atomically sets the element at position {@code i} to the given value
     * and returns the old value.
     *
     * @param i the index
     * @param newValue the new value
     * @return the previous value
     */
    CARAPI GetAndSet(
        /* [in] */ Int32 i,
        /* [in] */ Int64 newValue,
        /* [out] */ Int64* value);

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
        /* [in] */ Int32 i,
        /* [in] */ Int64 expect,
        /* [in] */ Int64 update,
        /* [out] */ Boolean* value);

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
        /* [in] */ Int32 i,
        /* [in] */ Int64 expect,
        /* [in] */ Int64 update,
        /* [out] */ Boolean* value);

    /**
     * Atomically increments by one the element at index {@code i}.
     *
     * @param i the index
     * @return the previous value
     */
    CARAPI GetAndIncrement(
        /* [in] */ Int32 i,
        /* [out] */ Int64* value);

    /**
     * Atomically decrements by one the element at index {@code i}.
     *
     * @param i the index
     * @return the previous value
     */
    CARAPI GetAndDecrement(
        /* [in] */ Int32 i,
        /* [out] */ Int64* value);

    /**
     * Atomically adds the given value to the element at index {@code i}.
     *
     * @param i the index
     * @param delta the value to add
     * @return the previous value
     */
    CARAPI GetAndAdd(
        /* [in] */ Int32 i,
        /* [in] */ Int64 delta,
        /* [out] */ Int64* value);

    /**
     * Atomically increments by one the element at index {@code i}.
     *
     * @param i the index
     * @return the updated value
     */
    CARAPI IncrementAndGet(
        /* [in] */ Int32 i,
        /* [out] */ Int64* value);

    /**
     * Atomically decrements by one the element at index {@code i}.
     *
     * @param i the index
     * @return the updated value
     */
    CARAPI DecrementAndGet(
        /* [in] */ Int32 i,
        /* [out] */ Int64* value);

    /**
     * Atomically adds the given value to the element at index {@code i}.
     *
     * @param i the index
     * @param delta the value to add
     * @return the updated value
     */
    CARAPI AddAndGet(
        /* [in] */ Int32 i,
        /* [in] */ Int64 delta,
        /* [out] */ Int64* value);

    /**
     * Returns the String representation of the current values of array.
     * @return the String representation of the current values of array
     */
    CARAPI ToString(
        /* [out] */ String* str);
private:
    CARAPI_(Int64) CheckedByteOffset(
        /* [in] */ Int32 i);

    static CARAPI_(Int64) ByteOffset(
        /* [in] */ Int32 i);

    CARAPI_(Int64) GetRaw(
        /* [in] */ Int64 offset);

    CARAPI_(Boolean) CompareAndSetRaw(
        /* [in] */ Int64 offset,
        /* [in] */ Int64 expect,
        /* [in] */ Int64 update);

private:
    static const Int64 mSerialVersionUID; // = -2308431214976778248L;

    // private static final Unsafe unsafe = Unsafe.getUnsafe();
    static const Int32 sBase; // = unsafe.arrayBaseOffset(long[].class);
    static const Int32 sShift;
    AutoPtr< ArrayOf<Int64> > mArray;
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICINTEGER64_H__
