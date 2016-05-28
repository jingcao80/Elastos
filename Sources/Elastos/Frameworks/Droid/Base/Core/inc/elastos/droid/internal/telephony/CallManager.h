
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLMANAGER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Internal::Telephony::IPhone)

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * @hide
 *
 * CallManager class provides an abstract layer for PhoneApp to access
 * and control calls. It implements Phone interface.
 *
 * CallManager provides call and connection control as well as
 * channel capability.
 *
 * There are three categories of APIs CallManager provided
 *
 *  1. Call control and operation, such as dial() and hangup()
 *  2. Channel capabilities, such as CanConference()
 *  3. Register notification
 *
 *
 */
class CallManager
    : public Object
    , public ICallManager
{
private:
    class CallManagerHandler: public Handler
    {
    public:
        CallManagerHandler(
            /* [in] */ CallManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CallManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * get singleton instance of CallManager
     * @return CallManager
     */
    static CARAPI_(AutoPtr<CallManager>) GetInstance();

    /**
     * Check if two phones refer to the same PhoneBase obj
     *
     * Note: PhoneBase, not PhoneProxy, is to be used inside of CallManager
     *
     * Both PhoneBase and PhoneProxy implement Phone interface, so
     * they have same phone APIs, such as dial(). The real implementation, for
     * example in GSM,  is in GSMPhone as extend from PhoneBase, so that
     * foregroundCall.getPhone() returns GSMPhone obj. On the other hand,
     * PhoneFactory.getDefaultPhone() returns PhoneProxy obj, which has a class
     * member of GSMPhone.
     *
     * So for phone returned by PhoneFacotry, which is used by PhoneApp,
     *        phone.getForegroundCall().getPhone() != phone
     * but
     *        isSamePhone(phone, phone.getForegroundCall().getPhone()) == TRUE
     *
     * @param p1 is the first Phone obj
     * @param p2 is the second Phone obj
     * @return TRUE if p1 and p2 refer to the same phone
     */
    static CARAPI_(Boolean) IsSamePhone(
        /* [in] */ IPhone* p1,
        /* [in] */ IPhone* p2);

    /**
     * Returns all the registered phone objects.
     * @return all the registered phone objects.
     */
    CARAPI GetAllPhones(
        /* [out] */ IList** phones);

    /**
     * Get current coarse-grained voice call state.
     * If the Call Manager has an active call and call waiting occurs,
     * then the phone state is RINGING not OFFHOOK
     *
     */
    CARAPI GetState(
        /* [out] */ PhoneConstantsState* state);

    /**
     * Get current coarse-grained voice call state on a subId.
     * If the Call Manager has an active call and call waiting occurs,
     * then the phone state is RINGING not OFFHOOK
     *
     */
    CARAPI GetState(
        /* [in] */ Int64 subId,
        /* [out] */ PhoneConstantsState* state);

    /**
     * @return the service state of CallManager, which represents the
     * highest priority state of all the service states of phones
     *
     * The priority is defined as
     *
     * STATE_IN_SERIVCE > STATE_OUT_OF_SERIVCE > STATE_EMERGENCY > STATE_POWER_OFF
     *
     */
    CARAPI GetServiceState(
        /* [out] */ Int32* result);

    /**
     * @return the Phone service state corresponds to subId
     */
    CARAPI GetServiceState(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * @return the phone associated with any call
     */
    CARAPI GetPhoneInCall(
        /* [out] */ IPhone** result);

    CARAPI GetPhoneInCall(
        /* [in] */ Int64 subId,
        /* [out] */ IPhone** result);

    /**
     * Register phone to CallManager
     * @param phone to be registered
     * @return TRUE if register successfully
     */
    CARAPI RegisterPhone(
        /* [in] */ IPhone* phone,
        /* [out] */ Boolean* result);

    /**
     * unregister phone from CallManager
     * @param phone to be unregistered
     */
    CARAPI UnregisterPhone(
        /* [in] */ IPhone* phone);

    /**
     * return the default phone or NULL if no phone available
     */
    CARAPI GetDefaultPhone(
        /* [out] */ IPhone** phone);

    /**
     * @return the phone associated with the foreground call
     */
    CARAPI GetFgPhone(
        /* [out] */ IPhone** phone);

    /**
     * @return the phone associated with the foreground call
     * of a particular subId
     */
    CARAPI GetFgPhone(
        /* [in] */ Int64 subId,
        /* [out] */ IPhone** phone);

    /**
     * @return the phone associated with the background call
     */
    CARAPI GetBgPhone(
        /* [out] */ IPhone** phone);

    /**
     * @return the phone associated with the background call
     * of a particular subId
     */
    CARAPI GetBgPhone(
        /* [in] */ Int64 subId,
        /* [out] */ IPhone** result);

    /**
     * @return the phone associated with the ringing call
     */
    CARAPI GetRingingPhone(
        /* [out] */ IPhone** phone);

    /**
     * @return the phone associated with the ringing call
     * of a particular subId
     */
    CARAPI GetRingingPhone(
        /* [in] */ Int64 subId,
        /* [out] */ IPhone** result);

    /**
     * Answers a ringing or waiting call.
     *
     * Active call, if any, go on hold.
     * If active call can't be held, i.e., a background call of the same channel exists,
     * the active call will be hang up.
     *
     * Answering occurs asynchronously, and final notification occurs via
     * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) registerForPreciseCallStateChanged()}.
     *
     * @exception CallStateException when call is not ringing or waiting
     */
    CARAPI AcceptCall(
        /* [in] */ ICall* ringingCall) /*throws CallStateException*/;

    /**
     * Reject (ignore) a ringing call. In GSM, this means UDUB
     * (User Determined User Busy). Reject occurs asynchronously,
     * and final notification occurs via
     * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) registerForPreciseCallStateChanged()}.
     *
     * @exception CallStateException when no call is ringing or waiting
     */
    CARAPI RejectCall(
        /* [in] */ ICall* ringingCall) /*throws CallStateException*/;

    /**
     * Places active call on hold, and makes held call active.
     * Switch occurs asynchronously and may fail.
     *
     * There are 4 scenarios
     * 1. only active call but no held call, aka, hold
     * 2. no active call but only held call, aka, unhold
     * 3. both active and held calls from same phone, aka, swap
     * 4. active and held calls from different phones, aka, phone swap
     *
     * Final notification occurs via
     * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) registerForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if active call is ringing, waiting, or
     * dialing/alerting, or heldCall can't be active.
     * In these cases, this operation may not be performed.
     */
    CARAPI SwitchHoldingAndActive(
        /* [in] */ ICall* heldCall) /*throws CallStateException*/;

    /**
     * Hangup foreground call and resume the specific background call
     *
     * Note: this is noop if there is no foreground call or the heldCall is NULL
     *
     * @param heldCall to become foreground
     * @throws CallStateException
     */
    CARAPI HangupForegroundResumeBackground(
        /* [in] */ ICall* heldCall) /*throws CallStateException*/;

    /**
     * Whether or not the phone can conference in the current phone
     * state--that is, one call holding and one call active.
     * @return TRUE if the phone can conference; FALSE otherwise.
     */
    CARAPI CanConference(
        /* [in] */ ICall* heldCall,
        /* [out] */ Boolean* result);

    /**
     * Whether or not the phone can conference in the current phone
     * state--that is, one call holding and one call active.
     * This method consider the phone object which is specific
     * to the provided subId.
     * @return TRUE if the phone can conference; FALSE otherwise.
     */
    CARAPI CanConference(
        /* [in] */ ICall* heldCall,
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * Conferences holding and active. Conference occurs asynchronously
     * and may fail. Final notification occurs via
     * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) registerForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if canConference() would return FALSE.
     * In these cases, this operation may not be performed.
     */
    CARAPI Conference(
        /* [in] */ ICall* heldCall) /*throws CallStateException*/;

    /**
     * Initiate a new voice connection. This happens asynchronously, so you
     * cannot assume the audio path is connected (or a call index has been
     * assigned) until PhoneStateChanged notification has occurred.
     *
     * @exception CallStateException if a new outgoing call is not currently
     * possible because no more call slots exist or a call exists that is
     * dialing, alerting, ringing, or waiting.  Other errors are
     * handled asynchronously.
     */
    CARAPI Dial(
        /* [in] */ IPhone* phone,
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** retValue) /*throws CallStateException*/;

    /**
     * Initiate a new voice connection. This happens asynchronously, so you
     * cannot assume the audio path is connected (or a call index has been
     * assigned) until PhoneStateChanged notification has occurred.
     *
     * @exception CallStateException if a new outgoing call is not currently
     * possible because no more call slots exist or a call exists that is
     * dialing, alerting, ringing, or waiting.  Other errors are
     * handled asynchronously.
     */
    CARAPI Dial(
        /* [in] */ IPhone* phone,
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** retValue) /*throws CallStateException*/;

    /**
     * clear disconnect connection for each phone
     */
    CARAPI ClearDisconnected();

    /**
     * clear disconnect connection for a phone specific
     * to the provided subId
     */
    CARAPI ClearDisconnected(
        /* [in] */ Int64 subId);

    /**
     * Whether or not the phone can do explicit call transfer in the current
     * phone state--that is, one call holding and one call active.
     * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
     */
    CARAPI CanTransfer(
        /* [in] */ ICall* heldCall,
        /* [out] */ Boolean* result);

    /**
     * Whether or not the phone specific to subId can do explicit call transfer
     * in the current phone state--that is, one call holding and one call active.
     * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
     */
    CARAPI CanTransfer(
        /* [in] */ ICall* heldCall,
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * Connects the held call and active call
     * Disconnects the subscriber from both calls
     *
     * Explicit Call Transfer occurs asynchronously
     * and may fail. Final notification occurs via
     * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) registerForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if canTransfer() would return FALSE.
     * In these cases, this operation may not be performed.
     */
    CARAPI ExplicitCallTransfer(
        /* [in] */ ICall* heldCall) /*throws CallStateException*/;

    /**
     * Returns a list of MMI codes that are pending for a phone. (They have initiated
     * but have not yet completed).
     * Presently there is only ever one.
     *
     * Use <code>registerForMmiInitiate</code>
     * and <code>registerForMmiComplete</code> for change notification.
     * @return NULL if phone doesn't have or support mmi code
     */
    CARAPI GetPendingMmiCodes(
        /* [in] */ IPhone* phone,
        /* [out] */ IList** codes);

    /**
     * Sends user response to a USSD REQUEST message.  An MmiCode instance
     * representing this response is sent to handlers registered with
     * registerForMmiInitiate.
     *
     * @param ussdMessge    Message to send in the response.
     * @return FALSE if phone doesn't support ussd service
     */
    CARAPI SendUssdResponse(
        /* [in] */ IPhone* phone,
        /* [in] */ const String& ussdMessge,
        /* [out] */ Boolean* result);

    /**
     * Mutes or unmutes the microphone for the active call. The microphone
     * is automatically unmuted if a call is answered, dialed, or resumed
     * from a holding state.
     *
     * @param muted TRUE to mute the microphone,
     * FALSE to activate the microphone.
     */
    CARAPI SetMute(
        /* [in] */ Boolean muted);

    /**
     * Gets current mute status. Use
     * {@link #registerForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) registerForPreciseCallStateChanged()}
     * as a change notifcation, although presently phone state changed is not
     * fired when setMute() is called.
     *
     * @return TRUE is muting, FALSE is unmuting
     */
    CARAPI GetMute(
        /* [out] */ Boolean* result);

    /**
     * Enables or disables echo suppression.
     */
    CARAPI SetEchoSuppressionEnabled();

    /**
     * Play a DTMF tone on the active call.
     *
     * @param c should be one of 0-9, '*' or '#'. Other values will be
     * silently ignored.
     * @return FALSE if no active call or the active call doesn't support
     *         dtmf tone
     */
    CARAPI SendDtmf(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* retValue);

    /**
     * Start to paly a DTMF tone on the active call.
     * or there is a playing DTMF tone.
     * @param c should be one of 0-9, '*' or '#'. Other values will be
     * silently ignored.
     *
     * @return FALSE if no active call or the active call doesn't support
     *         dtmf tone
     */
    CARAPI StartDtmf(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* retValue);

    /**
     * Stop the playing DTMF tone. Ignored if there is no playing DTMF
     * tone or no active call.
     */
    CARAPI StopDtmf();

    /**
     * send burst DTMF tone, it can send the string as single character or multiple character
     * ignore if there is no active call or not valid digits string.
     * Valid digit means only includes characters ISO-LATIN characters 0-9, *, #
     * The difference between sendDtmf and sendBurstDtmf is sendDtmf only sends one character,
     * this api can send single character and multiple character, also, this api has response
     * back to caller.
     *
     * @param dtmfString is string representing the dialing digit(s) in the active call
     * @param on the DTMF ON length in milliseconds, or 0 for default
     * @param off the DTMF OFF length in milliseconds, or 0 for default
     * @param onComplete is the callback message when the action is processed by BP
     *
     */
    CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* onComplete,
        /* [out] */ Boolean* result);

    /**
     * Notifies when a voice connection has disconnected, either due to local
     * or remote hangup or error.
     *
     *  Messages received from this will have the following members:<p>
     *  <ul><li>Message.obj will be an AsyncResult</li>
     *  <li>AsyncResult.userObj = obj</li>
     *  <li>AsyncResult.result = a Connection object that is
     *  no longer connected.</li></ul>
     */
    CARAPI RegisterForDisconnect(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for voice disconnection notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForDisconnect(
        /* [in] */ IHandler* h);

    /**
     * Register for getting notifications for change in the Call State {@link Call.State}
     * This is called PreciseCallState because the call state is more precise than what
     * can be obtained using the {@link PhoneStateListener}
     *
     * Resulting events will have an AsyncResult in <code>Message.obj</code>.
     * AsyncResult.userData will be set to the obj argument here.
     * The <em>h</em> parameter is held only by a weak reference.
     */
    CARAPI RegisterForPreciseCallStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for voice call state change notifications.
     * Extraneous calls are tolerated silently.
     */
    CARAPI UnregisterForPreciseCallStateChanged(
        /* [in] */ IHandler* h);

    /**
     * Notifies when a previously untracked non-ringing/waiting connection has appeared.
     * This is likely due to some other entity (eg, SIM card application) initiating a call.
     */
    CARAPI RegisterForUnknownConnection(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for unknown connection notifications.
     */
    CARAPI UnregisterForUnknownConnection(
        /* [in] */ IHandler* h);

    /**
     * Notifies when a new ringing or waiting connection has appeared.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = a Connection. <p>
     *  Please check Connection.isRinging() to make sure the Connection
     *  has not dropped since this message was posted.
     *  If Connection.isRinging() is TRUE, then
     *   Connection.getCall() == Phone.getRingingCall()
     */
    CARAPI RegisterForNewRingingConnection(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for new ringing connection notification.
     * Extraneous calls are tolerated silently
     */

    CARAPI UnregisterForNewRingingConnection(
        /* [in] */ IHandler* h);

    /**
     * Notifies when an incoming call rings.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = a Connection. <p>
     */
    CARAPI RegisterForIncomingRing(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for ring notification.
     * Extraneous calls are tolerated silently
     */

    CARAPI UnregisterForIncomingRing(
        /* [in] */ IHandler* h);

    /**
     * Notifies when out-band ringback tone is needed.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = Boolean, TRUE to start play ringback tone
     *                       and FALSE to stop. <p>
     */
    CARAPI RegisterForRingbackTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for ringback tone notification.
     */

    CARAPI UnregisterForRingbackTone(
        /* [in] */ IHandler* h);

    /**
     * Notifies when out-band on-hold tone is needed.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = Boolean, TRUE to start play on-hold tone
     *                       and FALSE to stop. <p>
     */
    CARAPI RegisterForOnHoldTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for on-hold tone notification.
     */

    CARAPI UnregisterForOnHoldTone(
        /* [in] */ IHandler* h);

    /**
     * Registers the handler to reset the uplink mute state to get
     * uplink audio.
     */
    CARAPI RegisterForResendIncallMute(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for resend incall mute notifications.
     */
    CARAPI UnregisterForResendIncallMute(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications of initiation of a new MMI code request.
     * MMI codes for GSM are discussed in 3GPP TS 22.030.<p>
     *
     * Example: If Phone.dial is called with "*#31#", then the app will
     * be notified here.<p>
     *
     * The returned <code>Message.obj</code> will contain an AsyncResult.
     *
     * <code>obj.result</code> will be an "MmiCode" object.
     */
    CARAPI RegisterForMmiInitiate(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for new MMI initiate notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForMmiInitiate(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications that an MMI request has completed
     * its network activity and is in its final state. This may mean a state
     * of COMPLETE, FAILED, or CANCELLED.
     *
     * <code>Message.obj</code> will contain an AsyncResult.
     * <code>obj.result</code> will be an "MmiCode" object
     */
    CARAPI RegisterForMmiComplete(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for MMI complete notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForMmiComplete(
        /* [in] */ IHandler* h);

    /**
     * Registration point for Ecm timer reset
     * @param h handler to notify
     * @param what user-defined message code
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForEcmTimerReset(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notification for Ecm timer reset
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForEcmTimerReset(
        /* [in] */ IHandler* h);

    /**
     * Register for ServiceState changed.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a ServiceState instance
     */
    CARAPI RegisterForServiceStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for ServiceStateChange notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForServiceStateChanged(
        /* [in] */ IHandler* h);

    /**
     * Register for supplementary service notifications.
     * Message.obj will contain an AsyncResult.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for supplementary service notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSuppServiceNotification(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications when a supplementary service attempt fails.
     * Message.obj will contain an AsyncResult.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForSuppServiceFailed(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notifications when a supplementary service attempt fails.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSuppServiceFailed(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications when a sInCall VoicePrivacy is enabled
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notifications when a sInCall VoicePrivacy is enabled
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications when a sInCall VoicePrivacy is disabled
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notifications when a sInCall VoicePrivacy is disabled
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications when CDMA call waiting comes
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForCallWaiting(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notifications when CDMA Call waiting comes
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForCallWaiting(
        /* [in] */ IHandler* h);

    /**
     * Register for signal information notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForSignalInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for signal information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSignalInfo(
        /* [in] */ IHandler* h);

    /**
     * Register for display information notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForDisplayInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregisters for display information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForDisplayInfo(
        /* [in] */ IHandler* h);

    /**
     * Register for notifications when CDMA OTA Provision status change
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notifications when CDMA OTA Provision status change
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h);

    /**
     * Registration point for subcription info ready
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Unregister for notifications for subscription info
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h);

    CARAPI RegisterForSubscriptionChange(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForSubscriptionChange(
        /* [in] */ IHandler* h);

    CARAPI SetActiveSubscription(
        /* [in] */ Int64 subscription);

    /**
     * Sets an event to be fired when the telephony system processes
     * a post-dial character on an outgoing call.<p>
     *
     * Messages of type <code>what</code> will be sent to <code>h</code>.
     * The <code>obj</code> field of these Message's will be instances of
     * <code>AsyncResult</code>. <code>Message.obj.result</code> will be
     * a Connection object.<p>
     *
     * Message.arg1 will be the post dial character being processed,
     * or 0 ('\0') if end of string.<p>
     *
     * If Connection.getPostDialState() == WAIT,
     * the application must call
     * {@link com.android.internal.telephony.Connection#proceedAfterWaitChar()
     * Connection.proceedAfterWaitChar()} or
     * {@link com.android.internal.telephony.Connection#cancelPostDial()
     * Connection.cancelPostDial()}
     * for the telephony system to continue playing the post-dial
     * DTMF sequence.<p>
     *
     * If Connection.getPostDialState() == WILD,
     * the application must call
     * {@link com.android.internal.telephony.Connection#proceedAfterWildChar
     * Connection.proceedAfterWildChar()}
     * or
     * {@link com.android.internal.telephony.Connection#cancelPostDial()
     * Connection.cancelPostDial()}
     * for the telephony system to continue playing the
     * post-dial DTMF sequence.<p>
     *
     */
    CARAPI RegisterForPostDialCharacter(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForPostDialCharacter(
        /* [in] */ IHandler* h);

    /* APIs to access foregroudCalls, backgroudCalls, and ringingCalls
     * 1. APIs to access list of calls
     * 2. APIs to check if any active call, which has connection other than
     * disconnected ones, pleaser refer to Call.isIdle()
     * 3. APIs to return first active call
     * 4. APIs to return the connections of first active call
     * 5. APIs to return other property of first active call
     */

    /**
     * @return list of all ringing calls
     */
    CARAPI GetRingingCalls(
        /* [out] */ IList** result);

    /**
     * @return list of all foreground calls
     */
    CARAPI GetForegroundCalls(
        /* [out] */ IList** result);

    /**
     * @return list of all background calls
     */
    CARAPI GetBackgroundCalls(
        /* [out] */ IList** result);

    /**
     * Return TRUE if there is at least one active foreground call
     */
    CARAPI HasActiveFgCall(
        /* [out] */ Boolean* result);

    /**
     * Return TRUE if there is at least one active foreground call
     * on a particular subId or an active sip call
     */
    CARAPI HasActiveFgCall(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * Return TRUE if there is at least one active background call
     */
    CARAPI HasActiveBgCall(
        /* [out] */ Boolean* result);

    /**
     * Return TRUE if there is at least one active background call
     * on a particular subId or an active sip call
     */
    CARAPI HasActiveBgCall(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * Return TRUE if there is at least one active ringing call
     *
     */
    CARAPI HasActiveRingingCall(
        /* [out] */ Boolean* result);

    /**
     * Return TRUE if there is at least one active ringing call
     */
    CARAPI HasActiveRingingCall(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * return the active foreground call from foreground calls
     *
     * Active call means the call is NOT in Call.State.IDLE
     *
     * 1. If there is active foreground call, return it
     * 2. If there is no active foreground call, return the
     *    foreground call associated with default phone, which state is IDLE.
     * 3. If there is no phone registered at all, return NULL.
     *
     */
    CARAPI GetActiveFgCall(
        /* [out] */ ICall** result);

    CARAPI GetActiveFgCall(
        /* [in] */ Int64 subId,
        /* [out] */ ICall** result);

    /**
     * return one active background call from background calls
     *
     * Active call means the call is NOT idle defined by Call.isIdle()
     *
     * 1. If there is only one active background call, return it
     * 2. If there is more than one active background call, return the first one
     * 3. If there is no active background call, return the background call
     *    associated with default phone, which state is IDLE.
     * 4. If there is no background call at all, return NULL.
     *
     * Complete background calls list can be get by getBackgroundCalls()
     */
    CARAPI GetFirstActiveBgCall(
        /* [out] */ ICall** result);

    /**
     * return one active background call from background calls of the
     * requested subId.
     *
     * Active call means the call is NOT idle defined by Call.isIdle()
     *
     * 1. If there is only one active background call on given sub or
     *    on SIP Phone, return it
     * 2. If there is more than one active background call, return the background call
     *    associated with the active sub.
     * 3. If there is no background call at all, return NULL.
     *
     * Complete background calls list can be get by getBackgroundCalls()
     */
    CARAPI GetFirstActiveBgCall(
        /* [in] */ Int64 subId,
        /* [out] */ ICall** result);

    /**
     * return one active ringing call from ringing calls
     *
     * Active call means the call is NOT idle defined by Call.isIdle()
     *
     * 1. If there is only one active ringing call, return it
     * 2. If there is more than one active ringing call, return the first one
     * 3. If there is no active ringing call, return the ringing call
     *    associated with default phone, which state is IDLE.
     * 4. If there is no ringing call at all, return NULL.
     *
     * Complete ringing calls list can be get by getRingingCalls()
     */
    CARAPI GetFirstActiveRingingCall(
        /* [out] */ ICall** result);

    CARAPI GetFirstActiveRingingCall(
        /* [in] */ Int64 subId,
        /* [out] */ ICall** result);

    /**
     * @return the state of active foreground call
     * return IDLE if there is no active foreground call
     */
    CARAPI GetActiveFgCallState(
        /* [out] */ ICallState* result);

    CARAPI GetActiveFgCallState(
        /* [in] */ Int64 subId,
        /* [out] */ ICallState* result);

    /**
     * @return the connections of active foreground call
     * return empty list if there is no active foreground call
     */
    CARAPI GetFgCallConnections(
        /* [out] */ IList** result);

    /**
     * @return the connections of active foreground call
     * return empty list if there is no active foreground call
     */
    CARAPI GetFgCallConnections(
        /* [in] */ Int64 subId,
        /* [out] */ IList** result);

    /**
     * @return the connections of active background call
     * return empty list if there is no active background call
     */
    CARAPI GetBgCallConnections(
        /* [out] */ IList** result);

    /**
     * @return the connections of active background call
     * return empty list if there is no active background call
     */
    CARAPI GetBgCallConnections(
        /* [in] */ Int64 subId,
        /* [out] */ IList** result);

    /**
     * @return the latest connection of active foreground call
     * return NULL if there is no active foreground call
     */
    CARAPI GetFgCallLatestConnection(
        /* [out] */ IConnection** result);

    /**
     * @return the latest connection of active foreground call
     * return NULL if there is no active foreground call
     */
    CARAPI GetFgCallLatestConnection(
        /* [in] */ Int64 subId,
        /* [out] */ IConnection** result);

    /**
     * @return TRUE if there is at least one Foreground call in disconnected state
     */
    CARAPI HasDisconnectedFgCall(
        /* [out] */ Boolean* result);

    /**
     * @return TRUE if there is at least one Foreground call in disconnected state
     */
    CARAPI HasDisconnectedFgCall(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /**
     * @return TRUE if there is at least one background call in disconnected state
     */
    CARAPI HasDisconnectedBgCall(
        /* [out] */ Boolean* result);

    /**
     * @return TRUE if there is at least one background call in disconnected state
     */
    CARAPI HasDisconnectedBgCall(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CallManager();

    /**
     * Get the corresponding PhoneBase obj
     *
     * @param phone a Phone object
     * @return the corresponding PhoneBase obj in Phone if Phone
     * is a PhoneProxy obj
     * or the Phone itself if Phone is not a PhoneProxy obj
     */
    static CARAPI_(AutoPtr<IPhone>) GetPhoneBase(
        /* [in] */ IPhone* phone);

    /**
     * get Phone object corresponds to subId
     * @return Phone
     */
    CARAPI_(AutoPtr<IPhone>) GetPhone(
        /* [in] */ Int64 subId);

    CARAPI_(AutoPtr<IContext>) GetContext();

    CARAPI_(void) RegisterForPhoneStates(
        /* [in] */ IPhone* phone);

    CARAPI_(void) UnregisterForPhoneStates(
        /* [in] */ IPhone* phone);

    /**
     * Phone can make a call only if ALL of the following are TRUE:
     *        - Phone is not powered off
     *        - There's no incoming or waiting call
     *        - The foreground call is ACTIVE or IDLE or DISCONNECTED.
     *          (We mainly need to make sure it *isn't* DIALING or ALERTING.)
     * @param phone
     * @return TRUE if the phone can make a new call
     */
    CARAPI_(Boolean) CanDial(
        /* [in] */ IPhone* phone);

    // Returns the first call that is not in IDLE state. If both active calls
    // and disconnecting/disconnected calls exist, return the first active call.
    CARAPI_(AutoPtr<ICall>) GetFirstNonIdleCall(
        /* [in] */ IList* calls);

    // Returns the first call that is not in IDLE state. If both active calls
    // and disconnecting/disconnected calls exist, return the first active call.
    CARAPI_(AutoPtr<ICall>) GetFirstNonIdleCall(
        /* [in] */ IList* calls,
        /* [in] */ Int64 subId);

    /**
     * @return the first active call from a call list
     */
    CARAPI_(AutoPtr<ICall>) GetFirstActiveCall(
        /* [in] */ IArrayList* calls);

    /**
     * @return the first active call from a call list
     */
    CARAPI_(AutoPtr<ICall>) GetFirstActiveCall(
        /* [in] */ IArrayList* calls,
        /* [in] */ Int64 subId);

    /**
     * @return the first call in a the Call.state from a call list
     */
    CARAPI_(AutoPtr<ICall>) GetFirstCallOfState(
        /* [in] */ IArrayList* calls,
        /* [in] */ ICallState state);

    /**
     * @return the first call in a the Call.state from a call list
     */
    CARAPI_(AutoPtr<ICall>) GetFirstCallOfState(
        /* [in] */ IArrayList* calls,
        /* [in] */ ICallState state,
        /* [in] */ Int64 subId);

    CARAPI_(Boolean) HasMoreThanOneRingingCall();

    /**
     * @return TRUE if more than one active ringing call exists on
     * the active subId.
     * This checks for the active calls on provided
     * subId and also active calls on SIP Phone.
     *
     */
    CARAPI_(Boolean) HasMoreThanOneRingingCall(
        /* [in] */ Int64 subId);

    /**
     * @return TRUE if more than one active background call exists on
     * the provided subId.
     * This checks for the background calls on provided
     * subId and also background calls on SIP Phone.
     *
     */
    CARAPI_(Boolean) HasMoreThanOneHoldingCall(
        /* [in] */ Int64 subId);

protected:
    // state registrants
    AutoPtr<RegistrantList> mPreciseCallStateRegistrants;
    AutoPtr<RegistrantList> mNewRingingConnectionRegistrants;
    AutoPtr<RegistrantList> mIncomingRingRegistrants;
    AutoPtr<RegistrantList> mDisconnectRegistrants;
    AutoPtr<RegistrantList> mMmiRegistrants;
    AutoPtr<RegistrantList> mUnknownConnectionRegistrants;
    AutoPtr<RegistrantList> mRingbackToneRegistrants;
    AutoPtr<RegistrantList> mOnHoldToneRegistrants;
    AutoPtr<RegistrantList> mInCallVoicePrivacyOnRegistrants;
    AutoPtr<RegistrantList> mInCallVoicePrivacyOffRegistrants;
    AutoPtr<RegistrantList> mCallWaitingRegistrants;
    AutoPtr<RegistrantList> mDisplayInfoRegistrants;
    AutoPtr<RegistrantList> mSignalInfoRegistrants;
    AutoPtr<RegistrantList> mCdmaOtaStatusChangeRegistrants;
    AutoPtr<RegistrantList> mResendIncallMuteRegistrants;
    AutoPtr<RegistrantList> mMmiInitiateRegistrants;
    AutoPtr<RegistrantList> mMmiCompleteRegistrants;
    AutoPtr<RegistrantList> mEcmTimerResetRegistrants;
    AutoPtr<RegistrantList> mSubscriptionInfoReadyRegistrants;
    AutoPtr<RegistrantList> mSuppServiceNotifyRegistrants;
    AutoPtr<RegistrantList> mSuppServiceFailedRegistrants;
    AutoPtr<RegistrantList> mServiceStateChangedRegistrants;
    AutoPtr<RegistrantList> mPostDialCharacterRegistrants;
    AutoPtr<RegistrantList> mActiveSubChangeRegistrants;

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    static const Int32 EVENT_DISCONNECT;
    static const Int32 EVENT_PRECISE_CALL_STATE_CHANGED;
    static const Int32 EVENT_NEW_RINGING_CONNECTION;
    static const Int32 EVENT_UNKNOWN_CONNECTION;
    static const Int32 EVENT_INCOMING_RING;
    static const Int32 EVENT_RINGBACK_TONE;
    static const Int32 EVENT_IN_CALL_VOICE_PRIVACY_ON;
    static const Int32 EVENT_IN_CALL_VOICE_PRIVACY_OFF;
    static const Int32 EVENT_CALL_WAITING;
    static const Int32 EVENT_DISPLAY_INFO;
    static const Int32 EVENT_SIGNAL_INFO;
    static const Int32 EVENT_CDMA_OTA_STATUS_CHANGE;
    static const Int32 EVENT_RESEND_INCALL_MUTE;
    static const Int32 EVENT_MMI_INITIATE;
    static const Int32 EVENT_MMI_COMPLETE;
    static const Int32 EVENT_ECM_TIMER_RESET;
    static const Int32 EVENT_SUBSCRIPTION_INFO_READY;
    static const Int32 EVENT_SUPP_SERVICE_FAILED;
    static const Int32 EVENT_SERVICE_STATE_CHANGED;
    static const Int32 EVENT_POST_DIAL_CHARACTER;
    static const Int32 EVENT_ONHOLD_TONE;
    static const Int32 EVENT_SUPP_SERVICE_NOTIFY;

    // Singleton instance
    static AutoPtr<CallManager> INSTANCE;

    // list of registered phones, which are PhoneBase objs
    AutoPtr<IArrayList> mPhones;

    // list of supported ringing calls
    AutoPtr<IArrayList> mRingingCalls;

    // list of supported background calls
    AutoPtr<IArrayList> mBackgroundCalls;

    // list of supported foreground calls
    AutoPtr<IArrayList> mForegroundCalls;

    // empty connection list
    AutoPtr<IArrayList> mEmptyConnections;

    // mapping of phones to registered handler instances used for callbacks from RIL
    HashMap<AutoPtr<IPhone>, AutoPtr<CallManagerHandler> > mHandlerMap/* = new HashMap<>()*/;

    // default phone as the first phone registered, which is PhoneBase obj
    AutoPtr<IPhone> mDefaultPhone;

    Boolean mSpeedUpAudioForMtCall;

    // Holds the current active SUB, all actions would be
    // taken on this sub.
    static Int64 mActiveSub;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLMANAGER_H__
