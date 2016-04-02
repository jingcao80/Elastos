#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTHANDLE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTHANDLE_H__

#include "_Elastos_Droid_Telecomm_Telecom_CPhoneAccountHandle.h"
#include "elastos/droid/telecomm/telecom/PhoneAccountHandle.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CPhoneAccountHandle)
    , public PhoneAccountHandle
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTHANDLE_H__