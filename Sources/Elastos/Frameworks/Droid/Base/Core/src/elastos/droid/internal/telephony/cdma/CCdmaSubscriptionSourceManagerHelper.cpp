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

#include "elastos/droid/internal/telephony/cdma/CCdmaSubscriptionSourceManagerHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaSubscriptionSourceManagerHelper, Singleton, ICdmaSubscriptionSourceManagerHelper)
CAR_SINGLETON_IMPL(CCdmaSubscriptionSourceManagerHelper)


ECode CCdmaSubscriptionSourceManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ ICdmaSubscriptionSourceManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaSubscriptionSourceManager> sm = CdmaSubscriptionSourceManager::GetInstance(context, ci, h, what, obj);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSubscriptionSourceManagerHelper::GetDefault(
    /* [in] */ IContext* context,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CdmaSubscriptionSourceManager::GetDefault(context);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
