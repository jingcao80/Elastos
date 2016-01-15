
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CAMERABINDERDECORATOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CAMERABINDERDECORATOR_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

using Elastos::Droid::Hardware::Camera2::Utils::IDecoratorListener;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class CameraBinderDecorator
{
private:
    class CameraBinderDecoratorListener
        : public Object
        , public ICameraBinderDecoratorListener
        , public IDecoratorListener
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        OnBeforeInvocation(
            /* [in] */ IMethodInfo* m,
            /* [in] */ ArrayOf<IInterface*>* args);

        //@Override
        OnAfterInvocation(
            /* [in] */ IMethodInfo* m,
            /* [in] */ ArrayOf<IInterface*>* args,
            /* [in] */ IInterface* result);

        //@Override
        OnCatchException(
            /* [in] */ IMethodInfo* m,
            /* [in] */ ArrayOf<IInterface*>* args,
            /* [in] */ ECode t,
            /* [out] */ Boolean* result);

        //@Override
        OnFinally(
            /* [in] */ IMethodInfo* m,
            /* [in] */ ArrayOf<IInterface*>* args);
    };

public:
    /**
     * Throw error codes returned by the camera service as exceptions.
     *
     * @param errorFlag error to throw as an exception.
     */
    static CARAPI ThrowOnError(
        /* [in] */ Int32 errorFlag);

    /**
     * <p>
     * Wraps the type T with a proxy that will check 'status_t' return codes
     * from the native side of the camera service, and throw Java exceptions
     * automatically based on the code.
     * </p>
     * <p>
     * In addition it also rewrites binder's RemoteException into either a
     * CameraAccessException or an UnsupportedOperationException.
     * </p>
     * <p>
     * As a result of calling any method on the proxy, RemoteException is
     * guaranteed never to be thrown.
     * </p>
     *
     * @param obj object that will serve as the target for all method calls
     * @param <T> the type of the element you want to wrap. This must be an interface.
     * @return a proxy that will intercept all invocations to obj
     */
    static CARAPI NewInstance(
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** result);
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CAMERABINDERDECORATOR_H__
