
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONEPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONEPROXY_H__

#include "_Elastos_Droid_Internal_Telephony_CPhoneProxy.h"
#include "elastos/droid/internal/telephony/PhoneProxy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CPhoneProxy), public PhoneProxy
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONEPROXY_H__
