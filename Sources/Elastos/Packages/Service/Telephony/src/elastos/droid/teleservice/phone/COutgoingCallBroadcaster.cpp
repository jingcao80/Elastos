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


#include "elastos/droid/teleservice/phone/COutgoingCallBroadcaster.h"
#include "elastos/droid/teleservice/phone/COutgoingCallReceiver.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "elastos/droid/teleservice/phone/OtaUtils.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.View.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/core/Thread.h"
#include <elastos/core/StringBuilder.h>
#include "R.h"

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_IDLE;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::CTelephonyCapabilities;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::View::IView;
using Elastos::Core::Thread;
using Elastos::Core::StringBuilder;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

COutgoingCallBroadcaster::MyHandler::MyHandler(
    /* [in] */ COutgoingCallBroadcaster* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode COutgoingCallBroadcaster::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == EVENT_OUTGOING_CALL_TIMEOUT) {
        Logger::I(TAG, "Outgoing call takes too long. Showing the spinner.");
        IView::Probe(mHost->mWaitingSpinner)->SetVisibility(IView::VISIBLE);
    }
    else if (what == EVENT_DELAYED_FINISH) {
        mHost->Finish();
    }
    else {
        Logger::W(TAG, "Unknown message id: %d", what);
    }
    return NOERROR;
}

const String COutgoingCallBroadcaster::OutgoingCallReceiver::TAG("OutgoingCallReceiver");

ECode COutgoingCallBroadcaster::OutgoingCallReceiver::constructor(
    /* [in] */ IOutgoingCallBroadcaster* host)
{
    mHost = (COutgoingCallBroadcaster*)host;
    return BroadcastReceiver::constructor();
}

ECode COutgoingCallBroadcaster::OutgoingCallReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mHandler->RemoveMessages(EVENT_OUTGOING_CALL_TIMEOUT);
    Boolean isAttemptingCall;
    DoReceive(context, intent, &isAttemptingCall);
    if (DBG) Logger::V(TAG, "OutgoingCallReceiver is going to finish the Activity itself.");

    // We cannot finish the activity immediately here because it would cause the temporary
    // black screen of OutgoingBroadcaster to go away and we need it to stay up until the
    // UI (in a different process) has time to come up.
    // However, if we know we are not attemping a call, we need to finish the activity
    // immediately so that subsequent CALL intents will retrigger a new
    // OutgoingCallReceiver. see b/10857203
    if (isAttemptingCall) {
        mHost->StartDelayedFinish();
    }
    else {
        mHost->Finish();
    }
    return NOERROR;
}

