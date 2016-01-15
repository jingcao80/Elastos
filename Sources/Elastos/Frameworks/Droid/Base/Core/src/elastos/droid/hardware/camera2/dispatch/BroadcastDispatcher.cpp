
#include "elastos/droid/hardware/camera2/dispatch/BroadcastDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(BroadcastDispatcher, Object, IBroadcastDispatcher, IDispatchable)

BroadcastDispatcher::BroadcastDispatcher()
{
}

ECode BroadcastDispatcher::constructor()
{
    return NOERROR;
}

ECode BroadcastDispatcher::constructor(
    /* [in] */ ArrayOf<IDispatchable*>* dispatchTargets)
{
    //FAIL_RETURN(Preconditions::CheckNotNull(dispatchTargets,
    //        String("dispatchTargets must not be null")))
    if (dispatchTargets == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    return Arrays::AsList(dispatchTargets, (IList**)&mDispatchTargets);
}

ECode BroadcastDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    Boolean gotResult = FALSE;

    Int32 size;
    mDispatchTargets->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDispatchTargets->Get(i, (IInterface**)&obj);
        AutoPtr<IDispatchable> dispatchTarget = IDispatchable::Probe(obj);

        dispatchTarget->Dispatch(method, args);

        if (!gotResult) {
            gotResult = TRUE;
        }
    }
    return NOERROR;
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
