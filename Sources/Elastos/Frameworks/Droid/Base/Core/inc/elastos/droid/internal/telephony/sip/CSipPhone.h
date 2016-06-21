#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_CSIPPHONE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_CSIPPHONE_H__

#include "_Elastos_Droid_Internal_Telephony_Sip_CSipPhone.h"
#include "elastos/droid/internal/telephony/sip/SipPhone.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

CarClass(CSipPhone)
    , public SipPhone
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SIP_CSIPPHONE_H__
