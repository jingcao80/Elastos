#include "elastos/droid/os/ConditionVariable.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(ConditionVariable, Object, IConditionVariable)

ConditionVariable::ConditionVariable()
    : mCondition(FALSE)
{}

ConditionVariable::ConditionVariable(
    /* [in] */ Boolean state)
    : mCondition(state)
{}

ECode ConditionVariable::Open()
{
    synchronized(this) {
        Boolean old = mCondition;
        mCondition = TRUE;
        if (!old) {
            this->NotifyAll();
        }
    }
    return NOERROR;
}

ECode ConditionVariable::Close()
{
    synchronized(this) {
        mCondition = FALSE;
    }
    return NOERROR;
}

ECode ConditionVariable::Block()
{
    synchronized(this) {
        while (!mCondition) {
            // try {
                this->Wait();
            // }
            // catch (InterruptedException e) {
            // }
        }
    }
    return NOERROR;
}

ECode ConditionVariable::Block(
    /* [in] */ Int64 timeout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // Object.wait(0) means wait forever, to mimic this, we just
    // call the other block() method in that case.  It simplifies
    // this code for the common case.
    if (timeout != 0) {
        synchronized(this) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 now;
            system->GetCurrentTimeMillis(&now);
            Int64 end = now + timeout;
            while (!mCondition && now < end) {
                // try {
                    this->Wait(end-now);
                // }
                // catch (InterruptedException e) {
                // }
                system->GetCurrentTimeMillis(&now);
            }
            *result = mCondition;
        }
    }
    else {
        this->Block();
        *result =  TRUE;
    }
    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos