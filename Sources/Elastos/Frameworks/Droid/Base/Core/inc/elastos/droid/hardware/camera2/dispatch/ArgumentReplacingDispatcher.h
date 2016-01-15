
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_ARGUMENTREPLACINGDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_ARGUMENTREPLACINGDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class ArgumentReplacingDispatcher
    : public Object
    , public IArgumentReplacingDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    ArgumentReplacingDispatcher();

    virtual ~ArgumentReplacingDispatcher() {}

    CARAPI constructor();

    /**
     * Create a new argument replacing dispatcher; dispatches are forwarded to {@code target}
     * after the argument is replaced.
     *
     * <p>For example, if a method {@code onAction(T1 a, Integer b, T2 c)} is invoked, and we wanted
     * to replace all occurrences of {@code b} with {@code 0xDEADBEEF}, we would set
     * {@code argumentIndex = 1} and {@code replaceWith = 0xDEADBEEF}.</p>
     *
     * <p>If a method dispatched has less arguments than {@code argumentIndex}, it is
     * passed through with the arguments unchanged.</p>
     *
     * @param target destination dispatch type, methods will be redirected to this dispatcher
     * @param argumentIndex the numeric index of the argument {@code >= 0}
     * @param replaceWith arguments matching {@code argumentIndex} will be replaced with this object
     */
    CARAPI constructor(
        /* [in] */ IDispatchable* target,
        /* [in] */ Int32 argumentIndex,
        /* [in] */ IInterface* replaceWith);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ ArrayOf<IInterface*>* args);

private:
    static CARAPI_(AutoPtr<ArrayOf<IInterface*> >) ArrayCopy(
        /* [in] */ ArrayOf<IInterface*>* array);

private:
    AutoPtr<IDispatchable> mTarget;
    Int32 mArgumentIndex;
    AutoPtr<IInterface> mReplaceWith;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_ARGUMENTREPLACINGDISPATCHER_H__
