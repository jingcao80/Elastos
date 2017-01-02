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

#ifndef __ELASTOS_DROID_HARDWARE_CCAMERAHELPER_H__
#define __ELASTOS_DROID_HARDWARE_CCAMERAHELPER_H__

#include "_Elastos_Droid_Hardware_CHardwareCameraHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CHardwareCameraHelper)
    , public Singleton
    , public IHardwareCameraHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCameraInfo(
        /* [in] */ Int32 cameraId,
        /* [in] */ IHardwareCameraInfo* cameraInfo);

    CARAPI Open(
        /* [in] */ Int32 cameraId,
        /* [out] */ IHardwareCamera** camera);

    CARAPI Open(
        /* [out] */ IHardwareCamera** camera);

    CARAPI GetEmptyParameters(
        /* [out] */ IParameters** para);

    /**
     * Returns the number of physical cameras available on this device.
     */
    /*public native*/
    CARAPI GetNumberOfCameras(
        /* [out] */ Int32* num);

    /**
     * @hide
     */
    CARAPI CheckInitErrors(
        /* [in] */ Int32 err,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    CARAPI OpenUninitialized(
       /* [out] */ IHardwareCamera** camera);

    /**
     * Returns a copied {@link Parameters}; for shim use only.
     *
     * @param parameters a non-{@code null} parameters
     * @return a Parameter object, with all the parameters copied from {@code parameters}.
     *
     * @throws NullPointerException if {@code parameters} was {@code null}
     * @hide
     */
    CARAPI GetParametersCopy(
        /* [in] */ IParameters* parameters,
        /* [out] */ IParameters** para);

    /**
     * Creates a new Camera object to access a particular hardware camera with
     * given hal API version. If the same camera is opened by other applications
     * or the hal API version is not supported by this device, this will throw a
     * RuntimeException.
     * <p>
     * You must call {@link #release()} when you are done using the camera,
     * otherwise it will remain locked and be unavailable to other applications.
     * <p>
     * Your application should only have one Camera object active at a time for
     * a particular hardware camera.
     * <p>
     * Callbacks from other methods are delivered to the event loop of the
     * thread which called open(). If this thread has no event loop, then
     * callbacks are delivered to the main application event loop. If there is
     * no main application event loop, callbacks are not delivered.
     * <p class="caution">
     * <b>Caution:</b> On some devices, this method may take a long time to
     * complete. It is best to call this method from a worker thread (possibly
     * using {@link android.os.AsyncTask}) to avoid blocking the main
     * application UI thread.
     *
     * @param cameraId The hardware camera to access, between 0 and
     * {@link #getNumberOfCameras()}-1.
     * @param halVersion The HAL API version this camera device to be opened as.
     * @return a new Camera object, connected, locked and ready for use.
     *
     * @throws IllegalArgumentException if the {@code halVersion} is invalid
     *
     * @throws RuntimeException if opening the camera fails (for example, if the
     * camera is in use by another process or device policy manager has disabled
     * the camera).
     *
     * @see android.app.admin.DevicePolicyManager#getCameraDisabled(android.content.ComponentName)
     * @see #CAMERA_HAL_API_VERSION_1_0
     *
     * @hide
     */
    CARAPI OpenLegacy(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 halVersion,
        /* [out] */ IHardwareCamera** camera);
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CCAMERAHELPER_H__
