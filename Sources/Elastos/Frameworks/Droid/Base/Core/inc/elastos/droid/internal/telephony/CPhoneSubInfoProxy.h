
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESUBINFOPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESUBINFOPROXY_H__

#include "_Elastos_Droid_Internal_Telephony_CPhoneSubInfoProxy.h"
#include "elastos/droid/internal/telephony/PhoneSubInfoProxy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CPhoneSubInfoProxy), public PhoneSubInfoProxy
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESUBINFOPROXY_H__
