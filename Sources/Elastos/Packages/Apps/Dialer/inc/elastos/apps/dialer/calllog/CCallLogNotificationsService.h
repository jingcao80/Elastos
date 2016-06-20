
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSSERVICE_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSSERVICE_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallLogNotificationsService.h"
#include "elastos/droid/app/IntentService.h"

using Elastos::Droid::App::IntentService;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

/**
 * Provides operations for managing notifications.
 * <p>
 * It handles the following actions:
 * <ul>
 * <li>{@link #ACTION_MARK_NEW_VOICEMAILS_AS_OLD}: marks all the new voicemails in the call log as
 * old; this is called when a notification is dismissed.</li>
 * <li>{@link #ACTION_UPDATE_NOTIFICATIONS}: updates the content of the new items notification; it
 * may include an optional extra {@link #EXTRA_NEW_VOICEMAIL_URI}, containing the URI of the new
 * voicemail that has triggered this update (if any).</li>
 * </ul>
 */
CarClass(CCallLogNotificationsService)
    , public IntentService
    , public ICallLogNotificationsService
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

protected:
    // @Override
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

private:
    static const String TAG; // = "CallLogNotificationsService";

    AutoPtr<ICallLogQueryHandler> mCallLogQueryHandler;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSSERVICE_H__
