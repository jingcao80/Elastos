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

#include "elastos/droid/internal/telephony/CTelephonyDevControllerHelper.h"
#include "elastos/droid/internal/telephony/TelephonyDevController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CTelephonyDevControllerHelper, Singleton, ITelephonyDevControllerHelper)

CAR_SINGLETON_IMPL(CTelephonyDevControllerHelper)

ECode CTelephonyDevControllerHelper::Create(
    /* [out] */ ITelephonyDevController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITelephonyDevController> res = TelephonyDevController::Create();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyDevControllerHelper::GetInstance(
    /* [out] */ ITelephonyDevController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITelephonyDevController> res = TelephonyDevController::GetInstance();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyDevControllerHelper::RegisterRIL(
    /* [in] */ ICommandsInterface* cmdsIf)
{
    return TelephonyDevController::RegisterRIL(cmdsIf);
}

ECode CTelephonyDevControllerHelper::UnregisterRIL(
    /* [in] */ ICommandsInterface* cmdsIf)
{
    return TelephonyDevController::UnregisterRIL(cmdsIf);
}

ECode CTelephonyDevControllerHelper::GetModemCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 res = TelephonyDevController::GetModemCount();
    *result = res;
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony