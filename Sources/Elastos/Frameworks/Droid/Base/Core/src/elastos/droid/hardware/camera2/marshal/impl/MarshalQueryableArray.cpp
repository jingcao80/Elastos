
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableArray.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalRegistry.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Marshal::MarshalRegistry;
using Elastos::Core::IInteger32;
using Elastos::Core::Math;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::IO::IBuffer;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

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
    typeReference->GetClassType(&mClass);
    typeReference->GetComponent((ITypeReference**)&mComponent);

    MarshalRegistry::GetMarshaler(mComponent, mNativeType, (IMarshaler**)&mComponentMarshaler);
}

ECode MarshalQueryableArray::MarshalerArray::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(value);

    Int32 length;
    array->GetLength(&length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> element;
        array->Get(i, (IInterface**)&element);
        MarshalArrayElement(mComponentMarshaler, buffer, element);
    }
    return NOERROR;
}

ECode MarshalQueryableArray::MarshalerArray::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    IBuffer* buf = IBuffer::Probe(buffer);
    AutoPtr<IInterface> array;

    Int32 elementSize;
    mComponentMarshaler->GetNativeSize(&elementSize);

    if (elementSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        Int32 remaining;
        buf->GetRemaining(&remaining);
        Int32 arraySize = remaining / elementSize;

        if (remaining % elementSize != 0) {
            Logger::E(TAG, "Arrays for %s must be packed tighly into a multiple of %d; "
                "but there are %d left over bytes",
                TO_CSTR(mTypeReference), elementSize,  (remaining % elementSize));
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        if (VERBOSE) {
            Logger::V(TAG, "Attempting to unpack array (count = %d, element size = %d, bytes "
                "remaining = %d) for type %s", arraySize, elementSize, remaining, mClass);
        }

        InterfaceID interfaceId;
        mComponent->GetInterfaceType(&interfaceId);
        AutoPtr<IArrayOf> arrayOf;
        CArrayOf::New(interfaceId, arraySize, (IArrayOf**)&arrayOf);
        for (Int32 i = 0; i < arraySize; ++i) {
            AutoPtr<IInterface> elem;
            mComponentMarshaler->Unmarshal(buffer, (IInterface**)&elem);
            arrayOf->Set(i, elem);
        }
        array = arrayOf.Get();
    }
    else {
        // Dynamic size, use an array list.
        AutoPtr<IArrayList> arrayList;
        CArrayList::New((IArrayList**)&arrayList);

        // Assumes array is packed tightly; no unused bytes allowed
        Boolean res;
        while (buf->HasRemaining(&res), res) {
            AutoPtr<IInterface> elem;
            mComponentMarshaler->Unmarshal(buffer, (IInterface**)&elem);
            arrayList->Add(elem);
        }

        array = CopyListToArray(arrayList);
    }

    Int32 res;
    IBuffer::Probe(buffer)->GetRemaining(&res);
    if (res != 0) {
        Logger::E(TAG, "Trailing bytes ( %d ) left over after unpacking %d", res, mClass);
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

    AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
    Int32 arrayLength;
    array->GetLength(&arrayLength);

    if (elementSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        // The fast way. Every element size is uniform.
        *outvalue = elementSize * arrayLength;
        return NOERROR;
    }
    else {
        // The slow way. Accumulate size for each element.
        Int32 size = 0;
        for (Int32 i = 0; i < arrayLength; ++i) {
            AutoPtr<IInterface> element;
            array->Get(i, (IInterface**)&element);
            size += CalculateElementMarshalSize(mComponentMarshaler, element);
        }

        *outvalue = size;
        return NOERROR;
    }
    return NOERROR;
}

ECode MarshalQueryableArray::MarshalerArray::MarshalArrayElement(
    /* [in] */ IMarshaler* marshaler,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ IInterface* element)
{
    return marshaler->Marshal(element, buffer);
}

AutoPtr<IInterface> MarshalQueryableArray::MarshalerArray::CopyListToArray(
    /* [in] */ IArrayList* arrayList)
{
    InterfaceID interfaceId;
    mComponent->GetInterfaceType(&interfaceId);
    Int32 size;
    arrayList->GetSize(&size);

    AutoPtr<IArrayOf> array;
    CArrayOf::New(interfaceId, size, (IArrayOf**)&array);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        arrayList->Get(i, (IInterface**)&obj);
        array->Set(i, obj);
    }

    return array;
}

Int32 MarshalQueryableArray::MarshalerArray::CalculateElementMarshalSize(
    /* [in] */ IMarshaler* marshaler,
    /* [in] */ IInterface* element)
{
    Int32 outvalue;
    marshaler->CalculateMarshalSize(element, &outvalue);
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

    AutoPtr<IMarshaler> marshaler = new MarshalerArray(managedType, nativeType, this);
    *outmar = marshaler;
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

    // support both ConcreteType[] and GenericType<ConcreteType>[]
    ClassID cls;
    managedType->GetClassType(&cls);
    if (cls == ECLSID_CArrayOf) {
        *value = TRUE;
    }

    // TODO: Should this recurse deeper and check that there is
    // a valid marshaler for the ConcreteType as well?
    return NOERROR;
}


} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
