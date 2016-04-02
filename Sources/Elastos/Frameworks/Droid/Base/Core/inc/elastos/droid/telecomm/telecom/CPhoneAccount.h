#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNT_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNT_H__

#include "_Elastos_Droid_Telecomm_Telecom_CPhoneAccount.h"
#include "elastos/droid/telecomm/telecom/PhoneAccount.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CPhoneAccount)
    , public PhoneAccount
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNT_H__