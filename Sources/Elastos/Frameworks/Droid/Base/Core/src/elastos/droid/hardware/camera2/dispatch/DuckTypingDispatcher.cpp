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

ECode DuckTypingDispatcher::constructor(
    /* [in] */ IDispatchable* target,
    /* [in] */ IInterfaceInfo* targetClass)
{
    FAIL_RETURN(Preconditions::CheckNotNull(targetClass, String("targetClass must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(target, String("target must not be null")))

    return CMethodNameInvoker::New(target, targetClass, (IMethodNameInvoker**)&mDuck);
}

ECode DuckTypingDispatcher::Dispatch(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    return mDuck->Invoke(method, args);
}

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
