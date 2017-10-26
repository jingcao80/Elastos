//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.Core.h>
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/Arrays.h>
#include "R.h"

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Bluetooth::IIBluetoothHeadsetPhone;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::Internal::Telephony::CCallerInfo;
using Elastos::Droid::Internal::Telephony::ICallerInfoAsyncQueryHelper;
using Elastos::Droid::Internal::Telephony::CCallerInfoAsyncQueryHelper;
using Elastos::Droid::Internal::Telephony::IMmiCodeState_PENDING;
using Elastos::Droid::Internal::Telephony::IMmiCodeState_CANCELLED;
using Elastos::Droid::Internal::Telephony::IMmiCodeState_COMPLETE;
using Elastos::Droid::Internal::Telephony::IMmiCodeState_FAILED;
using Elastos::Droid::Internal::Telephony::IMmiCodeState;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_IDLE;
using Elastos::Droid::Internal::Telephony::ICallState_DISCONNECTED;
using Elastos::Droid::Internal::Telephony::ICallState_HOLDING;
using Elastos::Droid::Internal::Telephony::ICallState_IDLE;
using Elastos::Droid::Internal::Telephony::ICallState_ACTIVE;
using Elastos::Droid::Internal::Telephony::ICallState_INCOMING;
using Elastos::Droid::Internal::Telephony::ICallState_WAITING;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::EIID_ICallerInfoAsyncQueryOnQueryCompleteListener;
using Elastos::Droid::Internal::Telephony::ICallerInfoHelper;
using Elastos::Droid::Internal::Telephony::CCallerInfoHelper;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::ICallManagerHelper;
//using Elastos::Droid::Internal::Telephony::CCallManagerHelper;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Internal::Telephony::ICallState_DISCONNECTING;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

PhoneUtils::ConnectionHandler::ConnectionHandler()
{
    Handler::constructor();
}

ECode PhoneUtils::ConnectionHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_CHECK_STATUS_ANSWERCALL:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<FgRingCalls> frC = (FgRingCalls*)IObject::Probe(obj);
            Int32 arg1;
            Int32 state;
            Boolean res;
            // wait for finishing disconnecting
            // before check the ringing call state
            if ((frC->mFgCall != NULL) &&
                ((frC->mFgCall->GetState(&state), state) == ICallState_DISCONNECTING) &&
                ((msg->GetArg1(&arg1), arg1) < DISCONNECTING_POLLING_TIMES_LIMIT)) {
                AutoPtr<IMessage> retryMsg;
                mConnectionHandler->ObtainMessage(MSG_CHECK_STATUS_ANSWERCALL,
                        (IMessage**)&retryMsg);
                retryMsg->SetArg1(1 + arg1);
                retryMsg->SetObj(TO_IINTERFACE(obj));
                Boolean tmp;
                mConnectionHandler->SendMessageDelayed(retryMsg,
                    DISCONNECTING_POLLING_INTERVAL_MS, &tmp);
            // since hangupActiveCall() also accepts the ringing call
            // check if the ringing call was already answered or not
            // only answer it when the call still is ringing
            }
            else if (frC->mRinging->IsRinging(&res), res) {
                if ((msg->GetArg1(&arg1), arg1) == DISCONNECTING_POLLING_TIMES_LIMIT) {
                    Logger::E(TAG, "DISCONNECTING time out");
                }
                AnswerCall(frC->mRinging);
            }
            break;
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(PhoneUtils::MyDialogInterfaceOnDismissListener, Object,
        IDialogInterfaceOnDismissListener)

ECode PhoneUtils::MyDialogInterfaceOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    return sUssdMsg.SetLength(0);
}

CAR_INTERFACE_IMPL(PhoneUtils::MyDialogInterfaceOnClickListener, Object,
        IDialogInterfaceOnClickListener)

ECode PhoneUtils::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 whichButton)
{
    switch (whichButton) {
        case IDialogInterface::BUTTON_POSITIVE:
        {
            // As per spec 24.080, valid length of ussd string
            // is 1 - 160. If length is out of the range then
            // display toast message & Cancel MMI operation.
            Int32 length;
            ITextView::Probe(mInputText)->GetLength(&length);
            if (length < MIN_USSD_LEN || length > MAX_USSD_LEN) {

                AutoPtr<IResources> resources;
                mApp->GetResources((IResources**)&resources);
                AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
                AutoPtr<IInteger32> num1 = CoreUtils::Convert(MIN_USSD_LEN);
                AutoPtr<IInteger32> num2 = CoreUtils::Convert(MAX_USSD_LEN);
                array->Set(0, TO_IINTERFACE(num1));
                array->Set(1, TO_IINTERFACE(num2));
                String str;
                resources->GetString(Elastos::Droid::TeleService::R::string::enter_input,
                        array, &str);
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);

                AutoPtr<IToastHelper> helper;
                CToastHelper::AcquireSingleton((IToastHelper**)&helper);
                AutoPtr<IToast> toast;
                helper->MakeText(mApp, cchar, IToast::LENGTH_LONG, (IToast**)&toast);
                toast->Show();

                Boolean res;
                if (mMiCode->IsCancelable(&res), res) {
                    mMiCode->Cancel();
                }
            }
            else {
                AutoPtr<ICharSequence> cchar;
                ITextView::Probe(mInputText)->GetText((ICharSequence**)&cchar);
                String str;
                cchar->ToString(&str);
                mPhone->SendUssdResponse(str);
            }
            break;
        }
        case IDialogInterface::BUTTON_NEGATIVE:
        {
            Boolean res;
            if (mMiCode->IsCancelable(&res), res) {
                mMiCode->Cancel();
            }
            break;
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(PhoneUtils::MyViewOnKeyListener, Object, IViewOnKeyListener)

ECode PhoneUtils::MyViewOnKeyListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (keyCode) {
        case IKeyEvent::KEYCODE_CALL:
        case IKeyEvent::KEYCODE_ENTER:
        {
            Int32 action;
            event->GetAction(&action);
            if(action == IKeyEvent::ACTION_DOWN) {
                AutoPtr<ICharSequence> cchar;
                ITextView::Probe(mInputText)->GetText((ICharSequence**)&cchar);
                String str;
                cchar->ToString(&str);
                mPhone->SendUssdResponse(str);
                IDialogInterface::Probe(mNewDialog)->Dismiss();
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(PhoneUtils::CallerInfoToken, Object, IPhoneUtilsCallerInfoToken)

CAR_INTERFACE_IMPL(PhoneUtils::MyCallerInfoAsyncQueryOnQueryCompleteListener, Object,
        ICallerInfoAsyncQueryOnQueryCompleteListener)

ECode PhoneUtils::MyCallerInfoAsyncQueryOnQueryCompleteListener::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICallerInfo* ci)
{
    if (DBG) Log(String("query complete, updating connection.userdata"));
    AutoPtr<IConnection> conn = IConnection::Probe(cookie);

    // Added a check if CallerInfo is coming from ContactInfo or from Connection.
    // If no ContactInfo, then we want to use CNAP information coming from network
    if (DBG) Log(String("- onQueryComplete: CallerInfo:") + TO_CSTR(ci));

    Boolean res1, res2;

    Boolean contactExists;
    ci->GetContactExists(&contactExists);
    if (contactExists || (ci->IsEmergencyNumber(&res1), res1) || (ci->IsVoiceMailNumber(&res2), res2)) {
        // If the number presentation has not been set by
        // the ContactInfo, use the one from the
        // connection.

        // TODO: Need a new util method to merge the info
        // from the Connection in a CallerInfo object.
        // Here 'ci' is a new CallerInfo instance read
        // from the DB. It has lost all the connection
        // info preset before the query (see PhoneUtils
        // line 1334). We should have a method to merge
        // back into this new instance the info from the
        // connection object not set by the DB. If the
        // Connection already has a CallerInfo instance in
        // userData, then we could use this instance to
        // fill 'ci' in. The same routine could be used in
        // PhoneUtils.
        Int32 numberPresentation;
        ci->GetNumberPresentation(&numberPresentation);
        if (0 == numberPresentation) {
            Int32 result;
            conn->GetNumberPresentation(&result);
            ci->SetNumberPresentation(result);
        }
    }
    else {
        // No matching contact was found for this number.
        // Return a new CallerInfo based solely on the CNAP
        // information from the network.

        AutoPtr<ICallerInfo> newCi = GetCallerInfo(NULL, conn);

        // ...but copy over the (few) things we care about
        // from the original CallerInfo object:
        if (newCi != NULL) {
            String phoneNumber;
            ci->GetPhoneNumber(&phoneNumber);
            newCi->SetPhoneNumber(phoneNumber); // To get formatted phone number

            String geoDescription;
            ci->GetGeoDescription(&geoDescription);
            newCi->SetGeoDescription(geoDescription); // To get geo description string

            ci = newCi;
        }
    }

    if (DBG) {
        StringBuilder sb;
        sb += "==> Stashing CallerInfo ";
        sb += ci;
        sb += " into the connection...";
        Log(sb.ToString());
    }
    return conn->SetUserData(ci);
}

const String PhoneUtils::TAG("PhoneUtils");
const Boolean PhoneUtils::DBG = (IPhoneGlobals::DBG_LEVEL >= 2);

const Boolean PhoneUtils::VDBG = FALSE;

const Boolean PhoneUtils::DBG_SETAUDIOMODE_STACK = FALSE;

const String PhoneUtils::ADD_CALL_MODE_KEY("add_call_mode");

const Int32 PhoneUtils::AUDIO_IDLE = 0;  /** audio behaviour at phone idle */
const Int32 PhoneUtils::AUDIO_RINGING = 1;  /** audio behaviour while ringing */
const Int32 PhoneUtils::AUDIO_OFFHOOK = 2;  /** audio behaviour while in call. */

const Int32 PhoneUtils::MIN_USSD_LEN = 1;
const Int32 PhoneUtils::MAX_USSD_LEN = 160;

Boolean PhoneUtils::sIsSpeakerEnabled = FALSE;

AutoPtr<PhoneUtils::ConnectionHandler> PhoneUtils::mConnectionHandler;

const Int32 PhoneUtils::PHONE_STATE_CHANGED = -1;

const Int32 PhoneUtils::DISCONNECTING_POLLING_INTERVAL_MS = 200;

const Int32 PhoneUtils::DISCONNECTING_POLLING_TIMES_LIMIT = 8;

const Int32 PhoneUtils::CNAP_SPECIAL_CASE_NO = -1;

Boolean PhoneUtils::sIsNoiseSuppressionEnabled = TRUE;

AutoPtr<IAlertDialog> PhoneUtils::sUssdDialog;
StringBuilder PhoneUtils::sUssdMsg;

const Int32 PhoneUtils::QUERY_TOKEN = -1;

AutoPtr<ICallerInfoAsyncQueryOnQueryCompleteListener> PhoneUtils::sCallerInfoQueryListener =
    new PhoneUtils::MyCallerInfoAsyncQueryOnQueryCompleteListener();

ECode PhoneUtils::InitializeConnectionHandler(
    /* [in] */ ICallManager* cm)
{
    if (mConnectionHandler == NULL) {
        mConnectionHandler = new PhoneUtils::ConnectionHandler();
    }

    // pass over cm as user.obj
    return cm->RegisterForPreciseCallStateChanged(mConnectionHandler, PHONE_STATE_CHANGED, cm);
}

Boolean PhoneUtils::AnswerCall(
    /* [in] */ ICall* ringingCall)
{
    StringBuilder sb;
    sb += "answerCall(";
    sb += ringingCall;
    sb += ")...";
    Log(sb.ToString());
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<CallNotifier> notifier = app->mNotifier;

    AutoPtr<IPhone> phone;
    ringingCall->GetPhone((IPhone**)&phone);
    Int32 type;
    phone->GetPhoneType(&type);
    Boolean phoneIsCdma = (type == IPhoneConstants::PHONE_TYPE_CDMA);
    Boolean answered = FALSE;
    AutoPtr<IIBluetoothHeadsetPhone> btPhone;

    if (phoneIsCdma) {
        // Stop any signalInfo tone being played when a Call waiting gets answered
        Int32 state;
        ringingCall->GetState(&state);
        if (state == ICallState_WAITING) {
            notifier->StopSignalInfoTone();
        }
    }

    Boolean res;
    if (ringingCall != NULL && (ringingCall->IsRinging(&res), res)) {
        if (DBG) {
            Int32 state;
            ringingCall->GetState(&state);
            Log(String("answerCall: call state = ") + StringUtils::ToString(state));
        }
        //try
        ECode ec = NOERROR;
        {
            if (phoneIsCdma) {
                CdmaPhoneCallState::PhoneCallState state;
                if ((app->mCdmaPhoneCallState->GetCurrentCallState(&state), state)
                        == CdmaPhoneCallState::IDLE) {
                    // This is the FIRST incoming call being answered.
                    // Set the Phone Call State to SINGLE_ACTIVE
                    FAIL_GOTO(ec = app->mCdmaPhoneCallState->SetCurrentCallState(
                            CdmaPhoneCallState::SINGLE_ACTIVE), ERROR)
                }
                else {
                    // This is the CALL WAITING call being answered.
                    // Set the Phone Call State to CONF_CALL
                    FAIL_GOTO(ec = app->mCdmaPhoneCallState->SetCurrentCallState(
                            CdmaPhoneCallState::CONF_CALL), ERROR)
                    // Enable "Add Call" option after answering a Call Waiting as the user
                    // should be allowed to add another call in case one of the parties
                    // drops off
                    FAIL_GOTO(ec = app->mCdmaPhoneCallState->SetAddCallMenuStateAfterCallWaiting(
                            TRUE), ERROR)
                }
            }

            Int32 state;
            FAIL_GOTO(ec = ringingCall->GetState(&state), ERROR)
            Boolean isRealIncomingCall = IsRealIncomingCall(state);

            //if (DBG) log("sPhone.acceptCall");
            FAIL_GOTO(ec = app->mCM->AcceptCall(ringingCall), ERROR)
            answered = TRUE;

            SetAudioMode();

            // Check is phone in any dock, and turn on speaker accordingly
            Boolean speakerActivated = ActivateSpeakerIfDocked(phone);

            AutoPtr<IBluetoothManager> btManager;
            FAIL_GOTO(ec = app->GetBluetoothManager((IBluetoothManager**)&btManager), ERROR)

            // When answering a phone call, the user will move the phone near to her/his ear
            // and start conversation, without checking its speaker status. If some other
            // application turned on the speaker mode before the call and didn't turn it off,
            // Phone app would need to be responsible for the speaker phone.
            // Here, we turn off the speaker if
            // - the phone call is the first in-coming call,
            // - we did not activate speaker by ourselves during the process above, and
            // - Bluetooth headset is not in use.
            Boolean res;
            if (isRealIncomingCall && !speakerActivated && IsSpeakerOn(app)
                    && (btManager->IsBluetoothHeadsetAudioOn(&res), !res)) {
                // This is not an error but might cause users' confusion. Add log just in case.
                Logger::I(TAG, "Forcing speaker off due to new incoming call...");
                TurnOnSpeaker(app, FALSE, TRUE);
            }
        }
        //catch (CallStateException ex) {
    ERROR:
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::W(TAG, "answerCall: caught %d", ec);

            if (phoneIsCdma) {
                // restore the cdmaPhoneCallState and btPhone.cdmaSetSecondCallState:
                CdmaPhoneCallState::PhoneCallState state;
                app->mCdmaPhoneCallState->GetPreviousCallState(&state);
                app->mCdmaPhoneCallState->SetCurrentCallState(state);
                if (btPhone != NULL) {
                    //try {
                    ec = btPhone->CdmaSetSecondCallState(FALSE);
                    //} catch (RemoteException e) {
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Logger::E(TAG, "RemoteException, %d", ec);
                    }
                    //}
                }
            }
        //}
        }
    }
    return answered;
}

ECode PhoneUtils::HangupAllCalls(
    /* [in] */ ICallManager* cm)
{
    AutoPtr<ICall> ringing;
    cm->GetFirstActiveRingingCall((ICall**)&ringing);
    AutoPtr<ICall> fg;
    cm->GetActiveFgCall((ICall**)&fg);
    AutoPtr<ICall> bg;
    cm->GetFirstActiveBgCall((ICall**)&bg);

    // We go in reverse order, BG->FG->RINGING because hanging up a ringing call or an active
    // call can move a bg call to a fg call which would force us to loop over each call
    // several times.  This ordering works best to ensure we dont have any more calls.
    Boolean res;
    if (bg != NULL && (bg->IsIdle(&res), !res)) {
        Hangup(bg);
    }
    if (fg != NULL && (fg->IsIdle(&res), !res)) {
        Hangup(fg);
    }
    if (ringing != NULL && (ringing->IsIdle(&res), !res)) {
        HangupRingingCall(fg);
    }
    return NOERROR;
}

Boolean PhoneUtils::Hangup(
    /* [in] */ ICallManager* cm)
{
    Boolean hungup = FALSE;
    AutoPtr<ICall> ringing;
    cm->GetFirstActiveRingingCall((ICall**)&ringing);
    AutoPtr<ICall> fg;
    cm->GetActiveFgCall((ICall**)&fg);
    AutoPtr<ICall> bg;
    cm->GetFirstActiveBgCall((ICall**)&bg);

    Boolean res;
    if (ringing->IsIdle(&res), !res) {
        Log(String("hangup(): hanging up ringing call"));
        hungup = HangupRingingCall(ringing);
    }
    else if (fg->IsIdle(&res), !res) {
        Log(String("hangup(): hanging up foreground call"));
        hungup = Hangup(fg);
    }
    else if (bg->IsIdle(&res), !res) {
        Log(String("hangup(): hanging up background call"));
        hungup = Hangup(bg);
    }
    else {
        // No call to hang up!  This is unlikely in normal usage,
        // since the UI shouldn't be providing an "End call" button in
        // the first place.  (But it *can* happen, rarely, if an
        // active call happens to disconnect on its own right when the
        // user is trying to hang up..)
        Log(String("hangup(): no active call to hang up"));
    }
    if (DBG) Log(String("==> hungup = ") + StringUtils::ToString(hungup));

    return hungup;
}

Boolean PhoneUtils::HangupRingingCall(
    /* [in] */ ICall* ringing)
{
    if (DBG) Log(String("hangup ringing call"));

    AutoPtr<IPhone> phone;
    ringing->GetPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    ICallState state;
    ringing->GetState(&state);

    if (state == ICallState_INCOMING) {
        // Regular incoming call (with no other active calls)
        Log(String("hangupRingingCall(): regular incoming call: hangup()"));
        return Hangup(ringing);
    }
    else {
        // Unexpected state: the ringing call isn't INCOMING or
        // WAITING, so there's no reason to have called
        // hangupRingingCall() in the first place.
        // (Presumably the incoming call went away at the exact moment
        // we got here, so just do nothing.)
        Logger::W(TAG, "hangupRingingCall: no INCOMING or WAITING call");
        return FALSE;
    }
}

Boolean PhoneUtils::HangupActiveCall(
    /* [in] */ ICall* foreground)
{
    if (DBG) Log(String("hangup active call"));
    return Hangup(foreground);
}

Boolean PhoneUtils::HangupHoldingCall(
    /* [in] */ ICall* background)
{
    if (DBG) Log(String("hangup holding call"));
    return Hangup(background);
}

Boolean PhoneUtils::HangupRingingAndActive(
    /* [in] */ IPhone* phone)
{
    Boolean hungUpRingingCall = FALSE;
    Boolean hungUpFgCall = FALSE;
    AutoPtr<ICall> ringingCall;
    phone->GetRingingCall((ICall**)&ringingCall);
    AutoPtr<ICall> fgCall;
    phone->GetForegroundCall((ICall**)&fgCall);

    // Hang up any Ringing Call
    Boolean res;
    if (ringingCall->IsIdle(&res), !res) {
        Log(String("hangupRingingAndActive: Hang up Ringing Call"));
        hungUpRingingCall = HangupRingingCall(ringingCall);
    }

    // Hang up any Active Call
    if (fgCall->IsIdle(&res), !res) {
        Log(String("hangupRingingAndActive: Hang up Foreground Call"));
        hungUpFgCall = HangupActiveCall(fgCall);
    }

    return hungUpRingingCall || hungUpFgCall;
}

Boolean PhoneUtils::Hangup(
    /* [in] */ ICall* call)
{
    //try
    ECode ec = NOERROR;
    {
        AutoPtr<PhoneGlobals> globals;
        PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
        AutoPtr<ICallManager> cm = globals->mCM;

        Int32 state;
        FAIL_GOTO(ec = call->GetState(&state), ERROR)
        Boolean res;
        if (state == ICallState_ACTIVE && (cm->HasActiveBgCall(&res), res)) {
            // handle foreground call hangup while there is background call
            Log(String("- hangup(Call): hangupForegroundResumeBackground..."));
            AutoPtr<ICall> call;
            cm->GetFirstActiveBgCall((ICall**)&call);
            FAIL_GOTO(ec = cm->HangupForegroundResumeBackground(call), ERROR)
        }
        else {
            Log(String("- hangup(Call): regular hangup()..."));
            FAIL_GOTO(ec = call->Hangup(), ERROR)
        }
        return TRUE;
    }
    // catch (CallStateException ex) {
ERROR:
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
        Logger::E(TAG, "Call hangup: caught ", ec);
    }
    //}

    return FALSE;
}

ECode PhoneUtils::Hangup(
    /* [in] */ IConnection* c)
{
    //try{
    if (c != NULL) {
        ECode ec = c->Hangup();
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::W(TAG, "Connection hangup: caught ", ec);
        }
    }
    // catch (CallStateException ex) {
        //Log.w(TAG, "Connection hangup: caught " + ex, ex);
    //}
    return NOERROR;
}

