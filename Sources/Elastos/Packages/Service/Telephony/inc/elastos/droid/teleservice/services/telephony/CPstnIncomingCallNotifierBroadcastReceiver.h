#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_CPstnIncomingCallNotifierBroadcastReceiver.h"
#include "elastos/droid/teleservice/services/telephony/PstnIncomingCallNotifier.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

CarClass(CPstnIncomingCallNotifierBroadcastReceiver)
    , public PstnIncomingCallNotifier::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__