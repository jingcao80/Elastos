
#include "elastos/droid/hardware/camera2/dispatch/NullDispatcher.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(NullDispatcher, Object, INullDispatcher, IDispatchable)

NullDispatcher::NullDispatcher()
{
}

ECode NullDispatcher::constructor()
{
    return NOERROR;
}

ECode NullDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    return NOERROR;
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
