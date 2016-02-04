
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableRange.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableRange::MarshalerRange::MarshalerRange(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableRange* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    // mClass = typeReference.getRawType();

    // /*
    //  * Lookup the actual type argument, e.g. Range<Integer> --> Integer
    //  * and then get the marshaler for that managed type.
    //  */
    // ParameterizedType paramType;
    // try {
    //     paramType = (ParameterizedType) typeReference.getType();
    // } catch (ClassCastException e) {
    //     throw new AssertionError("Raw use of Range is not supported", e);
    // }
    // Type actualTypeArgument = paramType.getActualTypeArguments()[0];

    // TypeReference<?> actualTypeArgToken =
    //         TypeReference.createSpecializedTypeReference(actualTypeArgument);

    // mNestedTypeMarshaler = (Marshaler<T>)MarshalRegistry.getMarshaler(
    //         actualTypeArgToken, mNativeType);
    // try {
    //     mConstructor = (Constructor<Range<T>>)mClass.getConstructor(
    //             Comparable.class, Comparable.class);
    // } catch (NoSuchMethodException e) {
    //     throw new AssertionError(e);
    // }
}

ECode MarshalQueryableRange::MarshalerRange::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    // mNestedTypeMarshaler.marshal(value.getLower(), buffer);
    // mNestedTypeMarshaler.marshal(value.getUpper(), buffer);

    return NOERROR;
}

ECode MarshalQueryableRange::MarshalerRange::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<IInterface> lower;
    mNestedTypeMarshaler->Unmarshal(buffer, (IInterface**)&lower);
    AutoPtr<IInterface> upper;
    mNestedTypeMarshaler->Unmarshal(buffer, (IInterface**)&upper);

    assert(0);
    // try {
    //     return mConstructor.newInstance(lower, upper);
    // } catch (InstantiationException e) {
    //     throw new AssertionError(e);
    // } catch (IllegalAccessException e) {
    //     throw new AssertionError(e);
    // } catch (IllegalArgumentException e) {
    //     throw new AssertionError(e);
    // } catch (InvocationTargetException e) {
    //     throw new AssertionError(e);
    // }

    return NOERROR;
}

ECode MarshalQueryableRange::MarshalerRange::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int32 nestedSize;
    mNestedTypeMarshaler->GetNativeSize(&nestedSize);

    if (nestedSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        *value = nestedSize * RANGE_COUNT;
        return NOERROR;
    }
    else {
        *value = IMarshaler::NATIVE_SIZE_DYNAMIC;
        return NOERROR;
    }

    return NOERROR;
}

ECode MarshalQueryableRange::MarshalerRange::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(value);
    *outvalue = 0;

    Int32 nativeSize;
    GetNativeSize(&nativeSize);

    if (nativeSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        *outvalue = nativeSize;
        return NOERROR;
    }
    else {
        assert(0);
        Int32 lowerSize;// = mNestedTypeMarshaler.calculateMarshalSize(value.getLower());
        Int32 upperSize;// = mNestedTypeMarshaler.calculateMarshalSize(value.getUpper());

        *outvalue = lowerSize + upperSize;
        return NOERROR;
    }
    return NOERROR;
}

const Int32 MarshalQueryableRange::RANGE_COUNT = 2;

CAR_INTERFACE_IMPL_2(MarshalQueryableRange, Object,
        IMarshalQueryableRange, IMarshalQueryable)

ECode MarshalQueryableRange::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerRange(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableRange::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    //return (Range.class.equals(managedType.getRawType()));
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
