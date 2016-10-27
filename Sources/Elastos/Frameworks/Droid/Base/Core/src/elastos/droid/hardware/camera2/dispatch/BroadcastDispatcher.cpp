
#include "elastos/droid/hardware/camera2/dispatch/BroadcastDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(BroadcastDispatcher, Object, IBroadcastDispatcher, IDispatchable)

BroadcastDispatcher::BroadcastDispatcher()
{
}

ECode BroadcastDispatcher::constructor(
    /* [in] */ ArrayOf<IDispatchable*>* dispatchTargets)
{
    if (dispatchTargets == NULL) {
        Logger::E("BroadcastDispatcher", "dispatchTargets must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mDispatchTargets = dispatchTargets;
    return NOERROR;
}

ECode BroadcastDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
     for (Int32 i = 0; i < mDispatchTargets->GetLength(); i++) {
        (*mDispatchTargets)[i]->Dispatch(method, args);
    }
    return NOERROR;
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
