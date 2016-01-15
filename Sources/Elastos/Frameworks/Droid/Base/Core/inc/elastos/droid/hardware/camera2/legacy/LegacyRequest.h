
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGECYREQUEST_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGECYREQUEST_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::IParameters;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyRequest
    : public Object
    , public ILegacyRequest
{
public:
    CAR_INTERFACE_DECL()

    LegacyRequest();

    virtual ~LegacyRequest() {}

    CARAPI constructor();

    /**
     * Create a new legacy request; the parameters are copied.
     *
     * @param characteristics immutable static camera characteristics for this camera
     * @param captureRequest immutable user-defined capture request
     * @param previewSize immutable internal preview size used for {@link Camera#setPreviewSurface}
     * @param parameters the initial camera1 parameter state; (copied) can be mutated
     */
    CARAPI constructor(
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ ICaptureRequest* captureRequest,
        /* [in] */ ISize* previewSize,
        /* [in] */ IParameters* parameters);

    /**
     * Update the current parameters in-place to be a copy of the new parameters.
     *
     * @param parameters non-{@code null} parameters for api1 camera
     */
    CARAPI SetParameters(
        /* [in] */ IParameters* parameters);

    CARAPI GetCameraCharacteristics(
        /* [out] */ ICameraCharacteristics** character);

    CARAPI GetCaptureRequest(
        /* [out] */ ICaptureRequest** request);

    CARAPI GetSize(
        /* [out] */ ISize** size);

    CARAPI GetParameters(
        /* [out] */ IParameters** para);

public:
    /** Immutable characteristics for the camera corresponding to this request */
    AutoPtr<ICameraCharacteristics> mCharacteristics;
    /** Immutable capture request, as requested by the user */
    AutoPtr<ICaptureRequest> mCaptureRequest;
    /** Immutable api1 preview buffer size at the time of the request */
    AutoPtr<ISize> mPreviewSize;
    /** <em>Mutable</em> camera parameters */
    AutoPtr<IParameters> mParameters;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGECYREQUEST_H__
