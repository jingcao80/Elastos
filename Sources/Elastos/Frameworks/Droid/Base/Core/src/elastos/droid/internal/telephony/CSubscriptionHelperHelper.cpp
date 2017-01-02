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

#include "elastos/droid/internal/telephony/CSubscriptionHelperHelper.h"
#include "elastos/droid/internal/telephony/SubscriptionHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSubscriptionHelperHelper, Singleton, ISubscriptionHelperHelper)

CAR_SINGLETON_IMPL(CSubscriptionHelperHelper)

ECode CSubscriptionHelperHelper::Init(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ ISubscriptionHelper** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionHelper> res = SubscriptionHelper::Init(c, ci);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSubscriptionHelperHelper::GetInstance(
    /* [out] */ ISubscriptionHelper** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionHelper> res = SubscriptionHelper::GetInstance();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony