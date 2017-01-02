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

#include "elastos/droid/telephony/gsm/CGSMSmsManagerHelper.h"
#include "elastos/droid/telephony/gsm/GSMSmsManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGSMSmsManagerHelper, Singleton, ISmsManagerHelper);

CAR_SINGLETON_IMPL(CGSMSmsManagerHelper);

ECode CGSMSmsManagerHelper::GetDefault(
    /* [out] */ ISmsManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsManager> sm = GSMSmsManager::GetDefault();
    *result = sm;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos
