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

#include "elastos/droid/telephony/CSmsManagerHelper.h"
#include "elastos/droid/telephony/SmsManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CSmsManagerHelper, Singleton, ISmsManagerHelper)
CAR_SINGLETON_IMPL(CSmsManagerHelper)

ECode CSmsManagerHelper::GetDefault(
    /* [out] */ ISmsManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetDefault();
    *result = smsManager;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsManagerHelper::GetSmsManagerForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ ISmsManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber(subId);
    *result = smsManager;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsManagerHelper::GetDefaultSmsSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmsManager::GetDefaultSmsSubId();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