ECode COutgoingCallBroadcaster::OutgoingCallReceiver::DoReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Logger::V(TAG, "doReceive: %d", TO_CSTR(intent));

    Boolean alreadyCalled;
    String number;
    String originalUri;

    intent->GetBooleanExtra(
            IOutgoingCallBroadcaster::EXTRA_ALREADY_CALLED, FALSE, &alreadyCalled);
    if (alreadyCalled) {
        if (DBG) Logger::V(TAG, "CALL already placed -- returning.");
        *result = FALSE;
        return NOERROR;
    }

    // Once the NEW_OUTGOING_CALL broadcast is finished, the resultData
    // is used as the actual number to call. (If null, no call will be
    // placed.)

    GetResultData(&number);
    if (VDBG) Logger::V(TAG, "- got number from resultData: '%s'", number.string());

    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    // OTASP-specific checks.
    // TODO: This should probably all happen in
    // OutgoingCallBroadcaster.onCreate(), since there's no reason to
    // even bother with the NEW_OUTGOING_CALL broadcast if we're going
    // to disallow the outgoing call anyway...
    Boolean res;
    AutoPtr<ITelephonyCapabilities> helper;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
    helper->SupportsOtasp(app->mPhone, &res);
    if (res) {
        Boolean activateState = (app->mCdmaOtaScreenState->mOtaScreenState
                == OtaUtils::CdmaOtaScreenState::OTA_STATUS_ACTIVATION);
        Boolean dialogState = (app->mCdmaOtaScreenState->mOtaScreenState
                == OtaUtils::CdmaOtaScreenState::OTA_STATUS_SUCCESS_FAILURE_DLG);
        Boolean isOtaCallActive = FALSE;

        // TODO: Need cleaner way to check if OTA is active.
        // Also, this check seems to be broken in one obscure case: if
        // you interrupt an OTASP call by pressing Back then Skip,
        // otaScreenState somehow gets left in either PROGRESS or
        // LISTENING.
        if ((app->mCdmaOtaScreenState->mOtaScreenState
                == OtaUtils::CdmaOtaScreenState::OTA_STATUS_PROGRESS)
                || (app->mCdmaOtaScreenState->mOtaScreenState
                == OtaUtils::CdmaOtaScreenState::OTA_STATUS_LISTENING)) {
            isOtaCallActive = TRUE;
        }

        if (activateState || dialogState) {
            // The OTASP sequence is active, but either (1) the call
            // hasn't started yet, or (2) the call has ended and we're
            // showing the success/failure screen.  In either of these
            // cases it's OK to make a new outgoing call, but we need
            // to take down any OTASP-related UI first.
            if (dialogState) app->DismissOtaDialogs();
            app->ClearOtaState();
        }
        else if (isOtaCallActive) {
            // The actual OTASP call is active.  Don't allow new
            // outgoing calls at all from this state.
            Logger::W(TAG, "OTASP call is active: disallowing a new outgoing call.");
            *result = FALSE;
            return NOERROR;
        }
    }

    AutoPtr<IPhoneNumberUtils> helper2;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper2);
    Int32 state;
    Boolean isSupport, isNumber;
    if (number.IsNull()) {
        if (DBG) Logger::V(TAG, "CALL cancelled (null number), returning...");
        *result = FALSE;
        return NOERROR;
    }
    else if ((helper->SupportsOtasp(app->mPhone, &isSupport), isSupport)
            && ((app->mPhone->GetState(&state), state) != PhoneConstantsState_IDLE)
            && ((app->mPhone->IsOtaSpNumber(number, &isNumber), isNumber))) {
        if (DBG) Logger::V(TAG, "Call is active, a 2nd OTA call cancelled -- returning.");
        *result = FALSE;
        return NOERROR;
    }
    else if (helper2->IsPotentialLocalEmergencyNumber(context, number, &isNumber), isNumber) {
        // Just like 3rd-party apps aren't allowed to place emergency
        // calls via the ACTION_CALL intent, we also don't allow 3rd
        // party apps to use the NEW_OUTGOING_CALL broadcast to rewrite
        // an outgoing call into an emergency number.
        Logger::W(TAG, "Cannot modify outgoing call to emergency number %s.", number.string());
        *result = FALSE;
        return NOERROR;
    }

    intent->GetStringExtra(
            IOutgoingCallBroadcaster::EXTRA_ORIGINAL_URI, &originalUri);
    if (originalUri == NULL) {
        Logger::E(TAG, "Intent is missing EXTRA_ORIGINAL_URI -- returning.");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IUriHelper> helper3;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper3);
    AutoPtr<IUri> uri;
    helper3->Parse(originalUri, (IUri**)&uri);

    // We already called convertKeypadLettersToDigits() and
    // stripSeparators() way back in onCreate(), before we sent out the
    // NEW_OUTGOING_CALL broadcast.  But we need to do it again here
    // too, since the number might have been modified/rewritten during
    // the broadcast (and may now contain letters or separators again.)
    AutoPtr<IPhoneNumberUtils> helper4;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper4);
    helper4->ConvertKeypadLettersToDigits(number, &number);
    helper4->StripSeparators(number, &number);

    if (DBG) Logger::V(TAG, "doReceive: proceeding with call...");
    if (VDBG) Logger::V(TAG, "- uri: %p", TO_CSTR(uri));
    if (VDBG) Logger::V(TAG, "- actual number to dial: '%s'", number.string());

    mHost->StartSipCallOptionHandler(context, intent, uri, number);

    *result = TRUE;
    return NOERROR;
}

