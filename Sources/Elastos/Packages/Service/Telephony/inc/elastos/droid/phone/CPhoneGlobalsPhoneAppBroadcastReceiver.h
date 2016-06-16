#ifndef  __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__

#include "elastos/droid/telephony/phone/PhoneGlobals.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CPhoneGlobalsPhoneAppBroadcastReceiver)
    , public PhoneGlobals::PhoneAppBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__