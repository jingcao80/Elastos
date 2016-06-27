#ifndef  __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__

#include "_Elastos_Droid_TeleService_Phone_CPhoneGlobalsPhoneAppBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::PhoneGlobals;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CPhoneGlobalsPhoneAppBroadcastReceiver)
    , public PhoneGlobals::PhoneAppBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEGLOBALSPHONEAPPBROADCASTRECEIVER_H__