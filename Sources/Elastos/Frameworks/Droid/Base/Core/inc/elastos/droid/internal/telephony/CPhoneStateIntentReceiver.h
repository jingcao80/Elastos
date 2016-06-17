
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESTATEINTENTRECEIVER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESTATEINTENTRECEIVER_H__

#include "_Elastos_Droid_Internal_Telephony_CPhoneStateIntentReceiver.h"
#include "elastos/droid/internal/telephony/PhoneStateIntentReceiver.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CPhoneStateIntentReceiver), public PhoneStateIntentReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CPHONESTATEINTENTRECEIVER_H__
