
#include "elastos/apps/dialer/calllog/CCallLogNotificationsHelper.h"
#include "elastos/apps/dialer/calllog/CallLogNotificationsHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CCallLogNotificationsHelper, Singleton, ICallLogNotificationsHelper);

CAR_SINGLETON_IMPL(CCallLogNotificationsHelper);

ECode CCallLogNotificationsHelper::RemoveMissedCallNotifications(
    /* [in] */ IContext* context)
{
    CallLogNotificationsHelper::RemoveMissedCallNotifications(context);
    return NOERROR;
}

ECode CCallLogNotificationsHelper::UpdateVoicemailNotifications(
    /* [in] */ IContext* context)
{
    CallLogNotificationsHelper::UpdateVoicemailNotifications(context);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
