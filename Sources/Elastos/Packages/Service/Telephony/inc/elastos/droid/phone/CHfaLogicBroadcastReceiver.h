#ifndef  __ELASTOS_DROID_PHONE_CHFALOGICBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CHFALOGICBROADCASTRECEIVER_H__

#include "elastos/droid/telephony/phone/CHfaLogicBroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CHfaLogicBroadcastReceiver)
    , public HfaLogic::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CHFALOGICBROADCASTRECEIVER_H__