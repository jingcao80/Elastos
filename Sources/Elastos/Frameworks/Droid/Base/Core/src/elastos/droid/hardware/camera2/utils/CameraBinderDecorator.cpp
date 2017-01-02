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

#include "elastos/droid/hardware/camera2/utils/CameraBinderDecorator.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(CameraBinderDecorator::CameraBinderDecoratorListener, Object,
        ICameraBinderDecoratorListener, IDecoratorListener)

ECode CameraBinderDecorator::CameraBinderDecoratorListener::OnBeforeInvocation(
    /* [in] */ IMethodInfo* m,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    return NOERROR;
}

ECode CameraBinderDecorator::CameraBinderDecoratorListener::OnAfterInvocation(
    /* [in] */ IMethodInfo* m,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [in] */ IInterface* result)
{
    assert(0 && "TODO");
    // int return type => status_t => convert to exception
    // if (m.getReturnType() == Integer.TYPE) {
    //     Int32 returnValue = (Integer) result;
    //     ThrowOnError(returnValue);
    // }
    return NOERROR;
}

ECode CameraBinderDecorator::CameraBinderDecoratorListener::OnCatchException(
    /* [in] */ IMethodInfo* m,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [in] */ ECode t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (t == (ECode)E_DEAD_OBJECT_EXCEPTION) {
        // throw new CameraRuntimeException(CAMERA_DISCONNECTED,
        //         "Process hosting the camera service has died unexpectedly",
        //         t);
        Logger::E("CameraBinderDecorator", "CAMERA_DISCONNECTED , Process hosting the camera service has died unexpectedly");
        return E_CAMERA_RUNTIME_EXCEPTION;
    }
    else if (t == (ECode)E_REMOTE_EXCEPTION) {
        // throw new UnsupportedOperationException("An unknown RemoteException was thrown" +
        //         " which should never happen.", t);
        Logger::E("CameraBinderDecorator", "An unknown RemoteException was thrown which should never happen. %d", t);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CameraBinderDecorator::CameraBinderDecoratorListener::OnFinally(
    /* [in] */ IMethodInfo* m,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    return NOERROR;
}

ECode CameraBinderDecorator::ThrowOnError(
    /* [in] */ Int32 errorFlag)
{
    switch (errorFlag) {
        case ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR:
            return NOERROR;
        case ICameraBinderDecorator::ICameraBinderDecorator_PERMISSION_DENIED:
            Logger::E("CameraBinderDecorator", "Lacking privileges to access camera service");
            return E_SECURITY_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_ALREADY_EXISTS:
            // This should be handled at the call site. Typically this isn't bad,
            // just means we tried to do an operation that already completed.
            return NOERROR;
        case ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE:
            Logger::E("CameraBinderDecorator", "Bad argument passed to camera service");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_DEAD_OBJECT:
            Logger::E("CameraBinderDecorator", "E_CAMERA_RUNTIME_EXCEPTION");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_TIMED_OUT:
            Logger::E("CameraBinderDecorator", "CAMERA_ERROR, Operation timed out in camera service");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EACCES:
            Logger::E("CameraBinderDecorator", "CAMERA_DISABLED");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EBUSY:
            Logger::E("CameraBinderDecorator", "CAMERA_IN_USE");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EUSERS:
            Logger::E("CameraBinderDecorator", "MAX_CAMERAS_IN_USE");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_ENODEV:
            Logger::E("CameraBinderDecorator", "CAMERA_DISCONNECTED");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EOPNOTSUPP:
            Logger::E("CameraBinderDecorator", "CAMERA_DEPRECATED_HAL");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION:
            Logger::E("CameraBinderDecorator", "CAMERA_ERROR , Illegal state encountered in camera service.");
            return E_CAMERA_RUNTIME_EXCEPTION;
    }

    /**
     * Trap the rest of the negative return values. If we have known
     * error codes i.e. ALREADY_EXISTS that aren't really runtime
     * errors, then add them to the top switch statement
     */
    if (errorFlag < 0) {
        Logger::E("CameraBinderDecorator", "Unknown error %d(%08x)", errorFlag, errorFlag);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode CameraBinderDecorator::NewInstance(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(0 && "TODO");
    // Decorator.<T> newInstance(obj, new CameraBinderDecoratorListener());
    // REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