const String COutgoingCallBroadcaster::TAG("OutgoingCallBroadcaster");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);

    return (PhoneGlobals::DBG_LEVEL >= 1) && (value == 1);
}

const Boolean COutgoingCallBroadcaster::DBG = initDBG();

const Boolean COutgoingCallBroadcaster::VDBG = TRUE;

const String COutgoingCallBroadcaster::PERMISSION = Elastos::Droid::Manifest::permission::PROCESS_OUTGOING_CALLS;

const Int32 COutgoingCallBroadcaster::DIALOG_NOT_VOICE_CAPABLE = 1;

const Int32 COutgoingCallBroadcaster::EVENT_OUTGOING_CALL_TIMEOUT = 101;
const Int32 COutgoingCallBroadcaster::EVENT_DELAYED_FINISH = 102;

const Int32 COutgoingCallBroadcaster::OUTGOING_CALL_TIMEOUT_THRESHOLD = 2000; // msec
const Int32 COutgoingCallBroadcaster::DELAYED_FINISH_TIME = 2000; // msec

CAR_INTERFACE_IMPL_3(COutgoingCallBroadcaster, Activity, IOutgoingCallBroadcaster,
        IDialogInterfaceOnClickListener, IDialogInterfaceOnCancelListener)

CAR_OBJECT_IMPL(COutgoingCallBroadcaster)

COutgoingCallBroadcaster::COutgoingCallBroadcaster()
{
    mHandler = new MyHandler(this);
}

ECode COutgoingCallBroadcaster::constructor()
{
    return Activity::constructor();
}

void COutgoingCallBroadcaster::StartDelayedFinish()
{
    Boolean res;
    mHandler->SendEmptyMessageDelayed(EVENT_DELAYED_FINISH, DELAYED_FINISH_TIME, &res);
}

void COutgoingCallBroadcaster::StartSipCallOptionHandler(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [in] */ IUri* uri,
    /* [in] */ const String& number)
{
    // TODO: Remove this code.
    return;
}

ECode COutgoingCallBroadcaster::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);
    SetContentView(Elastos::Droid::TeleService::R::layout::outgoing_call_broadcaster);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::spinner, (IView**)&view);
    mWaitingSpinner = IProgressBar::Probe(view);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (DBG) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> configuration;
        resources->GetConfiguration((IConfiguration**)&configuration);
        Logger::V(TAG, "onCreate: this = %s, icicle = %s", TO_CSTR(this), TO_CSTR(icicle));
        Logger::V(TAG, " - getIntent() = %s", TO_CSTR(intent));
        Logger::V(TAG, " - configuration = %s", TO_CSTR(configuration));
    }

    if (icicle != NULL) {
        // A non-null icicle means that this activity is being
        // re-initialized after previously being shut down.
        //
        // In practice this happens very rarely (because the lifetime
        // of this activity is so short!), but it *can* happen if the
        // framework detects a configuration change at exactly the
        // right moment; see bug 2202413.
        //
        // In this case, do nothing.  Our onCreate() method has already
        // run once (with icicle==null the first time), which means
        // that the NEW_OUTGOING_CALL broadcast for this new call has
        // already been sent.
        Logger::I(TAG, "onCreate: non-null icicle! Bailing out, not sending NEW_OUTGOING_CALL broadcast...");

        // No need to finish() here, since the OutgoingCallReceiver from
        // our original instance will do that.  (It'll actually call
        // finish() on our original instance, which apparently works fine
        // even though the ActivityManager has already shut that instance
        // down.  And note that if we *do* call finish() here, that just
        // results in an "ActivityManager: Duplicate finish request"
        // warning when the OutgoingCallReceiver runs.)

        return NOERROR;
    }

    ProcessIntent(intent);

    // isFinishing() return false when 1. broadcast is still ongoing, or 2. dialog is being
    // shown. Otherwise finish() is called inside processIntent(), is isFinishing() here will
    // return true.

    if (DBG) {
        Boolean res;
        IsFinishing(&res);
        Logger::V(TAG, "At the end of onCreate(). isFinishing(): %d", res);
    }
    return NOERROR;
}

