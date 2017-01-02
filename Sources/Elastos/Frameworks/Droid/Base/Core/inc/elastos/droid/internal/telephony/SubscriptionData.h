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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONDATA_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SubscriptionData
    : public Object
    , public ISubscriptionData
{
public:
    CAR_INTERFACE_DECL()

    constructor(
        /* [in] */ Int32 numSub);

    CARAPI IsSame(
        /* [in] */ ISubscriptionData* sub,
        /* [out] */ Boolean* result);

    CARAPI GetLength(
        /* [out] */ Int32* result);

    CARAPI CopyFrom(
        /* [in] */ ISubscriptionData* from,
        /* [out] */ ISubscriptionData** result);

    CARAPI GetIccId(
        /* [out] */ String* result);

    CARAPI HasSubscription(
        /* [in] */ ISubscription* sub,
        /* [out] */ Boolean* result);

    CARAPI GetSubscription(
        /* [in] */ ISubscription* sub,
        /* [out] */ ISubscription** result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetSubscriptions(
        /* [out, callee] */ ArrayOf<ISubscription*>** result);

private:
    static const String LOGTAG;
    Boolean DEBUG;

    AutoPtr<ArrayOf<ISubscription*> > mSubscription;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONDATA_H__
