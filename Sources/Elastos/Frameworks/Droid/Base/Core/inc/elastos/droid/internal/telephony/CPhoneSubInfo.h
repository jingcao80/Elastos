
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESUBINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESUBINFO_H__

#include "_Elastos_Droid_Internal_Telephony_CPhoneSubInfo.h"
#include "elastos/droid/internal/telephony/PhoneSubInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CPhoneSubInfo), public PhoneSubInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESUBINFO_H__