void COutgoingCallBroadcaster::ProcessIntent(
    /* [in] */ IIntent* intent)
{
    if (DBG) {
        StringBuilder sb;
        sb += "processIntent() = ";
        sb += TO_CSTR(intent);
        sb += ", thread: ";
        AutoPtr<IThread> currentThread = Thread::GetCurrentThread();
        sb += TO_CSTR(currentThread);
        Logger::V(TAG, sb.ToString());
    }
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);

    // Outgoing phone calls are only allowed on "voice-capable" devices.
    if (!PhoneGlobals::sVoiceCapable) {
        Logger::I(TAG, "This device is detected as non-voice-capable device.");
        HandleNonVoiceCapable(intent);
        return;
    }

    String action;
    intent->GetAction(&action);
    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    String number;
    helper->GetNumberFromIntent(intent, this, &number);
    // Check the number, don't convert for sip uri
    // TODO put uriNumber under PhoneNumberUtils
    if (!number.IsNull()) {
        Boolean res;
        helper->IsUriNumber(number, &res);
        if (!res) {
            helper->ConvertKeypadLettersToDigits(number, &number);
            helper->StripSeparators(number, &number);
        }
    }
    else {
        Logger::W(TAG, "The number obtained from Intent is null.");
    }

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    AutoPtr<IAppOpsManager> appOps = IAppOpsManager::Probe(obj);
    Int32 launchedFromUid;
    String launchedFromPackage;
    //try
    ECode ec = NOERROR;
    {
        AutoPtr<IIActivityManager> manageer = ActivityManagerNative::GetDefault();
        AutoPtr<IBinder> binder;
        ECode ec;
        FAIL_GOTO(ec = GetActivityToken((IBinder**)&binder), ERROR)
        FAIL_GOTO(ec = manageer->GetLaunchedFromUid(binder, &launchedFromUid), ERROR)
        FAIL_GOTO(ec = manageer->GetLaunchedFromPackage(binder, &launchedFromPackage), ERROR)
    }
    // catch (RemoteException e) {
