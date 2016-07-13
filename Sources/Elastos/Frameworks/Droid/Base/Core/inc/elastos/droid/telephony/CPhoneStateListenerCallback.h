
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENERCALLBACK_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENERCALLBACK_H__

#include "elastos/droid/telephony/PhoneStateListener.h"
#include "_Elastos_Droid_Telephony_CPhoneStateListenerCallback.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneStateListenerCallback)
    , public PhoneStateListener::PhoneStateListenerCallback
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENERCALLBACK_H__
