
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGRECEIVER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGRECEIVER_H__

#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * Receiver for call log events.
 * <p>
 * It is currently used to handle {@link VoicemailContract#ACTION_NEW_VOICEMAIL} and
 * {@link Intent#ACTION_BOOT_COMPLETED}.
 */
class CallLogReceiver
    : public BroadcastReceiver
{
public:
    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG; // = "CallLogReceiver";
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CALLLOGRECEIVER_H__
