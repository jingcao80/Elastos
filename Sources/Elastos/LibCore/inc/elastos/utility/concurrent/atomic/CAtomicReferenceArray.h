
#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICREFERENCEARRAY_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICREFERENCEARRAY_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicReferenceArray.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicReferenceArray)
    , public Object
    , public IAtomicReferenceArray
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new AtomicReferenceArray of the given length, with all
     * elements initially null.
     *
     * @param length the length of the array
     */
    CARAPI constructor(
        /* [in] */ Int32 length);

    /**
     * Creates a new AtomicReferenceArray with the same length as, and
     * all elements copied from, the given array.
     *
     * @param array the array to copy elements from
     * @throws NullPointerException if array is null
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<IInterface*>& array);

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
        /* [out] */ IInterface** outface);

    /**
     * Sets the element at position {@code i} to the given value.
     *
     * @param i the index
     * @param newValue the new value
     */
    CARAPI Set(
        /* [in] */ Int32 i,
        /* [in] */ IInterface* newValue);

    /**
     * Eventually sets the element at position {@code i} to the given value.
     *
     * @param i the index
     * @param newValue the new value
     * @since 1.6
     */
    CARAPI LazySet(
        /* [in] */ Int32 i,
        /* [in] */ IInterface* newValue);


    /**
     * Atomically sets the element at position {@code i} to the given
     * value and returns the old value.
     *
     * @param i the index
     * @param newValue the new value
     * @return the previous value
     */
    CARAPI GetAndSet(
        /* [in] */ Int32 i,
        /* [in] */ IInterface* newValue,
        /* [out] */ IInterface** outface);

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
        /* [in] */ IInterface* expect,
        /* [in] */ IInterface* update,
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
        /* [in] */ IInterface* expect,
        /* [in] */ IInterface* update,
        /* [out] */ Boolean* value);

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

    CARAPI_(AutoPtr<IInterface>) GetRaw(
        /* [in] */ Int64 offset);

    CARAPI_(Boolean) CompareAndSetRaw(
        /* [in] */ Int64 offset,
        /* [in] */ IInterface* expect,
        /* [in] */ IInterface* update);

    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* s);

private:
    static const Int64 sSerialVersionUID; // = -6209656149925076980L;

    // private static final Unsafe unsafe;
    static const Int32 sBase;
    static const Int32 sShift;
    static const Int64 sArrayFieldOffset;
    AutoPtr< ArrayOf<IInterface*> > mArray; // must have exact type Object[]

    // TODO
    // static {
    //     int scale;
    //     try {
    //         unsafe = Unsafe.getUnsafe();
    //         arrayFieldOffset = unsafe.objectFieldOffset
    //             (AtomicReferenceArray.class.getDeclaredField("array"));
    //         base = unsafe.arrayBaseOffset(Object[].class);
    //         scale = unsafe.arrayIndexScale(Object[].class);
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    //     if ((scale & (scale - 1)) != 0)
    //         throw new Error("data type scale not a power of two");
    //     shift = 31 - Integer.numberOfLeadingZeros(scale);
    // }
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICREFERENCEARRAY_H__
