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

#include "elastos/droid/app/admin/CDevicePolicyManagerHelper.h"
#include "elastos/droid/app/admin/CDevicePolicyManager.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

CAR_INTERFACE_IMPL(CDevicePolicyManagerHelper, Singleton, IDevicePolicyManagerHelper)

CAR_SINGLETON_IMPL(CDevicePolicyManagerHelper)

ECode CDevicePolicyManagerHelper::Create(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [out] */ IDevicePolicyManager** policymanager)
{
    VALIDATE_NOT_NULL(policymanager)

    return CDevicePolicyManager::Create(context, handler, policymanager);
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos