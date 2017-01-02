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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CUserManagerHelper.h"
#include "elastos/droid/os/CUserManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CUserManagerHelper, Singleton, IUserManagerHelper)

CAR_SINGLETON_IMPL(CUserManagerHelper)

ECode CUserManagerHelper::Get(
    /* [in] */ IContext* ctx,
    /* [out] */ IUserManager** userManager)
{
    VALIDATE_NOT_NULL(userManager)
    AutoPtr<IUserManager> um = CUserManager::Get(ctx);
    *userManager = um;
    REFCOUNT_ADD(*userManager)
    return NOERROR;
}

ECode CUserManagerHelper::SupportsMultipleUsers(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CUserManager::SupportsMultipleUsers();
    return NOERROR;
}

ECode CUserManagerHelper::GetMaxSupportedUsers(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = CUserManager::GetMaxSupportedUsers();
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
