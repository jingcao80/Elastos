#ifndef __ELASTOS_DROID_SERVER_CTORCHSERVICE_H__
#define __ELASTOS_DROID_SERVER_CTORCHSERVICE_H__

#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CTorchService.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Hardware::IITorchService;
using Elastos::Droid::Hardware::IITorchCallback;
using Elastos::Droid::Hardware::Camera2::ICameraDevice;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSession;
using Elastos::Droid::Hardware::Camera2::ICameraManager;
using Elastos::Droid::Hardware::Camera2::ICameraDevice;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSession;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionStateCallback;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionStateListener;
using Elastos::Droid::Hardware::Camera2::ICameraManagerAvailabilityCallback;
using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateListener;
using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * @hide
 */
CarClass(CTorchService)
    , public Object
    , public IITorchService
{
private:
    class CameraUserRecord
        : public Object
    {
    public:
        CameraUserRecord(
            /* [in] */ IBinder* token);

    public:
        AutoPtr<IBinder> mToken;
        Int32 mPid;
        Int32 mUid;
    };

    class UpdateRunnable
        : public Runnable
    {
    public:
        UpdateRunnable(
            /* [in] */ CTorchService* host);

        CARAPI Run();

    public:
        CTorchService* mHost;
    };

    class KillRunnable
        : public Runnable
    {
    public:
        KillRunnable(
            /* [in] */ CTorchService* host);

        CARAPI Run();

    public:
        CTorchService* mHost;
    };

    class CameraDeviceStateListener
        : public Object
        , public ICameraDeviceStateCallback
        , public ICameraDeviceStateListener
    {
    public:
        CAR_INTERFACE_DECL()

        CameraDeviceStateListener(
            /* [in] */ CTorchService* host);

        CARAPI OnOpened(
            /* [in] */ ICameraDevice* camera);

        CARAPI OnDisconnected(
            /* [in] */ ICameraDevice* camera);

        CARAPI OnClosed(
            /* [in] */ ICameraDevice* camera) { return NOERROR; }

        CARAPI OnError(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 error);

    public:
        CTorchService* mHost;
    };

    class CameraCaptureSessionStateListener
        : public Object
        , public ICameraCaptureSessionStateCallback
        , public ICameraCaptureSessionStateListener
    {
    public:
        CAR_INTERFACE_DECL()

        CameraCaptureSessionStateListener(
            /* [in] */ CTorchService* host);

        CARAPI OnConfigured(
            /* [in] */ ICameraCaptureSession* session);

        CARAPI OnConfigureFailed(
            /* [in] */ ICameraCaptureSession* session);

        CARAPI OnReady(
            /* [in] */ ICameraCaptureSession* session) { return NOERROR; }

        CARAPI OnActive(
            /* [in] */ ICameraCaptureSession* session) { return NOERROR; }

        CARAPI OnClosed(
            /* [in] */ ICameraCaptureSession* session) { return NOERROR; }

    public:
        CTorchService* mHost;
    };

    class CameraManagerAvailabilityCallback
        : public Object
        , public ICameraManagerAvailabilityCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CameraManagerAvailabilityCallback(
            /* [in] */ CTorchService* host);

        CARAPI OnCameraAvailable(
            /* [in] */ const String& cameraId);

        CARAPI OnCameraUnavailable(
            /* [in] */ const String& cameraId);

        CARAPI SetTorchAvailable(
            /* [in] */ Boolean available);

    public:
        CTorchService* mHost;
    };

    class BinderDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        BinderDeathRecipient(
            /* [in] */ IBinder* token,
            /* [in] */ Int32 cameraId,
            /* [in] */ CTorchService* host);

        CARAPI ProxyDied();

    public:
        CTorchService* mHost;
        Int32 mCameraId;
        AutoPtr<IBinder> mToken;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CTorchService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Initialize();

    CARAPI OnCameraOpened(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 cameraId);

    CARAPI OnCameraClosed(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 cameraId);

    CARAPI SetTorchEnabled(
        /* [in] */ Boolean enabled);

    CARAPI ToggleTorch();

    CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    CARAPI IsTorchOn(
        /* [out] */ Boolean* result);

    CARAPI AddListener(
        /* [in] */ IITorchCallback* l);

    CARAPI RemoveListener(
        /* [in] */ IITorchCallback* l);

private:
    CARAPI_(void) EnsureHandler();

    CARAPI_(void) StartDevice();

    CARAPI_(void) StartSession();

    CARAPI_(AutoPtr<ISize>) GetSmallestSize(
        /* [in] */ String cameraId);

    CARAPI_(void) PostUpdateFlashlight();

    CARAPI_(String) GetCameraId();

    CARAPI_(void) UpdateFlashlight(
        /* [in] */ Boolean forceDisable);

    CARAPI_(void) RemoveCameraUserLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 cameraId);

    CARAPI_(void) TeardownTorch();

    CARAPI_(void) HandleError();

    CARAPI_(void) DispatchStateChange(
        /* [in] */ Boolean on);

    CARAPI_(void) DispatchError();

    CARAPI_(void) DispatchAvailabilityChanged(
        /* [in] */ Boolean available);

    CARAPI_(void) DispatchListeners(
        /* [in] */ Int32 message,
        /* [in] */ Boolean argument);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 DISPATCH_ERROR;
    static const Int32 DISPATCH_STATE_CHANGE;
    static const Int32 DISPATCH_AVAILABILITY_CHANGED;

    AutoPtr<IContext> mContext;

    AutoPtr<ISparseArray> mCamerasInUse;

    /** Call {@link #ensureHandler()} before using */
    AutoPtr<IHandler> mHandler;

    /** Lock on mListeners when accessing */
    AutoPtr<IRemoteCallbackList> mListeners;

    /** Lock on {@code this} when accessing */
    Boolean mTorchEnabled;

    /** Whether the camera is available **/
    Boolean mTorchAvailable;

    Int32 mTorchAppUid;
    Int32 mTorchCameraId;

    AutoPtr<ICameraManager> mCameraManager;
    AutoPtr<ICameraDevice> mCameraDevice;
    Boolean mOpeningCamera;
    AutoPtr<ICaptureRequest> mFlashlightRequest;
    AutoPtr<ICameraCaptureSession> mSession;
    AutoPtr<ISurfaceTexture> mSurfaceTexture;
    AutoPtr<ISurface> mSurface;

    AutoPtr<IRunnable> mUpdateFlashlightRunnable;
    AutoPtr<IRunnable> mKillFlashlightRunnable;

    AutoPtr<ICameraDeviceStateCallback> mTorchCameraListener;
    AutoPtr<ICameraCaptureSessionStateCallback> mTorchSessionListener;
    AutoPtr<ICameraManagerAvailabilityCallback> mAvailabilityCallback;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CTORCHSERVICE_H__