Boolean PhoneUtils::AnswerAndEndHolding(
    /* [in] */ ICallManager* cm,
    /* [in] */ ICall* ringing)
{
    if (DBG) Log(String("end holding & answer waiting: 1"));
    AutoPtr<ICall> call;
    cm->GetFirstActiveBgCall((ICall**)&call);
    if (!HangupHoldingCall(call)) {
        Logger::E(TAG, "end holding failed!");
        return FALSE;
    }

    if (DBG) Log(String("end holding & answer waiting: 2"));
    return AnswerCall(ringing);
}

Boolean PhoneUtils::AnswerAndEndActive(
    /* [in] */ ICallManager* cm,
    /* [in] */ ICall* ringing)
{
    if (DBG) Log(String("answerAndEndActive()..."));

    // Unlike the answerCall() method, we *don't* need to stop the
    // ringer or change audio modes here since the user is already
    // in-call, which means that the audio mode is already set
    // correctly, and that we wouldn't have started the ringer in the
    // first place.

    // hanging up the active call also accepts the waiting call
    // while active call and waiting call are from the same phone
    // i.e. both from GSM phone
    AutoPtr<ICall> fgCall;
    cm->GetActiveFgCall((ICall**)&fgCall);
    if (!HangupActiveCall(fgCall)) {
        Logger::W(TAG, "end active call failed!");
        return FALSE;
    }

    mConnectionHandler->RemoveMessages(MSG_CHECK_STATUS_ANSWERCALL);
    AutoPtr<IMessage> msg;
    mConnectionHandler->ObtainMessage(MSG_CHECK_STATUS_ANSWERCALL, (IMessage**)&msg);
    msg->SetArg1(1);
    AutoPtr<FgRingCalls> tmp = new FgRingCalls(fgCall, ringing);
    msg->SetObj(TO_IINTERFACE(tmp));
    Boolean res;
    mConnectionHandler->SendMessage(msg, &res);

    return TRUE;
}

void PhoneUtils::UpdateCdmaCallStateOnNewOutgoingCall(
    /* [in] */ PhoneGlobals* app,
    /* [in] */ IConnection* connection)
{
    CdmaPhoneCallState::PhoneCallState state;
    if ((app->mCdmaPhoneCallState->GetCurrentCallState(&state), state) ==
        CdmaPhoneCallState::IDLE) {
        // This is the first outgoing call. Set the Phone Call State to ACTIVE
        app->mCdmaPhoneCallState->SetCurrentCallState(
            CdmaPhoneCallState::SINGLE_ACTIVE);
    }
    else {
        // This is the second outgoing call. Set the Phone Call State to 3WAY
        app->mCdmaPhoneCallState->SetCurrentCallState(
            CdmaPhoneCallState::THRWAY_ACTIVE);

        // TODO: Remove this code.
        //app.getCallModeler().setCdmaOutgoing3WayCall(connection);
    }
}

Int32 PhoneUtils::PlaceCall(
    /* [in] */ IContext* context,
    /* [in] */ IPhone* phone,
    /* [in] */ const String& number,
    /* [in] */ IUri* contactRef,
    /* [in] */ Boolean isEmergencyCall)
{
    return PlaceCall(context, phone, number, contactRef, isEmergencyCall,
            CallGatewayManager::EMPTY_INFO, NULL);
}

Int32 PhoneUtils::PlaceCall(
    /* [in] */ IContext* context,
    /* [in] */ IPhone* phone,
    /* [in] */ const String& number,
    /* [in] */ IUri* contactRef,
    /* [in] */ Boolean isEmergencyCall,
    /* [in] */ CallGatewayManager::RawGatewayInfo* gatewayInfo,
    /* [in] */ CallGatewayManager* callGateway)
{
    AutoPtr<IUri> gatewayUri = gatewayInfo->mGatewayUri;

    if (VDBG) {
        StringBuilder sb;
        sb += "placeCall()... number: '";
        sb += number;
        sb += "'";
        sb += ", GW:'";
        sb += gatewayUri;
        sb += "'";
        sb += ", contactRef:";
        sb += contactRef;
        sb += ", isEmergencyCall: ";
        sb += isEmergencyCall;
        Log(sb.ToString());
    }
    else {
        StringBuilder sb;
        sb += "placeCall()... number: ";
        sb += ToLogSafePhoneNumber(number);
        sb += ", GW: ";
        sb += gatewayUri != NULL ? "non-null" : "null";
        sb += ", emergency? ";
        sb += isEmergencyCall;
        Log(sb.ToString());
    }
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    Boolean useGateway = FALSE;
    if (NULL != gatewayUri &&
        !isEmergencyCall &&
        PhoneUtils::IsRoutableViaGateway(number)) {  // Filter out MMI, OTA and other codes.
        useGateway = TRUE;
    }

    Int32 status = CALL_STATUS_DIALED;
    AutoPtr<IConnection> connection;
    String numberToDial;
    if (useGateway) {
        // TODO: 'tel' should be a constant defined in framework base
        // somewhere (it is in webkit.)
        String scheme;
        if (NULL != gatewayUri) {
            gatewayUri->GetScheme(&scheme);
        }
        if (NULL == gatewayUri || !IPhoneAccount::SCHEME_TEL.Equals(scheme)) {
            Logger::E(TAG, "Unsupported URL: %s", TO_CSTR(gatewayUri));
            return CALL_STATUS_FAILED;
        }

        // We can use getSchemeSpecificPart because we don't allow #
        // in the gateway numbers (treated a fragment delim.) However
        // if we allow more complex gateway numbers sequence (with
        // passwords or whatnot) that use #, this may break.
        // TODO: Need to support MMI codes.
        gatewayUri->GetSchemeSpecificPart(&numberToDial);
    }
    else {
        numberToDial = number;
    }

    // Remember if the phone state was in IDLE state before this call.
    // After calling CallManager#dial(), getState() will return different state.
    Int32 state;
    app->mCM->GetState(&state);
    Boolean initiallyIdle = state == PhoneConstantsState_IDLE;

    //try {
    ECode ec  = app->mCM->Dial(phone, numberToDial, IVideoProfileVideoState::AUDIO_ONLY,
            (IConnection**)&connection);
    //} catch (CallStateException ex) {
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
        // CallStateException means a new outgoing call is not currently
        // possible: either no more call slots exist, or there's another
        // call already in the process of dialing or ringing.
        Logger::W(TAG, "Exception from app.mCM.dial() %d", ec);
        return CALL_STATUS_FAILED;

        // Note that it's possible for CallManager.dial() to return
        // null *without* throwing an exception; that indicates that
        // we dialed an MMI (see below).
    }

    Int32 phoneType;
    phone->GetPhoneType(&phoneType);

    // On GSM phones, null is returned for MMI codes
    if (NULL == connection) {
        status = CALL_STATUS_FAILED;
    }
    else {
        // Now that the call is successful, we can save the gateway info for the call
        if (callGateway != NULL) {
            callGateway->SetGatewayInfoForConnection(connection, gatewayInfo);
        }

        if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
            UpdateCdmaCallStateOnNewOutgoingCall(app, connection);
        }

        if (gatewayUri == NULL) {
            // phone.dial() succeeded: we're now in a normal phone call.
            // attach the URI to the CallerInfo Object if it is there,
            // otherwise just attach the Uri Reference.
            // if the uri does not have a "content" scheme, then we treat
            // it as if it does NOT have a unique reference.
            AutoPtr<IContentResolver> contentResolver;
            context->GetContentResolver((IContentResolver**)&contentResolver);
            String content = contentResolver->SCHEME_CONTENT;

            String scheme;
            if (contactRef != NULL) {
                contactRef->GetScheme(&scheme);
            }
            if ((contactRef != NULL) && (scheme.Equals(content))) {
                AutoPtr<IInterface> userDataObject;
                connection->GetUserData((IInterface**)&userDataObject);
                if (userDataObject == NULL) {
                    connection->SetUserData(contactRef);
                }
                else {
                    // TODO: This branch is dead code, we have
                    // just created the connection which has
                    // no user data (null) by default.
                    if (ICallerInfo::Probe(userDataObject) != NULL) {
                        ICallerInfo::Probe(userDataObject)->SetContactRefUri(contactRef);
                    }
                    else {
                        ((CallerInfoToken*)IObject::Probe(userDataObject))->mCurrentInfo
                                ->SetContactRefUri(contactRef);
                    }
                }
            }
        }

        StartGetCallerInfo(context, connection, NULL, NULL, gatewayInfo);

        SetAudioMode();

        if (DBG) Log(String("about to activate speaker"));
        // Check is phone in any dock, and turn on speaker accordingly
        Boolean speakerActivated = ActivateSpeakerIfDocked(phone);

        AutoPtr<IBluetoothManager> btManager;
        app->GetBluetoothManager((IBluetoothManager**)&btManager);

        // See also similar logic in answerCall().
        Boolean res;
        if (initiallyIdle && !speakerActivated && IsSpeakerOn(app)
                && (btManager->IsBluetoothHeadsetAudioOn(&res), !res)) {
            // This is not an error but might cause users' confusion. Add log just in case.
            Logger::I(TAG, "Forcing speaker off when initiating a new outgoing call...");
            PhoneUtils::TurnOnSpeaker(app, FALSE, TRUE);
        }
    }

    return status;
}

String PhoneUtils::ToLogSafePhoneNumber(
    /* [in] */ const String& number)
{
    // For unknown number, log empty string.
    if (number.IsNull()) {
        return String("");
    }

    if (VDBG) {
        // When VDBG is true we emit PII.
        return number;
    }

    // Do exactly same thing as Uri#toSafeString() does, which will enable us to compare
    // sanitized phone numbers.
    StringBuilder builder;
    for (Int32 i = 0; i < number.GetLength(); i++) {
        Char32 c = number.GetChar(i);
        if (c == '-' || c == '@' || c == '.') {
            builder += c;
        }
        else {
            builder += 'x';
        }
    }
    return builder.ToString();
}

ECode PhoneUtils::SendEmptyFlash(
    /* [in] */ IPhone* phone)
{
    Int32 type;
    phone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        AutoPtr<ICall> fgCall;
        phone->GetForegroundCall((ICall**)&fgCall);
        Int32 state;
        fgCall->GetState(&state);
        if (state == ICallState_ACTIVE) {
            // Send the empty flash
            if (DBG) Logger::D(TAG, "onReceive: (CDMA) sending empty flash to network");
            AutoPtr<ICall> call;
            phone->GetBackgroundCall((ICall**)&call);
            SwitchHoldingAndActive(call);
        }
    }
    return NOERROR;
}

ECode PhoneUtils::Swap()
{
    AutoPtr<PhoneGlobals> mApp;
    PhoneGlobals::GetInstance((PhoneGlobals**)&mApp);
    Boolean res;
    if (OkToSwapCalls(mApp->mCM, &res), !res) {
        // TODO: throw an error instead?
        return NOERROR;
    }

    // Swap the fg and bg calls.
    // In the future we may provide some way for user to choose among
    // multiple background calls, for now, always act on the first background call.
    AutoPtr<ICall> call;
    mApp->mCM->GetFirstActiveBgCall((ICall**)&call);
    return PhoneUtils::SwitchHoldingAndActive(call);
}

ECode PhoneUtils::SwitchHoldingAndActive(
    /* [in] */ ICall* heldCall)
{
    Log(String("switchHoldingAndActive()..."));

    ECode ec = NOERROR;
    //try
    {
        AutoPtr<PhoneGlobals> globals;
        PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
        AutoPtr<ICallManager> cm = globals->mCM;

        Boolean res;
        if (heldCall->IsIdle(&res), res) {
            // no heldCall, so it is to hold active call
            AutoPtr<IPhone> phone;
            FAIL_GOTO(ec = cm->GetFgPhone((IPhone**)&phone), ERROR)
            AutoPtr<ICall> call;
            phone->GetBackgroundCall((ICall**)&call);
            FAIL_GOTO(ec = cm->SwitchHoldingAndActive(call), ERROR)
        }
        else {
            // has particular heldCall, so to switch
            FAIL_GOTO(ec = cm->SwitchHoldingAndActive(heldCall), ERROR)
        }
        SetAudioMode(cm);
    }
    // catch (CallStateException ex) {
ERROR:
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
        Logger::W(TAG, "switchHoldingAndActive: caught %d", ec);
    }
    //}
    return NOERROR;
}

ECode PhoneUtils::MergeCalls()
{
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    return MergeCalls(globals->mCM);
}

ECode PhoneUtils::MergeCalls(
    /* [in] */ ICallManager* cm)
{
    AutoPtr<IPhone> phone;
    cm->GetFgPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);

    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        Log(String("mergeCalls(): CDMA..."));

        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        CdmaPhoneCallState::PhoneCallState state;
        if ((app->mCdmaPhoneCallState->GetCurrentCallState(&state), state)
                    == CdmaPhoneCallState::THRWAY_ACTIVE) {
            // Set the Phone Call State to conference
            app->mCdmaPhoneCallState->SetCurrentCallState(
                    CdmaPhoneCallState::CONF_CALL);

            // Send flash cmd
            // TODO: Need to change the call from switchHoldingAndActive to
            // something meaningful as we are not actually trying to swap calls but
            // instead are merging two calls by sending a Flash command.
            Log(String("- sending flash..."));
            AutoPtr<ICall> call;
            cm->GetFirstActiveBgCall((ICall**)&call);
            SwitchHoldingAndActive(call);
        }
    }
    else {
        //try {
        ECode ec;
        Log(String("mergeCalls(): calling cm.conference()..."));
        AutoPtr<ICall> call;
        FAIL_GOTO(ec = cm->GetFirstActiveBgCall((ICall**)&call), ERROR)
        FAIL_GOTO(ec = cm->Conference(call), ERROR)
        //} catch (CallStateException ex) {
ERROR:
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::W(TAG, "mergeCalls: caught %d", ec);
        }
        //}
    }
    return NOERROR;
}

ECode PhoneUtils::SeparateCall(
    /* [in] */ IConnection* c)
{
    //try
    ECode ec;
    {
        if (DBG) {
            String address;
            FAIL_GOTO(ec = c->GetAddress(&address), ERROR)
            Log(String("separateCall: ") + ToLogSafePhoneNumber(address));
        }
        FAIL_GOTO(ec = c->Separate(), ERROR)
    }
    // catch (CallStateException ex) {
ERROR:
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
        Logger::W(TAG, "separateCall: caught %d", ec);
    }
    return NOERROR;
}

AutoPtr<IDialog> PhoneUtils::DisplayMMIInitiate(
    /* [in] */ IContext* context,
    /* [in] */ IMmiCode* mmiCode,
    /* [in] */ IMessage* buttonCallbackMessage,
    /* [in] */ IDialog* previousAlert)
{
    if (DBG) Log(String("displayMMIInitiate: ") + TO_CSTR(mmiCode));
    if (previousAlert != NULL) {
        IDialogInterface::Probe(previousAlert)->Dismiss();
    }

    // The UI paradigm we are using now requests that all dialogs have
    // user interaction, and that any other messages to the user should
    // be by way of Toasts.
    //
    // In adhering to this request, all MMI initiating "OK" dialogs
    // (non-cancelable MMIs) that end up being closed when the MMI
    // completes (thereby showing a completion dialog) are being
    // replaced with Toasts.
    //
    // As a side effect, moving to Toasts for the non-cancelable MMIs
    // also means that buttonCallbackMessage (which was tied into "OK")
    // is no longer invokable for these dialogs.  This is not a problem
    // since the only callback messages we supported were for cancelable
    // MMIs anyway.
    //
    // A cancelable MMI is really just a USSD request. The term
    // "cancelable" here means that we can cancel the request when the
    // system prompts us for a response, NOT while the network is
    // processing the MMI request.  Any request to cancel a USSD while
    // the network is NOT ready for a response may be ignored.
    //
    // With this in mind, we replace the cancelable alert dialog with
    // a progress dialog, displayed until we receive a request from
    // the the network.  For more information, please see the comments
    // in the displayMMIComplete() method below.
    //
    // Anything that is NOT a USSD request is a normal MMI request,
    // which will bring up a toast (desribed above).
    Boolean res;
    Boolean isCancelable = (mmiCode != NULL) && (mmiCode->IsCancelable(&res), res);

    if (!isCancelable) {
        if (DBG) Log(String("not a USSD code, displaying status toast."));
        AutoPtr<ICharSequence> text;
        context->GetText(Elastos::Droid::TeleService::R::string::mmiStarted, (ICharSequence**)&text);

        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(context, text, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();

        return NULL;
    }
    else {
        if (DBG) Log(String("running USSD code, displaying indeterminate progress."));

        // create the indeterminate progress dialog and display it.
        AutoPtr<IProgressDialog> pd;
        CProgressDialog::New(context, (IProgressDialog**)&pd);

        AutoPtr<ICharSequence> text;
        context->GetText(Elastos::Droid::TeleService::R::string::ussdRunning, (ICharSequence**)&text);
        IAlertDialog::Probe(pd)->SetMessage(text);
        IDialog::Probe(pd)->SetCancelable(FALSE);
        pd->SetIndeterminate(TRUE);
        AutoPtr<IWindow> window;
        IDialog::Probe(pd)->GetWindow((IWindow**)&window);
        window->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

        IDialog::Probe(pd)->Show();

        return IDialog::Probe(pd);
    }
}

ECode PhoneUtils::DisplayMMIComplete(
    /* [in] */ IPhone* phone,
    /* [in] */ IContext* context,
    /* [in] */ IMmiCode* mmiCode,
    /* [in] */ IMessage* dismissCallbackMessage,
    /* [in] */ IAlertDialog* previousAlert)
{
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<ICharSequence> text;
    Int32 title = 0;  // title for the progress dialog, if needed.
    IMmiCodeState state;
    mmiCode->GetState(&state);

    if (DBG) Log(String("displayMMIComplete: state=") + StringUtils::ToString(state));

    switch (state) {
        case IMmiCodeState_PENDING:
            // USSD code asking for feedback from user.
            mmiCode->GetMessage((ICharSequence**)&text);
            if (DBG) {
                StringBuilder sb;
                sb += "- using text from PENDING MMI message: '";
                sb += text;
                sb += "'";
                Log(sb.ToString());
            }
            break;
        case IMmiCodeState_CANCELLED:
            text = NULL;
            break;
        case IMmiCodeState_COMPLETE:
        {
            AutoPtr<IActivity> activity;
            app->GetPUKEntryActivity((IActivity**)&activity);
            if (activity != NULL) {
                // if an attempt to unPUK the device was made, we specify
                // the title and the message here.
                title = Elastos::Droid::R::string::PinMmi;
                context->GetText(Elastos::Droid::TeleService::R::string::puk_unlocked, (ICharSequence**)&text);
                break;
            }
            // All other conditions for the COMPLETE mmi state will cause
            // the case to fall through to message logic in common with
            // the FAILED case.
        }
        case IMmiCodeState_FAILED:
        {
            mmiCode->GetMessage((ICharSequence**)&text);
            if (DBG) {
                StringBuilder sb;
                sb += "- using text from MMI message: '";
                sb += text;
                sb += "'";
                Log(sb.ToString());
            }
            break;
        }
        default:
            //throw new IllegalStateException("Unexpected MmiCode state: " + state);
            Logger::E("PhoneUtils", "Unexpected MmiCode state: %d", state);
            return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (previousAlert != NULL) {
        IDialogInterface::Probe(previousAlert)->Dismiss();
    }

    // Check to see if a UI exists for the PUK activation.  If it does
    // exist, then it indicates that we're trying to unblock the PUK.
    AutoPtr<IActivity> activity;
    app->GetPUKEntryActivity((IActivity**)&activity);
    if ((activity != NULL) && (state == IMmiCodeState_COMPLETE)) {
        if (DBG) Log(String("displaying PUK unblocking progress dialog."));

        // create the progress dialog, make sure the flags and type are
        // set correctly.
        AutoPtr<IProgressDialog> pd;
        CProgressDialog::New(app, (IProgressDialog**)&pd);
        IDialog::Probe(pd)->SetTitle(title);
        IAlertDialog::Probe(pd)->SetMessage(text);
        IDialog::Probe(pd)->SetCancelable(FALSE);
        pd->SetIndeterminate(TRUE);

        AutoPtr<IWindow> window;
        IDialog::Probe(pd)->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG);
        window->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

        // display the dialog
        IDialog::Probe(pd)->Show();

        // indicate to the Phone app that the progress dialog has
        // been assigned for the PUK unlock / SIM READY process.
        app->SetPukEntryProgressDialog(pd);

    }
    else {
        // In case of failure to unlock, we'll need to reset the
        // PUK unlock activity, so that the user may try again.
        AutoPtr<IActivity> activity;
        app->GetPUKEntryActivity((IActivity**)&activity);
        if (activity != NULL) {
            app->SetPukEntryActivity(NULL);
        }

        // A USSD in a pending state means that it is still
        // interacting with the user.
        if (state != IMmiCodeState_PENDING) {
            if (DBG) Log(String("MMI code has finished running."));

            if (DBG) {
                StringBuilder sb;
                sb += "Extended NW displayMMIInitiate (";
                sb += text;
                sb += "'";
                Log(sb.ToString());
            }
            Int32 length = 0;
            if (text == NULL || (text->GetLength(&length), length) == 0)
                return NOERROR;

            // displaying system alert dialog on the screen instead of
            // using another activity to display the message.  This
            // places the message at the forefront of the UI.

            if (sUssdDialog == NULL) {
                AutoPtr<IAlertDialogBuilder> builder;
                CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
                builder->SetPositiveButton(Elastos::Droid::TeleService::R::string::ok, NULL);
                builder->SetCancelable(TRUE);
                AutoPtr<IDialogInterfaceOnDismissListener> listener =
                        new MyDialogInterfaceOnDismissListener();
                builder->SetOnDismissListener(listener);
                builder->Create((IAlertDialog**)&sUssdDialog);

                AutoPtr<IWindow> window;
                IDialog::Probe(sUssdDialog)->GetWindow((IWindow**)&window);
                window->SetType(
                        IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG);
                window->AddFlags(
                        IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
            }
            if (sUssdMsg.GetLength() != 0) {
                sUssdMsg.Insert(0, String("\n"));
                AutoPtr<IResources> resources;
                app->GetResources((IResources**)&resources);
                String str;
                resources->GetString(Elastos::Droid::TeleService::R::string::ussd_dialog_sep, &str);
                sUssdMsg.Insert(0, str);
                sUssdMsg.Insert(0, String("\n"));
            }
            sUssdMsg.Insert(0, text);
            AutoPtr<ICharSequence> m = CoreUtils::Convert(sUssdMsg.ToString());
            sUssdDialog->SetMessage(m);
            IDialog::Probe(sUssdDialog)->Show();
        }
        else {
            if (DBG) Log(String("USSD code has requested user input. Constructing input dialog."));

            // USSD MMI code that is interacting with the user.  The
            // basic set of steps is this:
            //   1. User enters a USSD request
            //   2. We recognize the request and displayMMIInitiate
            //      (above) creates a progress dialog.
            //   3. Request returns and we get a PENDING or COMPLETE
            //      message.
            //   4. These MMI messages are caught in the PhoneApp
            //      (onMMIComplete) and the InCallScreen
            //      (mHandler.handleMessage) which bring up this dialog
            //      and closes the original progress dialog,
            //      respectively.
            //   5. If the message is anything other than PENDING,
            //      we are done, and the alert dialog (directly above)
            //      displays the outcome.
            //   6. If the network is requesting more information from
            //      the user, the MMI will be in a PENDING state, and
            //      we display this dialog with the message.
            //   7. User input, or cancel requests result in a return
            //      to step 1.  Keep in mind that this is the only
            //      time that a USSD should be canceled.

            // inflate the layout with the scrolling text area for the dialog.
            AutoPtr<IInterface> obj;
            context->GetSystemService(
                    IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
            AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);
            AutoPtr<IView> dialogView;
            inflater->Inflate(Elastos::Droid::TeleService::R::layout::dialog_ussd_response,
                    NULL, (IView**)&dialogView);

            // get the input field.
            AutoPtr<IView> view;
            dialogView->FindViewById(Elastos::Droid::TeleService::R::id::input_field,
                    (IView**)&view);
            AutoPtr<IEditText> inputText = IEditText::Probe(view);

            // specify the dialog's click listener, with SEND and CANCEL logic.
            AutoPtr<IDialogInterfaceOnClickListener> mUSSDDialogListener =
                    new MyDialogInterfaceOnClickListener(inputText, app, mmiCode, phone);

            // build the dialog
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
            builder->SetMessage(text);
            builder->SetView(dialogView);
            builder->SetPositiveButton(Elastos::Droid::TeleService::R::string::send_button,
                    mUSSDDialogListener);
            builder->SetNegativeButton(Elastos::Droid::TeleService::R::string::cancel,
                    mUSSDDialogListener);
            builder->SetCancelable(FALSE);
            AutoPtr<IAlertDialog> newDialog;
            builder->Create((IAlertDialog**)&newDialog);

            // attach the key listener to the dialog's input field and make
            // sure focus is set.
            AutoPtr<IViewOnKeyListener> mUSSDDialogInputListener =
                    new MyViewOnKeyListener(phone, inputText, newDialog);
            IView::Probe(inputText)->SetOnKeyListener(mUSSDDialogInputListener);
            Boolean res;
            IView::Probe(inputText)->RequestFocus(&res);

            // set the window properties of the dialog
            AutoPtr<IWindow> window;
            IDialog::Probe(newDialog)->GetWindow((IWindow**)&window);
            window->SetType(
                    IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG);
            window->AddFlags(
                    IWindowManagerLayoutParams::FLAG_DIM_BEHIND);

            // now show the dialog!
            IDialog::Probe(newDialog)->Show();
        }
    }
    return NOERROR;
}

Boolean PhoneUtils::CancelMmiCode(
    /* [in] */ IPhone* phone)
{
    AutoPtr<IList> pendingMmis;
    phone->GetPendingMmiCodes((IList**)&pendingMmis);
    Int32 count;
    pendingMmis->GetSize(&count);
    if (DBG) Log(String("cancelMmiCode: num pending MMIs = ") + StringUtils::ToString(count));

    Boolean canceled = FALSE;
    if (count > 0) {
        // assume that we only have one pending MMI operation active at a time.
        // I don't think it's possible to enter multiple MMI codes concurrently
        // in the phone UI, because during the MMI operation, an Alert panel
        // is displayed, which prevents more MMI code from being entered.
        AutoPtr<IInterface> obj;
        pendingMmis->Get(0, (IInterface**)&obj);
        AutoPtr<IMmiCode> mmiCode = IMmiCode::Probe(obj);
        Boolean res;
        if (mmiCode->IsCancelable(&res), res) {
            mmiCode->Cancel();
            canceled = TRUE;
        }
    }
    return canceled;
}

ECode PhoneUtils::GetInitialNumber(
    /* [in] */ IIntent* intent,
    /* [out] */ String* num)
{
    VALIDATE_NOT_NULL(num)

    if (DBG) Log(String("getInitialNumber(): ") + TO_CSTR(intent));

    String action;
    intent->GetAction(&action);
    if (TextUtils::IsEmpty(action)) {
        *num = NULL;
        return NOERROR;
    }

    // If the EXTRA_ACTUAL_NUMBER_TO_DIAL extra is present, get the phone
    // number from there.  (That extra takes precedence over the actual data
    // included in the intent.)
    Boolean res;
    intent->HasExtra(IOutgoingCallBroadcaster::EXTRA_ACTUAL_NUMBER_TO_DIAL, &res);
    if (res) {
        String actualNumberToDial;
        intent->GetStringExtra(IOutgoingCallBroadcaster::EXTRA_ACTUAL_NUMBER_TO_DIAL
                , &actualNumberToDial);
        if (DBG) {
            StringBuilder sb;
            sb += "==> got EXTRA_ACTUAL_NUMBER_TO_DIAL; returning '";
            sb += ToLogSafePhoneNumber(actualNumberToDial);
            sb += "'";
            Log(sb.ToString());
        }
        *num = actualNumberToDial;
        return NOERROR;
    }

    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    return GetNumberFromIntent(globals, intent, num);
}

ECode PhoneUtils::GetNumberFromIntent(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    String scheme;
    uri->GetScheme(&scheme);

    // The sip: scheme is simple: just treat the rest of the URI as a
    // SIP address.
    if (IPhoneAccount::SCHEME_SIP.Equals(scheme)) {
        return uri->GetSchemeSpecificPart(str);
    }

    // Otherwise, let PhoneNumberUtils.getNumberFromIntent() handle
    // the other cases (i.e. tel: and voicemail: and contact: URIs.)

    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    String number;
    helper->GetNumberFromIntent(intent, context, &number);

    // Check for a voicemail-dialing request.  If the voicemail number is
    // empty, throw a VoiceMailNumberMissingException.
    if (IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme) &&
            (number.IsNull() || TextUtils::IsEmpty(number))) {
        //throw new VoiceMailNumberMissingException();
        Logger::E("PhoneUtils", "VoiceMailNumberMissingException");
        assert(0);
        //return VoiceMailNumberMissingException;
    }

    *str = number;
    return NOERROR;
}

AutoPtr<ICallerInfo> PhoneUtils::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IConnection* c)
{
    AutoPtr<ICallerInfo> info;

    if (c != NULL) {
        //See if there is a URI attached.  If there is, this means
        //that there is no CallerInfo queried yet, so we'll need to
        //replace the URI with a full CallerInfo object.
        AutoPtr<IInterface> userDataObject;
        c->GetUserData((IInterface**)&userDataObject);
        if (IUri::Probe(userDataObject) != NULL) {
            AutoPtr<ICallerInfoHelper> helper;
            CCallerInfoHelper::AcquireSingleton((ICallerInfoHelper**)&helper);
            helper->GetCallerInfo(context, IUri::Probe(userDataObject), (ICallerInfo**)&info);
            if (info != NULL) {
                c->SetUserData(info);
            }
        }
        else {
            if (IPhoneUtilsCallerInfoToken::Probe(userDataObject) != NULL) {
                //temporary result, while query is running
                AutoPtr<IPhoneUtilsCallerInfoToken> tmp = IPhoneUtilsCallerInfoToken::Probe(userDataObject);
                info = ((CallerInfoToken*)tmp.Get())->mCurrentInfo;
            }
            else {
                //final query result
                info = ICallerInfo::Probe(userDataObject);
            }
            if (info == NULL) {
                // No URI, or Existing CallerInfo, so we'll have to make do with
                // querying a new CallerInfo using the connection's phone number.
                String number;
                c->GetAddress(&number);

                if (DBG) Log(String("getCallerInfo: number = ") + ToLogSafePhoneNumber(number));

                if (!TextUtils::IsEmpty(number)) {
                    AutoPtr<ICallerInfoHelper> helper;
                    CCallerInfoHelper::AcquireSingleton((ICallerInfoHelper**)&helper);
                    helper->GetCallerInfo(context, number, (ICallerInfo**)&info);
                    if (info != NULL) {
                        c->SetUserData(info);
                    }
                }
            }
        }
    }
    return info;
}

ECode PhoneUtils::StartGetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ ICall* call,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ CallerInfoToken** token)
{
    VALIDATE_NOT_NULL(token)

    AutoPtr<IConnection> conn;
    AutoPtr<IPhone> phone;
    call->GetPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        call->GetLatestConnection((IConnection**)&conn);
    }
    else if ((phoneType == IPhoneConstants::PHONE_TYPE_GSM)
            || (phoneType == IPhoneConstants::PHONE_TYPE_SIP)
            || (phoneType == IPhoneConstants::PHONE_TYPE_IMS)
            || (phoneType == IPhoneConstants::PHONE_TYPE_THIRD_PARTY)) {
        call->GetEarliestConnection((IConnection**)&conn);
    }
    else {
        //throw new IllegalStateException("Unexpected phone type: " + phoneType);
        Logger::E("PhoneUtils", "Unexpected phone type: %d", phoneType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<CallerInfoToken> t = StartGetCallerInfo(context, conn, listener, cookie);
    *token = t;
    REFCOUNT_ADD(*token);
    return NOERROR;
}

AutoPtr<PhoneUtils::CallerInfoToken> PhoneUtils::StartGetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IConnection* c,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie)
{
    return StartGetCallerInfo(context, c, listener, cookie, NULL);
}

AutoPtr<PhoneUtils::CallerInfoToken> PhoneUtils::StartGetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IConnection* c,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [in] */ CallGatewayManager::RawGatewayInfo* info)
{
    AutoPtr<CallerInfoToken> cit;

    if (c == NULL) {
        //TODO: perhaps throw an exception here.
        cit = new CallerInfoToken();
        cit->mAsyncQuery = NULL;
        return cit;
    }

    AutoPtr<IInterface> userDataObject;
    c->GetUserData((IInterface**)&userDataObject);

    // There are now 3 states for the Connection's userData object:
    //
    //   (1) Uri - query has not been executed yet
    //
    //   (2) CallerInfoToken - query is executing, but has not completed.
    //
    //   (3) CallerInfo - query has executed.
    //
    // In each case we have slightly different behaviour:
    //   1. If the query has not been executed yet (Uri or null), we start
    //      query execution asynchronously, and note it by attaching a
    //      CallerInfoToken as the userData.
    //   2. If the query is executing (CallerInfoToken), we've essentially
    //      reached a state where we've received multiple requests for the
    //      same callerInfo.  That means that once the query is complete,
    //      we'll need to execute the additional listener requested.
    //   3. If the query has already been executed (CallerInfo), we just
    //      return the CallerInfo object as expected.
    //   4. Regarding isFinal - there are cases where the CallerInfo object
    //      will not be attached, like when the number is empty (caller id
    //      blocking).  This flag is used to indicate that the
    //      CallerInfoToken object is going to be permanent since no
    //      query results will be returned.  In the case where a query
    //      has been completed, this flag is used to indicate to the caller
    //      that the data will not be updated since it is valid.
    //
    //      Note: For the case where a number is NOT retrievable, we leave
    //      the CallerInfo as null in the CallerInfoToken.  This is
    //      something of a departure from the original code, since the old
    //      code manufactured a CallerInfo object regardless of the query
    //      outcome.  From now on, we will append an empty CallerInfo
    //      object, to mirror previous behaviour, and to avoid Null Pointer
    //      Exceptions.

    if (IUri::Probe(userDataObject) != NULL) {
        // State (1): query has not been executed yet

        //create a dummy callerinfo, populate with what we know from URI.
        cit = new CallerInfoToken();
        CCallerInfo::New((ICallerInfo**)&(cit->mCurrentInfo));

        AutoPtr<ICallerInfoAsyncQueryHelper> helper;
        CCallerInfoAsyncQueryHelper::AcquireSingleton((ICallerInfoAsyncQueryHelper**)&helper);

        helper->StartQuery(QUERY_TOKEN, context,
                IUri::Probe(userDataObject), sCallerInfoQueryListener, c,
                (ICallerInfoAsyncQuery**)&(cit->mAsyncQuery));

        cit->mAsyncQuery->AddQueryListener(QUERY_TOKEN, listener, cookie);
        cit->mIsFinal = FALSE;

        c->SetUserData(TO_IINTERFACE(cit));

        if (DBG) Log(String("startGetCallerInfo: query based on Uri: ") + TO_CSTR(userDataObject));

    }
    else if (userDataObject == NULL) {
        // No URI, or Existing CallerInfo, so we'll have to make do with
        // querying a new CallerInfo using the connection's phone number.
        String number;
        c->GetAddress(&number);

        if (info != NULL && info != CallGatewayManager::EMPTY_INFO) {
            // Gateway number, the connection number is actually the gateway number.
            // need to lookup via dialed number.
            number = info->mTrueNumber;
        }

        if (DBG) {
            Log(String("PhoneUtils.startGetCallerInfo: new query for phone number..."));
            Log(String("- number (address): ") + ToLogSafePhoneNumber(number));
            Log(String("- c: ") + TO_CSTR(c));
            AutoPtr<ICall> call;
            c->GetCall((ICall**)&call);
            AutoPtr<IPhone> phone;
            call->GetPhone((IPhone**)&phone);
            Log(String("- phone: ") + TO_CSTR(phone));
            Int32 phoneType;
            phone->GetPhoneType(&phoneType);
            Log(String("- phoneType: ") + StringUtils::ToString(phoneType));
            switch (phoneType) {
                case IPhoneConstants::PHONE_TYPE_NONE: Log(String("  ==> PHONE_TYPE_NONE")); break;
                case IPhoneConstants::PHONE_TYPE_GSM: Log(String("  ==> PHONE_TYPE_GSM")); break;
                case IPhoneConstants::PHONE_TYPE_IMS: Log(String("  ==> PHONE_TYPE_IMS")); break;
                case IPhoneConstants::PHONE_TYPE_CDMA: Log(String("  ==> PHONE_TYPE_CDMA")); break;
                case IPhoneConstants::PHONE_TYPE_SIP: Log(String("  ==> PHONE_TYPE_SIP")); break;
                case IPhoneConstants::PHONE_TYPE_THIRD_PARTY:
                    Log(String("  ==> PHONE_TYPE_THIRD_PARTY"));
                    break;
                default: Log(String("  ==> Unknown phone type")); break;
            }
        }

        cit = new CallerInfoToken();
        CCallerInfo::New((ICallerInfo**)&(cit->mCurrentInfo));

        // Store CNAP information retrieved from the Connection (we want to do this
        // here regardless of whether the number is empty or not).
        String name;
        c->GetCnapName(&name);
        cit->mCurrentInfo->SetCnapName(name);

        cit->mCurrentInfo->SetName(name); // This can still get overwritten
                                                         // by ContactInfo later

        Int32 presentation;
        c->GetNumberPresentation(&presentation);
        cit->mCurrentInfo->SetNumberPresentation(presentation);

        Int32 presentation2;
        c->GetCnapNamePresentation(&presentation2);
        cit->mCurrentInfo->SetNamePresentation(presentation2);

        if (VDBG) {
            Log(String("startGetCallerInfo: number = ") + number);

            StringBuilder sb;
            sb += "startGetCallerInfo: CNAP Info from FW(1): name=";
            String name;
            cit->mCurrentInfo->GetCnapName(&name);
            sb += name;
            sb += ", Name/Number Pres=";
            Int32 presentation;
            cit->mCurrentInfo->GetNumberPresentation(&presentation);
            sb +=  presentation;
            Log(sb.ToString());
        }

        // handling case where number is null (caller id hidden) as well.
        if (!TextUtils::IsEmpty(number)) {
            // Check for special CNAP cases and modify the CallerInfo accordingly
            // to be sure we keep the right information to display/log later
            Int32 presentation;
            cit->mCurrentInfo->GetNumberPresentation(&presentation);

            number = ModifyForSpecialCnapCases(context, cit->mCurrentInfo, number,
                    presentation);

            cit->mCurrentInfo->SetPhoneNumber(number);
            // For scenarios where we may receive a valid number from the network but a
            // restricted/unavailable presentation, we do not want to perform a contact query
            // (see note on isFinal above). So we set isFinal to true here as well.
            if (presentation != IPhoneConstants::PRESENTATION_ALLOWED) {
                cit->mIsFinal = TRUE;
            }
            else {
                if (DBG) Log(String("==> Actually starting CallerInfoAsyncQuery.startQuery()..."));

                AutoPtr<ICallerInfoAsyncQueryHelper> helper;
                CCallerInfoAsyncQueryHelper::AcquireSingleton((ICallerInfoAsyncQueryHelper**)&helper);

                helper->StartQuery(QUERY_TOKEN, context,
                        number, sCallerInfoQueryListener, c, (ICallerInfoAsyncQuery**)&(cit->mAsyncQuery));
                cit->mAsyncQuery->AddQueryListener(QUERY_TOKEN, listener, cookie);
                cit->mIsFinal = FALSE;
            }
        }
        else {
            // This is the case where we are querying on a number that
            // is null or empty, like a caller whose caller id is
            // blocked or empty (CLIR).  The previous behaviour was to
            // throw a null CallerInfo object back to the user, but
            // this departure is somewhat cleaner.
            if (DBG) Log(String("startGetCallerInfo: No query to start, send trivial reply."));
            cit->mIsFinal = TRUE; // please see note on isFinal, above.
        }

        c->SetUserData(TO_IINTERFACE(cit));

        if (DBG) {
            Log(String("startGetCallerInfo: query based on number: ") + ToLogSafePhoneNumber(number));
        }

    }
    else if (IPhoneUtilsCallerInfoToken::Probe(userDataObject) != NULL) {
        // State (2): query is executing, but has not completed.

        // just tack on this listener to the queue.
        AutoPtr<IPhoneUtilsCallerInfoToken> tmp = IPhoneUtilsCallerInfoToken::Probe(userDataObject);
        cit = (CallerInfoToken*)tmp.Get();

        // handling case where number is null (caller id hidden) as well.
        if (cit->mAsyncQuery != NULL) {
            cit->mAsyncQuery->AddQueryListener(QUERY_TOKEN, listener, cookie);

            // if (DBG) Log(String("startGetCallerInfo: query already running, adding listener: ") +
            //         listener.getClass().toString());
        }
        else {
            // handling case where number/name gets updated later on by the network
            String updatedNumber;
            c->GetAddress(&updatedNumber);

            if (info != NULL) {
                // Gateway number, the connection number is actually the gateway number.
                // need to lookup via dialed number.
                updatedNumber = info->mTrueNumber;
            }

            if (DBG) {
                Log(String("startGetCallerInfo: updatedNumber initially = ")
                        + ToLogSafePhoneNumber(updatedNumber));
            }
            if (!TextUtils::IsEmpty(updatedNumber)) {
                // Store CNAP information retrieved from the Connection
                String name;
                c->GetCnapName(&name);
                cit->mCurrentInfo->SetCnapName(name);

                // This can still get overwritten by ContactInfo
                cit->mCurrentInfo->SetName(name);

                Int32 npresentation;
                c->GetNumberPresentation(&npresentation);
                cit->mCurrentInfo->SetNumberPresentation(npresentation);

                Int32 cpresentation;
                c->GetCnapNamePresentation(&cpresentation);
                cit->mCurrentInfo->SetNamePresentation(cpresentation);

                updatedNumber = ModifyForSpecialCnapCases(context, cit->mCurrentInfo,
                        updatedNumber, npresentation);

                cit->mCurrentInfo->SetPhoneNumber(updatedNumber);
                if (DBG) {
                    Log(String("startGetCallerInfo: updatedNumber=")
                            + ToLogSafePhoneNumber(updatedNumber));
                }
                if (VDBG) {
                    StringBuilder sb;
                    sb += "startGetCallerInfo: CNAP Info from FW(2): name=";
                    sb += name;
                    sb += ", Name/Number Pres=";
                    sb += npresentation;
                    Log(sb.ToString());
                }
                else if (DBG) {
                    Log(String("startGetCallerInfo: CNAP Info from FW(2)"));
                }
                // For scenarios where we may receive a valid number from the network but a
                // restricted/unavailable presentation, we do not want to perform a contact query
                // (see note on isFinal above). So we set isFinal to true here as well.
                if (npresentation != IPhoneConstants::PRESENTATION_ALLOWED) {
                    cit->mIsFinal = TRUE;
                }
                else {
                    AutoPtr<ICallerInfoAsyncQueryHelper> helper;
                    CCallerInfoAsyncQueryHelper::AcquireSingleton((ICallerInfoAsyncQueryHelper**)&helper);

                    helper->StartQuery(QUERY_TOKEN, context,
                            updatedNumber, sCallerInfoQueryListener, c,
                            (ICallerInfoAsyncQuery**)&(cit->mAsyncQuery));
                    cit->mAsyncQuery->AddQueryListener(QUERY_TOKEN, listener, cookie);
                    cit->mIsFinal = FALSE;
                }
            }
            else {
                if (DBG) Log(String("startGetCallerInfo: No query to attach to, send trivial reply."));
                if (cit->mCurrentInfo == NULL) {
                    CCallerInfo::New((ICallerInfo**)&(cit->mCurrentInfo));
                }
                // Store CNAP information retrieved from the Connection
                String name;
                c->GetCnapName(&name);
                cit->mCurrentInfo->SetCnapName(name);  // This can still get
                                                             // overwritten by ContactInfo
                cit->mCurrentInfo->SetName(name);

                Int32 npresentation;
                c->GetNumberPresentation(&npresentation);
                cit->mCurrentInfo->SetNumberPresentation(npresentation);

                Int32 cpresentation;
                c->GetCnapNamePresentation(&cpresentation);
                cit->mCurrentInfo->SetNamePresentation(cpresentation);

                if (VDBG) {
                    StringBuilder sb;
                    sb += "startGetCallerInfo: CNAP Info from FW(3): name=";
                    sb += name;
                    sb += ", Name/Number Pres=";
                    sb += npresentation;
                    Log(sb.ToString());
                }
                else if (DBG) {
                    Log(String("startGetCallerInfo: CNAP Info from FW(3)"));
                }
                cit->mIsFinal = TRUE; // please see note on isFinal, above.
            }
        }
    }
    else {
        // State (3): query is complete.

        // The connection's userDataObject is a full-fledged
        // CallerInfo instance.  Wrap it in a CallerInfoToken and
        // return it to the user.

        cit = new CallerInfoToken();
        cit->mCurrentInfo = ICallerInfo::Probe(userDataObject);
        cit->mAsyncQuery = NULL;
        cit->mIsFinal = TRUE;
        // since the query is already done, call the listener.
        if (DBG) Log(String("startGetCallerInfo: query already done, returning CallerInfo"));
        if (DBG) Log(String("==> cit.currentInfo = ") + TO_CSTR(cit->mCurrentInfo));
    }
    return cit;
}

