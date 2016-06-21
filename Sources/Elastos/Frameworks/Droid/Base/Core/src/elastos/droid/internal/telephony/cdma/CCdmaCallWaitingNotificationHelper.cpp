#include "elastos/droid/internal/telephony/cdma/CCdmaCallWaitingNotificationHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallWaitingNotification.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaCallWaitingNotificationHelper, Singleton, ICdmaCallWaitingNotificationHelper)
CAR_SINGLETON_IMPL(CCdmaCallWaitingNotificationHelper)

ECode CCdmaCallWaitingNotificationHelper::PresentationFromCLIP(
    /* [in] */ Int32 cli,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CdmaCallWaitingNotification::PresentationFromCLIP(cli);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
