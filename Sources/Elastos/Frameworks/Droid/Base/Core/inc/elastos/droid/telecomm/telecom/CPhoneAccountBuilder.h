
#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTBUILDER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTBUILDER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CPhoneAccountBuilder.h"
#include "elastos/droid/telecomm/telecom/PhoneAccount.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * Describes a distinct account, line of service or call placement method that the system
 * can use to place phone calls.
 * @hide
 */
// @SystemApi
CarClass(CPhoneAccountBuilder)
    , public PhoneAccount::Builder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELECOMM_TELECOM_CPHONEACCOUNTBUILDER_H__
