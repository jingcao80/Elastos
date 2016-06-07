
#ifndef __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEMONITOR_H__
#define __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEMONITOR_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Monitors events from CallsManager and plays in-call tones for events which require them, such as
 * different type of call disconnections (busy tone, congestion tone, etc).
 */
class InCallToneMonitor
    : public CallsManagerListenerBase
{
public:
    CARAPI constructor(
        /* [in] */ InCallTonePlayer::Factory* playerFactory,
        /* [in] */ CallsManager* callsManager);

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

private:
    AutoPtr<InCallTonePlayer::Factory> mPlayerFactory;

    AutoPtr<CallsManager> mCallsManager;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_INCALLTONEMONITOR_H__
