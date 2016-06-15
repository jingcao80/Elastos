#ifndef __ELASTOS_DROID_TELEPHONY_GSM_CGSMSUBMITPDU_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_CGSMSUBMITPDU_H__

#include "_Elastos_Droid_Telephony_Gsm_CGSMSubmitPdu.h"
#include "elastos/droid/telephony/gsm/GSMSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CarClass(CGSMSubmitPdu)
    , public GSMSmsMessage::SubmitPdu
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_CGSMSUBMITPDU_H__
