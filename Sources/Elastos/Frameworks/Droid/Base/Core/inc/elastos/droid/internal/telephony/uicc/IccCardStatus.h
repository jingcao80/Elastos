#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDSTATUS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDSTATUS_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * See also RIL_CardStatus in include/telephony/ril.h
  *
  * {@hide}
  */
class IccCardStatus
    : public Object
    , public IIccCardStatus
{
public:
    CAR_INTERFACE_DECL();

    virtual CARAPI SetCardState(
        /* [in] */ Int32 state);

    virtual CARAPI SetUniversalPinState(
        /* [in] */ Int32 state);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    static const Int32 CARD_MAX_APPS = 8;
    CardState mCardState;
    PinState mUniversalPinState;
    Int32 mGsmUmtsSubscriptionAppIndex;
    Int32 mCdmaSubscriptionAppIndex;
    Int32 mImsSubscriptionAppIndex;
    AutoPtr<ArrayOf<IIccCardApplicationStatus*> > mApplications;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDSTATUS_H__

