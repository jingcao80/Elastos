#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELECOMACCOUNTREGISTRYBROADCASTERCEIVER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELECOMACCOUNTREGISTRYBROADCASTERCEIVER_H__

#include "_Elastos_Droid_Services_Telephony_CTelecomAccountRegistryBroadcastReceiver.h"
#include "elastos/droid/services/telephony/TelecomAccountRegistry.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

CarClass(CTelecomAccountRegistryBroadcastReceiver)
    , public TelecomAccountRegistry::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CTELECOMACCOUNTREGISTRYBROADCASTERCEIVER_H__