
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_DECORATOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_DECORATOR_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class Decorator        //, public InvocationHandler
    : public Object
    , public ITypeReference
{
public:
    CAR_INTERFACE_DECL()

    Decorator() {}

    virtual ~Decorator() {}

    CARAPI constructor();

    /**
     * Create a decorator wrapping the specified object's method calls.
     *
     * @param obj the object whose method calls you want to intercept
     * @param listener the decorator handler for intercepted method calls
     * @param <T> the type of the element you want to wrap. This must be an interface.
     * @return a wrapped interface-compatible T
     */
    //@SuppressWarnings("unchecked")
    static CARAPI NewInstance(
        /* [in] */ IInterface* obj,
        /* [in] */ IDecoratorListener* listener,
        /* [out] */ IInterface** result);

    //@Override
    CARAPI Invoke(
        /* [in] */ IInterface* proxy,
        /* [in] */ IMethodInfo* Method m,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out] */ IInterface** result);

private:
    CARAPI constructor(
        /* [in] */ IInterface* obj,
        /* [in] */ IDecoratorListener* listener);

private:
    AutoPtr<IInterface> mObject;
    AutoPtr<IDecoratorListener> mListener;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_DECORATOR_H__
