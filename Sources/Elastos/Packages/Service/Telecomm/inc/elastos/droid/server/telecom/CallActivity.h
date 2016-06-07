
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLACTIVITY_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLACTIVITY_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Activity that handles system CALL actions and forwards them to {@link CallReceiver}.
 * Handles all three CALL action types: CALL, CALL_PRIVILEGED, and CALL_EMERGENCY.
 *
 * Pre-L, the only way apps were were allowed to make outgoing emergency calls was the
 * ACTION_CALL_PRIVILEGED action (which requires the system only CALL_PRIVILEGED permission).
 *
 * In L, any app that has the CALL_PRIVILEGED permission can continue to make outgoing emergency
 * calls via ACTION_CALL_PRIVILEGED.
 *
 * In addition, the default dialer (identified via
 * {@link android.telecom.TelecomManager#getDefaultPhoneApp()} will also be granted the ability to
 * make emergency outgoing calls using the CALL action. In order to do this, it must call
 * startActivityForResult on the CALL intent to allow its package name to be passed to
 * {@link CallActivity}. Calling startActivity will continue to work on all non-emergency numbers
 * just like it did pre-L.
 */
class CallActivity
    : public Activity
    , public ICallActivity
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* bundle);

private:
    /**
     * Processes intents sent to the activity.
     *
     * @param intent The intent.
     */
    CARAPI ProcessIntent(
        /* [in] */ IIntent* intent);

    CARAPI VerifyCallAction(
        /* [in] */ IIntent* intent);

    CARAPI ProcessOutgoingCallIntent(
        /* [in] */ IIntent* intent);

    CARAPI IsTtyModeEnabled(
        /* [out] */ Boolean* result);

    CARAPI ProcessIncomingCallIntent(
        /* [in] */ IIntent* intent);

    CARAPI IsDefaultDialer(
        /* [out] */ Boolean* result);

    /**
     * Returns whether the device is voice-capable (e.g. a phone vs a tablet).
     *
     * @return {@code True} if the device is voice-capable.
     */
    CARAPI IsVoiceCapable(
        /* [out] */ Boolean* result);

    /**
     * Trampolines the intent to the broadcast receiver that runs only as the primary user.
     */
    CARAPI SendBroadcastToReceiver(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean incoming,
        /* [out] */ Boolean* result);
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLACTIVITY_H__
