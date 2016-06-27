#ifndef  __ELASTOS_DROID_PHONE_CHFALOGICBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CHFALOGICBROADCASTRECEIVER_H__

#include "_Elastos_Droid_TeleService_Phone_CHfaLogicBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/HfaLogic.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CHfaLogicBroadcastReceiver)
    , public HfaLogic::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CHFALOGICBROADCASTRECEIVER_H__