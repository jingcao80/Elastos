
#include "CAtomicReferenceArray.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

const Int64 CAtomicReferenceArray::sSerialVersionUID = -6209656149925076980L;

// private static final Unsafe unsafe;
const Int32 CAtomicReferenceArray::sBase = 0;
const Int32 CAtomicReferenceArray::sShift = 0;
const Int64 CAtomicReferenceArray::sArrayFieldOffset = 0;

CAR_INTERFACE_IMPL_2(CAtomicReferenceArray, Object, IAtomicReferenceArray, ISerializable)

CAR_OBJECT_IMPL(CAtomicReferenceArray)

ECode CAtomicReferenceArray::constructor(
    /* [in] */ Int32 length)
{
    mArray = ArrayOf<IInterface*>::Alloc(length);
    return NOERROR;
}

ECode CAtomicReferenceArray::constructor(
    /* [in] */ const ArrayOf<IInterface*>& array)
{
    // Visibility guaranteed by final field guarantees
    // this.array = Arrays.copyOf(array, array.length, Object[].class);
    mArray->Copy((ArrayOf<IInterface*>*)&array, array.GetLength());
    return NOERROR;
}

ECode CAtomicReferenceArray::GetLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mArray->GetLength();
    return NOERROR;
}

ECode CAtomicReferenceArray::Get(
    /* [in] */ Int32 i,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    AutoPtr<IInterface> temp = GetRaw(CheckedByteOffset(i));
    *outface = temp;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode CAtomicReferenceArray::Set(
    /* [in] */ Int32 i,
    /* [in] */ IInterface* newValue)
{
    assert(0 && "TODO");
    // unsafe.putObjectVolatile(array, checkedByteOffset(i), newValue);
    return NOERROR;
}

ECode CAtomicReferenceArray::LazySet(
    /* [in] */ Int32 i,
    /* [in] */ IInterface* newValue)
{
    assert(0 && "TODO");
    // unsafe.putOrderedObject(array, checkedByteOffset(i), newValue);
    return NOERROR;
}

ECode CAtomicReferenceArray::GetAndSet(
    /* [in] */ Int32 i,
    /* [in] */ IInterface* newValue,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    Int64 offset = CheckedByteOffset(i);
    while (TRUE) {
        AutoPtr<IInterface> current = GetRaw(offset);
        if (CompareAndSetRaw(offset, current, newValue)) {
            *outface = current;
            REFCOUNT_ADD(*outface)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAtomicReferenceArray::CompareAndSet(
    /* [in] */ Int32 i,
    /* [in] */ IInterface* expect,
    /* [in] */ IInterface* update,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = CompareAndSetRaw(CheckedByteOffset(i), expect, update);
    return NOERROR;
}

ECode CAtomicReferenceArray::WeakCompareAndSet(
    /* [in] */ Int32 i,
    /* [in] */ IInterface* expect,
    /* [in] */ IInterface* update,
    /* [out] */ Boolean* value)
{
    return CompareAndSet(i, expect, update, value);
}

ECode CAtomicReferenceArray::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 iMax = mArray->GetLength() - 1;
    if (iMax == -1) {
        *str = "[]";
        return NOERROR;
    }

    StringBuilder b;
    b.AppendChar('[');
    for (Int32 i = 0; ; i++) {
        b.Append(GetRaw(ByteOffset(i)));
        if (i == iMax) {
            b.AppendChar(']');
            return b.ToString(str);
        }
        b.AppendChar(',');
        b.AppendChar(' ');
    }
    return NOERROR;
}

Int64 CAtomicReferenceArray::CheckedByteOffset(
    /* [in] */ Int32 i)
{
    if (i < 0 || i >= mArray->GetLength()) {
        // throw new IndexOutOfBoundsException("index " + i);
        return 0;
    }

    return ByteOffset(i);
}

Int64 CAtomicReferenceArray::ByteOffset(
    /* [in] */ Int32 i)
{
    return ((Int64) i << sShift) + sBase;
}

AutoPtr<IInterface> CAtomicReferenceArray::GetRaw(
    /* [in] */ Int64 offset)
{
    assert(0 && "TODO");
    // return (E) unsafe.getObjectVolatile(array, offset);
    return NULL;
}

Boolean CAtomicReferenceArray::CompareAndSetRaw(
    /* [in] */ Int64 offset,
    /* [in] */ IInterface* expect,
    /* [in] */ IInterface* update)
{
    assert(0 && "TODO");
    // return unsafe.compareAndSwapObject(array, offset, expect, update);
    return FALSE;
}

ECode CAtomicReferenceArray::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    assert(0 && "TODO");
    // // Note: This must be changed if any additional fields are defined
    // Object a = s.readFields().get("array", null);
    // if (a == null || !a.getClass().isArray())
    //     throw new java.io.InvalidObjectException("Not array type");
    // if (a.getClass() != Object[].class)
    //     a = Arrays.copyOf((Object[])a, Array.getLength(a), Object[].class);
    // unsafe.putObjectVolatile(this, arrayFieldOffset, a);
    return NOERROR;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
