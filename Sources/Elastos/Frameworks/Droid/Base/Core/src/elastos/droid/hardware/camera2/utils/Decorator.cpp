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

#include "elastos/droid/hardware/camera2/utils/Decorator.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(Decorator, Object, IDecorator)

ECode Decorator::constructor()
{
    return NOERROR;
}

ECode Decorator::constructor(
    /* [in] */ IInterface* obj,
    /* [in] */ IDecoratorListener* listener)
{
    mObject = obj;
    mListener = listener;
    return NOERROR;
}

ECode Decorator::NewInstance(
    /* [in] */ IInterface* obj,
    /* [in] */ IDecoratorListener* listener,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    assert(0)
    return NOERROR;
}

//@Override
ECode Decorator::Invoke(
    /* [in] */ IInterface* proxy,
    /* [in] */ IMethodInfo* Method m,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    assert(0)
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
