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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SECURECAMERALAUNCHMANAGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SECURECAMERALAUNCHMANAGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/hardware/camera2/CameraManager.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Camera2::CameraManager;
using Elastos::Droid::Hardware::Camera2::ICameraManager;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Handles launching the secure camera properly even when other applications may be using the camera
 * hardware.
 *
 * When other applications (e.g., Face Unlock) are using the camera, they must close the camera to
 * allow the secure camera to open it.  Since we want to minimize the delay when opening the secure
 * camera, other apps should close the camera at the first possible opportunity (i.e., as soon as
 * the user begins swiping to go to the secure camera).
 *
 * If the camera is unavailable when the user begins to swipe, the SecureCameraLaunchManager sends a
 * broadcast to tell other apps to close the camera.  When and if the user completes their swipe to
 * launch the secure camera, the SecureCameraLaunchManager delays launching the secure camera until
 * a callback indicates that the camera has become available.  If it doesn't receive that callback
 * within a specified timeout period, the secure camera is launched anyway.
 *
 * Ideally, the secure camera would handle waiting for the camera to become available.  This allows
 * some of the time necessary to close the camera to happen in parallel with starting the secure
 * camera app.  We can't rely on all third-party camera apps to handle this.  However, an app can
 * put com.android.systemui.statusbar.phone.will_wait_for_camera_available in its meta-data to
 * indicate that it will be responsible for waiting for the camera to become available.
 *
 * It is assumed that the functions in this class, including the constructor, will be called from
 * the UI thread.
 */
class SecureCameraLaunchManager: public Object
{
private:
    class CameraAvailabilityCallback: public CameraManager::AvailabilityCallback
    {
    public:
        CameraAvailabilityCallback(
            /* [in] */ SecureCameraLaunchManager* host);

        // @Override
        CARAPI OnCameraUnavailable(
            /* [in] */ const String& cameraId);

        // @Override
        CARAPI OnCameraAvailable(
            /* [in] */ const String& cameraId);

    private:
        SecureCameraLaunchManager* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ SecureCameraLaunchManager* host);

        // @Override
        CARAPI Run();

    private:
        SecureCameraLaunchManager* mHost;
    };

    class Runnable2: public Runnable
    {
    public:
        Runnable2(
            /* [in] */ SecureCameraLaunchManager* host);

        // @Override
        CARAPI Run();

    private:
        SecureCameraLaunchManager* mHost;
    };

public:
    SecureCameraLaunchManager(
        /* [in] */ IContext* context,
        /* [in] */ IKeyguardBottomAreaView* keyguardBottomArea);

    /**
     * Initializes the SecureCameraManager and starts listening for camera availability.
     */
    CARAPI_(void) Create();

    /**
     * Stops listening for camera availability and cleans up the SecureCameraManager.
     */
    CARAPI_(void) Destroy();

    /**
     * Called when the user is starting to swipe horizontally, possibly to start the secure camera.
     * Although this swipe ultimately may not result in the secure camera opening, we need to stop
     * all other camera usage (e.g., Face Unlock) as soon as possible.  We send out a broadcast to
     * notify other apps that they should close the camera immediately.  The broadcast is sent even
     * if the camera appears to be available, because there could be an app that is about to open
     * the camera.
     */
    CARAPI_(void) OnSwipingStarted();

    /**
     * Called when the secure camera should be started.  If the camera is available or the secure
     * camera app has indicated that it will wait for camera availability, the secure camera app is
     * launched immediately.  Otherwise, we wait for the camera to become available (or timeout)
     * before launching the secure camera.
     */
    CARAPI_(void) StartSecureCameraLaunch();

private:
    /**
     * Returns TRUE if all of the cameras we are tracking are currently available.
     */
    CARAPI_(Boolean) AreAllCamerasAvailable();

    /**
     * Determines if the secure camera app will wait for the camera hardware to become available
     * before trying to open the camera.  If so, we can fire off an intent to start the secure
     * camera app before the camera is available.  Otherwise, it is our responsibility to wait for
     * the camera hardware to become available before firing off the intent to start the secure
     * camera.
     *
     * Ideally we are able to fire off the secure camera intent as early as possibly so that, if the
     * camera is closing, it can continue to close while the secure camera app is opening.  This
     * improves secure camera startup time.
     */
    CARAPI_(Boolean) TargetWillWaitForCameraAvailable();

    /**
     * Determines if the activity that would be launched by the intent is the ResolverActivity.
     */
    CARAPI_(Boolean) WouldLaunchResolverActivity(
        /* [in] */ IResolveInfo* resolved,
        /* [in] */ IList/*<ResolveInfo>*/* appList);

private:
    static const Boolean DEBUG;
    static const String TAG;

    // Action sent as a broadcast to tell other apps to stop using the camera.  Other apps that use
    // the camera from keyguard (e.g., Face Unlock) should listen for this broadcast and close the
    // camera as soon as possible after receiving it.
    static const String CLOSE_CAMERA_ACTION_NAME;

    // Apps should put this field in their meta-data to indicate that they will take on the
    // responsibility of waiting for the camera to become available.  If this field is present, the
    // SecureCameraLaunchManager launches the secure camera even if the camera hardware has not
    // become available.  Having the secure camera app do the waiting is the optimal approach, but
    // without this field, the SecureCameraLaunchManager doesn't launch the secure camera until the
    // camera hardware is available.
    static const String META_DATA_WILL_WAIT_FOR_CAMERA_AVAILABLE;

    // If the camera hardware hasn't become available after this period of time, the
    // SecureCameraLaunchManager launches the secure camera anyway.
    static const Int32 CAMERA_AVAILABILITY_TIMEOUT_MS;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IKeyguardBottomAreaView> mKeyguardBottomArea;

    AutoPtr<ICameraManager> mCameraManager;
    AutoPtr<CameraAvailabilityCallback> mCameraAvailabilityCallback;
    AutoPtr<IMap> mCameraAvailabilityMap;  /*<String, Boolean*/
    Boolean mWaitingToLaunchSecureCamera;
    AutoPtr<IRunnable> mLaunchCameraRunnable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SECURECAMERALAUNCHMANAGER_H__
