
#ifndef __ELASTOS_DROID_SERVER_TELECOM_PHONESTATEBROADCASTER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_PHONESTATEBROADCASTER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Send a {@link TelephonyManager#ACTION_PHONE_STATE_CHANGED} broadcast when the call state
 * changes.
 */
class PhoneStateBroadcaster
    : public CallsManagerListenerBase
{
public:
    PhoneStateBroadcaster();

    CARAPI constructor();

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    // @Override
    CARAPI OnCallAdded(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallRemoved(
        /* [in] */ ICall* call);

    CARAPI GetCallState(
        /* [out] */ Int32* result);

private:
    CARAPI SendPhoneStateChangedBroadcast(
        /* [in] */ ICall* call,
        /* [in] */ Int32 phoneState);

    AutoPtr<IITelephonyRegistry> mRegistry;

    Int32 mCurrentState;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_PHONESTATEBROADCASTER_H__
