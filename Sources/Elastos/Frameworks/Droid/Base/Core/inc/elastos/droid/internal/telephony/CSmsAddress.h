
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSADDRESS_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsAddress.h"
#include "elastos/droid/internal/telephony/SmsAddress.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsAddress), public SmsAddress
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSADDRESS_H__
