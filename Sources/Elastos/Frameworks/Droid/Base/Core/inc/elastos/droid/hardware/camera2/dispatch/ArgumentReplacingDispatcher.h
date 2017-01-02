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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_ARGUMENTREPLACINGDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_ARGUMENTREPLACINGDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class ArgumentReplacingDispatcher
    : public Object
    , public IArgumentReplacingDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    ArgumentReplacingDispatcher();

    virtual ~ArgumentReplacingDispatcher() {}

    /**
     * Create a new argument replacing dispatcher; dispatches are forwarded to {@code target}
     * after the argument is replaced.
     *
     * <p>For example, if a method {@code onAction(T1 a, Integer b, T2 c)} is invoked, and we wanted
     * to replace all occurrences of {@code b} with {@code 0xDEADBEEF}, we would set
     * {@code argumentIndex = 1} and {@code replaceWith = 0xDEADBEEF}.</p>
     *
     * <p>If a method dispatched has less arguments than {@code argumentIndex}, it is
     * passed through with the arguments unchanged.</p>
     *
     * @param target destination dispatch type, methods will be redirected to this dispatcher
     * @param argumentIndex the numeric index of the argument {@code >= 0}
     * @param replaceWith arguments matching {@code argumentIndex} will be replaced with this object
     */
    CARAPI constructor(
        /* [in] */ IDispatchable* target,
        /* [in] */ Int32 argumentIndex,
        /* [in] */ IInterface* replaceWith);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    AutoPtr<IDispatchable> mTarget;
    Int32 mArgumentIndex;
    AutoPtr<IInterface> mReplaceWith;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_ARGUMENTREPLACINGDISPATCHER_H__
