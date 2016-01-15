
#include "elastos/droid/hardware/camera2/impl/CallbackProxies.h"
#include "elastos/droid/hardware/camera2/dispatch/CMethodNameInvoker.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Hardware::Camera2::Dispatch::CMethodNameInvoker;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CAR_INTERFACE_IMPL(CallbackProxies::DeviceStateCallbackProxy, CameraDeviceImpl::StateCallbackKK,
        ICallbackProxiesDeviceStateCallbackProxy)

ECode CallbackProxies::DeviceStateCallbackProxy::constructor()
{
    return NOERROR;
}

ECode CallbackProxies::DeviceStateCallbackProxy::constructor(
    /* [in] */ IDispatchable* dispatchTarget)
{
    FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))
    assert(0);
    const String moduleName("............../Elastos.Droid.Core.eco");
    const String klassName(".............../CCameraDeviceImplStateCallbackKK;");
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IModuleInfo> moduleInfo;
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo));
    ASSERT_SUCCEEDED(moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo))
    return CMethodNameInvoker::New(dispatchTarget, classInfo, (IMethodNameInvoker**)&mProxy);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnOpened(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onOpened"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onDisconnected"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, TO_IINTERFACE(camera));
    AutoPtr<IInteger32> obj = CoreUtils::Convert(error);
    args->Set(1, TO_IINTERFACE(obj));
    return mProxy->Invoke(String("onError"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnUnconfigured(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onUnconfigured"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnActive(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onActive"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnBusy(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onBusy"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnClosed(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onClosed"), args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnIdle(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(camera));
    return mProxy->Invoke(String("onIdle"), args);
}

CAR_INTERFACE_IMPL(CallbackProxies::DeviceCaptureCallbackProxy, CameraDeviceImpl::CaptureCallback,
        ICallbackProxiesDeviceCaptureCallbackProxy)

ECode CallbackProxies::DeviceCaptureCallbackProxy::constructor()
{
    return NOERROR;
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::constructor(
    /* [in] */ IDispatchable* dispatchTarget)
{
    FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))
    assert(0);
    const String moduleName("............../Elastos.Droid.Core.eco");
    const String klassName(".............../CCameraDeviceImplCaptureCallback;");
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IModuleInfo> moduleInfo;
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo));
    ASSERT_SUCCEEDED(moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo))
    return CMethodNameInvoker::New(dispatchTarget, classInfo, (IMethodNameInvoker**)&mProxy);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureStarted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int64 frameNumber)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, TO_IINTERFACE(camera));
    args->Set(1, TO_IINTERFACE(request));
    AutoPtr<IInteger64> obj = CoreUtils::Convert(timestamp);
    args->Set(2, TO_IINTERFACE(obj));
    AutoPtr<IInteger64> obj2 = CoreUtils::Convert(frameNumber);
    args->Set(3, TO_IINTERFACE(obj2));
    return mProxy->Invoke(String("onCaptureStarted"), args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCapturePartial(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* result)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, TO_IINTERFACE(camera));
    args->Set(1, TO_IINTERFACE(request));
    args->Set(2, TO_IINTERFACE(result));
    return mProxy->Invoke(String("onCapturePartial"), args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureProgressed(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* partialResult)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, TO_IINTERFACE(camera));
    args->Set(1, TO_IINTERFACE(request));
    args->Set(2, TO_IINTERFACE(partialResult));
    return mProxy->Invoke(String("onCaptureProgressed"), args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, TO_IINTERFACE(camera));
    args->Set(1, TO_IINTERFACE(request));
    args->Set(2, TO_IINTERFACE(result));
    return mProxy->Invoke(String("onCaptureCompleted"), args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureFailed(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureFailure* failure)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, TO_IINTERFACE(camera));
    args->Set(1, TO_IINTERFACE(request));
    args->Set(2, TO_IINTERFACE(failure));
    return mProxy->Invoke(String("onCaptureFailed"), args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureSequenceCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId,
    /* [in] */ Int64 frameNumber)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, TO_IINTERFACE(camera));
    AutoPtr<IInteger32> obj = CoreUtils::Convert(sequenceId);
    args->Set(1, TO_IINTERFACE(obj));
    AutoPtr<IInteger64> obj2 = CoreUtils::Convert(frameNumber);
    args->Set(2, TO_IINTERFACE(obj2));
    return mProxy->Invoke(String("onCaptureSequenceCompleted"), args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureSequenceAborted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, TO_IINTERFACE(camera));
    AutoPtr<IInteger32> obj = CoreUtils::Convert(sequenceId);
    args->Set(1, TO_IINTERFACE(obj));
    return mProxy->Invoke(String("onCaptureSequenceAborted"), args);
}

CAR_INTERFACE_IMPL(CallbackProxies::SessionStateCallbackProxy, CameraCaptureSession::StateCallback,
        ICallbackProxiesSessionStateCallbackProxy)

ECode CallbackProxies::SessionStateCallbackProxy::constructor()
{
    return NOERROR;
}

ECode CallbackProxies::SessionStateCallbackProxy::constructor(
    /* [in] */ IDispatchable* dispatchTarget)
{
    FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))
    assert(0);
    const String moduleName("............../Elastos.Droid.Core.eco");
    const String klassName(".............../CCameraCaptureSessionStateCallback;");
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IModuleInfo> moduleInfo;
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo));
    ASSERT_SUCCEEDED(moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo))
    return CMethodNameInvoker::New(dispatchTarget, classInfo, (IMethodNameInvoker**)&mProxy);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnConfigured(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(session));
    return mProxy->Invoke(String("onConfigured"), args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnConfigureFailed(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(session));
    return mProxy->Invoke(String("onConfigureFailed"), args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnReady(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(session));
    return mProxy->Invoke(String("onReady"), args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnActive(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(session));
    return mProxy->Invoke(String("onActive"), args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnClosed(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(session));
    return mProxy->Invoke(String("onClosed"), args);
}

CAR_INTERFACE_IMPL(CallbackProxies, Object, ICallbackProxies)

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos