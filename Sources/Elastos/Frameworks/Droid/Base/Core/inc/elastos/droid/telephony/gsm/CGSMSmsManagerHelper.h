#ifndef __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMANAGERHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMANAGERHELPER_H__

#include "_Elastos_Droid_Telephony_Gsm_CGSMSmsManagerHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CarClass(CGSMSmsManagerHelper)
    , public Singleton
    , public ISmsManagerHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetDefault(
        /* [out] */ ISmsManager** result);

};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMANAGERHELPER_H__