String PhoneUtils::GetCompactNameFromCallerInfo(
    /* [in] */ ICallerInfo* ci,
    /* [in] */ IContext* context)
{
    if (DBG) Log(String("getCompactNameFromCallerInfo: info = ") + TO_CSTR(ci));

    String compactName;
    Int32 npresentation;
    ci->GetNumberPresentation(&npresentation);
    if (ci != NULL) {
        String name;
        ci->GetName(&name);
        if (TextUtils::IsEmpty(name)) {
            // Perform any modifications for special CNAP cases to
            // the phone number being displayed, if applicable.
            String number;
            ci->GetPhoneNumber(&number);

            compactName = ModifyForSpecialCnapCases(context, ci, number, npresentation);
        }
        else {
            // Don't call modifyForSpecialCnapCases on regular name. See b/2160795.
            compactName = name;
        }
    }

    if ((compactName.IsNull()) || (TextUtils::IsEmpty(compactName))) {
        // If we're still null/empty here, then check if we have a presentation
        // string that takes precedence that we could return, otherwise display
        // "unknown" string.
        if (ci != NULL && npresentation == IPhoneConstants::PRESENTATION_RESTRICTED) {
            context->GetString(Elastos::Droid::TeleService::R::string::private_num, &compactName);
        }
        else if (ci != NULL && npresentation == IPhoneConstants::PRESENTATION_PAYPHONE) {
            context->GetString(Elastos::Droid::TeleService::R::string::payphone, &compactName);
        }
        else {
            context->GetString(Elastos::Droid::TeleService::R::string::unknown, &compactName);
        }
    }
    if (VDBG) Log(String("getCompactNameFromCallerInfo: compactName=") + compactName);
    return compactName;
}

Boolean PhoneUtils::IsConferenceCall(
    /* [in] */ ICall* call)
{
    // CDMA phones don't have the same concept of "conference call" as
    // GSM phones do; there's no special "conference call" state of
    // the UI or a "manage conference" function.  (Instead, when
    // you're in a 3-way call, all we can do is display the "generic"
    // state of the UI.)  So as far as the in-call UI is concerned,
    // Conference corresponds to generic display.
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    AutoPtr<IPhone> phone;
    call->GetPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        CdmaPhoneCallState::PhoneCallState state;
        app->mCdmaPhoneCallState->GetCurrentCallState(&state);
        Boolean res;
        if ((state == CdmaPhoneCallState::CONF_CALL)
                || ((state == CdmaPhoneCallState::THRWAY_ACTIVE)
                && (app->mCdmaPhoneCallState->IsThreeWayCallOrigStateDialing(&res), !res))) {
            return TRUE;
        }
    }
    else {
        AutoPtr<IList> connections;
        call->GetConnections((IList**)&connections);
        Int32 size;
        if (connections != NULL && (connections->GetSize(&size), size) > 1) {
            return TRUE;
        }
    }
    return FALSE;

    // TODO: We may still want to change the semantics of this method
    // to say that a given call is only really a conference call if
    // the number of ACTIVE connections, not the total number of
    // connections, is greater than one.  (See warning comment in the
    // javadoc above.)
    // Here's an implementation of that:
    //        if (connections == null) {
    //            return false;
    //        }
    //        int numActiveConnections = 0;
    //        for (Connection conn : connections) {
    //            if (DBG) log("  - CONN: " + conn + ", state = " + conn.getState());
    //            if (conn.getState() == Call.State.ACTIVE) numActiveConnections++;
    //            if (numActiveConnections > 1) {
    //                return true;
    //            }
    //        }
    //        return false;
}

Boolean PhoneUtils::StartNewCall(
    /* [in] */ ICallManager* cm)
{
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    // Sanity-check that this is OK given the current state of the phone.
    Boolean res;
    if (OkToAddCall(cm, &res), !res) {
        Logger::W(TAG, "startNewCall: can't add a new call in the current state");
        DumpCallManager();
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_DIAL, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    // when we request the dialer come up, we also want to inform
    // it that we're going through the "add call" option from the
    // InCallScreen / PhoneUtils.
    intent->PutExtra(ADD_CALL_MODE_KEY, TRUE);
    //try {
    ECode ec = app->StartActivity(intent);
    //} catch (ActivityNotFoundException e) {
    if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        // This is rather rare but possible.
        // Note: this method is used even when the phone is encrypted. At that moment
        // the system may not find any Activity which can accept this Intent.
        Logger::E(TAG, "Activity for adding calls isn't found.");
        return FALSE;
    }

    return TRUE;
}

ECode PhoneUtils::TurnOnSpeaker(
    /* [in] */ IContext* context,
    /* [in] */ Boolean flag,
    /* [in] */ Boolean store)
{
    if (DBG) {
        StringBuilder sb;
        sb += "turnOnSpeaker(flag=";
        sb += flag;
        sb += ", store=";
        sb += store;
        sb += ")...";
        Log(sb.ToString());
    }
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    audioManager->SetSpeakerphoneOn(flag);

    // record the speaker-enable value
    if (store) {
        sIsSpeakerEnabled = flag;
    }

    // We also need to make a fresh call to PhoneApp.updateWakeState()
    // any time the speaker state changes, since the screen timeout is
    // sometimes different depending on whether or not the speaker is
    // in use.
    app->UpdateWakeState();

    return app->mCM->SetEchoSuppressionEnabled();
}

ECode PhoneUtils::RestoreSpeakerMode(
    /* [in] */ IContext* context)
{
    if (DBG) Log(String("restoreSpeakerMode, restoring to: ") + StringUtils::ToString(sIsSpeakerEnabled));

    // change the mode if needed.
    if (IsSpeakerOn(context) != sIsSpeakerEnabled) {
        TurnOnSpeaker(context, sIsSpeakerEnabled, FALSE);
    }
    return NOERROR;
}

Boolean PhoneUtils::IsSpeakerOn(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);

    Boolean res;
    audioManager->IsSpeakerphoneOn(&res);
    return res;
}

ECode PhoneUtils::TurnOnNoiseSuppression(
    /* [in] */ IContext* context,
    /* [in] */ Boolean flag,
    /* [in] */ Boolean store)
{
    if (DBG) Log(String("turnOnNoiseSuppression: ") + StringUtils::ToString(flag));

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    if (resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::has_in_call_noise_suppression, &res), !res) {
        return NOERROR;
    }

    if (flag) {
        audioManager->SetParameters(String("noise_suppression=auto"));
    }
    else {
        audioManager->SetParameters(String("noise_suppression=off"));
    }

    // record the speaker-enable value
    if (store) {
        sIsNoiseSuppressionEnabled = flag;
    }

    // TODO: implement and manage ICON
    return NOERROR;
}

ECode PhoneUtils::RestoreNoiseSuppression(
    /* [in] */ IContext* context)
{
    if (DBG) Log(String("restoreNoiseSuppression, restoring to: ") +
            StringUtils::ToString(sIsNoiseSuppressionEnabled));

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    if (resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::has_in_call_noise_suppression, &res), !res) {
        return NOERROR;
    }

    // change the mode if needed.
    if (IsNoiseSuppressionOn(context) != sIsNoiseSuppressionEnabled) {
        TurnOnNoiseSuppression(context, sIsNoiseSuppressionEnabled, FALSE);
    }
    return NOERROR;
}

Boolean PhoneUtils::IsNoiseSuppressionOn(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    if (resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::has_in_call_noise_suppression, &res), !res) {
        return FALSE;
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);

    String noiseSuppression;
    audioManager->GetParameters(String("noise_suppression"), &noiseSuppression);
    if (DBG) Log(String("isNoiseSuppressionOn: ") + noiseSuppression);
    if (noiseSuppression.Contains("off")) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

