
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_FLASHLIGHTCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_FLASHLIGHTCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/hardware/camera2/CameraCaptureSession.h>
#include <elastos/droid/hardware/camera2/CameraDevice.h>
#include <elastos/droid/hardware/camera2/CameraManager.h>
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Hardware::Camera2::CameraCaptureSession;
using Elastos::Droid::Hardware::Camera2::CameraDevice;
using Elastos::Droid::Hardware::Camera2::CameraManager;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSession;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionStateListener;
using Elastos::Droid::Hardware::Camera2::ICameraDevice;
using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateCallback;
using Elastos::Droid::Hardware::Camera2::ICameraManager;
using Elastos::Droid::Hardware::Camera2::ICameraManagerAvailabilityCallback;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Core::Runnable;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::Droid::Utility::ISize;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Manages the flashlight.
 */
class FlashlightController
    : public Object
    , public IFlashlightController
{
private:
    class CameraListener: public CameraDevice::StateListener
    {
    public:
        TO_STRING_IMPL("FlashlightController::CameraListener")

        CameraListener(
            /* [in] */ IWeakReference* host);

        // @Override
        CARAPI OnOpened(
            /* [in] */ ICameraDevice* camera);

        // @Override
        CARAPI OnDisconnected(
            /* [in] */ ICameraDevice* camera);

        // @Override
        CARAPI OnError(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 error);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class SessionListener: public CameraCaptureSession::StateListener
    {
    public:
        TO_STRING_IMPL("FlashlightController::SessionListener")

        SessionListener(
            /* [in] */ IWeakReference* host);

        // @Override
        CARAPI OnConfigured(
            /* [in] */ ICameraCaptureSession* session);

        // @Override
        CARAPI OnConfigureFailed(
            /* [in] */ ICameraCaptureSession* session);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class UpdateFlashlightRunnable: public Runnable
    {
    public:
        UpdateFlashlightRunnable(
            /* [in] */ IWeakReference* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class KillFlashlightRunnable: public Runnable
    {
    public:
        KillFlashlightRunnable(
            /* [in] */ IWeakReference* host);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class AvailabilityCallback: public CameraManager::AvailabilityCallback
    {
    public:
        TO_STRING_IMPL("FlashlightController::AvailabilityCallback")

        AvailabilityCallback(
            /* [in] */ IWeakReference* host);

        // @Override
        CARAPI OnCameraAvailable(
            /* [in] */ const String& cameraId);

        // @Override
        CARAPI OnCameraUnavailable(
            /* [in] */ const String& cameraId);

    private:
        CARAPI_(void) SetCameraAvailable(
            /* [in] */ Boolean available,
            /* [in] */ FlashlightController* host);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_INTERFACE_DECL()

    FlashlightController();

    ~FlashlightController();

    CARAPI constructor(
        /* [in] */ IContext* mContext);

    /*synchronized*/ CARAPI SetFlashlight(
        /* [in] */ Boolean enabled);

    CARAPI KillFlashlight();

    /*synchronized*/ CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    CARAPI AddListener(
        /* [in] */ IFlashlightListener* l);

    CARAPI RemoveListener(
        /* [in] */ IFlashlightListener* l);

private:
    CARAPI_(void) EnsureHandler();

    CARAPI StartDevice() /*throws CameraAccessException*/;

    CARAPI StartSession() /*throws CameraAccessException*/;

    CARAPI GetSmallestSize(
        /* [in] */ const String& cameraId,
        /* [out] */ ISize** result) /*throws CameraAccessException*/;

    CARAPI_(void) PostUpdateFlashlight();

    CARAPI GetCameraId(
        /* [out] */ String* id) /*throws CameraAccessException*/;

    CARAPI_(void) UpdateFlashlight(
        /* [in] */ Boolean forceDisable);

    CARAPI_(void) Teardown();

    CARAPI_(void) HandleError();

    CARAPI_(void) DispatchOff();

    CARAPI_(void) DispatchError();

    CARAPI_(void) DispatchAvailabilityChanged(
        /* [in] */ Boolean available);

    CARAPI_(void) DispatchListeners(
        /* [in] */ Int32 message,
        /* [in] */ Boolean argument);

    CARAPI_(void) CleanUpListenersLocked(
        /* [in] */ IFlashlightListener* listener);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 DISPATCH_ERROR;
    static const Int32 DISPATCH_OFF;
    static const Int32 DISPATCH_AVAILABILITY_CHANGED;

    AutoPtr<ICameraManager> mCameraManager;
    /** Call {@link #ensureHandler()} before using */
    AutoPtr<IHandler> mHandler;

    /** Lock on mListeners when accessing */
    AutoPtr<IArrayList> mListeners;  /*<WeakReference<FlashlightListener>*/

    /** Lock on {@code this} when accessing */
    Boolean mFlashlightEnabled;

    String mCameraId;
    Boolean mCameraAvailable;
    AutoPtr<ICameraDevice> mCameraDevice;
    AutoPtr<ICaptureRequest> mFlashlightRequest;
    AutoPtr<ICameraCaptureSession> mSession;
    AutoPtr<ISurfaceTexture> mSurfaceTexture;
    AutoPtr<ISurface> mSurface;
    AutoPtr<CameraListener> mCameraListener;
    AutoPtr<SessionListener> mSessionListener;

    AutoPtr<IRunnable> mUpdateFlashlightRunnable;
    AutoPtr<IRunnable> mKillFlashlightRunnable;
    AutoPtr<ICameraManagerAvailabilityCallback> mAvailabilityCallback;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_FLASHLIGHTCONTROLLER_H__
