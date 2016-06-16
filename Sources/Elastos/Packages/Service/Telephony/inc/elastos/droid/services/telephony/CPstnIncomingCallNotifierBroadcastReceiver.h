#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__

#include "_Elastos_Droid_Services_Telephony_CPstnIncomingCallNotifierBroadcastReceiver.h"
#include "elastos/droid/services/telephony/PstnIncomingCallNotifier.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
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
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__