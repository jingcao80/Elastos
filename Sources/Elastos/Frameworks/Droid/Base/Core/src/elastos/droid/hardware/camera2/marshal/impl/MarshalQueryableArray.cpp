
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableArray.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::IInteger32;
using Elastos::Core::Math;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::IO::IBuffer;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableArray::MarshalerArray::MarshalerArray(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableArray* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    assert(0);
    // mClass = (Class<T>)typeReference.getRawType();

    // TypeReference<?> componentToken = typeReference.getComponentType();
    // mComponentMarshaler = MarshalRegistry.getMarshaler(componentToken, mNativeType);
    // mComponentClass = componentToken.getRawType();
}

ECode MarshalQueryableArray::MarshalerArray::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IArrayOf> obj = IArrayOf::Probe(value);

    Int32 length;
    obj->GetLength(&length);
    for (Int32 i = 0; i < length; ++i) {
        MarshalArrayElement(mComponentMarshaler, buffer, value, i);
    }
    return NOERROR;
}

ECode MarshalQueryableArray::MarshalerArray::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<IInterface> array;

    Int32 elementSize;
    mComponentMarshaler->GetNativeSize(&elementSize);

    if (elementSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        Int32 remaining;
        IBuffer::Probe(buffer)->GetRemaining(&remaining);
        Int32 arraySize = remaining / elementSize;

        if (remaining % elementSize != 0) {
            // throw new UnsupportedOperationException("Arrays for " + mTypeReference
            //         + " must be packed tighly into a multiple of " + elementSize
            //         + "; but there are " + (remaining % elementSize) + " left over bytes");
            String str;
            IObject::Probe(mTypeReference)->ToString(&str);
            Slogger::E(TAG, "Arrays for %s "
                    "must be packed tighly into a multiple of %d; "
                    "but there are %d left over bytes",str.string(), elementSize,  (remaining % elementSize));
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        if (VERBOSE) {
            Slogger::V(TAG, "Attempting to unpack array (count = %d, element size = %d, bytes "
                    "remaining = %d) for type %s",arraySize, elementSize, remaining, mClass);
        }

        AutoPtr<IArrayOf> _array;
        CArrayOf::New(mComponentClass, arraySize, (IArrayOf**)&_array);
        for (Int32 i = 0; i < arraySize; ++i) {
            AutoPtr<IInterface> elem;
            mComponentMarshaler->Unmarshal(buffer, (IInterface**)&elem);
            _array->Set(i, elem);
        }
        array = _array;
    }
    else {
        // Dynamic size, use an array list.
        AutoPtr<IArrayList> arrayList;
        CArrayList::New((IArrayList**)&arrayList);

        // Assumes array is packed tightly; no unused bytes allowed
        Boolean res;
        while (IBuffer::Probe(buffer)->HasRemaining(&res), res) {
            AutoPtr<IInterface> elem;
            mComponentMarshaler->Unmarshal(buffer, (IInterface**)&elem);
            arrayList->Add(elem);
        }

        array = CopyListToArray(arrayList);
    }

    Int32 res;
    IBuffer::Probe(buffer)->GetRemaining(&res);
    if (res != 0) {
        Slogger::E(TAG, "Trailing bytes ( %d ) left over after unpacking %d", res, mClass);
    }

    *outface = array;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableArray::MarshalerArray::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = IMarshaler::NATIVE_SIZE_DYNAMIC;
    return NOERROR;
}

ECode MarshalQueryableArray::MarshalerArray::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);
    *outvalue = 0;

    Int32 elementSize;
    mComponentMarshaler->GetNativeSize(&elementSize);

    AutoPtr<IArrayOf> obj = IArrayOf::Probe(value);
    Int32 arrayLength;
    obj->GetLength(&arrayLength);

    if (elementSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        // The fast way. Every element size is uniform.
        *outvalue = elementSize * arrayLength;
        return NOERROR;
    }
    else {
        // The slow way. Accumulate size for each element.
        Int32 size = 0;
        for (Int32 i = 0; i < arrayLength; ++i) {
            size += CalculateElementMarshalSize(mComponentMarshaler, value, i);
        }

        *outvalue = size;
        return NOERROR;
    }
    return NOERROR;
}

ECode MarshalQueryableArray::MarshalerArray::MarshalArrayElement(
    /* [in] */ IMarshaler* marshaler,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ IInterface* array,
    /* [in] */ Int32 index)
{
    AutoPtr<IArrayOf> _array = IArrayOf::Probe(array);
    AutoPtr<IInterface> elem;
    _array->Get(index, (IInterface**)&elem);
    return marshaler->Marshal(elem, buffer);
}

AutoPtr<IInterface> MarshalQueryableArray::MarshalerArray::CopyListToArray(
    /* [in] */ IArrayList* arrayList)
{
    AutoPtr<ArrayOf<IInterface*> > arrayDest;
    arrayList->ToArray((ArrayOf<IInterface*>**)&arrayDest);
    AutoPtr<IArrayOf> res = CoreUtils::Convert(arrayDest.Get());
    return TO_IINTERFACE(res);
}

Int32 MarshalQueryableArray::MarshalerArray::CalculateElementMarshalSize(
    /* [in] */ IMarshaler* marshaler,
    /* [in] */ IInterface* array,
    /* [in] */ Int32 index)
{
    AutoPtr<IArrayOf> _array = IArrayOf::Probe(array);
    AutoPtr<IInterface> elem;
    _array->Get(index, (IInterface**)&elem);

    Int32 outvalue;
    marshaler->CalculateMarshalSize(elem, &outvalue);
    return outvalue;
}

//MarshalQueryableArray.class.getSimpleName();
const String MarshalQueryableArray::TAG("MarshalQueryableArray");
const Boolean MarshalQueryableArray::VERBOSE = FALSE;// Log.isLoggable(TAG, Log.VERBOSE);

CAR_INTERFACE_IMPL_2(MarshalQueryableArray, Object,
        IMarshalQueryableArray, IMarshalQueryable)

ECode MarshalQueryableArray::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerArray(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableArray::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    // support both ConcreteType[] and GenericType<ConcreteType>[]
    //return managedType.getRawType().isArray();
    return NOERROR;

    // TODO: Should this recurse deeper and check that there is
    // a valid marshaler for the ConcreteType as well?
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
