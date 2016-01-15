
#include "elastos/droid/hardware/camera2/utils/Decorator.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(Decorator, Object, IDecorator)

ECode Decorator::constructor()
{
    return NOERROR;
}

ECode Decorator::constructor(
    /* [in] */ IInterface* obj,
    /* [in] */ IDecoratorListener* listener)
{
    mObject = obj;
    mListener = listener;
    return NOERROR;
}

ECode Decorator::NewInstance(
    /* [in] */ IInterface* obj,
    /* [in] */ IDecoratorListener* listener,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    assert(0)
    return NOERROR;
}

//@Override
ECode Decorator::Invoke(
    /* [in] */ IInterface* proxy,
    /* [in] */ IMethodInfo* Method m,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    assert(0)
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
