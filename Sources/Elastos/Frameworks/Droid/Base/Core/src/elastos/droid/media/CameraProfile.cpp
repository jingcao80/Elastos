
#include "Elastos.Droid.Media.h"
#include "elastos/droid/hardware/CHardwareCamera.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include "elastos/droid/media/CameraProfile.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
#include <media/MediaProfiles.h>

using Elastos::Droid::Hardware::CHardwareCamera;
using Elastos::Droid::Hardware::HardwareCamera;
using Elastos::Droid::Hardware::IHardwareCameraInfo;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Media {

ECode CameraProfile::GetJpegEncodingQualityParameter(
    /* [in] */ Int32 quality,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 numberOfCameras;
    HardwareCamera::GetNumberOfCameras(&numberOfCameras);

    AutoPtr<IHardwareCameraInfo> cameraInfo = new CHardwareCamera::CameraInfo();

    for (Int32 i = 0; i < numberOfCameras; i++) {
        HardwareCamera::GetCameraInfo(i, cameraInfo);
        Int32 facing;
        cameraInfo->GetFacing(&facing);
        if (facing == IHardwareCameraInfo::CAMERA_FACING_BACK) {
            return GetJpegEncodingQualityParameter(i, quality, result);
        }
    }
    *result = 0;
    return NOERROR;
}

ECode CameraProfile::GetJpegEncodingQualityParameter(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 quality,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (quality < ICameraProfile::QUALITY_LOW || quality > ICameraProfile::QUALITY_HIGH) {
        //throw new IllegalArgumentException("Unsupported quality level: " + quality);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    synchronized(mCacheLock) {
        AutoPtr<ArrayOf<Int32> > levels = sCache[cameraId];
        if (levels == NULL) {
            levels = GetImageEncodingQualityLevels(cameraId);
            sCache[cameraId] = levels;
        }
        *result = (*levels)[quality];
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CameraProfile::GetImageEncodingQualityLevels(
    /* [in] */ Int32 cameraId)
{
    Int32 nLevels = NativeGetNumImageEncodingQualityLevels(cameraId);
    if (nLevels != ICameraProfile::QUALITY_HIGH + 1) {
        //throw new RuntimeException("Unexpected Jpeg encoding quality levels " + nLevels);
        return NULL;
    }

    AutoPtr<ArrayOf<Int32> > levels = ArrayOf<Int32>::Alloc(nLevels);
    for (Int32 i = 0; i < nLevels; ++i) {
        (*levels)[i] = NativeGetImageEncodingQualityLevel(cameraId, i);
    }
    Arrays::Sort(levels);  // Lower quality level ALWAYS comes before higher one
    return levels;
}

static Object sLock;
android::MediaProfiles* sProfiles = NULL;

// Methods implemented by JNI
void CameraProfile::NativeInit()
{
    ALOGV("native_init");
    AutoLock lock(sLock);

    if (sProfiles == NULL) {
        sProfiles = android::MediaProfiles::getInstance();
    }
    return;
}

Int32 CameraProfile::NativeGetNumImageEncodingQualityLevels(
    /* [in] */ Int32 cameraId)
{
    ALOGV("NativeGetNumImageEncodingQualityLevels");
    return sProfiles->getImageEncodingQualityLevels(cameraId).size();
}

Int32 CameraProfile::NativeGetImageEncodingQualityLevel(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 index)
{
    ALOGV("NativeGetImageEncodingQualityLevel");
    android::Vector<int> levels = sProfiles->getImageEncodingQualityLevels(cameraId);
    if (index < 0 || index >= (Int32)levels.size()) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "out of array boundary");
        return -1;
    }
    return levels[index];
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
