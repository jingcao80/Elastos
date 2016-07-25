#ifndef __ELASTOS_DROID_TELECOM_CPHONEACCOUNTHANDLE_H__
#define __ELASTOS_DROID_TELECOM_CPHONEACCOUNTHANDLE_H__

#include "_Elastos_Droid_Telecom_CPhoneAccountHandle.h"
#include "elastos/droid/telecom/PhoneAccountHandle.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CPhoneAccountHandle)
    , public PhoneAccountHandle
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CPHONEACCOUNTHANDLE_H__