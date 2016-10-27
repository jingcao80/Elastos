
#include "elastos/droid/hardware/camera2/dispatch/HandlerDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"

using Elastos::Droid::Internal::Utility::Preconditions;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

HandlerDispatcher::MyRunnable::MyRunnable(
    /* [in] */ IDispatchable* target,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
    : mDispatchTarget(target)
    , mMethod(method)
    , mArgs(args)
{
}

ECode HandlerDispatcher::MyRunnable::Run()
{
    ECode ec = mDispatchTarget->Dispatch(mMethod, mArgs);
    mDispatchTarget = NULL;
    return NOERROR;
}

const String HandlerDispatcher::TAG("HandlerDispatcher");

CAR_INTERFACE_IMPL_2(HandlerDispatcher, Object, IHandlerDispatcher, IDispatchable)

HandlerDispatcher::HandlerDispatcher()
{
}

ECode HandlerDispatcher::constructor(
    /* [in] */ IDispatchable* dispatchTarget,
    /* [in] */ IHandler* handler)
{
    FAIL_RETURN(Preconditions::CheckNotNull(dispatchTarget, String("dispatchTarget must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(handler, String("handler must not be null")))
    mDispatchTarget = dispatchTarget;
    mHandler = handler;
    return NOERROR;
}

ECode HandlerDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    Boolean result;
    AutoPtr<MyRunnable> myRun = new MyRunnable(mDispatchTarget, method, args);
    return mHandler->Post(myRun, &result);
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
