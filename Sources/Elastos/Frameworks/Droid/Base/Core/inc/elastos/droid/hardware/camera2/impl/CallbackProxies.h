
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CALLBACKPROXIES_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CALLBACKPROXIES_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/hardware/camera2/CameraCaptureSession.h"
#include "elastos/droid/hardware/camera2/impl/CameraDeviceImpl.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Dispatch::IDispatchable;
using Elastos::Droid::Hardware::Camera2::Dispatch::IMethodNameInvoker;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

class CallbackProxies
    : public Object
    , public ICallbackProxies
{
public:
    class DeviceStateCallbackProxy
        : public CameraDeviceImpl::StateCallbackKK
        , public ICallbackProxiesDeviceStateCallbackProxy
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IDispatchable* dispatchTarget);

        //@Override
        CARAPI OnOpened(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnDisconnected(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnError(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 error);

        //@Override
        CARAPI OnUnconfigured(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnActive(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnBusy(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnClosed(
            /* [in] */ ICameraDevice* camera);

        //@Override
        CARAPI OnIdle(
            /* [in] */ ICameraDevice* camera);

        private:
        AutoPtr<IMethodNameInvoker> mProxy;
    };

    class DeviceCaptureCallbackProxy
        : public CameraDeviceImpl::CaptureCallback
        , public ICallbackProxiesDeviceCaptureCallbackProxy
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IDispatchable* dispatchTarget);

        //@Override
        CARAPI OnCaptureStarted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ Int64 timestamp,
            /* [in] */ Int64 frameNumber);

        //@Override
        CARAPI OnCapturePartial(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* result);

        //@Override
        CARAPI OnCaptureProgressed(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureResult* partialResult);

        //@Override
        CARAPI OnCaptureCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ITotalCaptureResult* result);

        //@Override
        CARAPI OnCaptureFailed(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ ICaptureFailure* failure);

        //@Override
        CARAPI OnCaptureSequenceCompleted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId,
            /* [in] */ Int64 frameNumber);

        //@Override
        CARAPI OnCaptureSequenceAborted(
            /* [in] */ ICameraDevice* camera,
            /* [in] */ Int32 sequenceId);

        private:
            AutoPtr<IMethodNameInvoker> mProxy;
    };

    class SessionStateCallbackProxy
        : public CameraCaptureSession::StateCallback
        , public ICallbackProxiesSessionStateCallbackProxy
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IDispatchable* dispatchTarget);

        //@Override
        CARAPI OnConfigured(
            /* [in] */ ICameraCaptureSession* session);

        //@Override
        CARAPI OnConfigureFailed(
            /* [in] */ ICameraCaptureSession* session);

        //@Override
        CARAPI OnReady(
            /* [in] */ ICameraCaptureSession* session);

        //@Override
        CARAPI OnActive(
            /* [in] */ ICameraCaptureSession* session);

        //@Override
        CARAPI OnClosed(
            /* [in] */ ICameraCaptureSession* session);

    private:
        AutoPtr<IMethodNameInvoker> mProxy;
    };

public:
    CAR_INTERFACE_DECL();

private:
    CallbackProxies(
        /* [in] */ IParcel* source) {}
};

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_IMPL_CALLBACKPROXIES_H__