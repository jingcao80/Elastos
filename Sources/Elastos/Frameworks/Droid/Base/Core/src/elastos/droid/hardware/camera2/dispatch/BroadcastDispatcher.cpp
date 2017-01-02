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
