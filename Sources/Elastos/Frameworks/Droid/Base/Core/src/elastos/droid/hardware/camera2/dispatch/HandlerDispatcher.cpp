
#include "elastos/droid/hardware/camera2/dispatch/HandlerDispatcher.h"
#include "elastos/droid/internal/utility/Preconditions.h"

using Elastos::Droid::Internal::Utility::Preconditions;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

HandlerDispatcher::MyRunnable::MyRunnable(
    /* [in] */ HandlerDispatcher* host,
    /* [in] */ IMethodInfo* method,
    /* [in] */ ArrayOf<IInterface*>* args)
    : mHost(host)
    , mMethod(method)
    , mArgs(args)
{
}

ECode HandlerDispatcher::MyRunnable::Run()
{
    // try {
    return  mHost->mDispatchTarget->Dispatch(mMethod, mArgs);
    // } catch (InvocationTargetException e) {
    //     Throwable t = e.getTargetException();
    //     // Potential UB. Hopefully 't' is a runtime exception.
    //     UncheckedThrow.throwAnyException(t);
    // } catch (IllegalAccessException e) {
    //     // Impossible
    //     Log.wtf(TAG, "IllegalAccessException while invoking " + method, e);
    // } catch (IllegalArgumentException e) {
    //     // Impossible
    //     Log.wtf(TAG, "IllegalArgumentException while invoking " + method, e);
    // } catch (Throwable e) {
    //     UncheckedThrow.throwAnyException(e);
    // }
}

const String HandlerDispatcher::TAG("HandlerDispatcher");

CAR_INTERFACE_IMPL_2(HandlerDispatcher, Object, IHandlerDispatcher, IDispatchable)

HandlerDispatcher::HandlerDispatcher()
{
}

ECode HandlerDispatcher::constructor()
{
    return NOERROR;
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
    /* [in] */ ArrayOf<IInterface*>* args)
{
    AutoPtr<MyRunnable> myRun = new MyRunnable(this, method, args);
    Boolean result;
    return mHandler->Post(IRunnable::Probe(myRun), &result);
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
