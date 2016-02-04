
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableHighSpeedVideoConfiguration.h"
#include "elastos/droid/hardware/camera2/params/CHighSpeedVideoConfiguration.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IHighSpeedVideoConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::CHighSpeedVideoConfiguration;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableHighSpeedVideoConfiguration::MarshalerHighSpeedVideoConfiguration::MarshalerHighSpeedVideoConfiguration(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableHighSpeedVideoConfiguration* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableHighSpeedVideoConfiguration::MarshalerHighSpeedVideoConfiguration::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IHighSpeedVideoConfiguration> configuration = IHighSpeedVideoConfiguration::Probe(value);

    Int32 width;
    configuration->GetWidth(&width);
    buffer->PutInt32(width);

    Int32 height;
    configuration->GetHeight(&height);
    buffer->PutInt32(height);

    Int32 fpsMin;
    configuration->GetFpsMin(&fpsMin);
    buffer->PutInt32(fpsMin);

    Int32 fpsMax;
    configuration->GetFpsMax(&fpsMax);
    buffer->PutInt32(fpsMax);
    return NOERROR;
}

ECode MarshalQueryableHighSpeedVideoConfiguration::MarshalerHighSpeedVideoConfiguration::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 width;
    buffer->GetInt32(&width);

    Int32 height;
    buffer->GetInt32(&height);

    Int32 fpsMin;
    buffer->GetInt32(&fpsMin);

    Int32 fpsMax;
    buffer->GetInt32(&fpsMax);

    AutoPtr<IHighSpeedVideoConfiguration> configuration;
    CHighSpeedVideoConfiguration::New(width, height, fpsMin, fpsMax,
            (IHighSpeedVideoConfiguration**)&configuration);
    *outface = TO_IINTERFACE(configuration);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableHighSpeedVideoConfiguration::MarshalerHighSpeedVideoConfiguration::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableHighSpeedVideoConfiguration::SIZE = IMarshalHelpers::SIZEOF_INT32 * 4;

CAR_INTERFACE_IMPL_2(MarshalQueryableHighSpeedVideoConfiguration, Object,
        IMarshalQueryableHighSpeedVideoConfiguration, IMarshalQueryable)

ECode MarshalQueryableHighSpeedVideoConfiguration::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerHighSpeedVideoConfiguration(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableHighSpeedVideoConfiguration::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    // return nativeType == ICameraMetadataNative::TYPE_INT32 &&
    //         managedType.getType().equals(HighSpeedVideoConfiguration.class);
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
