//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
