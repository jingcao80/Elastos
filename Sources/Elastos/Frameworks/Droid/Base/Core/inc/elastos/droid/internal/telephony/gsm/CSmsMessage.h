#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSMESSAGE_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/SmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CarClass(CSmsMessage)
    , public SmsMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSMESSAGE_H__
