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

// #include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/hardware/display/CDisplayManagerGlobalHelper.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CDisplayManagerGlobalHelper, Singleton, IDisplayManagerGlobalHelper)

CAR_SINGLETON_IMPL(CDisplayManagerGlobalHelper)

ECode CDisplayManagerGlobalHelper::GetInstance(
    /* [out] */ IDisplayManagerGlobal** global)
{
    VALIDATE_NOT_NULL(global);

    AutoPtr<IDisplayManagerGlobal> temp = DisplayManagerGlobal::GetInstance();
    *global = temp;
    REFCOUNT_ADD(*global);
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
