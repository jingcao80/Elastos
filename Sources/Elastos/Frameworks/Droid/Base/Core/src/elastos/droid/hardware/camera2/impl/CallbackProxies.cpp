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

#include "elastos/droid/hardware/camera2/impl/CallbackProxies.h"
#include "elastos/droid/hardware/camera2/dispatch/CMethodNameInvoker.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/ClassLoader.h>

using Elastos::Droid::Hardware::Camera2::Dispatch::CMethodNameInvoker;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::ClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

static const String TAG("CallbackProxies");

static AutoPtr<IInterfaceInfo> GetInterfaceInfo(
    /* [in] */ const String& itfcName)
{
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    AutoPtr<IInterfaceInfo> itfInfo;
    cl->LoadInterface(itfcName, (IInterfaceInfo**)&itfInfo);
    if (itfInfo == NULL) {
        Logger::E("CallbackProxies", "failed to load [%s] from %s", itfcName.string(), TO_CSTR(cl));
    }

    return itfInfo;
}

//==============================================================================================
// CallbackProxies::DeviceStateCallbackProxy
//==============================================================================================
CAR_INTERFACE_IMPL(CallbackProxies::DeviceStateCallbackProxy, CameraDeviceImpl::StateCallbackKK,
        ICallbackProxiesDeviceStateCallbackProxy)

ECode CallbackProxies::DeviceStateCallbackProxy::constructor(
    /* [in] */ IDispatchable* dispatchTarget)
{
    FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))

    const String itfcName("Elastos.Droid.Hardware.Camera2.Impl.ICameraDeviceImplStateCallbackKK");
    AutoPtr<IInterfaceInfo> itfInfo = GetInterfaceInfo(itfcName);
    if (itfInfo == NULL) {
        return E_INVALID_ARGUMENT;
    }

    return CMethodNameInvoker::New(dispatchTarget, itfInfo, (IMethodNameInvoker**)&mProxy);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnOpened(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnOpened"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnDisconnected"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnError"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*I32)E"), (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfInt32(1, error);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnUnconfigured(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnUnconfigured"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo));
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnActive(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnActive"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo));
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnBusy(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnBusy"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo));
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnClosed(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnClosed"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo));
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceStateCallbackProxy::OnIdle(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IMethodInfo> methodInfo;
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnIdle"),
        String("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*)E"), (IMethodInfo**)&methodInfo));
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);

    return mProxy->Invoke(methodInfo, args);
}

//==============================================================================================
// CallbackProxies::DeviceCaptureCallbackProxy
//==============================================================================================
CAR_INTERFACE_IMPL(CallbackProxies::DeviceCaptureCallbackProxy, CameraDeviceImpl::CaptureCallback,
        ICallbackProxiesDeviceCaptureCallbackProxy)

ECode CallbackProxies::DeviceCaptureCallbackProxy::constructor(
    /* [in] */ IDispatchable* dispatchTarget)
{
    FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))

    const String itfcName("Elastos.Droid.Hardware.Camera2.Impl.ICameraDeviceImplCaptureCallback");
    AutoPtr<IInterfaceInfo> itfInfo = GetInterfaceInfo(itfcName);
    if (itfInfo == NULL) {
        return E_INVALID_ARGUMENT;
    }

    return CMethodNameInvoker::New(dispatchTarget, itfInfo, (IMethodNameInvoker**)&mProxy);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureStarted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int64 frameNumber)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureRequest;*"
        "I64I64)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCaptureStarted"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfObjectPtr(1, request);
    args->SetInputArgumentOfInt64(2, timestamp);
    args->SetInputArgumentOfInt64(3, frameNumber);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCapturePartial(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* result)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureRequest;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureResult;*"
        ")E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCapturePartial"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfObjectPtr(1, request);
    args->SetInputArgumentOfObjectPtr(2, result);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureProgressed(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* partialResult)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureRequest;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureResult;*"
        ")E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCaptureProgressed"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfObjectPtr(1, request);
    args->SetInputArgumentOfObjectPtr(2, partialResult);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureRequest;*"
        "LElastos/Droid/Hardware/Camera2/ITotalCaptureResult;*"
        ")E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCaptureCompleted"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfObjectPtr(1, request);
    args->SetInputArgumentOfObjectPtr(2, result);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureFailed(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureFailure* failure)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureRequest;*"
        "LElastos/Droid/Hardware/Camera2/ICaptureFailure;*"
        ")E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCaptureFailed"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfObjectPtr(1, request);
    args->SetInputArgumentOfObjectPtr(2, failure);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureSequenceCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId,
    /* [in] */ Int64 frameNumber)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*I32I64)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCaptureSequenceCompleted"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfInt32(1, sequenceId);
    args->SetInputArgumentOfInt64(2, frameNumber);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::DeviceCaptureCallbackProxy::OnCaptureSequenceAborted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId)
{
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraDevice;*I32)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnCaptureSequenceAborted"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, camera);
    args->SetInputArgumentOfInt32(1, sequenceId);

    return mProxy->Invoke(methodInfo, args);
}

//==============================================================================================
// CallbackProxies::SessionStateCallbackProxy
//==============================================================================================
CAR_INTERFACE_IMPL(CallbackProxies::SessionStateCallbackProxy, CameraCaptureSession::StateCallback,
        ICallbackProxiesSessionStateCallbackProxy)

ECode CallbackProxies::SessionStateCallbackProxy::constructor(
    /* [in] */ IDispatchable* dispatchTarget)
{
   FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))

    const String itfcName("Elastos.Droid.Hardware.Camera2.ICameraCaptureSessionStateCallback");
    AutoPtr<IInterfaceInfo> itfInfo = GetInterfaceInfo(itfcName);
    assert(itfInfo != NULL);
    if (itfInfo == NULL) {
        return E_INVALID_ARGUMENT;
    }

    return CMethodNameInvoker::New(dispatchTarget, itfInfo, (IMethodNameInvoker**)&mProxy);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnConfigured(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::I(TAG, " >> OnConfigured");
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraCaptureSession;*)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnConfigured"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, session);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnConfigureFailed(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::I(TAG, " >> OnConfigureFailed");
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraCaptureSession;*)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnConfigureFailed"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, session);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnReady(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::I(TAG, " >> OnReady");
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraCaptureSession;*)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnReady"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, session);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnActive(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::I(TAG, " >> OnActive");
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraCaptureSession;*)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnActive"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, session);

    return mProxy->Invoke(methodInfo, args);
}

ECode CallbackProxies::SessionStateCallbackProxy::OnClosed(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::I(TAG, " >> OnClosed");
    AutoPtr<IMethodInfo> methodInfo;
    const String signature("(LElastos/Droid/Hardware/Camera2/ICameraCaptureSession;*)E");
    FAIL_RETURN(mProxy->GetMethodInfo(String("OnClosed"),signature, (IMethodInfo**)&methodInfo))
    AutoPtr<IArgumentList> args;
    ECode ec = methodInfo->CreateArgumentList((IArgumentList**)&args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create argument list at %s", __FUNCTION__);
        return ec;
    }
    args->SetInputArgumentOfObjectPtr(0, session);

    return mProxy->Invoke(methodInfo, args);
}

//==============================================================================================
// CallbackProxies
//==============================================================================================
CAR_INTERFACE_IMPL(CallbackProxies, Object, ICallbackProxies)

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos