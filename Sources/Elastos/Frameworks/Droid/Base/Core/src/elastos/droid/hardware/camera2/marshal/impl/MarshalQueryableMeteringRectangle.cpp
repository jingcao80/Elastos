
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableMeteringRectangle.h"
#include "elastos/droid/hardware/camera2/params/CMeteringRectangle.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::CMeteringRectangle;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::MarshalerMeteringRectangle(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableMeteringRectangle* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IMeteringRectangle> rectangle = IMeteringRectangle::Probe(value);

    Int32 xMin;
    rectangle->GetX(&xMin);

    Int32 yMin;
    rectangle->GetY(&yMin);

    Int32 width;
    rectangle->GetWidth(&width);
    Int32 xMax = xMin + width;

    Int32 height;
    rectangle->GetHeight(&height);
    Int32 yMax = yMin + height;

    Int32 weight;
    rectangle->GetMeteringWeight(&weight);

    buffer->PutInt32(xMin);
    buffer->PutInt32(yMin);
    buffer->PutInt32(xMax);
    buffer->PutInt32(yMax);
    buffer->PutInt32(weight);
    return NOERROR;
}

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 xMin;
    buffer->GetInt32(&xMin);
    Int32 yMin;
    buffer->GetInt32(&yMin);
    Int32 xMax;
    buffer->GetInt32(&xMax);
    Int32 yMax;
    buffer->GetInt32(&yMax);
    Int32 weight;
    buffer->GetInt32(&weight);

    Int32 width = xMax - xMin;
    Int32 height = yMax - yMin;

    AutoPtr<IMeteringRectangle> rectangle;
    CMeteringRectangle::New(xMin, yMin, width, height, weight, (IMeteringRectangle**)&rectangle);
    *outface = TO_IINTERFACE(rectangle);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableMeteringRectangle::SIZE = IMarshalHelpers::SIZEOF_INT32 * 4;

CAR_INTERFACE_IMPL_2(MarshalQueryableMeteringRectangle, Object,
        IMarshalQueryableMeteringRectangle, IMarshalQueryable)

ECode MarshalQueryableMeteringRectangle::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerMeteringRectangle(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableMeteringRectangle::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    //return nativeType == ICameraMetadataNative::TYPE_INT32 && MeteringRectangle.class.equals(managedType.getType());
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
