#include "elastos/droid/media/CameraProfile.h"
#include "elastos/droid/media/CCameraProfile.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CCameraProfile)
CAR_INTERFACE_IMPL(CCameraProfile, Singleton, ICameraProfile);

ECode CCameraProfile::GetJpegEncodingQualityParameter(
    /* [in] */ Int32 quality,
    /* [out] */ Int32* result)
{
    return CameraProfile::GetJpegEncodingQualityParameter(quality, result);
}

ECode CCameraProfile::GetJpegEncodingQualityParameter(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ Int32* result)
{
    return CameraProfile::GetJpegEncodingQualityParameter(cameraId, quality, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
