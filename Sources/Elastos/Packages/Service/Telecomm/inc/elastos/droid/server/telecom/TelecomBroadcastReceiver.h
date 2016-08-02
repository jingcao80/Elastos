
#ifndef __ELASTOS_DROID_SERVER_TELECOM_TELECOMBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TELECOMBROADCASTRECEIVER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <cutils/log.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Handles miscellaneous Telecom broadcast intents. This should be visible from outside, but
 * should not be in the "exported" state.
 */
class TelecomBroadcastReceiver
    : public BroadcastReceiver
    , public ITelecomBroadcastReceiver
{
public:
    TO_STRING_IMPL("TelecomBroadcastReceiver")

    CAR_INTERFACE_DECL()

    /** {@inheritDoc} */
    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    /**
     * Closes open system dialogs and the notification shade.
     */
    CARAPI CloseSystemDialogs(
        /* [in] */ IContext* context);

public:
    /** The action used to send SMS response for the missed call notification. */
    static const String ACTION_SEND_SMS_FROM_NOTIFICATION;

    /** The action used to call a handle back for the missed call notification. */
    static const String ACTION_CALL_BACK_FROM_NOTIFICATION;

    /** The action used to clear missed calls. */
    static const String ACTION_CLEAR_MISSED_CALLS;

    // For adding to Blacklist from call log
    static const String REMOVE_BLACKLIST;

    static const String EXTRA_NUMBER;

    static const String EXTRA_TYPE;

    static const String EXTRA_FROM_NOTIFICATION;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TELECOMBROADCASTRECEIVER_H__