ERROR:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        launchedFromUid = -1;
        launchedFromPackage = NULL;
    }
    //}
    Int32 tmp;
    if ((appOps->NoteOpNoThrow(IAppOpsManager::OP_CALL_PHONE, launchedFromUid, launchedFromPackage, &tmp), tmp)
            != IAppOpsManager::MODE_ALLOWED) {
        Logger::W(TAG, "Rejecting call from uid %d package %s", launchedFromUid
                , launchedFromPackage.string());
        Finish();
        return;
    }

    // If true, this flag will indicate that the current call is a special kind
    // of call (most likely an emergency number) that 3rd parties aren't allowed
    // to intercept or affect in any way.  (In that case, we start the call
    // immediately rather than going through the NEW_OUTGOING_CALL sequence.)
    Boolean callNow;

    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    String cname;
    componentName->GetClassName(&cname);
    AutoPtr<IComponentName> cn;
    GetComponentName((IComponentName**)&cn);
    String clsName;
    cn->GetClassName(&clsName);
    if (clsName.Equals(cname)) {
        // If we were launched directly from the OutgoingCallBroadcaster,
        // not one of its more privileged aliases, then make sure that
        // only the non-privileged actions are allowed.
        String action;
        intent->GetAction(&action);
        if (!IIntent::ACTION_CALL.Equals(action)) {
            Logger::W(TAG, "Attempt to deliver non-CALL action; forcing to CALL");
            intent->SetAction(IIntent::ACTION_CALL);
        }
    }

    // Check whether or not this is an emergency number, in order to
    // enforce the restriction that only the CALL_PRIVILEGED and
    // CALL_EMERGENCY intents are allowed to make emergency calls.
    //
    // (Note that the ACTION_CALL check below depends on the result of
    // isPotentialLocalEmergencyNumber() rather than just plain
    // isLocalEmergencyNumber(), to be 100% certain that we *don't*
    // allow 3rd party apps to make emergency calls by passing in an
    // "invalid" number like "9111234" that isn't technically an
    // emergency number but might still result in an emergency call
    // with some networks.)
    AutoPtr<IPhoneNumberUtils> helper3;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper3);
    Boolean result;
    Boolean isExactEmergencyNumber =
            (!number.IsNull()) && (helper3->IsLocalEmergencyNumber(this, number, &result), result);
    Boolean isPotentialEmergencyNumber =
            (!number.IsNull()) && (helper3->IsPotentialLocalEmergencyNumber(this, number, &result), result);
    if (VDBG) {
        Logger::V(TAG, " - Checking restrictions for number '%s':", number.string());
        Logger::V(TAG, "     isExactEmergencyNumber     = %d", isExactEmergencyNumber);
        Logger::V(TAG, "     isPotentialEmergencyNumber = %d", isPotentialEmergencyNumber);
    }

    /* Change CALL_PRIVILEGED into CALL or CALL_EMERGENCY as needed. */
    // TODO: This code is redundant with some code in InCallScreen: refactor.
    if (IIntent::ACTION_CALL_PRIVILEGED.Equals(action)) {
        // We're handling a CALL_PRIVILEGED intent, so we know this request came
        // from a trusted source (like the built-in dialer.)  So even a number
        // that's *potentially* an emergency number can safely be promoted to
        // CALL_EMERGENCY (since we *should* allow you to dial "91112345" from
        // the dialer if you really want to.)
        if (isPotentialEmergencyNumber) {
            Logger::I(TAG, "ACTION_CALL_PRIVILEGED is used while the number is a potential"
                    " emergency number. Use ACTION_CALL_EMERGENCY as an action instead.");
            action = IIntent::ACTION_CALL_EMERGENCY;
        }
        else {
            action = IIntent::ACTION_CALL;
        }
        if (DBG) Logger::V(TAG, " - updating action from CALL_PRIVILEGED to %s", action.string());
        intent->SetAction(action);
    }

    if (IIntent::ACTION_CALL.Equals(action)) {
        if (isPotentialEmergencyNumber) {
            Logger::W(TAG, "Cannot call potential emergency number '%s' with CALL Intent %s.",
                    number.string(), TO_CSTR(intent));
            Logger::I(TAG, "Launching default dialer instead...");

            AutoPtr<IIntent> invokeFrameworkDialer;
            CIntent::New((IIntent**)&invokeFrameworkDialer);

            // TwelveKeyDialer is in a tab so we really want
            // DialtactsActivity.  Build the intent 'manually' to
            // use the java resolver to find the dialer class (as
            // opposed to a Context which look up known android
            // packages only)
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            String package;
            resources->GetString(Elastos::Droid::TeleService::R::string::ui_default_package, &package);
            String classname;
            resources->GetString(Elastos::Droid::TeleService::R::string::dialer_default_class, &classname);
            invokeFrameworkDialer->SetClassName(package, classname);
            invokeFrameworkDialer->SetAction(IIntent::ACTION_DIAL);
            AutoPtr<IUri> data;
            intent->GetData((IUri**)&data);
            invokeFrameworkDialer->SetData(data);
            if (DBG) Logger::V(TAG, "onCreate(): calling startActivity for Dialer: %s"
                    , TO_CSTR(invokeFrameworkDialer));
            StartActivity(invokeFrameworkDialer);
            Finish();
            return;
        }
        callNow = FALSE;
    }
    else if (IIntent::ACTION_CALL_EMERGENCY.Equals(action)) {
        // ACTION_CALL_EMERGENCY case: this is either a CALL_PRIVILEGED
        // intent that we just turned into a CALL_EMERGENCY intent (see
        // above), or else it really is an CALL_EMERGENCY intent that
        // came directly from some other app (e.g. the EmergencyDialer
        // activity built in to the Phone app.)
        // Make sure it's at least *possible* that this is really an
        // emergency number.
        if (!isPotentialEmergencyNumber) {
            Logger::W(TAG, "Cannot call non-potential-emergency number %s with "
                    "EMERGENCY_CALL Intent %s."
                    " Finish the Activity immediately.", number.string(), TO_CSTR(intent));
            Finish();
            return;
        }
        callNow = TRUE;
    }
    else {
        Logger::E(TAG, "Unhandled Intent %s. Finish the Activity immediately.", TO_CSTR(intent));
        Finish();
        return;
    }

    // Make sure the screen is turned on.  This is probably the right
    // thing to do, and more importantly it works around an issue in the
    // activity manager where we will not launch activities consistently
    // when the screen is off (since it is trying to keep them paused
    // and has...  issues).
    //
    // Also, this ensures the device stays awake while doing the following
    // broadcast; technically we should be holding a wake lock here
    // as well.
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    globals->WakeUpScreen();

    // If number is null, we're probably trying to call a non-existent voicemail number,
    // send an empty flash or something else is fishy.  Whatever the problem, there's no
    // number, so there's no point in allowing apps to modify the number.
    if (TextUtils::IsEmpty(number)) {
        Boolean res;
        if (intent->GetBooleanExtra(EXTRA_SEND_EMPTY_FLASH, FALSE, &res), res) {
            Logger::I(TAG, "onCreate: SEND_EMPTY_FLASH...");
            AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
            PhoneUtils::SendEmptyFlash(phone);
            Finish();
            return;
        }
        else {
            Logger::I(TAG, "onCreate: null or empty number, setting callNow=true...");
            callNow = TRUE;
        }
    }

    if (callNow) {
        // This is a special kind of call (most likely an emergency number)
        // that 3rd parties aren't allowed to intercept or affect in any way.
        // So initiate the outgoing call immediately.

        Logger::I(TAG, "onCreate(): callNow case! Calling placeCall(): %s", TO_CSTR(intent));

        // Initiate the outgoing call, and simultaneously launch the
        // InCallScreen to display the in-call UI:
        AutoPtr<PhoneGlobals> globals;
        PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
        globals->mCallController->PlaceCall(intent);

        // Note we do *not* "return" here, but instead continue and
        // send the ACTION_NEW_OUTGOING_CALL broadcast like for any
        // other outgoing call.  (But when the broadcast finally
        // reaches the OutgoingCallReceiver, we'll know not to
        // initiate the call again because of the presence of the
        // EXTRA_ALREADY_CALLED extra.)
    }

    // For now, SIP calls will be processed directly without a
    // NEW_OUTGOING_CALL broadcast.
    //
    // TODO: In the future, though, 3rd party apps *should* be allowed to
    // intercept outgoing calls to SIP addresses as well.  To do this, we should
    // (1) update the NEW_OUTGOING_CALL intent documentation to explain this
    // case, and (2) pass the outgoing SIP address by *not* overloading the
    // EXTRA_PHONE_NUMBER extra, but instead using a new separate extra to hold
    // the outgoing SIP address.  (Be sure to document whether it's a URI or just
    // a plain address, whether it could be a tel: URI, etc.)
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    String scheme;
    uri->GetScheme(&scheme);
    AutoPtr<IPhoneNumberUtils> helper4;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper4);
    Boolean res;
    if (IPhoneAccount::SCHEME_SIP.Equals(scheme) || (helper4->IsUriNumber(number, &res), res)) {
        Logger::I(TAG, "The requested number was detected as SIP call.");
        StartSipCallOptionHandler(this, intent, uri, number);
        Finish();
        return;

        // TODO: if there's ever a way for SIP calls to trigger a
        // "callNow=true" case (see above), we'll need to handle that
        // case here too (most likely by just doing nothing at all.)
    }

    AutoPtr<IIntent> broadcastIntent;
    CIntent::New(IIntent::ACTION_NEW_OUTGOING_CALL, (IIntent**)&broadcastIntent);
    if (!number.IsNull()) {
        broadcastIntent->PutExtra(IIntent::EXTRA_PHONE_NUMBER, number);
    }
    CallGatewayManager::CheckAndCopyPhoneProviderExtras(intent, broadcastIntent);
    broadcastIntent->PutBooleanExtra(EXTRA_ALREADY_CALLED, callNow);
    String str;
    IObject::Probe(uri)->ToString(&str);
    broadcastIntent->PutExtra(EXTRA_ORIGINAL_URI, str);
    // Need to raise foreground in-call UI as soon as possible while allowing 3rd party app
    // to intercept the outgoing call.
    broadcastIntent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    if (DBG) Logger::V(TAG, " - Broadcasting intent: %s.", TO_CSTR(broadcastIntent));

    // Set a timer so that we can prepare for unexpected delay introduced by the broadcast.
    // If it takes too much time, the timer will show "waiting" spinner.
    // This message will be removed when OutgoingCallReceiver#onReceive() is called before the
    // timeout.
    mHandler->SendEmptyMessageDelayed(EVENT_OUTGOING_CALL_TIMEOUT,
            OUTGOING_CALL_TIMEOUT_THRESHOLD, &res);

    AutoPtr<IBroadcastReceiver> receiver;
    COutgoingCallReceiver::New(this, (IBroadcastReceiver**)&receiver);
    AutoPtr<IUserHandleHelper> helper5;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper5);
    AutoPtr<IUserHandle> h;
    helper5->GetOWNER((IUserHandle**)&h);
    SendOrderedBroadcastAsUser(broadcastIntent, h,
            PERMISSION, receiver,
            NULL,  // scheduler
            IActivity::RESULT_OK,  // initialCode
            number,  // initialData: initial value for the result data
            NULL);  // initialExtras
}

