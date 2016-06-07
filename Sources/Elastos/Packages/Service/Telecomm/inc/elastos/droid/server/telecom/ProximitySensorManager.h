
#ifndef __ELASTOS_DROID_SERVER_TELECOM_PROXIMITYSENSORMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_PROXIMITYSENSORMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Hardware::ICmHardwareManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * This class manages the proximity sensor and allows callers to turn it on and off.
 */
class ProximitySensorManager
    : public CallsManagerListenerBase
{
public:
    ProximitySensorManager();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    /**
     * Turn the proximity sensor on.
     */
    CARAPI TurnOn();

    /**
     * Turn the proximity sensor off.
     * @param screenOnImmediately
     */
    CARAPI TurnOff(
        /* [in] */ Boolean screenOnImmediately);

private:
    static const String TAG;

    AutoPtr<IPowerManagerWakeLock> mProximityWakeLock;

    Boolean mWasTapToWakeEnabled;

    AutoPtr<ICmHardwareManager> mCmHardwareManager;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_PROXIMITYSENSORMANAGER_H__
