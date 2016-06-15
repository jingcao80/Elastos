#ifndef __ELASTOS_DROID_TELEPHONY_CSMSMANAGERHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSMANAGERHELPER_H__

#include "_Elastos_Droid_Telephony_CSmsManagerHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSmsManagerHelper)
    , public Singleton
    , public ISmsManagerHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetDefault(
        /* [out] */ ISmsManager** result);

    CARAPI GetSmsManagerForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ ISmsManager** result);

    /**
      * Get the default sms subId
      *
      * @return the default sms subId
      * @hide
      */
    CARAPI GetDefaultSmsSubId(
        /* [out] */ Int64* result);

};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSMANAGERHELPER_H__
