
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTION_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class Subscription
    : public Object
    , public ISubscription
{
public:
    CAR_INTERFACE_DECL()

    Subscription();

    /**
     * Returns the address of the SMS message in String form or NULL if unavailable
     */
    CARAPI IsSame(
        /* [in] */ ISubscription* sub,
        /* [out] */ Boolean* result);

    CARAPI Clear();

    CARAPI CopyFrom(
        /* [in] */ ISubscription* from,
        /* [out] */ ISubscription** result);

    CARAPI GetAppIndex(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Equals(IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetSlotId(
        /* [out] */ Int32* result);

    CARAPI SetSlotId(
        /* [in] */ Int32 slotId);

    CARAPI Get3gppIndex(
        /* [out] */ Int32* result);

    CARAPI Set3gppIndex(
        /* [in] */ Int32 index);

    CARAPI Get3gpp2Index(
        /* [out] */ Int32* result);

    CARAPI Set3gpp2Index(
        /* [in] */ Int32 index);

    CARAPI GetSubId(
        /* [out] */ Int32* result);

    CARAPI SetSubId(
        /* [in] */ Int32 subId);

    CARAPI GetSubscriptionStatus(
        /* [out] */ SubscriptionStatus* result);

    CARAPI SetSubscriptionStatus(
        /* [in] */ SubscriptionStatus status);

    CARAPI GetAppId(
        /* [out] */ String* result);

    CARAPI SetAppId(
        /* [in] */ const String& appId);

    CARAPI GetAppLabel(
        /* [out] */ String* result);

    CARAPI SetAppLabel(
        /* [in] */ const String& label);

    CARAPI GetAppType(
        /* [out] */ String* result);

    CARAPI SetAppType(
        /* [in] */ const String& type);

    CARAPI GetIccId(
        /* [out] */ String* result);

    CARAPI SetIccId(
        /* [in] */ const String& iccId);
public:
    //TODO
    Int32 mSlotId;                       // Slot id
    Int32 m3gppIndex;                   // Subscription index in the card for GSM
    Int32 m3gpp2Index;                  // Subscription index in the card for CDMA
    Int32 mSubId;                        // SUB 0 or SUB 1
    SubscriptionStatus mSubStatus;       // DEACTIVATE = 0, ACTIVATE = 1,
                                        // ACTIVATED = 2, DEACTIVATED = 3, INVALID = 4;
    String mAppId;
    String mAppLabel;
    String mAppType;
    String mIccId;

    static const Int32 SUBSCRIPTION_INDEX_INVALID;

private:
    static const String LOGTAG;
    Boolean DEBUG;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTION_H__
