
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallLogNotificationsHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * Helper class operating on call log notifications.
 */
CarClass(CCallLogNotificationsHelper)
    , public Singleton
    , public ICallLogNotificationsHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /** Removes the missed call notifications. */
    CARAPI RemoveMissedCallNotifications(
        /* [in] */ IContext* context);

    /** Update the voice mail notifications. */
    CARAPI UpdateVoicemailNotifications(
        /* [in] */ IContext* context);

};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSHELPER_H__
