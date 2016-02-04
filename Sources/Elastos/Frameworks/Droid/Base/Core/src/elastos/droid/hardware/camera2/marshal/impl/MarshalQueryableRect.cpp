
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableRect.h"
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

MarshalQueryableRect::MarshalerRect::MarshalerRect(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableRect* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableRect::MarshalerRect::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IRect> rect = IRect::Probe(value);
    Int32 left;
    rect->GetLeft(&left);
    buffer->PutInt32(left);
    Int32 top;
    rect->GetTop(&top);
    buffer->PutInt32(top);
    Int32 width;
    rect->GetWidth(&width);
    buffer->PutInt32(width);
    Int32 height;
    rect->GetHeight(&height);
    buffer->PutInt32(height);
    return NOERROR;
}

ECode MarshalQueryableRect::MarshalerRect::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 left;
    buffer->GetInt32(&left);
    Int32 top;
    buffer->GetInt32(&top);
    Int32 width;
    buffer->GetInt32(&width);
    Int32 height;
    buffer->GetInt32(&height);

    Int32 right = left + width;
    Int32 bottom = top + height;

    AutoPtr<IRect> rect;
    CRect::New(left, top, right, bottom, (IRect**)&rect);
    *outface = TO_IINTERFACE(rect);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableRect::MarshalerRect::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableRect::SIZE = IMarshalHelpers::SIZEOF_INT32 * 4;

CAR_INTERFACE_IMPL_2(MarshalQueryableRect, Object,
        IMarshalQueryableRect, IMarshalQueryable)

ECode MarshalQueryableRect::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerRect(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableRect::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    //return nativeType == ICameraMetadataNative::TYPE_INT32 && (Rect.class.equals(managedType.getType()));
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
