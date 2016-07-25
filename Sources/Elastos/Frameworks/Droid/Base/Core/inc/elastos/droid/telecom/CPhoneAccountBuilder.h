
#ifndef __ELASTOS_DROID_TELECOM_CPHONEACCOUNTBUILDER_H__
#define __ELASTOS_DROID_TELECOM_CPHONEACCOUNTBUILDER_H__

#include "_Elastos_Droid_Telecom_CPhoneAccountBuilder.h"
#include "elastos/droid/telecom/PhoneAccount.h"

namespace Elastos {
namespace Droid {
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
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELECOM_CPHONEACCOUNTBUILDER_H__
