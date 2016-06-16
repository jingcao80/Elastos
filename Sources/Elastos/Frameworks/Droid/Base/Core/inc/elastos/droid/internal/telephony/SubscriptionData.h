
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
