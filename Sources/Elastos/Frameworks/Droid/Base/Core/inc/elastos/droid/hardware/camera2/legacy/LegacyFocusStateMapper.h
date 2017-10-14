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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYFOCUSSTATEMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYFOCUSSTATEMAPPER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::IAutoFocusMoveCallback;
using Elastos::Droid::Hardware::IHardwareCamera;
using Elastos::Droid::Hardware::IParameters;
using Elastos::Droid::Hardware::IAutoFocusCallback;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyFocusStateMapper
    : public Object
    , public ILegacyFocusStateMapper
{
private:
    class MyMoveCallback
       : public Object
       , public IAutoFocusMoveCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyMoveCallback(
            /* [in] */ LegacyFocusStateMapper* host,
            /* [in] */ Int32 currentAfRun,
            /* [in] */ const String& afMode);
        /**
         * Called when the camera auto focus starts or stops.
         *
         * @param start true if focus starts to move, false if focus stops to move
         * @param camera the Camera service object
         */
        CARAPI OnAutoFocusMoving(
            /* [in] */ Boolean start,
            /* [in] */ IHardwareCamera* camera);

    private:
        LegacyFocusStateMapper* mHost;
        Int32 mCurrentAfRun;
        String mAfMode;
    };

    class MyFocusCallback
        : public Object
        , public IAutoFocusCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyFocusCallback(
            /* [in] */ LegacyFocusStateMapper* host,
            /* [in] */ Int32 currentAfRun,
            /* [in] */ const String& afMode);

        /**
         * Called when the camera auto focus completes.  If the camera
         * does not support auto-focus and autoFocus is called,
         * onAutoFocus will be called immediately with a fake value of
         * <code>success</code> set to <code>true</code>.
         *
         * The auto-focus routine does not lock auto-exposure and auto-white
         * balance after it completes.
         *
         * @param success true if focus was successful, false if otherwise
         * @param camera  the Camera service object
         * @see android.hardware.Camera.Parameters#setAutoExposureLock(boolean)
         * @see android.hardware.Camera.Parameters#setAutoWhiteBalanceLock(boolean)
         */
        CARAPI OnAutoFocus(
            /* [in] */ Boolean success,
            /* [in] */ IHardwareCamera* camera);

    private:
        LegacyFocusStateMapper* mHost;
        Int32 mCurrentAfRun;
        String mAfMode;
    };

public:
    CAR_INTERFACE_DECL()

    LegacyFocusStateMapper();

    virtual ~LegacyFocusStateMapper() {}

    CARAPI constructor();

    /**
     * Instantiate a new focus state mapper.
     *
     * @param camera a non-{@code null} camera1 device
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    CARAPI constructor(
        /* [in] */ IHardwareCamera* camera);

    /**
     * Process the AF triggers from the request as a camera1 autofocus routine.
     *
     * <p>This method should be called after the parameters are {@link LegacyRequestMapper mapped}
     * with the request.</p>
     *
     * <p>Callbacks are processed in the background, and the next call to {@link #mapResultTriggers}
     * will have the latest AF state as reflected by the camera1 callbacks.</p>
     *
     * <p>None of the arguments will be mutated.</p>
     *
     * @param captureRequest a non-{@code null} request
     * @param parameters a non-{@code null} parameters corresponding to this request (read-only)
     */
    CARAPI ProcessRequestTriggers(
        /* [in] */ ICaptureRequest* captureRequest,
        /* [in] */ IParameters* parameters);

    /**
     * Update the {@code result} camera metadata map with the new value for the
     * {@code control.afState}.
     *
     * <p>AF callbacks are processed in the background, and each call to {@link #mapResultTriggers}
     * will have the latest AF state as reflected by the camera1 callbacks.</p>
     *
     * @param result a non-{@code null} result
     */
    CARAPI MapResultTriggers(
        /* [in] */ ICameraMetadataNative* result);

private:
    static CARAPI_(String) AfStateToString(
        /* [in] */ Int32 afState);

private:
    static const String TAG;
    static const Boolean VERBOSE;

    AutoPtr<IHardwareCamera> mCamera;

    Int32 mAfStatePrevious;
    String mAfModePrevious;

    /** Guard mAfRun and mAfState */
    Object mLock;
    /** Guard access with mLock */
    Int32 mAfRun;
    /** Guard access with mLock */
    Int32 mAfState;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYFOCUSSTATEMAPPER_H__
