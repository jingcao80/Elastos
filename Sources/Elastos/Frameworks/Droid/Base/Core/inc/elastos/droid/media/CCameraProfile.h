
#ifndef __ELASTOS_DROID_MEDIA_CCAMERAPROFILE_H__
#define __ELASTOS_DROID_MEDIA_CCAMERAPROFILE_H__

#include "_Elastos_Droid_Media_CCameraProfile.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The CameraProfile class is used to retrieve the pre-defined still image
 * capture (jpeg) quality levels (0-100) used for low, medium, and high
 * quality settings in the Camera application.
 *
 */
CarClass(CCameraProfile)
    , public Singleton
    , public ICameraProfile
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a pre-defined still image capture (jpeg) quality level
     * used for the given quality level in the Camera application for
     * the first back-facing camera on the device. If the device has no
     * back-facing camera, this returns 0.
     *
     * @param quality The target quality level
     */
    CARAPI GetJpegEncodingQualityParameter(
        /* [in] */ Int32 quality,
        /* [out] */ Int32* result);

    /**
     * Returns a pre-defined still image capture (jpeg) quality level
     * used for the given quality level in the Camera application for
     * the specified camera.
     *
     * @param cameraId The id of the camera
     * @param quality The target quality level
     */
    CARAPI GetJpegEncodingQualityParameter(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ Int32* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CCAMERAPROFILE_H__
