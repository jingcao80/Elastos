#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CSMSENVELOPE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CSMSENVELOPE_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_Sms_CSmsEnvelope.h"
#include "elastos/droid/internal/telephony/cdma/sms/SmsEnvelope.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CarClass(CSmsEnvelope)
    , public SmsEnvelope
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CSMSENVELOPE_H__