Boolean PhoneUtils::IsInEmergencyCall(
    /* [in] */ ICallManager* cm)
{
    AutoPtr<ICall> call;
    cm->GetActiveFgCall((ICall**)&call);
    AutoPtr<IList> list;
    call->GetConnections((IList**)&list);

    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> cn = IConnection::Probe(obj);

        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        String address;
        cn->GetAddress(&address);

        AutoPtr<IPhoneNumberUtils> helper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
        Boolean res;
        if (helper->IsLocalEmergencyNumber(app, address, &res), res) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean PhoneUtils::GetMute()
{
    return FALSE;
}

ECode PhoneUtils::SetAudioMode()
{
    return NOERROR;
}

ECode PhoneUtils::SetAudioMode(
    /* [in] */ ICallManager* cm)
{
    return NOERROR;
}

Boolean PhoneUtils::HasDisconnectedConnections(
    /* [in] */ IPhone* phone)
{
    AutoPtr<ICall> fcall;
    AutoPtr<ICall> bcall;
    AutoPtr<ICall> rcall;
    phone->GetForegroundCall((ICall**)&fcall);
    phone->GetBackgroundCall((ICall**)&bcall);
    phone->GetRingingCall((ICall**)&rcall);

    return HasDisconnectedConnections(fcall) ||
            HasDisconnectedConnections(bcall) ||
            HasDisconnectedConnections(rcall);
}

Boolean PhoneUtils::HasDisconnectedConnections(
    /* [in] */ ICall* call)
{
    // look through all connections for non-active ones.
    AutoPtr<IList> list;
    call->GetConnections((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> cn = IConnection::Probe(obj);

        Boolean res;
        if (cn->IsAlive(&res), !res) {
            return TRUE;
        }

    }
    return FALSE;
}

Boolean PhoneUtils::OkToHoldCall(
    /* [in] */ ICallManager* cm)
{
    AutoPtr<ICall> fgCall;
    cm->GetActiveFgCall((ICall**)&fgCall);
    Boolean hasHoldingCall;
    cm->HasActiveBgCall(&hasHoldingCall);
    ICallState fgCallState;
    fgCall->GetState(&fgCallState);

    // The "Hold" control is disabled entirely if there's
    // no way to either hold or unhold in the current state.
    Boolean okToHold = (fgCallState == ICallState_ACTIVE) && !hasHoldingCall;
    Boolean res;
    Boolean okToUnhold = (cm->HasActiveBgCall(&res), res) && (fgCallState == ICallState_IDLE);
    Boolean canHold = okToHold || okToUnhold;

    return canHold;
}

Boolean PhoneUtils::OkToSupportHold(
    /* [in] */ ICallManager* cm)
{
    Boolean supportsHold = FALSE;

    AutoPtr<ICall> fgCall;
    cm->GetActiveFgCall((ICall**)&fgCall);
    Boolean hasHoldingCall;
    cm->HasActiveBgCall(&hasHoldingCall);
    ICallState fgCallState;
    fgCall->GetState(&fgCallState);

    AutoPtr<IPhone> phone;
    fgCall->GetPhone((IPhone**)&phone);
    AutoPtr<ITelephonyCapabilities> helper;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
    Boolean res;
    if (helper->SupportsHoldAndUnhold(phone, &res), res) {
        // This phone has the concept of explicit "Hold" and "Unhold" actions.
        supportsHold = TRUE;
    }
    else if (hasHoldingCall && (fgCallState == ICallState_IDLE)) {
        // Even when foreground phone device doesn't support hold/unhold, phone devices
        // for background holding calls may do.
        AutoPtr<ICall> bgCall;
        cm->GetFirstActiveBgCall((ICall**)&bgCall);
        if (bgCall != NULL) {
            AutoPtr<IPhone> phone;
            bgCall->GetPhone((IPhone**)&phone);
            if (helper->SupportsHoldAndUnhold(phone, &res), res) {
                supportsHold = TRUE;
            }
        }
    }
    return supportsHold;
}

ECode PhoneUtils::OkToSwapCalls(
    /* [in] */ ICallManager* cm,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IPhone> phone;
    cm->GetDefaultPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        // CDMA: "Swap" is enabled only when the phone reaches a *generic*.
        // state by either accepting a Call Waiting or by merging two calls
        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        CdmaPhoneCallState::PhoneCallState state;
        app->mCdmaPhoneCallState->GetCurrentCallState(&state);
        *result = (state == CdmaPhoneCallState::CONF_CALL);
        return NOERROR;
    }
    else if ((phoneType == IPhoneConstants::PHONE_TYPE_GSM)
            || (phoneType == IPhoneConstants::PHONE_TYPE_SIP)
            || (phoneType == IPhoneConstants::PHONE_TYPE_IMS)
            || (phoneType == IPhoneConstants::PHONE_TYPE_THIRD_PARTY)) {
        // GSM: "Swap" is available if both lines are in use and there's no
        // incoming call.  (Actually we need to verify that the active
        // call really is in the ACTIVE state and the holding call really
        // is in the HOLDING state, since you *can't* actually swap calls
        // when the foreground call is DIALING or ALERTING.)

        Boolean res;
        if ((cm->HasActiveRingingCall(&res), !res)) {
            AutoPtr<ICall> call;
            cm->GetActiveFgCall((ICall**)&call);
            Int32 state;
            call->GetState(&state);
            if (state == ICallState_ACTIVE) {
                AutoPtr<ICall> call;
                cm->GetFirstActiveBgCall((ICall**)&call);
                Int32 state;
                call->GetState(&state);
                if (state == ICallState_HOLDING) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
        *result = FALSE;
        return NOERROR;
    }
    else {
        //throw new IllegalStateException("Unexpected phone type: " + phoneType);
        Logger::E("PhoneUtils", "Unexpected phone type: %d", phoneType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Boolean PhoneUtils::OkToMergeCalls(
    /* [in] */ ICallManager* cm)
{
    AutoPtr<IPhone> phone;
    cm->GetFgPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        // CDMA: "Merge" is enabled only when the user is in a 3Way call.
        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        CdmaPhoneCallState::PhoneCallState state;
        app->mCdmaPhoneCallState->GetCurrentCallState(&state);
        Boolean res;
        return ((state == CdmaPhoneCallState::THRWAY_ACTIVE)
                && (app->mCdmaPhoneCallState->IsThreeWayCallOrigStateDialing(&res), !res));
    }
    else {
        // GSM: "Merge" is available if both lines are in use and there's no
        // incoming call, *and* the current conference isn't already
        // "full".
        // TODO: shall move all okToMerge logic to CallManager
        Boolean res1,res2,res3,res4;
        AutoPtr<ICall> call;
        cm->GetFirstActiveBgCall((ICall**)&call);
        return (cm->HasActiveRingingCall(&res1), !res1) && (cm->HasActiveFgCall(&res2), res2)
                && (cm->HasActiveBgCall(&res3), res3)
                && (cm->CanConference(call, &res4), res4);
    }
}

ECode PhoneUtils::OkToAddCall(
    /* [in] */ ICallManager* cm,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ICall> call;
    cm->GetActiveFgCall((ICall**)&call);
    AutoPtr<IPhone> phone;
    call->GetPhone((IPhone**)&phone);

    // "Add call" is never allowed in emergency callback mode (ECM).
    if (IsPhoneInEcm(phone)) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    ICallState fgCallState;
    call->GetState(&fgCallState);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        // CDMA: "Add call" button is only enabled when:
        // - ForegroundCall is in ACTIVE state
        // - After 30 seconds of user Ignoring/Missing a Call Waiting call.
        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        Boolean res;
        *result = ((fgCallState == ICallState_ACTIVE)
                && (app->mCdmaPhoneCallState->GetAddCallMenuStateAfterCallWaiting(&res), res));
        return NOERROR;
    }
    else if ((phoneType == IPhoneConstants::PHONE_TYPE_GSM)
            || (phoneType == IPhoneConstants::PHONE_TYPE_SIP)
            || (phoneType == IPhoneConstants::PHONE_TYPE_IMS)
            || (phoneType == IPhoneConstants::PHONE_TYPE_THIRD_PARTY)) {
        // GSM: "Add call" is available only if ALL of the following are true:
        // - There's no incoming ringing call
        // - There's < 2 lines in use
        // - The foreground call is ACTIVE or IDLE or DISCONNECTED.
        //   (We mainly need to make sure it *isn't* DIALING or ALERTING.)
        Boolean hasRingingCall;
        cm->HasActiveRingingCall(&hasRingingCall);
        Boolean hasActiveCall;
        cm->HasActiveFgCall(&hasActiveCall);
        Boolean hasHoldingCall;
        cm->HasActiveBgCall(&hasHoldingCall);
        Boolean allLinesTaken = hasActiveCall && hasHoldingCall;

        *result = !hasRingingCall
                && !allLinesTaken
                && ((fgCallState == ICallState_ACTIVE)
                    || (fgCallState == ICallState_IDLE)
                    || (fgCallState == ICallState_DISCONNECTED));
        return NOERROR;
    }
    else {
        //throw new IllegalStateException("Unexpected phone type: " + phoneType);
        Logger::E("PhoneUtils", "Unexpected phone type: %d", phoneType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Int32 PhoneUtils::CheckCnapSpecialCases(
    /* [in] */ const String& n)
{
    if (n.Equals(String("PRIVATE")) ||
            n.Equals(String("P")) ||
            n.Equals(String("RES"))) {
        if (DBG) Log(String("checkCnapSpecialCases, PRIVATE string: ") + n);
        return IPhoneConstants::PRESENTATION_RESTRICTED;
    }
    else if (n.Equals(String("UNAVAILABLE")) ||
            n.Equals(String("UNKNOWN")) ||
            n.Equals(String("UNA")) ||
            n.Equals(String("U"))) {
        if (DBG) Log(String("checkCnapSpecialCases, UNKNOWN string: ") + n);
        return IPhoneConstants::PRESENTATION_UNKNOWN;
    }
    else {
        if (DBG) Log(String("checkCnapSpecialCases, normal str. number: ") + n);
        return CNAP_SPECIAL_CASE_NO;
    }
}

String PhoneUtils::ModifyForSpecialCnapCases(
    /* [in] */ IContext* context,
    /* [in] */ ICallerInfo* ci,
    /* [in] */ const String& _number,
    /* [in] */ Int32 presentation)
{
    String number = _number;
    // Obviously we return number if ci == null, but still return number if
    // number == null, because in these cases the correct string will still be
    // displayed/logged after this function returns based on the presentation value.
    if (ci == NULL || number.IsNull()) return number;

    if (DBG) {
        StringBuilder sb;
        sb += "modifyForSpecialCnapCases: initially, number=";
        sb += ToLogSafePhoneNumber(number);
        sb += ", presentation=";
        sb += presentation;
        sb += " ci ";
        sb += TO_CSTR(ci);
        Log(sb.ToString());
    }

    // "ABSENT NUMBER" is a possible value we could get from the network as the
    // phone number, so if this happens, change it to "Unknown" in the CallerInfo
    // and fix the presentation to be the same.
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ArrayOf<String> > absentNumberValues;
    resources->GetStringArray(Elastos::Droid::TeleService::R::array::absent_num,
            (ArrayOf<String>**)&absentNumberValues);

    AutoPtr<IList> outList;
    Arrays::AsList(absentNumberValues, (IList**)&outList);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(number);
    Boolean res;
    outList->Contains(TO_IINTERFACE(cchar), &res);
    if (res && presentation == IPhoneConstants::PRESENTATION_ALLOWED) {
        context->GetString(Elastos::Droid::TeleService::R::string::unknown, &number);
        ci->SetNumberPresentation(IPhoneConstants::PRESENTATION_UNKNOWN);
    }

    // Check for other special "corner cases" for CNAP and fix them similarly. Corner
    // cases only apply if we received an allowed presentation from the network, so check
    // if we think we have an allowed presentation, or if the CallerInfo presentation doesn't
    // match the presentation passed in for verification (meaning we changed it previously
    // because it's a corner case and we're being called from a different entry point).
    Int32 npresentation;
    ci->GetNumberPresentation(&npresentation);
    if (npresentation == IPhoneConstants::PRESENTATION_ALLOWED
            || (npresentation != presentation
                    && presentation == IPhoneConstants::PRESENTATION_ALLOWED)) {
        Int32 cnapSpecialCase = CheckCnapSpecialCases(number);
        if (cnapSpecialCase != CNAP_SPECIAL_CASE_NO) {
            // For all special strings, change number & numberPresentation.
            if (cnapSpecialCase == IPhoneConstants::PRESENTATION_RESTRICTED) {
                context->GetString(Elastos::Droid::TeleService::R::string::private_num, &number);
            }
            else if (cnapSpecialCase == IPhoneConstants::PRESENTATION_UNKNOWN) {
                context->GetString(Elastos::Droid::TeleService::R::string::unknown, &number);
            }
            if (DBG) {
                StringBuilder sb;
                sb += "SpecialCnap: number=";
                sb += ToLogSafePhoneNumber(number);
                sb += "; presentation now=";
                sb += cnapSpecialCase;
                Log(sb.ToString());
            }
            ci->SetNumberPresentation(cnapSpecialCase);
        }
    }
    if (DBG) {
        Log(String("modifyForSpecialCnapCases: returning number string=")
                + ToLogSafePhoneNumber(number));
    }
    return number;
}

Boolean PhoneUtils::IsRoutableViaGateway(
    /* [in] */ const String& _number)
{
    String number = _number;
    if (TextUtils::IsEmpty(number)) {
        return FALSE;
    }

    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    helper->StripSeparators(number, &number);

    String tmp;
    helper->ConvertKeypadLettersToDigits(number, &tmp);
    if (!number.Equals(tmp)) {
        return FALSE;
    }
    helper->ExtractNetworkPortion(number, &number);
    Boolean res;
    helper->IsGlobalPhoneNumber(number, &res);
    return res;
}

Boolean PhoneUtils::ActivateSpeakerIfDocked(
    /* [in] */ IPhone* phone)
{
    if (DBG) Log(String("activateSpeakerIfDocked()..."));

    Boolean activated = FALSE;
    if (PhoneGlobals::mDockState != IIntent::EXTRA_DOCK_STATE_UNDOCKED) {
        if (DBG) Log(String("activateSpeakerIfDocked(): In a dock -> may need to turn on speaker."));
        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);

        // TODO: This function should move to AudioRouter
        AutoPtr<IBluetoothManager> btManager;
        app->GetBluetoothManager((IBluetoothManager**)&btManager);
        //final WiredHeadsetManager wiredHeadset = app.getWiredHeadsetManager();
        //final AudioRouter audioRouter = app.getAudioRouter();

        /*if (!wiredHeadset.isHeadsetPlugged() && !btManager.isBluetoothHeadsetAudioOn()) {
            //audioRouter.setSpeaker(true);
            activated = true;
        }*/
    }
    return activated;
}

Boolean PhoneUtils::IsPhoneInEcm(
    /* [in] */ IPhone* phone)
{
    AutoPtr<ITelephonyCapabilities> helper;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
    Boolean res;
    if ((phone != NULL) && (helper->SupportsEcm(phone, &res), res)) {
        // For phones that support ECM, return true iff PROPERTY_INECM_MODE == "true".
        // TODO: There ought to be a better API for this than just
        // exposing a system property all the way up to the app layer,
        // probably a method like "inEcm()" provided by the telephony
        // layer.
        AutoPtr<ISystemProperties> helper;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
        String ecmMode;
        helper->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &ecmMode);
        if (!ecmMode.IsNull()) {
            assert(0 && "Equals with true or TRUE or 1 ???");
            return ecmMode.Equals(String("true"));
        }
    }
    return FALSE;
}

AutoPtr<IPhone> PhoneUtils::PickPhoneBasedOnNumber(
    /* [in] */ ICallManager* cm,
    /* [in] */ const String& scheme,
    /* [in] */ const String& number,
    /* [in] */ const String& primarySipUri,
    /* [in] */ IComponentName* thirdPartyCallComponent)
{
    if (DBG) {
        StringBuilder sb;
        sb += "pickPhoneBasedOnNumber: scheme ";
        sb += scheme;
        sb += ", number ";
        sb += ToLogSafePhoneNumber(number);
        sb += ", sipUri ";
        String tmp;
        if (!primarySipUri.IsNull()) {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            AutoPtr<IUri> uri;
            helper->Parse(primarySipUri, (IUri**)&uri);
            uri->ToSafeString(&tmp);
        }
        else {
            tmp = String("null");
        }
        sb += tmp;
        sb += ", thirdPartyCallComponent: ";
        sb += thirdPartyCallComponent;
        Log(sb.ToString());
    }

    if (!primarySipUri.IsNull()) {
        AutoPtr<IPhone> phone = GetSipPhoneFromUri(cm, primarySipUri);
        if (phone != NULL) return phone;
    }

    AutoPtr<IPhone> phone;
    cm->GetDefaultPhone((IPhone**)&phone);
    return phone;
}

AutoPtr<IPhone> PhoneUtils::GetSipPhoneFromUri(
    /* [in] */ ICallManager* cm,
    /* [in] */ const String& target)
{
    AutoPtr<IList> list;
    cm->GetAllPhones((IList**)&list);

    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        Int32 type;
        phone->GetPhoneType(&type);
        if (type == IPhoneConstants::PHONE_TYPE_SIP) {
            String sipUri;
            ISipPhone::Probe(phone)->GetSipUri(&sipUri);
            if (target.Equals(sipUri)) {
                if (DBG) {
                    StringBuilder sb;
                    sb += "- pickPhoneBasedOnNumber:";
                    sb += "found SipPhone! obj = ";
                    sb += phone;
                    sb += ", ";
                    assert(0);
                    //sb += phone.getClass();
                    Log(sb.ToString());
                }
                return phone;
            }
        }

    }

    return NULL;
}

Boolean PhoneUtils::IsRealIncomingCall(
    /* [in] */ ICallState state)
{
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    Boolean res;
    return (state == ICallState_INCOMING && (globals->mCM->HasActiveFgCall(&res), !res));
}

String PhoneUtils::GetPresentationString(
    /* [in] */ IContext* context,
    /* [in] */ Int32 presentation)
{
    String name;
    context->GetString(Elastos::Droid::TeleService::R::string::unknown, &name);
    if (presentation == IPhoneConstants::PRESENTATION_RESTRICTED) {
        context->GetString(Elastos::Droid::TeleService::R::string::private_num, &name);
    }
    else if (presentation == IPhoneConstants::PRESENTATION_PAYPHONE) {
        context->GetString(Elastos::Droid::TeleService::R::string::payphone, &name);
    }
    return name;
}

void PhoneUtils::SendViewNotificationAsync(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactUri)
{
    if (DBG) Logger::D(TAG, "Send view notification to Contacts (uri: %d)", TO_CSTR(contactUri));
    AutoPtr<IIntent> intent;
    CIntent::New(String("com.android.contacts.VIEW_NOTIFICATION"), contactUri, (IIntent**)&intent);
    intent->SetClassName(String("com.android.contacts"),
            String("com.android.contacts.ViewNotificationService"));
    AutoPtr<IComponentName> name;
    context->StartService(intent, (IComponentName**)&name);
}

void PhoneUtils::DumpCallState(
    /* [in] */ IPhone* phone)
{
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    Logger::D(TAG,"dumpCallState():");
    String name;
    phone->GetPhoneName(&name);
    Int32 state;
    phone->GetState(&state);
    Logger::D(TAG, "- Phone: %s, name = %s, state = %d",
            TO_CSTR(phone), name.string(), state);

    StringBuilder b;
    AutoPtr<ICall> call;
    phone->GetForegroundCall((ICall**)&call);
    b.SetLength(0);
    b += "  - FG call: ";
    ICallState cstate;
    call->GetState(&cstate);
    b += cstate;
    // b.append(" isAlive ").append(call.getState().isAlive());
    // b.append(" isRinging ").append(call.getState().isRinging());
    // b.append(" isDialing ").append(call.getState().isDialing());
    b += " isIdle ";
    Boolean res;
    call->IsIdle(&res);
    b += res;
    Boolean hasActiveCall = !res;
    b += " hasConnections ";
    call->HasConnections(&res);
    b += res;
    Logger::D(TAG, b.ToString());

    call = NULL;
    phone->GetBackgroundCall((ICall**)&call);
    b.SetLength(0);
    b += "  - BG call: ";
    call->GetState(&cstate);
    b += cstate;
    // b.append(" isAlive ").append(call.getState().isAlive());
    // b.append(" isRinging ").append(call.getState().isRinging());
    // b.append(" isDialing ").append(call.getState().isDialing());
    b += " isIdle ";
    call->IsIdle(&res);
    b += res;
    Boolean hasHoldingCall = !res;
    b += " hasConnections ";
    call->HasConnections(&res);
    b += res;
    Logger::D(TAG, b.ToString());


    call = NULL;
    phone->GetRingingCall((ICall**)&call);
    b.SetLength(0);
    b += "  - RINGING call: ";
    call->GetState(&cstate);
    b += cstate;
    // b.append(" isAlive ").append(call.getState().isAlive());
    // b.append(" isRinging ").append(call.getState().isRinging());
    // b.append(" isDialing ").append(call.getState().isDialing());
    b += " isIdle ";
    call->IsIdle(&res);
    b += res;
    Boolean hasRingingCall = !res;
    b += " hasConnections ";
    call->HasConnections(&res);
    b += res;
    Logger::D(TAG, b.ToString());

    Boolean allLinesTaken = hasActiveCall && hasHoldingCall;

    b.SetLength(0);
    b += "  - hasRingingCall ";
    b += hasRingingCall;
    b += " hasActiveCall ";
    b += hasActiveCall;
    b += " hasHoldingCall ";
    b += hasHoldingCall;
    b += " allLinesTaken ";
    b += allLinesTaken;
    Logger::D(TAG, b.ToString());

    // On CDMA phones, dump out the CdmaPhoneCallState too:
    Int32 type;
    phone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        if (app->mCdmaPhoneCallState != NULL) {
            CdmaPhoneCallState::PhoneCallState state;
            Logger::D(TAG, "  - CDMA call state: %d"
                    , (app->mCdmaPhoneCallState->GetCurrentCallState(&state), state));
        }
        else {
            Logger::D(TAG, "  - CDMA device, but null cdmaPhoneCallState!");
        }
    }
}

void PhoneUtils::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

void PhoneUtils::DumpCallManager()
{
    AutoPtr<ICall> call;
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    AutoPtr<ICallManager> cm = globals->mCM;
    StringBuilder b;

    Logger::D(TAG, "############### dumpCallManager() ##############");
    // TODO: Don't log "cm" itself, since CallManager.toString()
    // already spews out almost all this same information.
    // We should fix CallManager.toString() to be more minimal, and
    // use an explicit dumpState() method for the verbose dump.
    // Log.d(TAG, "CallManager: " + cm
    //         + ", state = " + cm.getState());
    Int32 state;
    cm->GetState(&state);
    Logger::D(TAG, "CallManager: state = %d", state);
    b.SetLength(0);
    cm->GetActiveFgCall((ICall**)&call);
    b += " - FG call: ";
    Boolean res;
    b += (cm->HasActiveFgCall(&res), res)? "YES ": "NO ";
    b += TO_CSTR(call);
    b += "  State: ";
    cm->GetActiveFgCallState(&state);
    b += state;
    b += "  Conn: ";
    AutoPtr<IList> list;
    cm->GetFgCallConnections((IList**)&list);
    b += TO_CSTR(list);
    Logger::D(TAG, b.ToString());

    b.SetLength(0);
    call = NULL;
    cm->GetFirstActiveBgCall((ICall**)&call);
    b += " - BG call: ";
    b += (cm->HasActiveBgCall(&res), res)? "YES ": "NO ";
    b += TO_CSTR(call);
    b += "  State: ";
    call->GetState(&state);
    b += state;
    b += "  Conn: ";
    list = NULL;
    cm->GetBgCallConnections((IList**)&list);
    b += TO_CSTR(list);
    Logger::D(TAG, b.ToString());


    b.SetLength(0);
    call = NULL;
    cm->GetFirstActiveRingingCall((ICall**)&call);
    b += " - RINGING call: ";
    b += (cm->HasActiveRingingCall(&res), res)? "YES ": "NO ";
    b += TO_CSTR(call);
    b += "  State: ";
    call->GetState(&state);
    b += state;
    Logger::D(TAG, b.ToString());


    AutoPtr<ICallManagerHelper> helper;
    assert(0);
    //CCallManagerHelper::AcquireSingleton((ICallManagerHelper**)&helper);
    AutoPtr<ICallManager> manager;
    helper->GetInstance((ICallManager**)&manager);
    AutoPtr<IList> plist;
    manager->GetAllPhones((IList**)&plist);

    Int32 size;
    plist->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        plist->Get(i, (IInterface**)&obj);
        AutoPtr<IPhone> phone = IPhone::Probe(obj);

        if (phone != NULL) {
            String name;
            phone->GetPhoneName(&name);
            Int32 state;
            phone->GetState(&state);
            Logger::D(TAG, "Phone: %s, name = %s, state = %d", TO_CSTR(phone), name.string(), state);
            b.SetLength(0);
            call = NULL;
            phone->GetForegroundCall((ICall**)&call);
            b += " - FG call: ";
            b += TO_CSTR(call);
            b +=  "  State: ";
            call->GetState(&state);
            b += state;
            b +=  "  Conn: ";
            Boolean res;
            call->HasConnections(&res);
            b += res;
            Logger::D(TAG, b.ToString());

            b.SetLength(0);
            call = NULL;
            phone->GetBackgroundCall((ICall**)&call);
            b += " - BG call: ";
            b += TO_CSTR(call);
            b +=  "  State: ";
            call->GetState(&state);
            b += state;
            b +=  "  Conn: ";
            call->HasConnections(&res);
            b += res;
            Logger::D(TAG, b.ToString());

            b.SetLength(0);
            call = NULL;
            phone->GetRingingCall((ICall**)&call);
            b +=" - RINGING call: ";
            b += TO_CSTR(call);
            b += "  State: ";
            call->GetState(&state);
            b += (state);
            b += "  Conn: ";
            call->HasConnections(&res);
            b += res;
            Logger::D(TAG, b.ToString());
        }
    }

    Logger::D(TAG, "############## END dumpCallManager() ###############");
}

Boolean PhoneUtils::IsLandscape(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    return orientation == IConfiguration::ORIENTATION_LANDSCAPE;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos