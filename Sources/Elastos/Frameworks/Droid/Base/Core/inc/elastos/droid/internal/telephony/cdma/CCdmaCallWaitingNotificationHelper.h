#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMACALLWAITINGNOTIFICATIONHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMACALLWAITINGNOTIFICATIONHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaCallWaitingNotificationHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaCallWaitingNotificationHelper)
    , public Singleton
    , public ICdmaCallWaitingNotificationHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI PresentationFromCLIP(
        /* [in] */ Int32 cli,
        /* [out] */ Int32* result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMACALLWAITINGNOTIFICATIONHELPER_H__
