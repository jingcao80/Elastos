
#include "elastos/droid/hardware/camera2/utils/CameraBinderDecorator.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

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

    if ((UInt32)t == E_DEAD_OBJECT_EXCEPTION) {
        // throw new CameraRuntimeException(CAMERA_DISCONNECTED,
        //         "Process hosting the camera service has died unexpectedly",
        //         t);
        Slogger::E("CameraBinderDecorator", "CAMERA_DISCONNECTED , Process hosting the camera service has died unexpectedly");
        return E_CAMERA_RUNTIME_EXCEPTION;
    }
    else if ((UInt32)t == E_REMOTE_EXCEPTION) {
        // throw new UnsupportedOperationException("An unknown RemoteException was thrown" +
        //         " which should never happen.", t);
        Slogger::E("CameraBinderDecorator", "An unknown RemoteException was thrown which should never happen. %d", t);
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
            //throw new SecurityException("Lacking privileges to access camera service");
            Slogger::E("CameraBinderDecorator", "Lacking privileges to access camera service");
            return E_SECURITY_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_ALREADY_EXISTS:
            // This should be handled at the call site. Typically this isn't bad,
            // just means we tried to do an operation that already completed.
            return NOERROR;
        case ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE:
            //throw new IllegalArgumentException("Bad argument passed to camera service");
            Slogger::E("CameraBinderDecorator", "Bad argument passed to camera service");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_DEAD_OBJECT:
            //throw new CameraRuntimeException(CAMERA_DISCONNECTED);
            Slogger::E("CameraBinderDecorator", "E_CAMERA_RUNTIME_EXCEPTION");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_TIMED_OUT:
            // throw new CameraRuntimeException(CAMERA_ERROR,
            //         "Operation timed out in camera service");
            Slogger::E("CameraBinderDecorator", "CAMERA_ERROR, Operation timed out in camera service");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EACCES:
            //throw new CameraRuntimeException(CAMERA_DISABLED);
            Slogger::E("CameraBinderDecorator", "CAMERA_DISABLED");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EBUSY:
            //throw new CameraRuntimeException(CAMERA_IN_USE);
            Slogger::E("CameraBinderDecorator", "CAMERA_IN_USE");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EUSERS:
            //throw new CameraRuntimeException(MAX_CAMERAS_IN_USE);
            Slogger::E("CameraBinderDecorator", "MAX_CAMERAS_IN_USE");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_ENODEV:
            //throw new CameraRuntimeException(CAMERA_DISCONNECTED);
            Slogger::E("CameraBinderDecorator", "CAMERA_DISCONNECTED");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_EOPNOTSUPP:
            //throw new CameraRuntimeException(CAMERA_DEPRECATED_HAL);
            Slogger::E("CameraBinderDecorator", "CAMERA_DEPRECATED_HAL");
            return E_CAMERA_RUNTIME_EXCEPTION;
        case ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION:
            // throw new CameraRuntimeException(CAMERA_ERROR,
            //         "Illegal state encountered in camera service.");
            Slogger::E("CameraBinderDecorator", "CAMERA_ERROR , Illegal state encountered in camera service.");
            return E_CAMERA_RUNTIME_EXCEPTION;
    }

    /**
     * Trap the rest of the negative return values. If we have known
     * error codes i.e. ALREADY_EXISTS that aren't really runtime
     * errors, then add them to the top switch statement
     */
    if (errorFlag < 0) {
        // throw new UnsupportedOperationException(String.format("Unknown error %d",
        //         errorFlag));
        Slogger::E("CameraBinderDecorator", "Unknown error %d", errorFlag);
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
