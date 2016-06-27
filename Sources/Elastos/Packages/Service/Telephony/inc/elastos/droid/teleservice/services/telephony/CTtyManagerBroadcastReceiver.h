#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CTTYMANAGERBROADCASTERCEIVER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CTTYMANAGERBROADCASTERCEIVER_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_CTtyManagerBroadcastReceiver.h"
#include "elastos/droid/teleservice/services/telephony/TtyManager.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

CarClass(CTtyManagerBroadcastReceiver)
    , public TtyManager::TtyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CTTYMANAGERBROADCASTERCEIVER_H__
