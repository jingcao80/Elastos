#ifndef  __ELASTOS_DROID_PHONE_EMERGENCYCALLHELPER_H__
#define  __ELASTOS_DROID_PHONE_EMERGENCYCALLHELPER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Helper class for the {@link CallController} that implements special
 * behavior related to emergency calls.  Specifically, this class handles
 * the case of the user trying to dial an emergency number while the radio
 * is off (i.e. the device is in airplane mode), by forcibly turning the
 * radio back on, waiting for it to come up, and then retrying the
 * emergency call.
 *
 * This class is instantiated lazily (the first time the user attempts to
 * make an emergency call from airplane mode) by the the
 * {@link CallController} singleton.
 */
class EmergencyCallHelper
    : public Handler
{
public:
    TO_STRING_IMPL("EmergencyCallHelper")

    EmergencyCallHelper();

    CARPAI constructor(
        /* [in] */ ICallController* callController);

    //@Override
    CARPAI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * Starts the "emergency call from airplane mode" sequence.
     *
     * This is the (single) external API of the EmergencyCallHelper class.
     * This method is called from the CallController placeCall() sequence
     * if the user dials a valid emergency number, but the radio is
     * powered-off (presumably due to airplane mode.)
     *
     * This method kicks off the following sequence:
     * - Power on the radio
     * - Listen for the service state change event telling us the radio has come up
     * - Then launch the emergency call
     * - Retry if the call fails with an OUT_OF_SERVICE error
     * - Retry if we've gone 5 seconds without any response from the radio
     * - Finally, clean up any leftover state (progress UI, wake locks, etc.)
     *
     * This method is safe to call from any thread, since it simply posts
     * a message to the EmergencyCallHelper's handler (thus ensuring that
     * the rest of the sequence is entirely serialized, and runs only on
     * the handler thread.)
     *
     * This method does *not* force the in-call UI to come up; our caller
     * is responsible for doing that (presumably by calling
     * PhoneApp.displayCallScreen().)
     */
    CARPAI StartEmergencyCallFromAirplaneModeSequence(
        /* [in] */ const String& number);

private:
    /**
     * Actual implementation of startEmergencyCallFromAirplaneModeSequence(),
     * guaranteed to run on the handler thread.
     * @see #startEmergencyCallFromAirplaneModeSequence
     */
    CARPAI_(void) StartSequenceInternal(
        /* [in] */ IMessage* msg);

    /**
     * Handles the SERVICE_STATE_CHANGED event.
     *
     * (Normally this event tells us that the radio has finally come
     * up.  In that case, it's now safe to actually place the
     * emergency call.)
     */
    CARPAI_(void) OnServiceStateChanged(
        /* [in] */ IMessage* msg);

    /**
     * Handles a DISCONNECT event from the telephony layer.
     *
     * Even after we successfully place an emergency call (after powering
     * on the radio), it's still possible for the call to fail with the
     * disconnect cause OUT_OF_SERVICE.  If so, schedule a retry.
     */
    CARPAI_(void) OnDisconnect(
        /* [in] */ IMessage* msg);

    /**
     * Handles the retry timer expiring.
     */
    CARPAI_(void) OnRetryTimeout();

    /**
     * Attempt to power on the radio (i.e. take the device out
     * of airplane mode.)
     *
     * Additionally, start listening for service state changes;
     * we'll eventually get an onServiceStateChanged() callback
     * when the radio successfully comes up.
     */
    CARPAI_(void) PowerOnRadio();

    /**
     * Actually initiate the outgoing emergency call.
     * (We do this once the radio has successfully been powered-up.)
     *
     * If the call succeeds, we're done.
     * If the call fails, schedule a retry of the whole sequence.
     */
    CARPAI_(void) PlaceEmergencyCall();

    /**
     * Schedules a retry in response to some failure (either the radio
     * failing to power on, or a failure when trying to place the call.)
     * Or, if we've hit the retry limit, bail out of this whole sequence
     * and display a failure message to the user.
     */
    CARPAI_(void) ScheduleRetryOrBailOut();

    /**
     * Clean up when done with the whole sequence: either after
     * successfully placing *and* ending the emergency call, or after
     * bailing out because of too many failures.
     *
     * The exact cleanup steps are:
     * - Take down any progress UI (and also ask the in-call UI to refresh itself,
     *   if it's still visible)
     * - Double-check that we're not still registered for any telephony events
     * - Clean up any extraneous handler messages (like retry timeouts) still in the queue
     * - Make sure we're not still holding any wake locks
     *
     * Basically this method guarantees that there will be no more
     * activity from the EmergencyCallHelper until the CallController
     * kicks off the whole sequence again with another call to
     * startEmergencyCallFromAirplaneModeSequence().
     *
     * Note we don't call this method simply after a successful call to
     * placeCall(), since it's still possible the call will disconnect
     * very quickly with an OUT_OF_SERVICE error.
     */
    CARPAI_(void) Cleanup();

    CARPAI_(void) StartRetryTimer();

    CARPAI_(void) CancelRetryTimer();

    CARPAI_(void) RegisterForServiceStateChanged();

    CARPAI_(void) UnregisterForServiceStateChanged();

    CARPAI_(void) RegisterForDisconnect();

    CARPAI_(void) UnregisterForDisconnect();

    //
    // Debugging
    //
    static CARPAI_(void) Log(
        /* [in] */ const String& msg);

public:
    // Number of times to retry the call, and time between retry attempts.
    static const Int32 MAX_NUM_RETRIES;
    static const Int64 TIME_BETWEEN_RETRIES;  // msec

    // Timeout used with our wake lock (just as a safety valve to make
    // sure we don't hold it forever).
    static const Int64 WAKE_LOCK_TIMEOUT;  // 5 minutes in msec

private:
    static const String TAG;
    static const Boolean DBG;

    // Handler message codes; see handleMessage()
    static const Int32 START_SEQUENCE;
    static const Int32 SERVICE_STATE_CHANGED;
    static const Int32 DISCONNECT;
    static const Int32 RETRY_TIMEOUT;

    AutoPtr<ICallController> mCallController;
    AutoPtr<IPhoneGlobals> mApp;
    AutoPtr<ICallManager> mCM;
    String mNumber;  // The emergency number we're trying to dial
    Int32 mNumRetriesSoFar;

    // Wake lock we hold while running the whole sequence
    AutoPtr<IPowerManagerWakeLock> mPartialWakeLock;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_EMERGENCYCALLHELPER_H__