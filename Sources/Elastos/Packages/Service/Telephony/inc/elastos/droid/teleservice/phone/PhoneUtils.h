#ifndef  __ELASTOS_DROID_PHONE_PHONEUTILS_H__
#define  __ELASTOS_DROID_PHONE_PHONEUTILS_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/CallGatewayManager.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecomm.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::CTelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::ICallerInfoAsyncQuery;
using Elastos::Droid::Internal::Telephony::ICallerInfoAsyncQueryOnQueryCompleteListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Core::Object;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Misc utilities for the Phone app.
 */
class PhoneUtils
{
public:
    // static class VoiceMailNumberMissingException extends Exception {
    //     VoiceMailNumberMissingException() {
    //         super();
    //     }

    //     VoiceMailNumberMissingException(String msg) {
    //         super(msg);
    //     }
    // };

    /**
     * Class returned by the startGetCallerInfo call to package a temporary
     * CallerInfo Object, to be superceded by the CallerInfo Object passed
     * into the listener when the query with token mAsyncQueryToken is complete.
     */
    class CallerInfoToken
        : public Object
        , public IPhoneUtilsCallerInfoToken
    {
    public:
        TO_STRING_IMPL("PhoneUtils::CallerInfoToken")

        CAR_INTERFACE_DECL()

        /**indicates that there will no longer be updates to this request.*/
        Boolean mIsFinal;

        AutoPtr<ICallerInfo> mCurrentInfo;
        AutoPtr<ICallerInfoAsyncQuery> mAsyncQuery;
    };

    class MyCallerInfoAsyncQueryOnQueryCompleteListener
        : public Object
        , public ICallerInfoAsyncQueryOnQueryCompleteListener
    {
    public:
        TO_STRING_IMPL("PhoneUtils::MyCallerInfoAsyncQueryOnQueryCompleteListener")

        CAR_INTERFACE_DECL()

        /**
         * When the query completes, we stash the resulting CallerInfo
         * object away in the Connection's "userData" (where it will
         * later be retrieved by the in-call UI.)
         */
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICallerInfo* ci);
    };

private:
    class FgRingCalls
        : public Object
    {
    public:
        TO_STRING_IMPL("PhoneUtils::FgRingCalls")

        FgRingCalls(
            /* [in] */ ICall* fg,
            /* [in] */ ICall* ring)
            : mFgCall(fg)
            , mRinging(ring)
        {}

    public:
        AutoPtr<ICall> mFgCall;
        AutoPtr<ICall> mRinging;
    };

    /**
     * Handler that tracks the connections and updates the value of the
     * Mute settings for each connection as needed.
     */
    class ConnectionHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PhoneUtils::ConnectionHandler")

        ConnectionHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

    class MyDialogInterfaceOnDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        TO_STRING_IMPL("PhoneUtils::ConnectionHandler")

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);
    };

    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("PhoneUtils::MyDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ IEditText* inputText,
            /* [in] */ PhoneGlobals* app,
            /* [in] */ IMmiCode* mmiCode,
            /* [in] */ IPhone* phone)
            : mInputText(inputText)
            , mApp(app)
            , mMiCode(mmiCode)
            , mPhone(phone)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 whichButton);

    private:
        AutoPtr<IEditText> mInputText;
        AutoPtr<PhoneGlobals> mApp;
        AutoPtr<IMmiCode> mMiCode;
        AutoPtr<IPhone> mPhone;
    };

    class MyViewOnKeyListener
        : public Object
        , public IViewOnKeyListener
    {
    public:
        TO_STRING_IMPL("PhoneUtils::MyViewOnKeyListener")

        MyViewOnKeyListener(
            /* [in] */ IPhone* phone,
            /* [in] */ IEditText* inputText,
            /* [in] */ IAlertDialog* newDialog)
            : mPhone(phone)
            , mInputText(inputText)
            , mNewDialog(newDialog)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IPhone> mPhone;
        AutoPtr<IEditText> mInputText;
        AutoPtr<IAlertDialog> mNewDialog;
    };

