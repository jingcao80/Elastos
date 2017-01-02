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

#include "elastos/droid/internal/inputmethod/InputMethodSubtypeSwitchingController.h"
#include "elastos/droid/internal/inputmethod/CInputMethodSubtypeSwitchingControllerHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace InputMethod {

CAR_INTERFACE_IMPL(CInputMethodSubtypeSwitchingControllerHelper,
    Singleton, IInputMethodSubtypeSwitchingControllerHelper)

CAR_SINGLETON_IMPL(CInputMethodSubtypeSwitchingControllerHelper)

ECode CInputMethodSubtypeSwitchingControllerHelper::CreateInstanceLocked(
    /* [in] */ IInputMethodSettings* settings,
    /* [in] */ IContext* context,
    /* [out] */ IInputMethodSubtypeSwitchingController** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<IInputMethodSubtypeSwitchingController> temp =
        InputMethodSubtypeSwitchingController::CreateInstanceLocked(settings, context);
    *instance = temp;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

} // namespace InputMethod
} // namespace Internal
} // namespace Droid
} // namespace Elastos
