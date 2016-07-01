
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGNOTIFICATIONSHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGNOTIFICATIONSHELPER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Helper class operating on call log notifications.
 */
class CallLogNotificationsHelper
    : public Object
{
public:
    /** Removes the missed call notifications. */
    static CARAPI_(void) RemoveMissedCallNotifications(
        /* [in] */ IContext* context);

    /** Update the voice mail notifications. */
    static CARAPI_(void) UpdateVoicemailNotifications(
        /* [in] */ IContext* context);

};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGNOTIFICATIONSHELPER_H__
