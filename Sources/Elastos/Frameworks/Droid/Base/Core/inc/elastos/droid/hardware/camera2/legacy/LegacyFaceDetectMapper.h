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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYFACEDETECTMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYFACEDETECTMAPPER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::IHardwareCamera;
using Elastos::Droid::Hardware::ICameraFace;
using Elastos::Droid::Hardware::IFaceDetectionListener;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyFaceDetectMapper
    : public Object
    , public ILegacyFaceDetectMapper
{
private:
    class MyListener
        : public Object
        , public IFaceDetectionListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ LegacyFaceDetectMapper* host);

        /**
         * Notify the listener of the detected faces in the preview frame.
         *
         * @param faces The detected faces in a list
         * @param camera  The {@link Camera} service object
         */
        CARAPI OnFaceDetection(
            /* [in] */ ArrayOf<ICameraFace*>* faces,
            /* [in] */ IHardwareCamera* camera);

    private:
        LegacyFaceDetectMapper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    LegacyFaceDetectMapper();

    CARAPI constructor();

    /**
     * Instantiate a new face detect mapper.
     *
     * @param camera a non-{@code null} camera1 device
     * @param characteristics a  non-{@code null} camera characteristics for that camera1
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    CARAPI constructor(
        /* [in] */ IHardwareCamera* camera,
        /* [in] */ ICameraCharacteristics* characteristics);

    /**
     * Process the face detect mode from the capture request into an api1 face detect toggle.
     *
     * <p>This method should be called after the parameters are {@link LegacyRequestMapper mapped}
     * with the request.</p>
     *
     * <p>Callbacks are processed in the background, and the next call to {@link #mapResultTriggers}
     * will have the latest faces detected as reflected by the camera1 callbacks.</p>
     *
     * <p>None of the arguments will be mutated.</p>
     *
     * @param captureRequest a non-{@code null} request
     * @param parameters a non-{@code null} parameters corresponding to this request (read-only)
     */
    CARAPI ProcessFaceDetectMode(
        /* [in] */ ICaptureRequest* captureRequest,
        /* [in] */ IParameters* parameters);

    /**
     * Update the {@code result} camera metadata map with the new value for the
     * {@code statistics.faces} and {@code statistics.faceDetectMode}.
     *
     * <p>Face detect callbacks are processed in the background, and each call to
     * {@link #mapResultFaces} will have the latest faces as reflected by the camera1 callbacks.</p>
     *
     * <p>If the scene mode was set to {@code FACE_PRIORITY} but face detection is disabled,
     * the camera will still run face detection in the background, but no faces will be reported
     * in the capture result.</p>
     *
     * @param result a non-{@code null} result
     * @param legacyRequest a non-{@code null} request (read-only)
     */
    CARAPI MapResultFaces(
        /* [in] */ ICameraMetadataNative* result,
        /* [in] */ ILegacyRequest* legacyRequest);

private:
    static const String TAG;
    static const Boolean VERBOSE;// = Log.isLoggable(TAG, Log.VERBOSE);

    AutoPtr<IHardwareCamera> mCamera;
    /** Is the camera capable of face detection? */
    Boolean mFaceDetectSupported;
    /** Is the camera is running face detection? */
    Boolean mFaceDetectEnabled;
    /** Did the last request say to use SCENE_MODE = FACE_PRIORITY? */
    Boolean mFaceDetectScenePriority;
    /** Did the last request enable the face detect mode to ON? */
    Boolean mFaceDetectReporting;

    /** Synchronize access to all fields */
    Object mLock;
    AutoPtr<ArrayOf<ICameraFace*> > mFaces;
    AutoPtr<ArrayOf<ICameraFace*> > mFacesPrev;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYFACEDETECTMAPPER_H__
