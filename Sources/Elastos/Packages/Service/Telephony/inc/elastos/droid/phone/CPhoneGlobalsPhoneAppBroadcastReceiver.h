#ifndef  __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Phone_CPhoneGlobalsPhoneAppBroadcastReceiver.h"
#include "elastos/droid/phone/PhoneGlobals.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Phone::PhoneGlobals;

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