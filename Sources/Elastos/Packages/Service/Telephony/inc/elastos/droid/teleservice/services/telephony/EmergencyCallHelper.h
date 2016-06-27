#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_EMERGENCYCALLHELPER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_EMERGENCYCALLHELPER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

class EmergencyCallHelper
    : public Object
    , public IEmergencyCallHelper
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("EmergencyCallHelper::MyHandler")

        MyHandler(
            /* [in] */ EmergencyCallHelper* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        EmergencyCallHelper* mHost;
    };

public:
    TO_STRING_IMPL("EmergencyCallHelper")

    CAR_INTERFACE_DECL()

    EmergencyCallHelper(
        /* [in] */ IContext* context);

    /**
     * Starts the "turn on radio" sequence. This is the (single) external API of the
     * EmergencyCallHelper class.
     *
     * This method kicks off the following sequence:
     * - Power on the radio.
     * - Listen for the service state change event telling us the radio has come up.
     * - Retry if we've gone {@link #TIME_BETWEEN_RETRIES_MILLIS} without any response from the
     *   radio.
     * - Finally, clean up any leftover state.
     *
     * This method is safe to call from any thread, since it simply posts a message to the
     * EmergencyCallHelper's handler (thus ensuring that the rest of the sequence is entirely
     * serialized, and runs only on the handler thread.)
     */
    CARAPI StartTurnOnRadioSequence(
        /* [in] */ IPhone* phone,
        /* [in] */ IEmergencyCallHelperCallback* _callback);

private:
    /**
     * Actual implementation of startTurnOnRadioSequence(), guaranteed to run on the handler thread.
     * @see #startTurnOnRadioSequence
     */
    CARAPI_(void) StartSequenceInternal(
        /* [in] */ IPhone* phone,
        /* [in] */ IEmergencyCallHelperCallback* _callback);

    /**
     * Handles the SERVICE_STATE_CHANGED event. Normally this event tells us that the radio has
     * finally come up. In that case, it's now safe to actually place the emergency call.
     */
    CARAPI_(void) OnServiceStateChanged(
        /* [in] */ IServiceState* state);

    CARAPI_(Boolean) IsOkToCall(
        /* [in] */ Int32 serviceState,
        /* [in] */ PhoneConstantsState phoneState);

    /**
     * Handles the retry timer expiring.
     */
    CARAPI_(void) OnRetryTimeout();

    /**
     * Attempt to power on the radio (i.e. take the device out of airplane mode.)
     * Additionally, start listening for service state changes; we'll eventually get an
     * onServiceStateChanged() callback when the radio successfully comes up.
     */
    CARAPI_(void) PowerOnRadio();

    /**
     * Clean up when done with the whole sequence: either after successfully turning on the radio,
     * or after bailing out because of too many failures.
     *
     * The exact cleanup steps are:
     * - Notify callback if we still hadn't sent it a response.
     * - Double-check that we're not still registered for any telephony events
     * - Clean up any extraneous handler messages (like retry timeouts) still in the queue
     *
     * Basically this method guarantees that there will be no more activity from the
     * EmergencyCallHelper until someone kicks off the whole sequence again with another call to
     * {@link #startTurnOnRadioSequence}
     *
     * TODO: Do the work for the comment below:
     * Note we don't call this method simply after a successful call to placeCall(), since it's
     * still possible the call will disconnect very quickly with an OUT_OF_SERVICE error.
     */
    CARAPI_(void) Cleanup();

    CARAPI_(void) StartRetryTimer();

    CARAPI_(void) CancelRetryTimer();

    CARAPI_(void) RegisterForServiceStateChanged();

    CARAPI_(void) UnregisterForServiceStateChanged();

    CARAPI_(void) OnComplete(
        /* [in] */ Boolean isRadioReady);

private:
    // Handler message codes; see handleMessage()
    static const Int32 MSG_START_SEQUENCE = 1;
    static const Int32 MSG_SERVICE_STATE_CHANGED = 2;
    static const Int32 MSG_RETRY_TIMEOUT = 3;

    AutoPtr<IContext> mContext;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IEmergencyCallHelperCallback> mCallback;  // The callback to notify upon completion.
    AutoPtr<IPhone> mPhone;  // The phone that will attempt to place the call.
    Int32 mNumRetriesSoFar;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_EMERGENCYCALLHELPER_H__