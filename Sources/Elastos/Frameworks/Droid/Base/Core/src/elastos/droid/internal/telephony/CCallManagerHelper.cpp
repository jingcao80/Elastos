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

#include "elastos/droid/internal/telephony/CCallManagerHelper.h"
#include "elastos/droid/internal/telephony/CallManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CCallManagerHelper, Singleton, ICallManagerHelper)
CAR_SINGLETON_IMPL(CCallManagerHelper)

ECode CCallManagerHelper::GetInstance(
    /* [out] */ ICallManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CallManager> cm = CallManager::GetInstance();
    *result = cm;
    REFCOUNT_ADD(*result);
    return NOERROR;

}

ECode CCallManagerHelper::IsSamePhone(
    /* [in] */ IPhone* p1,
    /* [in] */ IPhone* p2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CallManager::IsSamePhone(p1, p2);
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
