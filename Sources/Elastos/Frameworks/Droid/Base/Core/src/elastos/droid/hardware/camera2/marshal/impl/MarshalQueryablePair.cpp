
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryablePair.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include "elastos/droid/utility/CPairHelper.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryablePair::MarshalerPair::MarshalerPair(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryablePair* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    assert(0);
    // mClass = typeReference.getRawType();

    // /*
    //  * Lookup the actual type arguments, e.g. Pair<Integer, Float> --> [Integer, Float]
    //  * and then get the marshalers for that managed type.
    //  */
    // ParameterizedType paramType;
    // try {
    //     paramType = (ParameterizedType) typeReference.getType();
    // } catch (ClassCastException e) {
    //     throw new AssertionError("Raw use of Pair is not supported", e);
    // }

    // // Get type marshaler for T1
    // {
    //     Type actualTypeArgument = paramType.getActualTypeArguments()[0];

    //     TypeReference<?> actualTypeArgToken =
    //             TypeReference.createSpecializedTypeReference(actualTypeArgument);

    //     mNestedTypeMarshalerFirst = (Marshaler<T1>)MarshalRegistry.getMarshaler(
    //             actualTypeArgToken, mNativeType);
    // }
    // // Get type marshaler for T2
    // {
    //     Type actualTypeArgument = paramType.getActualTypeArguments()[1];

    //     TypeReference<?> actualTypeArgToken =
    //             TypeReference.createSpecializedTypeReference(actualTypeArgument);

    //     mNestedTypeMarshalerSecond = (Marshaler<T2>)MarshalRegistry.getMarshaler(
    //             actualTypeArgToken, mNativeType);
    // }
    // try {
    //     mConstructor = (Constructor<Pair<T1, T2>>)mClass.getConstructor(
    //             Object.class, Object.class);
    // } catch (NoSuchMethodException e) {
    //     throw new AssertionError(e);
    // }
}

ECode MarshalQueryablePair::MarshalerPair::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IPair> pair = IPair::Probe(value);
    AutoPtr<IInterface> firstObj;
    AutoPtr<IInterface> secondObj;
    pair->GetFirst((IInterface**)&firstObj);
    if (firstObj == NULL) {
        //throw new UnsupportedOperationException("Pair#first must not be null");
        Slogger::E("MarshalQueryablePair", "Pair#first must not be null");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    else {
        pair->GetSecond((IInterface**)&secondObj);
        if (secondObj == NULL) {
            //throw new UnsupportedOperationException("Pair#second must not be null");
            Slogger::E("MarshalQueryablePair", "Pair#second must not be null");
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    mNestedTypeMarshalerFirst->Marshal(firstObj, buffer);
    return mNestedTypeMarshalerSecond->Marshal(secondObj, buffer);
}

ECode MarshalQueryablePair::MarshalerPair::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<IInterface> first;
    mNestedTypeMarshalerFirst->Unmarshal(buffer, (IInterface**)&first);
    AutoPtr<IInterface> second;
    mNestedTypeMarshalerSecond->Unmarshal(buffer, (IInterface**)&second);

    //try {
    AutoPtr<IPairHelper> helper;
    CPairHelper::AcquireSingleton((IPairHelper**)&helper);
    AutoPtr<IPair> pair;
    ECode ec = helper->Create(first, second, (IPair**)&pair);
    if (FAILED(ec)) {
        Slogger::E("MarshalQueryablePair", "AssertionError");
        return E_ASSERTION_ERROR;
    }
    *outface = TO_IINTERFACE(pair);
    REFCOUNT_ADD(*outface);
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

ECode MarshalQueryablePair::MarshalerPair::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int32 firstSize;
    mNestedTypeMarshalerFirst->GetNativeSize(&firstSize);
    Int32 secondSize;
    mNestedTypeMarshalerSecond->GetNativeSize(&secondSize);

    if (firstSize != IMarshaler::NATIVE_SIZE_DYNAMIC && secondSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        *value = firstSize + secondSize;
        return NOERROR;
    }
    else {
        *value = NATIVE_SIZE_DYNAMIC;
        return NOERROR;
    }
    return NOERROR;
}

ECode MarshalQueryablePair::MarshalerPair::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);
    *outvalue = 0;

    Int32 nativeSize;
    GetNativeSize(&nativeSize);

    if (nativeSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        *outvalue = nativeSize;
        return NOERROR;
    }
    else {
        AutoPtr<IPair> pair = IPair::Probe(value);
        Int32 firstSize;
        AutoPtr<IInterface> firstObj;
        pair->GetFirst((IInterface**)&firstObj);
        mNestedTypeMarshalerFirst->CalculateMarshalSize(firstObj, &firstSize);

        AutoPtr<IInterface> secondObj;
        pair->GetSecond((IInterface**)&secondObj);
        Int32 secondSize;
        mNestedTypeMarshalerSecond->CalculateMarshalSize(secondObj, &secondSize);

        *outvalue = firstSize + secondSize;
        return NOERROR;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(MarshalQueryablePair, Object,
        IMarshalQueryablePair, IMarshalQueryable)

ECode MarshalQueryablePair::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerPair(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryablePair::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    //return (Pair.class.equals(managedType.getRawType()));
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
