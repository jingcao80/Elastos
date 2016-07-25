#ifndef __ELASTOS_DROID_TELECOM_CPHONEACCOUNT_H__
#define __ELASTOS_DROID_TELECOM_CPHONEACCOUNT_H__

#include "_Elastos_Droid_Telecom_CPhoneAccount.h"
#include "elastos/droid/telecom/PhoneAccount.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CPhoneAccount)
    , public PhoneAccount
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPHONEACCOUNT_H__