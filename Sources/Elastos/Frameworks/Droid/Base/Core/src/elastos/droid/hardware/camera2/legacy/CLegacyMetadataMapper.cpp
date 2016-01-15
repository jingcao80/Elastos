
#include "elastos/droid/hardware/camera2/legacy/CLegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CLegacyMetadataMapper, Singleton, ILegacyMetadataMapperHelper)

CAR_SINGLETON_IMPL(CLegacyMetadataMapper)

ECode CLegacyMetadataMapper::CreateCharacteristics(
    /* [in] */ IParameters* parameters,
    /* [in] */ IHardwareCameraInfo* info,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc)

    return LegacyMetadataMapper::CreateCharacteristics(parameters, info, outcc);
}

ECode CLegacyMetadataMapper::CreateCharacteristics(
    /* [in] */ const String& parameters,
    /* [in] */ ICameraInfo* info,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc)

    return LegacyMetadataMapper::CreateCharacteristics(parameters, info, outcc);
}

ECode CLegacyMetadataMapper::ConvertRequestMetadata(
    /* [in] */ ILegacyRequest* request)
{
    return LegacyMetadataMapper::ConvertRequestMetadata(request);
}

ECode CLegacyMetadataMapper::CreateRequestTemplate(
        /* [in] */ ICameraCharacteristics* c,
        /* [in] */ Int32 templateId,
        /* [out] */ ICameraMetadataNative** outcm)
{
    VALIDATE_NOT_NULL(outcm)

    return LegacyMetadataMapper::CreateRequestTemplate(c, templateId, outcm);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