ECode COutgoingCallBroadcaster::OnStop()
{
    // Clean up (and dismiss if necessary) any managed dialogs.
    //
    // We don't do this in onPause() since we can be paused/resumed
    // due to orientation changes (in which case we don't want to
    // disturb the dialog), but we *do* need it here in onStop() to be
    // sure we clean up if the user hits HOME while the dialog is up.
    //
    // Note it's safe to call removeDialog() even if there's no dialog
    // associated with that ID.
    RemoveDialog(DIALOG_NOT_VOICE_CAPABLE);

    return Activity::OnStop();
}

void COutgoingCallBroadcaster::HandleNonVoiceCapable(
    /* [in] */ IIntent* intent)
{
    if (DBG) Logger::V(TAG, "handleNonVoiceCapable: handling %s"
            " on non-voice-capable device...", TO_CSTR(intent));

    // Just show a generic "voice calling not supported" dialog.
    ShowDialog(DIALOG_NOT_VOICE_CAPABLE);
    // ...and we'll eventually finish() when the user dismisses
    // or cancels the dialog.
}

ECode COutgoingCallBroadcaster::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** outdialog)
{
    VALIDATE_NOT_NULL(outdialog)

    AutoPtr<IAlertDialog> dialog;
    switch(id) {
        case DIALOG_NOT_VOICE_CAPABLE:
        {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
            builder->SetTitle(Elastos::Droid::TeleService::R::string::not_voice_capable);
            builder->SetIconAttribute(Elastos::Droid::R::attr::alertDialogIcon);
            builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
            builder->SetOnCancelListener(this);
            builder->Create((IAlertDialog**)&dialog);
            break;
        }
        default:
            Logger::W(TAG, "onCreateDialog: unexpected ID %d" + id);
            dialog = NULL;
            break;
    }
    *outdialog = IDialog::Probe(dialog);
    REFCOUNT_ADD(*outdialog)
    return NOERROR;
}

ECode COutgoingCallBroadcaster::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    // DIALOG_NOT_VOICE_CAPABLE is the only dialog we ever use (so far
    // at least), and its only button is "OK".
    return Finish();
}

ECode COutgoingCallBroadcaster::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    // DIALOG_NOT_VOICE_CAPABLE is the only dialog we ever use (so far
    // at least), and canceling it is just like hitting "OK".
    return Finish();
}

ECode COutgoingCallBroadcaster::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    Activity::OnConfigurationChanged(newConfig);
    if (DBG) Logger::V(TAG, "onConfigurationChanged: newConfig = %s", TO_CSTR(newConfig));
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
