
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableSize.h"
#include "elastos/droid/utility/CSize.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Utility::ECLSID_CSize;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableSize::MarshalerSize::MarshalerSize(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableSize* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableSize::MarshalerSize::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<ISize> size = ISize::Probe(value);

    Int32 width;
    size->GetWidth(&width);
    buffer->PutInt32(width);
    Int32 height;
    size->GetHeight(&height);
    return buffer->PutInt32(height);
}

ECode MarshalQueryableSize::MarshalerSize::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 width;
    buffer->GetInt32(&width);
    Int32 height;
    buffer->GetInt32(&height);

    AutoPtr<ISize> size;
    CSize::New(width, height, (ISize**)&size);
    *outface = TO_IINTERFACE(size);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableSize::MarshalerSize::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableSize::SIZE = IMarshalHelpers::SIZEOF_INT32 * 2;

CAR_INTERFACE_IMPL_2(MarshalQueryableSize, Object,
        IMarshalQueryableSize, IMarshalQueryable)

ECode MarshalQueryableSize::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerSize(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableSize::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_INT32) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CSize) {
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
