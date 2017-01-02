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

#include "elastos/droid/internal/telephony/CSubscription.h"
#include "elastos/droid/internal/telephony/SubscriptionData.h"
#include <elastos/utility/Arrays.h>

using Elastos::Utility::Arrays;

using Elastos::Droid::Internal::Telephony::CSubscription;
using Elastos::Droid::Internal::Telephony::ISubscription;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Class holding a list of subscriptions
 */
CAR_INTERFACE_IMPL(SubscriptionData, Object, ISubscriptionData)

ECode SubscriptionData::constructor(
    /* [in] */ Int32 numSub)
{
    mSubscription = ArrayOf<ISubscription*>::Alloc(numSub);
    for (Int32 i = 0; i < numSub; i++) {
        CSubscription::New((ISubscription**)&(*mSubscription)[i]);
    }
    return NOERROR;
}

ECode SubscriptionData::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mSubscription != NULL) {
        *result = mSubscription->GetLength();
    }
    return NOERROR;
}

ECode SubscriptionData::CopyFrom(
    /* [in] */ ISubscriptionData* from,
    /* [out] */ ISubscriptionData** result)
{
    if (from != NULL) {
        Int32 length;
        from->GetLength(&length);
        mSubscription = NULL;
        mSubscription = ArrayOf<ISubscription*>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
             CSubscription::New((ISubscription**)&(*mSubscription)[i]);

             AutoPtr<ArrayOf<ISubscription*> > subscriptionFrom;
             from->GetSubscriptions((ArrayOf<ISubscription*>**)&subscriptionFrom);

             AutoPtr<ISubscription> sub;
            (*mSubscription)[i]->CopyFrom((*subscriptionFrom)[i], (ISubscription**)&sub);
        }
    }
    if (result != NULL) {
        *result = this;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode SubscriptionData::GetIccId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 length = mSubscription->GetLength();
    if (length > 0 && (*mSubscription)[0] != NULL) {
        (*mSubscription)[0]->GetIccId(result);
    } else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode SubscriptionData::HasSubscription(
    /* [in] */ ISubscription* sub,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    Int32 length = mSubscription->GetLength();
    for (Int32 i = 0; i < length; i++) {
        Boolean bSame = FALSE;
        (*mSubscription)[i]->IsSame(sub, &bSame);
        if (bSame) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode SubscriptionData::GetSubscription(
    /* [in] */ ISubscription* sub,
    /* [out] */ ISubscription** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 length = mSubscription->GetLength();
    for (Int32 i = 0; i < length; i++) {
        Boolean bSame = FALSE;
        (*mSubscription)[i]->IsSame(sub, &bSame);
        if (bSame) {
            *result = (*mSubscription)[i];
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SubscriptionData::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = Arrays::ToString(mSubscription);
    return NOERROR;
}

ECode SubscriptionData::GetSubscriptions(
    /* [out, callee] */ ArrayOf<ISubscription*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSubscription;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
