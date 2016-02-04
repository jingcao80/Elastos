
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableSizeF.h"
#include "elastos/droid/utility/CSizeF.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::ISizeF;
using Elastos::Droid::Utility::CSizeF;


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableSizeF::MarshalerSizeF::MarshalerSizeF(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableSizeF* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableSizeF::MarshalerSizeF::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<ISizeF> sizef = ISizeF::Probe(value);

    Float width;
    sizef->GetWidth(&width);
    buffer->PutFloat(width);
    Float height;
    sizef->GetHeight(&height);
    return buffer->PutFloat(height);
}

ECode MarshalQueryableSizeF::MarshalerSizeF::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Float width;
    buffer->GetFloat(&width);
    Float height;
    buffer->GetFloat(&height);

    AutoPtr<ISizeF> sizef;
    CSizeF::New(width, height, (ISizeF**)&sizef);
    *outface = TO_IINTERFACE(sizef);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableSizeF::MarshalerSizeF::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableSizeF::SIZE = IMarshalHelpers::SIZEOF_FLOAT * 2;

CAR_INTERFACE_IMPL_2(MarshalQueryableSizeF, Object,
        IMarshalQueryableSizeF, IMarshalQueryable)

ECode MarshalQueryableSizeF::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerSizeF(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableSizeF::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    //return nativeType == ICameraMetadataNative::TYPE_FLOAT && (SizeF.class.equals(managedType.getType()));
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
