#ifndef __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMESSAGE_H__

#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Telephony_Gsm_CGSMSmsMessage.h"
#include "elastos/droid/telephony/gsm/GSMSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CarClass(CGSMSmsMessage)
    , public GSMSmsMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_CGSMSMSMESSAGE_H__
