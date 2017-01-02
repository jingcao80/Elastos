//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
