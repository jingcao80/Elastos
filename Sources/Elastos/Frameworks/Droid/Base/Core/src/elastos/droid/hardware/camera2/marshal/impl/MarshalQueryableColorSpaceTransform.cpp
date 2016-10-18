
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableColorSpaceTransform.h"
#include "elastos/droid/hardware/camera2/params/CColorSpaceTransform.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableColorSpaceTransform::MarshalerColorSpaceTransform::MarshalerColorSpaceTransform(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableColorSpaceTransform* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableColorSpaceTransform::MarshalerColorSpaceTransform::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IColorSpaceTransform> transform = IColorSpaceTransform::Probe(value);

    AutoPtr<ArrayOf<Int32> > transformAsArray = ArrayOf<Int32>::Alloc(ELEMENTS_INT32);
    transform->CopyElements(transformAsArray, /*offset*/0);

    for (Int32 i = 0; i < ELEMENTS_INT32; ++i) {
        buffer->PutInt32((*transformAsArray)[i]);
    }
    return NOERROR;
}

ECode MarshalQueryableColorSpaceTransform::MarshalerColorSpaceTransform::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<ArrayOf<Int32> > transformAsArray = ArrayOf<Int32>::Alloc(ELEMENTS_INT32);

    for (Int32 i = 0; i < ELEMENTS_INT32; ++i) {
        Int32 val;
        buffer->GetInt32(&val);
        (*transformAsArray)[i] = val;
    }

    AutoPtr<IColorSpaceTransform> transform;
    CColorSpaceTransform::New(transformAsArray, (IColorSpaceTransform**)&transform);
    *outface = TO_IINTERFACE(transform);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableColorSpaceTransform::MarshalerColorSpaceTransform::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableColorSpaceTransform::ELEMENTS_INT32 =
        3 * 3 * (IMarshalHelpers::SIZEOF_RATIONAL / IMarshalHelpers::SIZEOF_INT32);

const Int32 MarshalQueryableColorSpaceTransform::SIZE = IMarshalHelpers::SIZEOF_INT32 * ELEMENTS_INT32;

CAR_INTERFACE_IMPL_2(MarshalQueryableColorSpaceTransform, Object,
        IMarshalQueryableColorSpaceTransform, IMarshalQueryable)

ECode MarshalQueryableColorSpaceTransform::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerColorSpaceTransform(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableColorSpaceTransform::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_RATIONAL) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CColorSpaceTransform) {
            *value = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