public:
    TO_STRING_IMPL("PhoneUtils");

    /**
     * Register the ConnectionHandler with the phone, to receive connection events
     */
    static CARAPI InitializeConnectionHandler(
        /* [in] */ ICallManager* cm);

    /**
     * Answer the currently-ringing call.
     *
     * @return true if we answered the call, or false if there wasn't
     *         actually a ringing incoming call, or some other error occurred.
     *
     * @see #answerAndEndHolding(CallManager, Call)
     * @see #answerAndEndActive(CallManager, Call)
     */
    static CARAPI_(Boolean) AnswerCall(
        /* [in] */ ICall* ringingCall);

    /**
     * Hangs up all active calls.
     */
    static CARAPI HangupAllCalls(
        /* [in] */ ICallManager* cm);

    /**
     * Smart "hang up" helper method which hangs up exactly one connection,
     * based on the current Phone state, as follows:
     * <ul>
     * <li>If there's a ringing call, hang that up.
     * <li>Else if there's a foreground call, hang that up.
     * <li>Else if there's a background call, hang that up.
     * <li>Otherwise do nothing.
     * </ul>
     * @return true if we successfully hung up, or false
     *              if there were no active calls at all.
     */
    static CARAPI_(Boolean) Hangup(
        /* [in] */ ICallManager* cm);

    static CARAPI_(Boolean) HangupRingingCall(
        /* [in] */ ICall* ringing);

    static CARAPI_(Boolean) HangupActiveCall(
        /* [in] */ ICall* foreground);

    static CARAPI_(Boolean) HangupHoldingCall(
        /* [in] */ ICall* background);

    /**
     * Used in CDMA phones to end the complete Call session
     * @param phone the Phone object.
     * @return true if *any* call was successfully hung up
     */
    static CARAPI_(Boolean) HangupRingingAndActive(
        /* [in] */ IPhone* phone);

    /**
     * Trivial wrapper around Call.hangup(), except that we return a
     * Boolean success code rather than throwing CallStateException on
     * failure.
     *
     * @return true if the call was successfully hung up, or false
     *         if the call wasn't actually active.
     */
    static CARAPI_(Boolean) Hangup(
        /* [in] */ ICall* call);

    /**
     * Trivial wrapper around Connection.hangup(), except that we silently
     * do nothing (rather than throwing CallStateException) if the
     * connection wasn't actually active.
     */
    static CARAPI Hangup(
        /* [in] */ IConnection* c);

    static CARAPI_(Boolean) AnswerAndEndHolding(
        /* [in] */ ICallManager* cm,
        /* [in] */ ICall* ringing);

    /**
     * Answers the incoming call specified by "ringing", and ends the currently active phone call.
     *
     * This method is useful when's there's an incoming call which we cannot manage with the
     * current call. e.g. when you are having a phone call with CDMA network and has received
     * a SIP call, then we won't expect our telephony can manage those phone calls simultaneously.
     * Note that some types of network may allow multiple phone calls at once; GSM allows to hold
     * an ongoing phone call, so we don't need to end the active call. The caller of this method
     * needs to check if the network allows multiple phone calls or not.
     *
     * @see #answerCall(Call)
     * @see InCallScreen#internalAnswerCall()
     */
    static CARAPI_(Boolean) AnswerAndEndActive(
        /* [in] */ ICallManager* cm,
        /* [in] */ ICall* ringing);

    /**
     * @see placeCall below
     */
    static CARAPI_(Int32) PlaceCall(
        /* [in] */ IContext* context,
        /* [in] */ IPhone* phone,
        /* [in] */ const String& number,
        /* [in] */ IUri* contactRef,
        /* [in] */ Boolean isEmergencyCall);

    /**
     * Dial the number using the phone passed in.
     *
     * If the connection is establised, this method issues a sync call
     * that may block to query the caller info.
     * TODO: Change the logic to use the async query.
     *
     * @param context To perform the CallerInfo query.
     * @param phone the Phone object.
     * @param number to be dialed as requested by the user. This is
     * NOT the phone number to connect to. It is used only to build the
     * call card and to update the call log. See above for restrictions.
     * @param contactRef that triggered the call. Typically a 'tel:'
     * uri but can also be a 'content://contacts' one.
     * @param isEmergencyCall indicates that whether or not this is an
     * emergency call
     * @param gatewayUri Is the address used to setup the connection, null
     * if not using a gateway
     * @param callGateway Class for setting gateway data on a successful call.
     *
     * @return either CALL_STATUS_DIALED or CALL_STATUS_FAILED
     */
    static CARAPI_(Int32) PlaceCall(
        /* [in] */ IContext* context,
        /* [in] */ IPhone* phone,
        /* [in] */ const String& number,
        /* [in] */ IUri* contactRef,
        /* [in] */ Boolean isEmergencyCall,
        /* [in] */ CallGatewayManager::RawGatewayInfo* gatewayInfo,
        /* [in] */ CallGatewayManager* callGateway);

    static CARAPI_(String) ToLogSafePhoneNumber(
        /* [in] */ const String& number);

    /**
     * Wrapper function to control when to send an empty Flash command to the network.
     * Mainly needed for CDMA networks, such as scenarios when we need to send a blank flash
     * to the network prior to placing a 3-way call for it to be successful.
     */
    static CARAPI SendEmptyFlash(
        /* [in] */ IPhone* phone);

    static CARAPI Swap();

    /**
     * @param heldCall is the background call want to be swapped
     */
    static CARAPI SwitchHoldingAndActive(
        /* [in] */ ICall* heldCall);

    static CARAPI MergeCalls();

    static CARAPI MergeCalls(
        /* [in] */ ICallManager* cm);

    static CARAPI SeparateCall(
        /* [in] */ IConnection* c);

    /**
     * Handle the MMIInitiate message and put up an alert that lets
     * the user cancel the operation, if applicable.
     *
     * @param context context to get strings.
     * @param mmiCode the MmiCode object being started.
     * @param buttonCallbackMessage message to post when button is clicked.
     * @param previousAlert a previous alert used in this activity.
     * @return the dialog handle
     */
    static CARAPI_(AutoPtr<IDialog>) DisplayMMIInitiate(
        /* [in] */ IContext* context,
        /* [in] */ IMmiCode* mmiCode,
        /* [in] */ IMessage* buttonCallbackMessage,
        /* [in] */ IDialog* previousAlert);

    /**
     * Handle the MMIComplete message and fire off an intent to display
     * the message.
     *
     * @param context context to get strings.
     * @param mmiCode MMI result.
     * @param previousAlert a previous alert used in this activity.
     */
    static CARAPI DisplayMMIComplete(
        /* [in] */ IPhone* phone,
        /* [in] */ IContext* context,
        /* [in] */ IMmiCode* mmiCode,
        /* [in] */ IMessage* dismissCallbackMessage,
        /* [in] */ IAlertDialog* previousAlert);

    /**
     * Cancels the current pending MMI operation, if applicable.
     * @return true if we canceled an MMI operation, or false
     *         if the current pending MMI wasn't cancelable
     *         or if there was no current pending MMI at all.
     *
     * @see displayMMIInitiate
     */
    static CARAPI_(Boolean) CancelMmiCode(
        /* [in] */ IPhone* phone);

    /**
     * Given an Intent (which is presumably the ACTION_CALL intent that
     * initiated this outgoing call), figure out the actual phone number we
     * should dial.
     *
     * Note that the returned "number" may actually be a SIP address,
     * if the specified intent contains a sip: URI.
     *
     * This method is basically a wrapper around PhoneUtils.getNumberFromIntent(),
     * except it's also aware of the EXTRA_ACTUAL_NUMBER_TO_DIAL extra.
     * (That extra, if present, tells us the exact string to pass down to the
     * telephony layer.  It's guaranteed to be safe to dial: it's either a PSTN
     * phone number with separators and keypad letters stripped out, or a raw
     * unencoded SIP address.)
     *
     * @return the phone number corresponding to the specified Intent, or null
     *   if the Intent has no action or if the intent's data is malformed or
     *   missing.
     *
     * @throws VoiceMailNumberMissingException if the intent
     *   contains a "voicemail" URI, but there's no voicemail
     *   number configured on the device.
     */
    static CARAPI GetInitialNumber(
        /* [in] */ IIntent* intent,
        /* [out] */ String* num);

    /**
     * Returns the caller-id info corresponding to the specified Connection.
     * (This is just a simple wrapper around CallerInfo.getCallerInfo(): we
     * extract a phone number from the specified Connection, and feed that
     * number into CallerInfo.getCallerInfo().)
     *
     * The returned CallerInfo may be null in certain error cases, like if the
     * specified Connection was null, or if we weren't able to get a valid
     * phone number from the Connection.
     *
     * Finally, if the getCallerInfo() call did succeed, we save the resulting
     * CallerInfo object in the "userData" field of the Connection.
     *
     * NOTE: This API should be avoided, with preference given to the
     * asynchronous startGetCallerInfo API.
     */
    static CARAPI_(AutoPtr<ICallerInfo>) GetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IConnection* c);

    /**
     * Start a CallerInfo Query based on the earliest connection in the call.
     */
    static CARAPI StartGetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ ICall* call,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ CallerInfoToken** token);

    static CARAPI_(AutoPtr<CallerInfoToken>) StartGetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IConnection* c,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie);

    /**
     * place a temporary callerinfo object in the hands of the caller and notify
     * caller when the actual query is done.
     */
    static CARAPI_(AutoPtr<CallerInfoToken>) StartGetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IConnection* c,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [in] */ CallGatewayManager::RawGatewayInfo* info);

    /**
     * Returns a single "name" for the specified given a CallerInfo object.
     * If the name is null, return defaultString as the default value, usually
     * context.getString(R.string.unknown).
     */
    static CARAPI_(String) GetCompactNameFromCallerInfo(
        /* [in] */ ICallerInfo* ci,
        /* [in] */ IContext* context);

    /**
     * Returns true if the specified Call is a "conference call", meaning
     * that it owns more than one Connection object.  This information is
     * used to trigger certain UI changes that appear when a conference
     * call is active (like displaying the label "Conference call", and
     * enabling the "Manage conference" UI.)
     *
     * Watch out: This method simply checks the number of Connections,
     * *not* their states.  So if a Call has (for example) one ACTIVE
     * connection and one DISCONNECTED connection, this method will return
     * true (which is unintuitive, since the Call isn't *really* a
     * conference call any more.)
     *
     * @return true if the specified call has more than one connection (in any state.)
     */
    static CARAPI_(Boolean) IsConferenceCall(
        /* [in] */ ICall* call);

    /**
     * Launch the Dialer to start a new call.
     * This is just a wrapper around the ACTION_DIAL intent.
     */
    static CARAPI_(Boolean) StartNewCall(
        /* [in] */ ICallManager* cm);

    /**
     * Turns on/off speaker.
     *
     * @param context Context
     * @param flag True when speaker should be on. False otherwise.
     * @param store True when the settings should be stored in the device.
     */
    static CARAPI TurnOnSpeaker(
        /* [in] */ IContext* context,
        /* [in] */ Boolean flag,
        /* [in] */ Boolean store);

    /**
     * Restore the speaker mode, called after a wired headset disconnect
     * event.
     */
    static CARAPI RestoreSpeakerMode(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsSpeakerOn(
        /* [in] */ IContext* context);


    static CARAPI TurnOnNoiseSuppression(
        /* [in] */ IContext* context,
        /* [in] */ Boolean flag,
        /* [in] */ Boolean store);

    static CARAPI RestoreNoiseSuppression(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsNoiseSuppressionOn(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsInEmergencyCall(
        /* [in] */ ICallManager* cm);

    /**
     * Get the mute state of foreground phone, which has the current
     * foreground call
     */
    static CARAPI_(Boolean) GetMute();

    static CARAPI SetAudioMode();

    /**
     * Sets the audio mode per current phone state.
     */
    static CARAPI SetAudioMode(
        /* [in] */ ICallManager* cm);

    /**
     * Look for ANY connections on the phone that qualify as being
     * disconnected.
     *
     * @return true if we find a connection that is disconnected over
     * all the phone's call objects.
     */
    static CARAPI_(Boolean) HasDisconnectedConnections(
        /* [in] */ IPhone* phone);

    //
    // Misc UI policy helper functions
    //

    /**
     * @return true if we're allowed to hold calls, given the current
     * state of the Phone.
     */
    static CARAPI_(Boolean) OkToHoldCall(
        /* [in] */ ICallManager* cm);

    /**
     * @return true if we support holding calls, given the current
     * state of the Phone.
     */
    static CARAPI_(Boolean) OkToSupportHold(
        /* [in] */ ICallManager* cm);

    /**
     * @return true if we're allowed to swap calls, given the current
     * state of the Phone.
     */
    static CARAPI OkToSwapCalls(
        /* [in] */ ICallManager* cm,
        /* [out] */ Boolean* result);

    /**
     * @return true if we're allowed to merge calls, given the current
     * state of the Phone.
     */
    static CARAPI_(Boolean) OkToMergeCalls(
        /* [in] */ ICallManager* cm);

    /**
     * @return true if the UI should let you add a new call, given the current
     * state of the Phone.
     */
    static CARAPI OkToAddCall(
        /* [in] */ ICallManager* cm,
        /* [out] */ Boolean* result);

    /**
     * Handles certain "corner cases" for CNAP. When we receive weird phone numbers
     * from the network to indicate different number presentations, convert them to
     * expected number and presentation values within the CallerInfo object.
     * @param number number we use to verify if we are in a corner case
     * @param presentation presentation value used to verify if we are in a corner case
     * @return the new String that should be used for the phone number
     */
    static CARAPI_(String) ModifyForSpecialCnapCases(
        /* [in] */ IContext* context,
        /* [in] */ ICallerInfo* ci,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation);

    /**
     * Returns whether the phone is in ECM ("Emergency Callback Mode") or not.
     */
    static CARAPI_(Boolean) IsPhoneInEcm(
        /* [in] */ IPhone* phone);

    /**
     * Returns the most appropriate Phone object to handle a call
     * to the specified number.
     *
     * @param cm the CallManager.
     * @param scheme the scheme from the data URI that the number originally came from.
     * @param number the phone number, or SIP address.
     */
    static CARAPI_(AutoPtr<IPhone>) PickPhoneBasedOnNumber(
        /* [in] */ ICallManager* cm,
        /* [in] */ const String& scheme,
        /* [in] */ const String& number,
        /* [in] */ const String& primarySipUri,
        /* [in] */ IComponentName* thirdPartyCallComponent);

    static CARAPI_(AutoPtr<IPhone>) GetSipPhoneFromUri(
        /* [in] */ ICallManager* cm,
        /* [in] */ const String& target);

    /**
     * Returns true when the given call is in INCOMING state and there's no foreground phone call,
     * meaning the call is the first real incoming call the phone is having.
     */
    static CARAPI_(Boolean) IsRealIncomingCall(
        /* [in] */ ICallState state);

    static CARAPI_(String) GetPresentationString(
        /* [in] */ IContext* context,
        /* [in] */ Int32 presentation);

    static CARAPI_(void) SendViewNotificationAsync(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactUri);

    //
    // General phone and call state debugging/testing code
    //

    static CARAPI_(void) DumpCallState(
        /* [in] */ IPhone* phone);

    static CARAPI_(void) DumpCallManager();

    /**
     * @return if the context is in landscape orientation.
     */
    static CARAPI_(Boolean) IsLandscape(
        /* [in] */ IContext* context);

private:
    /** This class is never instantiated. */
    PhoneUtils() {}

    /**
     * For a CDMA phone, advance the call state upon making a new
     * outgoing call.
     *
     * <pre>
     *   IDLE -> SINGLE_ACTIVE
     * or
     *   SINGLE_ACTIVE -> THRWAY_ACTIVE
     * </pre>
     * @param app The phone instance.
     */
    static CARAPI_(void) UpdateCdmaCallStateOnNewOutgoingCall(
        /* [in] */ PhoneGlobals* app,
        /* [in] */ IConnection* connection);

    /**
     * Gets the phone number to be called from an intent.  Requires a Context
     * to access the contacts database, and a Phone to access the voicemail
     * number.
     *
     * <p>If <code>phone</code> is <code>null</code>, the function will return
     * <code>null</code> for <code>voicemail:</code> URIs;
     * if <code>context</code> is <code>null</code>, the function will return
     * <code>null</code> for person/phone URIs.</p>
     *
     * <p>If the intent contains a <code>sip:</code> URI, the returned
     * "number" is actually the SIP address.
     *
     * @param context a context to use (or
     * @param intent the intent
     *
     * @throws VoiceMailNumberMissingException if <code>intent</code> contains
     *         a <code>voicemail:</code> URI, but <code>phone</code> does not
     *         have a voicemail number set.
     *
     * @return the phone number (or SIP address) that would be called by the intent,
     *         or <code>null</code> if the number cannot be found.
     */
    static CARAPI GetNumberFromIntent(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [out] */ String* str);

    /**
     * Iterate over all connections in a call to see if there are any
     * that are not alive (disconnected or idle).
     *
     * @return true if we find a connection that is disconnected, and
     * pending removal via
     * {@link com.android.internal.telephony.gsm.GsmCall#clearDisconnected()}.
     */
    static CARAPI_(Boolean) HasDisconnectedConnections(
        /* [in] */ ICall* call);

    /**
     * Based on the input CNAP number string,
     * @return _RESTRICTED or _UNKNOWN for all the special CNAP strings.
     * Otherwise, return CNAP_SPECIAL_CASE_NO.
     */
    static CARAPI_(Int32) CheckCnapSpecialCases(
        /* [in] */ const String& n);

    //
    // Support for 3rd party phone service providers.
    //

    /**
     * Check if a phone number can be route through a 3rd party
     * gateway. The number must be a global phone number in numerical
     * form (1-800-666-SEXY won't work).
     *
     * MMI codes and the like cannot be used as a dial number for the
     * gateway either.
     *
     * @param number To be dialed via a 3rd party gateway.
     * @return true If the number can be routed through the 3rd party network.
     */
    static CARAPI_(Boolean) IsRoutableViaGateway(
        /* [in] */ const String& number);

    /**
    * This function is called when phone answers or places a call.
    * Check if the phone is in a car dock or desk dock.
    * If yes, turn on the speaker, when no wired or BT headsets are connected.
    * Otherwise do nothing.
    * @return true if activated
    */
    static CARAPI_(Boolean) ActivateSpeakerIfDocked(
        /* [in] */ IPhone* phone);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    friend class CallController;
    friend class CInCallScreenShowActivation;
    friend class CallNotifier;
    friend class EmergencyCallHelper;
    friend class  OtaUtils;

    static const String TAG;
    static const Boolean DBG;

    // Do not check in with VDBG = true, since that may write PII to the system log.
    static const Boolean VDBG;

    /** Control stack trace for Audio Mode settings */
    static const Boolean DBG_SETAUDIOMODE_STACK;

    /** Identifier for the "Add Call" intent extra. */
    static const String ADD_CALL_MODE_KEY;

    // Return codes from placeCall()
    static const Int32 CALL_STATUS_DIALED = 0;  // The number was successfully dialed
    static const Int32 CALL_STATUS_DIALED_MMI = 1;  // The specified number was an MMI code
    static const Int32 CALL_STATUS_FAILED = 2;  // The call failed

    // State of the Phone's audio modes
    // Each state can move to the other states, but within the state only certain
    //  transitions for AudioManager.setMode() are allowed.
    static const Int32 AUDIO_IDLE;  /** audio behaviour at phone idle */
    static const Int32 AUDIO_RINGING;  /** audio behaviour while ringing */
    static const Int32 AUDIO_OFFHOOK;  /** audio behaviour while in call. */

    // USSD string length for MMI operations
    static const Int32 MIN_USSD_LEN;
    static const Int32 MAX_USSD_LEN;

    /** Speaker state, persisting between wired headset connection events */
    static Boolean sIsSpeakerEnabled;

    /** Static handler for the connection/mute tracking */
    static AutoPtr<ConnectionHandler> mConnectionHandler;

    /** Phone state changed event*/
    static const Int32 PHONE_STATE_CHANGED;

    /** check status then decide whether answerCall */
    static const Int32 MSG_CHECK_STATUS_ANSWERCALL = 100;

    /** poll phone DISCONNECTING status interval */
    static const Int32 DISCONNECTING_POLLING_INTERVAL_MS;

    /** poll phone DISCONNECTING status times limit */
    static const Int32 DISCONNECTING_POLLING_TIMES_LIMIT;

    /** Define for not a special CNAP string */
    static const Int32 CNAP_SPECIAL_CASE_NO;

    /** Noise suppression status as selected by user */
    static Boolean sIsNoiseSuppressionEnabled;

    /** USSD information used to aggregate all USSD messages */
    static AutoPtr<IAlertDialog> sUssdDialog;
    static StringBuilder sUssdMsg;

    /**
     * Static CallerInfoAsyncQuery.OnQueryCompleteListener instance that
     * we use with all our CallerInfoAsyncQuery.startQuery() requests.
     */
    static const Int32 QUERY_TOKEN;

    static AutoPtr<ICallerInfoAsyncQueryOnQueryCompleteListener> sCallerInfoQueryListener;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_PHONEUTILS_H__
