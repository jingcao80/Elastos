
#include "elastos/droid/hardware/camera2/dispatch/DuckTypingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CMethodNameInvoker.h"
#include "elastos/droid/internal/utility/Preconditions.h"

using Elastos::Droid::Hardware::Camera2::Dispatch::IMethodNameInvoker;
using Elastos::Droid::Hardware::Camera2::Dispatch::CMethodNameInvoker;
using Elastos::Droid::Internal::Utility::Preconditions;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_INTERFACE_IMPL_2(DuckTypingDispatcher, Object, IDuckTypingDispatcher, IDispatchable)

DuckTypingDispatcher::DuckTypingDispatcher()
{
}

ECode DuckTypingDispatcher::constructor()
{
    return NOERROR;
}

ECode DuckTypingDispatcher::constructor(
    /* [in] */ IDispatchable* target,
    /* [in] */ IClassInfo* targetClass)
{
    FAIL_RETURN(Preconditions::CheckNotNull(targetClass, String("targetClass must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(target, String("target must not be null")))

    return CMethodNameInvoker::New(target, targetClass, (IMethodNameInvoker**)&mDuck);
}

ECode DuckTypingDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    String name;
    method->GetName(&name);
    return mDuck->Invoke(name, args);
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
