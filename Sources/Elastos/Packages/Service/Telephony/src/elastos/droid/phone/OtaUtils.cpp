
#include "elastos/droid/phone/OtaUtils.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(OtaUtils::MyDialogInterfaceOnKeyListener, Object,
        IDialogInterfaceOnKeyListener)

ECode OtaUtils::MyDialogInterfaceOnKeyListener::OnKey(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log(String("Ignoring key events..."));
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(OtaUtils::MyDialogInterfaceOnKeyListener2, Object,
        IDialogInterfaceOnKeyListener)

ECode OtaUtils::MyDialogInterfaceOnKeyListener2::OnKey(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log(String("Ignoring key events..."));
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(OtaUtils::MyAlertDialogOnClickListener, Object,
        IAlertDialogOnClickListener)

OtaUtils::MyAlertDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return OtaSkipActivation();
}

const String OtaUtils::LOG_TAG("OtaUtils");
const Boolean OtaUtils::DBG = FALSE;

const String OtaUtils::OTASP_NUMBER("*228");
const String OtaUtils::OTASP_NUMBER_NON_INTERACTIVE("*22899");

Boolean OtaUtils::sIsWizardMode = TRUE;

const Int32 OtaUtils::OTA_CALL_LTE_RETRIES_MAX = 5;
const Int32 OtaUtils::OTA_CALL_LTE_RETRY_PERIOD = 3000;
Int32 OtaUtils::sOtaCallLteRetries = 0;

OtaUtils::OtaUtils()
    : OTA_SPC_TIMEOUT(60)
    , OTA_FAILURE_DIALOG_TIMEOUT(2)
    , mInteractive(TRUE)
{
}

ECode OtaUtils::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean interactive,
    /* [in] */ IBluetoothManager* bluetoothManager)
{
    if (DBG) Log(String("OtaUtils constructor..."));
    PhoneGlobals::GetInstance((PhoneGlobals**)&mApplication);
    mContext = context;
    mInteractive = interactive;
    mBluetoothManager = bluetoothManager;
    return NOERROR;
}

ECode OtaUtils::MaybeDoOtaCall(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 request,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<IPhone> phone = app->mPhone;

    if (ActivityManager::IsRunningInTestHarness()) {
        Logger::I(LOG_TAG, "Don't run provisioning when in test harness");
        *result = TRUE;
        return NOERROR;
    }

    if (!TelephonyCapabilities::SupportsOtasp(phone)) {
        // Presumably not a CDMA phone.
        if (DBG) log(String("maybeDoOtaCall: OTASP not supported on this device"));
        *result = TRUE;  // Nothing to do here.
        return NOERROR;
    }

    Boolean res;
    if (phone->IsMinInfoReady(&res), !res) {
        if (DBG) Log(String("MIN is not ready. Registering to receive notification."));
        phone->RegisterForSubscriptionInfoReady(handler, request, NULL);
        *result = FALSE;
        return NOERROR;
    }
    phone->UnregisterForSubscriptionInfoReady(handler);

    Int32 mode;
    GetLteOnCdmaMode(context, &mode);
    if (mode == IPhoneConstants::LTE_ON_CDMA_UNKNOWN) {
        if (sOtaCallLteRetries < OTA_CALL_LTE_RETRIES_MAX) {
            if (DBG) Log(String("maybeDoOtaCall: LTE state still unknown: retrying"));
            handler->SendEmptyMessageDelayed(request, OTA_CALL_LTE_RETRY_PERIOD);
            sOtaCallLteRetries++;
            *result = FALSE;
            return NOERROR;
        }
        else {
            Logger::W(LOG_TAG, "maybeDoOtaCall: LTE state still unknown: giving up");
            *result = TRUE;
            return NOERROR;
        }
    }

    Boolean phoneNeedsActivation;
    phone->NeedsOtaServiceProvisioning(&phoneNeedsActivation);
    if (DBG) Log(String("phoneNeedsActivation is set to ") + StringUtils::ToString(phoneNeedsActivation));

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Int32 otaShowActivationScreen;
    resources->GetInteger(R.integer.OtaShowActivationScreen, &otaShowActivationScreen);
    if (DBG) Log(String("otaShowActivationScreen: ") + StringUtils::ToString(otaShowActivationScreen));

    // Run the OTASP call in "interactive" mode only if
    // this is a non-LTE "voice capable" device.
    if (IPhoneGlobals::sVoiceCapable && mode == IPhoneConstants::LTE_ON_CDMA_FALSE) {
        if (phoneNeedsActivation
                && (otaShowActivationScreen == OTA_SHOW_ACTIVATION_SCREEN_ON)) {
            app->mCdmaOtaProvisionData->mIsOtaCallIntentProcessed = FALSE;
            sIsWizardMode = FALSE;

            if (DBG) Logger::D(LOG_TAG, "==> Starting interactive CDMA provisioning...");
            OtaUtils::StartInteractiveOtasp(context);

            if (DBG) Log(String("maybeDoOtaCall: voice capable; activation started."));
        }
        else {
            if (DBG) Log(String("maybeDoOtaCall: voice capable; activation NOT started."));
        }
    }
    else {
        if (phoneNeedsActivation) {
            app->mCdmaOtaProvisionData->mIsOtaCallIntentProcessed = FALSE;
            AutoPtr<IIntent> newIntent;
            CIntent::New(ACTION_PERFORM_VOICELESS_CDMA_PROVISIONING, (IIntent**)&newIntent);
            newIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            newIntent->PutExtra(EXTRA_VOICELESS_PROVISIONING_OFFER_DONTSHOW, TRUE);
            //try {
            ECode ec = context->StartActivity(newIntent);
            //} catch (ActivityNotFoundException e) {
            if (ec == (ECode)ActivityNotFoundException) {
                Loge(String("No activity Handling PERFORM_VOICELESS_CDMA_PROVISIONING!"));
                *result = FALSE;
                return NOERROR;
            }
            //}
            if (DBG) Log(String("maybeDoOtaCall: non-interactive; activation intent sent."));
        }
        else {
            if (DBG) Log(String("maybeDoOtaCall: non-interactive, no need for OTASP."));
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode OtaUtils::StartInteractiveOtasp(
    /* [in] */ IContext* context)
{
    if (DBG) Log(String("startInteractiveOtasp()..."));
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    // There are two ways to start OTASP on voice-capable devices:
    //
    // (1) via the PERFORM_CDMA_PROVISIONING intent
    //     - this is triggered by the "Activate device" button in settings,
    //       or can be launched automatically upon boot if the device
    //       thinks it needs to be provisioned.
    //     - the intent is handled by InCallScreenShowActivation.onCreate(),
    //       which calls this method
    //     - we prepare for OTASP by initializing the OtaUtils object
    //     - we bring up the InCallScreen in the ready-to-activate state
    //     - when the user presses the "Activate" button we launch the
    //       call by calling CallController.placeCall() via the
    //       otaPerformActivation() method.
    //
    // (2) by manually making an outgoing call to a special OTASP number
    //     like "*228" or "*22899".
    //     - That sequence does NOT involve this method (OtaUtils.startInteractiveOtasp()).
    //       Instead, the outgoing call request goes straight to CallController.placeCall().
    //     - CallController.placeCall() notices that it's an OTASP
    //       call, and initializes the OtaUtils object.
    //     - The InCallScreen is launched (as the last step of
    //       CallController.placeCall()).  The InCallScreen notices that
    //       OTASP is active and shows the correct UI.

    // Here, we start sequence (1):
    // Do NOT immediately start the call.  Instead, bring up the InCallScreen
    // in the special "activate" state (see OtaUtils.otaShowActivateScreen()).
    // We won't actually make the call until the user presses the "Activate"
    // button.

    AutoPtr<IIntent> activationScreenIntent;
    CIntent::New((IIntent**)&activationScreenIntent);
    activationScreenIntent->SetClass(context, InCallScreen.class)
    activationScreenIntent->SetAction(ACTION_DISPLAY_ACTIVATION_SCREEN);

    // Watch out: in the scenario where OTASP gets triggered from the
    // BOOT_COMPLETED broadcast (see OtaStartupReceiver.java), we might be
    // running in the PhoneApp's context right now.
    // So the FLAG_ACTIVITY_NEW_TASK flag is required here.
    activationScreenIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

    // We're about to start the OTASP sequence, so create and initialize the
    // OtaUtils instance.  (This needs to happen before bringing up the
    // InCallScreen.)
    OtaUtils::SetupOtaspCall(activationScreenIntent);

    // And bring up the InCallScreen...
    Logger::I(LOG_TAG, "startInteractiveOtasp: launching InCallScreen in 'activate' state: %s"
          + TO_CSTR(activationScreenIntent));
    return context->StartActivity(activationScreenIntent);
}

ECode OtaUtils::StartNonInteractiveOtasp(
    /* [in] */ IContext* context,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log(String("startNonInteractiveOtasp()..."));
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    if (app->mOtaUtils != NULL) {
        // An OtaUtils instance already exists, presumably from a previous OTASP call.
        Logger::I(LOG_TAG, "startNonInteractiveOtasp: "
                "OtaUtils already exists; nuking the old one and starting again...");
    }

    // Create the OtaUtils instance.
    AutoPtr<IBluetoothManager> manager;
    app->GetBluetoothManager((IBluetoothManager**)&manager);
    app->mOtaUtils = new OtaUtils(context, FALSE /* non-interactive mode */, manager);
    if (DBG) Log(String("- created OtaUtils: ") + TO_CSTR(app->mOtaUtils));

    // ... and kick off the OTASP call.
    // TODO(InCallScreen redesign): This should probably go through
    // the CallController, rather than directly calling
    // PhoneUtils.placeCall().
    AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
    String number = OTASP_NUMBER_NON_INTERACTIVE;
    Logger::I(LOG_TAG, String("startNonInteractiveOtasp: placing call to '") + number + String("'..."));
    Int32 callStatus = PhoneUtils::PlaceCall(context,
            phone,
            number,
            null,   // contactRef
            FALSE); //isEmergencyCall

    if (callStatus == IPhoneUtils::CALL_STATUS_DIALED) {
        if (DBG) Log(String("  ==> successful return from placeCall(): callStatus = ")
                + StringUtils::ToString(callStatus));
    }
    else {
        Logger::W(LOG_TAG, "Failure from placeCall() for OTA number '%s': code %d",
                number.string(), callStatus);
        *result = callStatus;
        return NOERROR;
    }

    // TODO: Any other special work to do here?
    // Such as:
    //
    // - manually kick off progress updates, either using TelephonyRegistry
    //   or else by sending PendingIntents directly to our caller?
    //
    // - manually silence the in-call audio?  (Probably unnecessary
    //   if Stingray truly has no audio path from phone baseband
    //   to the device's speakers.)
    //

    *result = callStatus;
    return NOERROR;
}

ECode OtaUtils::IsOtaspCallIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (DBG) {
        StringBuilder sb;
        sb += "isOtaspCallIntent(";
        sb += TO_CSTR(intent);
        sb += ")...";
        Log(sb.ToString());
    }
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<IPhone> phone;
    app->mCM->GetDefaultPhone((IPhone**)&phone);

    if (intent == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TelephonyCapabilities::SupportsOtasp(phone)) {
        *result = FALSE;
        return NOERROR;
    }

    String action;
    intent->GetAction(&action);
    if (action == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (!action.Equals(IIntent::ACTION_CALL)) {
        if (DBG) Log(String("isOtaspCallIntent: not a CALL action: '")
                 + action + String("' ==> not OTASP"));
        *result = FALSE;
        return NOERROR;
    }

    if ((app->mCdmaOtaScreenState == NULL) || (app->mCdmaOtaProvisionData == NULL)) {
        // Uh oh -- something wrong with our internal OTASP state.
        // (Since this is an OTASP-capable device, these objects
        // *should* have already been created by PhoneApp.onCreate().)
        // throw new IllegalStateException("isOtaspCallIntent: "
        //                                 + "app.cdmaOta* objects(s) not initialized");
        Logger::E("OtaUtils", "isOtaspCallIntent: app.cdmaOta* objects(s) not initialized");
        return IllegalStateException;
    }

    // This is an OTASP call iff the number we're trying to dial is one of
    // the magic OTASP numbers.
    String number;
    //try {
    ECode ec = PhoneUtils::GetInitialNumber(intent, &number);
    //} catch (PhoneUtils.VoiceMailNumberMissingException ex) {
    if (ec == (ECode)PhoneUtils.VoiceMailNumberMissingException) {
        // This was presumably a "voicemail:" intent, so it's
        // obviously not an OTASP number.
        if (DBG) Log(String("isOtaspCallIntent: VoiceMailNumberMissingException => not OTASP"));
        *result = FALSE;
        return NOERROR;
    }
    //}
    Boolean res;
    phone->IsOtaSpNumber(number, &res);
    if (res) {
        if (DBG) Log(String("isOtaSpNumber: ACTION_CALL to '")
                + number + String("' ==> OTASP call!"));
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode OtaUtils::SetupOtaspCall(
    /* [in] */ IIntent* intent)
{
    if (DBG) Log(String("setupOtaspCall(): preparing for OTASP call to ") + TO_CSTR(intent));
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    if (app->mOtaUtils != NULL) {
        // An OtaUtils instance already exists, presumably from a prior OTASP call.
        // Nuke the old one and start this call with a fresh instance.
        Logger::I(LOG_TAG, "setupOtaspCall: "
                "OtaUtils already exists; replacing with new instance...");
    }

    // Create the OtaUtils instance.
    AutoPtr<IContext> context;
    app->GetApplicationContext((IContext**)&context);
    AutoPtr<IBluetoothManager> manager;
    app->GetBluetoothManager((IBluetoothManager**)&manager);
    app->mOtaUtils = new OtaUtils(context, TRUE /* interactive */, manager);
    if (DBG) Log(String("- created OtaUtils: ") + TO_CSTR(app->mOtaUtils));

    // NOTE we still need to call OtaUtils.updateUiWidgets() once the
    // InCallScreen instance is ready; see InCallScreen.checkOtaspStateOnResume()

    // Make sure the InCallScreen knows that it needs to switch into OTASP mode.
    //
    // NOTE in gingerbread and earlier, we used to do
    //     setInCallScreenMode(InCallScreenMode.OTA_NORMAL);
    // directly in the InCallScreen, back when this check happened inside the InCallScreen.
    //
    // But now, set the global CdmaOtaInCallScreenUiState object into
    // NORMAL mode, which will then cause the InCallScreen (when it
    // comes up) to realize that an OTA call is active.

    app->mOtaUtils->SetCdmaOtaInCallScreenUiState(
        OtaUtils.CdmaOtaInCallScreenUiState.State.NORMAL);

    // TODO(OTASP): note app.inCallUiState.inCallScreenMode and
    // app.cdmaOtaInCallScreenUiState.state are mostly redundant.  Combine them.
    // app.inCallUiState.inCallScreenMode = InCallUiState.InCallScreenMode.OTA_NORMAL;

    // TODO(OTASP / bug 5092031): we ideally should call
    // otaShowListeningScreen() here to make sure that the DTMF dialpad
    // becomes visible at the start of the "*228" call:
    //
    //  // ...and get the OTASP-specific UI into the right state.
    //  app.otaUtils.otaShowListeningScreen();
    //  if (app.otaUtils.mInCallScreen != null) {
    //      app.otaUtils.mInCallScreen.requestUpdateScreen();
    //  }
    //
    // But this doesn't actually work; the call to otaShowListeningScreen()
    // *doesn't* actually bring up the listening screen, since the
    // cdmaOtaConfigData.otaShowListeningScreen config parameter hasn't been
    // initialized (we haven't run readXmlSettings() yet at this point!)

    // Also, since the OTA call is now just starting, clear out
    // the "committed" flag in app.cdmaOtaProvisionData.
    if (app->mCdmaOtaProvisionData != NULL) {
        app->mCdmaOtaProvisionData->mIsOtaCallCommitted = FALSE;
    }
    return NOERROR;
}

void OtaUtils::SetSpeaker(
    /* [in] */ Boolean state)
{
    if (DBG) Log(String("setSpeaker : ") + StringUtils::ToString(state));

    if (!mInteractive) {
        if (DBG) Log(String("non-interactive mode, ignoring setSpeaker."));
        return;
    }

    if (state == PhoneUtils::IsSpeakerOn(mContext)) {
        if (DBG) Log(String("no change. returning"));
        return;
    }

    Boolean res1, res2;
    if (state && (mBluetoothManager->IsBluetoothAvailable(&res1), res1)
            && (mBluetoothManager->IsBluetoothAudioConnected(&res2), res2)) {
        mBluetoothManager->DisconnectBluetoothAudio();
    }
    PhoneUtils::TurnOnSpeaker(mContext, state, TRUE);
}

ECode OtaUtils::OnOtaProvisionStatusChanged(
    /* [in] */ IAsyncResult* r)
{
    AutoPtr<ArrayOf<Int32> > OtaStatus[] = (int[]) r.result;
    if (DBG) Log(String("Provision status event!"));
    if (DBG) Log("onOtaProvisionStatusChanged(): status = "
                 + OtaStatus[0] + " ==> " + otaProvisionStatusToString(OtaStatus[0]));

    // In practice, in a normal successful OTASP call, events come in as follows:
    //   - SPL_UNLOCKED within a couple of seconds after the call starts
    //   - then a delay of around 45 seconds
    //   - then PRL_DOWNLOADED and MDN_DOWNLOADED and COMMITTED within a span of 2 seconds

    switch((*OtaStatus)[0]) {
        case IPhone::CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED:
            if (DBG) Log(String("onOtaProvisionStatusChanged(): RETRIES EXCEEDED"));
            UpdateOtaspProgress();
            mApplication.cdmaOtaProvisionData.otaSpcUptime = SystemClock.elapsedRealtime();
            if (mInteractive) {
                OtaShowSpcErrorNotice(OTA_SPC_TIMEOUT);
            }
            else {
                SendOtaspResult(OTASP_FAILURE_SPC_RETRIES);
            }
            // Power.shutdown();
            break;

        case IPhone::CDMA_OTA_PROVISION_STATUS_COMMITTED:
            if (DBG) {
                Log(String("onOtaProvisionStatusChanged(): DONE, isOtaCallCommitted set to true"));
            }
            mApplication->mCdmaOtaProvisionData.isOtaCallCommitted = TRUE;
            if (mApplication->mCdmaOtaScreenState.otaScreenState !=
                ICdmaOtaScreenStateOtaScreenState_OTA_STATUS_UNDEFINED) {
                UpdateOtaspProgress();
            }

            break;

        case IPhone::CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_SSD_UPDATED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED:
        case IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED:
            // Only update progress when OTA call is in normal state
            Int32 stae;
            GetCdmaOtaInCallScreenUiState(&stae);
            if (stae == ICdmaOtaInCallScreenUiStateState_NORMAL) {
                if (DBG) Log(String("onOtaProvisionStatusChanged(): change to ProgressScreen"));
                UpdateOtaspProgress();
            }
            break;

        default:
            if (DBG) Log(String("onOtaProvisionStatusChanged(): Ignoring OtaStatus ")
                     + StringUtils::ToString((*OtaStatus)[0]));
            break;
    }
    return NOERROR;
}

ECode OtaUtils::OnOtaspDisconnect()
{
    if (DBG) Log(String("onOtaspDisconnect()..."));
    // We only handle this event explicitly in non-interactive mode.
    // (In interactive mode, the InCallScreen does any post-disconnect
    // cleanup.)
    if (!mInteractive) {
        // Send a success or failure indication back to our caller.
        UpdateNonInteractiveOtaSuccessFailure();
    }
    return NOERROR;
}

void OtaUtils::OtaShowHome()
{
    if (DBG) Log(String("otaShowHome()..."));
    mApplication->mCdmaOtaScreenState.otaScreenState =
            CdmaOtaScreenState.OtaScreenState.OTA_STATUS_UNDEFINED;
    // mInCallScreen.endInCallScreenSession();
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_HOME);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    mContext->StartActivityAsUser(intent, IUserHandle::CURRENT);
    return;
}

void OtaUtils::OtaSkipActivation()
{
    if (DBG) Log(String("otaSkipActivation()..."));

    SendOtaspResult(OTASP_USER_SKIPPED);

    // if (mInteractive) mInCallScreen.finish();
    return;
}

void OtaUtils::OtaPerformActivation()
{
    if (DBG) Log(Strung("otaPerformActivation()..."));
    if (!mInteractive) {
        // We shouldn't ever get here in non-interactive mode!
        Logger::W(LOG_TAG, "otaPerformActivation: not interactive!");
        return;
    }

    if (!mApplication->CdmaOtaProvisionData->mInOtaSpcState) {
        // Place an outgoing call to the special OTASP number:
        AutoPre<IIntent> newIntent;
        CIntent::New(IIntent::ACTION_CALL, (IIntent**)&newIntent);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(IPhoneAccount::SCHEME_TEL, OTASP_NUMBER, NULL, (IUri**)&uri);
        newIntent->SetData(uri);

        // Initiate the outgoing call:
        mApplication->mCallController->PlaceCall(newIntent);

        // ...and get the OTASP-specific UI into the right state.
        OtaShowListeningScreen();
        // mInCallScreen.requestUpdateScreen();
    }
    return;
}

ECode OtaUtils::OtaShowActivateScreen()
{
    if (DBG) Log(String("otaShowActivateScreen()..."));
    if (mApplication->mCdmaOtaConfigData.otaShowActivationScreen
            == OTA_SHOW_ACTIVATION_SCREEN_ON) {
        if (DBG) Log(String("otaShowActivateScreen(): show activation screen"));
        if (!IsDialerOpened()) {
            OtaScreenInitialize();
            mOtaWidgetData->mOtaSkipButton->SetVisibility(sIsWizardMode ?
                    IView::VISIBLE : IView::INVISIBLE);
            mOtaWidgetData->mOtaTextActivate->SetVisibility(IView::VISIBLE);
            mOtaWidgetData->mCallCardOtaButtonsActivate->SetVisibility(IView::VISIBLE);
        }
        mApplication->mCdmaOtaScreenState.otaScreenState =
                ICdmaOtaScreenStateOtaScreenState_OTA_STATUS_ACTIVATION;
    }
    else {
        if (DBG) Log(String("otaShowActivateScreen(): show home screen"));
        OtaShowHome();
    }
    return NOERROR;
}

void OtaUtils::OtaShowListeningScreen()
{
    if (DBG) Log(String("otaShowListeningScreen()..."));
    if (!mInteractive) {
        // We shouldn't ever get here in non-interactive mode!
        Logger::W(LOG_TAG, "otaShowListeningScreen: not interactive!");
        return;
    }

    if (mApplication->mCdmaOtaConfigData.otaShowListeningScreen
            == OTA_SHOW_LISTENING_SCREEN_ON) {
        if (DBG) Log(String("otaShowListeningScreen(): show listening screen"));
        if (!IsDialerOpened()) {
            OtaScreenInitialize();
            mOtaWidgetData->mOtaTextListenProgress->SetVisibility(IView::VISIBLE);
            mOtaWidgetData->mOtaTextListenProgress->SetText(R.string.ota_listen);
            // mOtaWidgetData.otaDtmfDialerView.setVisibility(View.VISIBLE);
            mOtaWidgetData->mCallCardOtaButtonsListenProgress->SetVisibility(IView::VISIBLE);
            mOtaWidgetData->mOtaSpeakerButton->SetVisibility(IView::VISIBLE);
            Boolean speakerOn = PhoneUtils::IsSpeakerOn(mContext);
            mOtaWidgetData->mOtaSpeakerButton->SetChecked(speakerOn);
        }
        mApplication->mCdmaOtaScreenState.otaScreenState =
                CdmaOtaScreenState.OtaScreenState.OTA_STATUS_LISTENING;
    }
    else {
        if (DBG) Log("otaShowListeningScreen(): show progress screen");
        OtaShowInProgressScreen();
    }
}

void OtaUtils::UpdateOtaspProgress()
{
    if (DBG) Log(String("updateOtaspProgress()...  mInteractive = ")
            + StringUtils::ToString(mInteractive));
    if (mInteractive) {
        // On regular phones we just call through to
        // otaShowInProgressScreen(), which updates the
        // InCallScreen's onscreen UI.
        OtaShowInProgressScreen();
    }
    else {
        // We're not using the InCallScreen to show OTA progress.

        // For now, at least, there's nothing to do here.
        // The overall "success" or "failure" indication we send back
        // (to our caller) is triggered by the DISCONNECT event;
        // see updateNonInteractiveOtaSuccessFailure().

        // But if we ever need to send *intermediate* progress updates back
        // to our caller, we'd do that here, possbily using the same
        // PendingIntent that we already use to indicate success or failure.
    }
}

void OtaUtils::UpdateNonInteractiveOtaSuccessFailure()
{
    // This is basically the same logic as otaShowSuccessFailure(): we
    // check the isOtaCallCommitted bit, and if that's true it means
    // that activation was successful.

    if (DBG) Log(String("updateNonInteractiveOtaSuccessFailure(): isOtaCallCommitted = ")
                 + mApplication.cdmaOtaProvisionData.isOtaCallCommitted);
    Int32 resultCode =
            mApplication->mCdmaOtaProvisionData->mIsOtaCallCommitted
            ? OTASP_SUCCESS : OTASP_FAILURE;
    SendOtaspResult(resultCode);
}

void OtaUtils::SendOtaspResult(
    /* [in] */ Int32 resultCode)
{
    if (DBG) Log(String("sendOtaspResult: resultCode = ") + StringUtils::ToString(resultCode));

    // Pass the success or failure indication back to our caller by
    // adding an additional extra to the PendingIntent we already
    // have.
    // (NB: there's a PendingIntent send() method that takes a resultCode
    // directly, but we can't use that here since that call is only
    // meaningful for pending intents that are actually used as activity
    // results.)

    AutoPtr<IIntent> extraStuff;
    CIntent::New((IIntent**)&extraStuff);
    extraStuff->PutExtra(EXTRA_OTASP_RESULT_CODE, resultCode);
    // When we call PendingIntent.send() below, the extras from this
    // intent will get merged with any extras already present in
    // cdmaOtaScreenState.otaspResultCodePendingIntent.

    if (mApplication->mCdmaOtaScreenState == NULL) {
        Logger::E(LOG_TAG, "updateNonInteractiveOtaSuccessFailure: no cdmaOtaScreenState object!");
        return;
    }
    if (mApplication->mCdmaOtaScreenState.otaspResultCodePendingIntent == null) {
        Logger::w(LOG_TAG, "updateNonInteractiveOtaSuccessFailure: "
              + "null otaspResultCodePendingIntent!");
        return;
    }

    //try {
    if (DBG) Log(String("- sendOtaspResult:  SENDING PENDING INTENT: ") +
                 mApplication->mCdmaOtaScreenState.otaspResultCodePendingIntent);
    ECode ec = mApplication->mCdmaOtaScreenState.otaspResultCodePendingIntent->Send(
            mContext,
            0, /* resultCode (unused) */
            extraStuff);
    //} catch (CanceledException e) {
    if (ec == (ECode)CanceledException) {
        // should never happen because no code cancels the pending intent right now,
        Logger::E(LOG_TAG, "PendingIntent send() failed: %d", ec);
    }
    //}
}

void OtaUtils::OtaShowInProgressScreen()
{
    if (DBG) Log(String("otaShowInProgressScreen()..."));
    if (!mInteractive) {
        // We shouldn't ever get here in non-interactive mode!
        Logger::W(LOG_TAG, "otaShowInProgressScreen: not interactive!");
        return;
    }

    mApplication->mCdmaOtaScreenState.otaScreenState =
        CdmaOtaScreenState.OtaScreenState.OTA_STATUS_PROGRESS;

    if ((mOtaWidgetData == NULL) /* || (mInCallScreen == null) */) {
        Logger::W(LOG_TAG, "otaShowInProgressScreen: UI widgets not set up yet!");

        // TODO(OTASP): our CdmaOtaScreenState is now correct; we just set
        // it to OTA_STATUS_PROGRESS.  But we still need to make sure that
        // when the InCallScreen eventually comes to the foreground, it
        // notices that state and does all the same UI updating we do below.
        return;
    }

    if (!IsDialerOpened()) {
        OtaScreenInitialize();
        mOtaWidgetData->mOtaTextListenProgress->SetVisibility(IView::VISIBLE);
        mOtaWidgetData->mOtaTextListenProgress->SetText(R.string.ota_progress);
        mOtaWidgetData->mOtaTextProgressBar->SetVisibility(IView::VISIBLE);
        mOtaWidgetData->mCallCardOtaButtonsListenProgress->SetVisibility(IView::VISIBLE);
        mOtaWidgetData->mOtaSpeakerButton->SetVisibility(IView::VISIBLE);
        Boolean speakerOn = PhoneUtils::IsSpeakerOn(mContext);
        mOtaWidgetData->mOtaSpeakerButton->SetChecked(speakerOn);
    }
}

void OtaUtils::OtaShowProgramFailure(
        /* [in] */ Int32 length)
{
    if (DBG) Log(String("otaShowProgramFailure()..."));
    mApplication->mCdmaOtaProvisionData.activationCount++;
    if ((mApplication->mCdmaOtaProvisionData.activationCount <
            mApplication->mCdmaOtaConfigData.otaShowActivateFailTimes)
            && (mApplication->mCdmaOtaConfigData.otaShowActivationScreen ==
            OTA_SHOW_ACTIVATION_SCREEN_ON)) {
        if (DBG) Log(String("otaShowProgramFailure(): activationCount")
                + mApplication.cdmaOtaProvisionData.activationCount);
        if (DBG) Log(String("otaShowProgramFailure(): show failure notice"));
        OtaShowProgramFailureNotice(length);
    }
    else {
        if (DBG) Log(String("otaShowProgramFailure(): show failure dialog"));
        OtaShowProgramFailureDialog();
    }
}

ECode OtaUtils::OtaShowSuccessFailure()
{
    if (DBG) Log(String("otaShowSuccessFailure()..."));
    if (!mInteractive) {
        // We shouldn't ever get here in non-interactive mode!
        Logger::W(LOG_TAG, "otaShowSuccessFailure: not interactive!");
        return NOERROR;
    }

    OtaScreenInitialize();
    if (DBG) Log(String("otaShowSuccessFailure(): isOtaCallCommitted")
            + mApplication.cdmaOtaProvisionData.isOtaCallCommitted);
    if (mApplication.cdmaOtaProvisionData.isOtaCallCommitted) {
        if (DBG) Log(String("otaShowSuccessFailure(), show success dialog"));
        OtaShowProgramSuccessDialog();
    }
    else {
        if (DBG) Log(String("otaShowSuccessFailure(), show failure dialog"));
        OtaShowProgramFailure(OTA_FAILURE_DIALOG_TIMEOUT);
    }
    return NOERROR;
}

void OtaUtils::OtaShowProgramFailureDialog()
{
    if (DBG) Log(String("otaShowProgramFailureDialog()..."));
    mApplication->mCdmaOtaScreenState->mOtaScreenState =
            ICdmaOtaScreenStateOtaScreenState_OTA_STATUS_SUCCESS_FAILURE_DLG;
    mOtaWidgetData->mOtaTitle->SetText(R.string.ota_title_problem_with_activation);
    mOtaWidgetData->mOtaTextSuccessFail->SetVisibility(IView::VISIBLE);
    mOtaWidgetData->mOtaTextSuccessFail->SetText(R.string.ota_unsuccessful);
    mOtaWidgetData->mCallCardOtaButtonsFailSuccess->SetVisibility(IView::VISIBLE);
    mOtaWidgetData->mOtaTryAgainButton->SetVisibility(IView::VISIBLE);
    //close the dialer if open
    // if (isDialerOpened()) {
    //     mOtaCallCardDtmfDialer.closeDialer(false);
    // }
}


void OtaUtils::OtaShowProgramSuccessDialog()
{
    if (DBG) Log(String("otaShowProgramSuccessDialog()..."));
    mApplication->mCdmaOtaScreenState->mOtaScreenState =
            ICdmaOtaScreenStateOtaScreenState_OTA_STATUS_SUCCESS_FAILURE_DLG;
    mOtaWidgetData->mOtaTitle->SetText(R.string.ota_title_activate_success);
    mOtaWidgetData->mOtaTextSuccessFail->SetVisibility(IView::VISIBLE);
    mOtaWidgetData->mOtaTextSuccessFail->SetText(R.string.ota_successful);
    mOtaWidgetData->mCallCardOtaButtonsFailSuccess->SetVisibility(IView::VISIBLE);
    mOtaWidgetData->mOtaNextButton->SetVisibility(IView::VISIBLE);
    //close the dialer if open
    // if (isDialerOpened()) {
    //     mOtaCallCardDtmfDialer.closeDialer(false);
    // }
}

void OtaUtils::OtaShowSpcErrorNotice(
    /* [in] */ Int32 length)
{
    if (DBG) Log(String("otaShowSpcErrorNotice()..."));
    if (mOtaWidgetData->mSpcErrorDialog == NULL) {
        mApplication->mCdmaOtaProvisionData->mInOtaSpcState = TRUE;
        AutoPtr<IDialogInterfaceOnKeyListener> keyListener = MyDialogInterfaceOnKeyListener();

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(NULL /* mInCallScreen */, (IAlertDialogBuilder**)&builder);
        builder->SetMessage(R.string.ota_spc_failure);
        builder->SetOnKeyListener(keyListener);
        builder->Create((IAlertDialog**)&(mOtaWidgetData->mSpcErrorDialog));
        AutoPtr<IWindow> window;
        mOtaWidgetData->mSpcErrorDialog->GetWindow((IWindow**)&window);
        window->AddFlags(
                IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE
                | IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON);
        mOtaWidgetData->mSpcErrorDialog->Show();
        //close the dialer if open
        // if (isDialerOpened()) {
        //     mOtaCallCardDtmfDialer.closeDialer(false);
        // }
        Int64 noticeTime = length * 1000;
        if (DBG) Log(String("otaShowSpcErrorNotice(), remaining SPC noticeTime") +
                StringUtils::ToString(noticeTime));
        // mInCallScreen.requestCloseSpcErrorNotice(noticeTime);
    }
}

ECode OtaUtils::OnOtaCloseSpcNotice()
{
    if (DBG) Log(String("onOtaCloseSpcNotice(), send shutdown intent"));
    AutoPtr<IIntent> shutdown;
    CIntent::New(IIntent::ACTION_REQUEST_SHUTDOWN, (IIntent**)&shutdown);
    shutdown->PutExtra(IIntent::EXTRA_KEY_CONFIRM, FALSE);
    shutdown->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    return mContext->StartActivity(shutdown);
}

void OtaUtils::OtaShowProgramFailureNotice(
    /* [in] */ Int32 length)
{
    if (DBG) Log(String("otaShowProgramFailureNotice()..."));
    if (mOtaWidgetData->mOtaFailureDialog == NULL) {

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(NULL /* mInCallScreen */, (IAlertDialogBuilder**)&builder);
        builder->SetMessage(R.string.ota_failure)
        builder->Create((IAlertDialog**)&(mOtaWidgetData->mOtaFailureDialog));

        AutoPtr<IWindow> window;
        mOtaWidgetData->mOtaFailureDialog->GetWindow((IWindow**)*window);
        window->AddFlags(
                IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE
                | IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON);
        mOtaWidgetData->mOtaFailureDialog->Show();

        Int64 noticeTime = length * 1000;
        // mInCallScreen.requestCloseOtaFailureNotice(noticeTime);
    }
}

ECode OtaUtils::OnOtaCloseFailureNotice()
{
    if (DBG) Log(String("onOtaCloseFailureNotice()..."));
    if (mOtaWidgetData->mOtaFailureDialog != NULL) {
        mOtaWidgetData->mOtaFailureDialog->Dismiss();
        mOtaWidgetData->mOtaFailureDialog = NULL;
    }
    OtaShowActivateScreen();
    return NOERROR;
}

void OtaUtils::OtaScreenInitialize()
{
    if (DBG) Log(String("otaScreenInitialize()..."));

    if (!mInteractive) {
        // We should never be doing anything with UI elements in
        // non-interactive mode.
        Logger::W(LOG_TAG, "otaScreenInitialize: not interactive!");
        return;
    }

    // if (mInCallTouchUi != null) mInCallTouchUi.setVisibility(View.GONE);
    // if (mCallCard != null) {
    //     mCallCard.setVisibility(View.GONE);
    //     // TODO: try removing this.
    //     mCallCard.hideCallCardElements();
    // }

    mOtaWidgetData->mOtaTitle->SetText(R.string.ota_title_activate);
    mOtaWidgetData->mOtaTextActivate->SetVisibility(IView::GONE);
    mOtaWidgetData->mOtaTextListenProgress->SetVisibility(IView::GONE);
    mOtaWidgetData->mOtaTextProgressBar->SetVisibility(IView::GONE);
    mOtaWidgetData->mOtaTextSuccessFail->SetVisibility(IView::GONE);
    mOtaWidgetData->mCallCardOtaButtonsActivate->SetVisibility(IView::GONE);
    mOtaWidgetData->mCallCardOtaButtonsListenProgress->SetVisibility(IView::GONE);
    mOtaWidgetData->mCallCardOtaButtonsFailSuccess->SetVisibility(IView::GONE);
    // mOtaWidgetData.otaDtmfDialerView.setVisibility(View.GONE);
    mOtaWidgetData->mOtaSpeakerButton->SetVisibility(IView::GONE);
    mOtaWidgetData->mOtaTryAgainButton->SetVisibility(IView::GONE);
    mOtaWidgetData->mOtaNextButton->SetVisibility(IView::GONE);
    mOtaWidgetData->mOtaUpperWidgets->SetVisibility(IView::VISIBLE);
    mOtaWidgetData->mOtaSkipButton->SetVisibility(IView::VISIBLE);
}

ECode OtaUtils::HideOtaScreen()
{
    if (DBG) Log(String("hideOtaScreen()..."));

    mOtaWidgetData->mCallCardOtaButtonsActivate->SetVisibility(IView::GONE);
    mOtaWidgetData->mCallCardOtaButtonsListenProgress->SetVisibility(IView::GONE);
    mOtaWidgetData->mCallCardOtaButtonsFailSuccess->SetVisibility(IView::GONE);
    return mOtaWidgetData->mOtaUpperWidgets->SetVisibility(IView::GONE);
}

ECode OtaUtils::IsDialerOpened(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // boolean retval = (mOtaCallCardDtmfDialer != null && mOtaCallCardDtmfDialer.isOpened());
    Boolean retval = FALSE;
    if (DBG) Log(String("- isDialerOpened() ==> ") + StringUtils::ToString(retval));
    *result = retval;
    return NOERROR;
}

ECode OtaUtils::OtaShowProperScreen()
{
    if (DBG) Log(String("otaShowProperScreen()..."));
    if (!mInteractive) {
        // We shouldn't ever get here in non-interactive mode!
        Logger::W(LOG_TAG, "otaShowProperScreen: not interactive!");
        return NOERROR;
    }

    // if ((mInCallScreen != null) && mInCallScreen.isForegroundActivity()) {
    //     if (DBG) log("otaShowProperScreen(): InCallScreen in foreground, currentstate = "
    //             + mApplication.cdmaOtaScreenState.otaScreenState);
    //     if (mInCallTouchUi != null) {
    //         mInCallTouchUi.setVisibility(View.GONE);
    //     }
    //     if (mCallCard != null) {
    //         mCallCard.setVisibility(View.GONE);
    //     }
    //     if (mApplication.cdmaOtaScreenState.otaScreenState
    //             == CdmaOtaScreenState.OtaScreenState.OTA_STATUS_ACTIVATION) {
    //         otaShowActivateScreen();
    //     } else if (mApplication.cdmaOtaScreenState.otaScreenState
    //             == CdmaOtaScreenState.OtaScreenState.OTA_STATUS_LISTENING) {
    //         otaShowListeningScreen();
    //     } else if (mApplication.cdmaOtaScreenState.otaScreenState
    //             == CdmaOtaScreenState.OtaScreenState.OTA_STATUS_PROGRESS) {
    //         otaShowInProgressScreen();
    //     }

    //     if (mApplication.cdmaOtaProvisionData.inOtaSpcState) {
    //         otaShowSpcErrorNotice(getOtaSpcDisplayTime());
    //     }
    // }
    return NOERROR;
}

void OtaUtils::ReadXmlSettings()
{
    if (DBG) log("readXmlSettings()...");
    if (mApplication->mCdmaOtaConfigData->mConfigComplete) {
        return;
    }

    mApplication->mCdmaOtaConfigData->mConfigComplete = TRUE;

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);

    Int32 tmpOtaShowActivationScreen;
    resources->GetInteger(R.integer.OtaShowActivationScreen, &tmpOtaShowActivationScreen);
    mApplication->mCdmaOtaConfigData->mOtaShowActivationScreen = tmpOtaShowActivationScreen;
    if (DBG) Log(String("readXmlSettings(), otaShowActivationScreen = ")
            + mApplication->mCdmaOtaConfigData->mOtaShowActivationScreen);

    Int32 tmpOtaShowListeningScreen;
    resources->GetInteger(R.integer.OtaShowListeningScreen, &tmpOtaShowListeningScreen);
    mApplication->mCdmaOtaConfigData->mOtaShowListeningScreen = tmpOtaShowListeningScreen;
    if (DBG) Log(String("readXmlSettings(), otaShowListeningScreen = ")
            + mApplication->mCdmaOtaConfigData->mOtaShowListeningScreen);

    Int32 tmpOtaShowActivateFailTimes;
    resources->GetInteger(R.integer.OtaShowActivateFailTimes, &tmpOtaShowActivateFailTimes);
    mApplication->mCdmaOtaConfigData->mOtaShowActivateFailTimes = tmpOtaShowActivateFailTimes;
    if (DBG) Log(String("readXmlSettings(), otaShowActivateFailTimes = ")
            + mApplication->mCdmaOtaConfigData->mOtaShowActivateFailTimes);

    Int32 tmpOtaPlaySuccessFailureTone;
    resources->GetInteger(R.integer.OtaPlaySuccessFailureTone, &tmpOtaPlaySuccessFailureTone);
    mApplication->mCdmaOtaConfigData->mOtaPlaySuccessFailureTone = tmpOtaPlaySuccessFailureTone;
    if (DBG) Log(String("readXmlSettings(), otaPlaySuccessFailureTone = ")
            + mApplication->mCdmaOtaConfigData->mOtaPlaySuccessFailureTone);
}

ECode OtaUtils::OnClickHandler(
    /* [in] */ Int32 id)
{
    switch (id) {
        case R.id.otaEndButton:
            OnClickOtaEndButton();
            break;

        case R.id.otaSpeakerButton:
            OnClickOtaSpeakerButton();
            break;

        case R.id.otaActivateButton:
            OnClickOtaActivateButton();
            break;

        case R.id.otaSkipButton:
            OnClickOtaActivateSkipButton();
            break;

        case R.id.otaNextButton:
            OnClickOtaActivateNextButton();
            break;

        case R.id.otaTryAgainButton:
            OnClickOtaTryAgainButton();
            break;

        default:
            if (DBG) Log(String("onClickHandler: received a click event for unrecognized id"));
            break;
    }
    return NOERROR;
}

void OtaUtils::OnClickOtaTryAgainButton()
{
    if (DBG) Log(String("Activation Try Again Clicked!"));
    if (!mApplication->mCdmaOtaProvisionData->mInOtaSpcState) {
        OtaShowActivateScreen();
    }
}

void OtaUtils::OnClickOtaEndButton()
{
    if (DBG) Log(String("Activation End Call Button Clicked!"));
    if (!mApplication->mCdmaOtaProvisionData->mInOtaSpcState) {
        if (PhoneUtils::Hangup(mApplication->mCM) == FALSE) {
            // If something went wrong when placing the OTA call,
            // the screen is not updated by the call disconnect
            // handler and we have to do it here
            SetSpeaker(FALSE);
            // mInCallScreen.handleOtaCallEnd();
        }
    }
}

void OtaUtils::OnClickOtaSpeakerButton()
{
    if (DBG) log(String("OTA Speaker button Clicked!"));
    if (!mApplication->mCdmaOtaProvisionData->mInOtaSpcState) {
        Boolean isChecked = !PhoneUtils::IsSpeakerOn(mContext);
        SetSpeaker(isChecked);
    }
}

void OtaUtils::OnClickOtaActivateButton()
{
    if (DBG) Log(String("Call Activation Clicked!"));
    OtaPerformActivation();
}

void OtaUtils::OnClickOtaActivateSkipButton()
{
    if (DBG) Log(String("Activation Skip Clicked!"));
    AutoPtr<IDialogInterfaceOnKeyListener> keyListener = MyDialogInterfaceOnKeyListener2();

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(NULL /* mInCallScreen */, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R.string.ota_skip_activation_dialog_title);
    builder->SetMessage(R.string.ota_skip_activation_dialog_message);

    AutoPtr<IAlertDialogOnClickListener> listener = new MyAlertDialogOnClickListener();
    builder->SetPositiveButton(
            android.R.string.ok,
            // "OK" means "skip activation".
            listener);
    builder->SetNegativeButton(
                android.R.string.cancel,
                // "Cancel" means just dismiss the dialog.
                // Don't actually start an activation call.
                NULL)
    builder->SetOnKeyListener(keyListener);
    builder->Create((IAlertDialog**)&(mOtaWidgetData->mOtaSkipConfirmationDialog));
    mOtaWidgetData->mOtaSkipConfirmationDialog->Show();
}

void OtaUtils::OnClickOtaActivateNextButton()
{
    if (DBG) Log(String("Dialog Next Clicked!"));
    if (!mApplication->mCdmaOtaProvisionData->mInOtaSpcState) {
        mApplication->mCdmaOtaScreenState->mOtaScreenState =
                ICdmaOtaScreenStateOtaScreenState_OTA_STATUS_UNDEFINED;
        OtaShowHome();
    }
}

ECode OtaUtils::DismissAllOtaDialogs()
{
    if (mOtaWidgetData != NULL) {
        if (mOtaWidgetData->mSpcErrorDialog != NULL) {
            if (DBG) Log(String("- DISMISSING mSpcErrorDialog."));
            mOtaWidgetData->mSpcErrorDialog->Dismiss();
            mOtaWidgetData->mSpcErrorDialog = NULL;
        }
        if (mOtaWidgetData->mOtaFailureDialog != NULL) {
            if (DBG) Log(String("- DISMISSING mOtaFailureDialog."));
            mOtaWidgetData->mOtaFailureDialog->Dismiss();
            mOtaWidgetData->mOtaFailureDialog = NULL;
        }
    }
    return NOERROR;
}

Int32 OtaUtils::GetOtaSpcDisplayTime()
{
    if (DBG) Log(String("getOtaSpcDisplayTime()..."));
    Int32 tmpSpcTime = 1;
    if (mApplication->mCdmaOtaProvisionData->mInOtaSpcState) {
        Int64 tmpOtaSpcRunningTime = 0;
        Int64 tmpOtaSpcLeftTime = 0;
        tmpOtaSpcRunningTime = SystemClock::GetElapsedRealtime();
        tmpOtaSpcLeftTime =
            tmpOtaSpcRunningTime - mApplication->mCdmaOtaProvisionData->mOtaSpcUptime;
        if (tmpOtaSpcLeftTime >= OTA_SPC_TIMEOUT * 1000) {
            tmpSpcTime = 1;
        }
        else {
            tmpSpcTime = OTA_SPC_TIMEOUT - (Int32)tmpOtaSpcLeftTime / 1000;
        }
    }
    if (DBG) Log(String("getOtaSpcDisplayTime(), time for SPC error notice: ") +
            StringUtils::ToString(tmpSpcTime));
    return tmpSpcTime;
}

void OtaUtils::InitOtaInCallScreen()
{
    if (DBG) Log(String("initOtaInCallScreen()..."));
    // mOtaWidgetData.otaTitle = (TextView) mInCallScreen.findViewById(R.id.otaTitle);
    // mOtaWidgetData.otaTextActivate = (TextView) mInCallScreen.findViewById(R.id.otaActivate);
    mOtaWidgetData.otaTextActivate->SetVisibility(IView::GONE);
    // mOtaWidgetData.otaTextListenProgress =
    //         (TextView) mInCallScreen.findViewById(R.id.otaListenProgress);
    // mOtaWidgetData.otaTextProgressBar =
    //         (ProgressBar) mInCallScreen.findViewById(R.id.progress_large);
    mOtaWidgetData.otaTextProgressBar->SetIndeterminate(TRUE);
    // mOtaWidgetData.otaTextSuccessFail =
    //         (TextView) mInCallScreen.findViewById(R.id.otaSuccessFailStatus);

    // mOtaWidgetData.otaUpperWidgets =
    //         (ViewGroup) mInCallScreen.findViewById(R.id.otaUpperWidgets);
    // mOtaWidgetData.callCardOtaButtonsListenProgress =
    //         (View) mInCallScreen.findViewById(R.id.callCardOtaListenProgress);
    // mOtaWidgetData.callCardOtaButtonsActivate =
    //         (View) mInCallScreen.findViewById(R.id.callCardOtaActivate);
    // mOtaWidgetData.callCardOtaButtonsFailSuccess =
    //         (View) mInCallScreen.findViewById(R.id.callCardOtaFailOrSuccessful);

    // mOtaWidgetData.otaEndButton = (Button) mInCallScreen.findViewById(R.id.otaEndButton);
    // mOtaWidgetData.otaEndButton.setOnClickListener(mInCallScreen);
    // mOtaWidgetData.otaSpeakerButton =
    //         (ToggleButton) mInCallScreen.findViewById(R.id.otaSpeakerButton);
    // mOtaWidgetData.otaSpeakerButton.setOnClickListener(mInCallScreen);
    // mOtaWidgetData.otaActivateButton =
    //         (Button) mInCallScreen.findViewById(R.id.otaActivateButton);
    // mOtaWidgetData.otaActivateButton.setOnClickListener(mInCallScreen);
    // mOtaWidgetData.otaSkipButton = (Button) mInCallScreen.findViewById(R.id.otaSkipButton);
    // mOtaWidgetData.otaSkipButton.setOnClickListener(mInCallScreen);
    // mOtaWidgetData.otaNextButton = (Button) mInCallScreen.findViewById(R.id.otaNextButton);
    // mOtaWidgetData.otaNextButton.setOnClickListener(mInCallScreen);
    // mOtaWidgetData.otaTryAgainButton =
    //         (Button) mInCallScreen.findViewById(R.id.otaTryAgainButton);
    // mOtaWidgetData.otaTryAgainButton.setOnClickListener(mInCallScreen);

    // mOtaWidgetData.otaDtmfDialerView =
    //         (DTMFTwelveKeyDialerView) mInCallScreen.findViewById(R.id.otaDtmfDialerView);
    // Sanity-check: the otaDtmfDialerView widget should *always* be present.
    // if (mOtaWidgetData.otaDtmfDialerView == null) {
    //     throw new IllegalStateException("initOtaInCallScreen: couldn't find otaDtmfDialerView");
    // }

    // Create a new DTMFTwelveKeyDialer instance purely for use by the
    // DTMFTwelveKeyDialerView ("otaDtmfDialerView") that comes from
    // otacall_card.xml.
    // mOtaCallCardDtmfDialer = new DTMFTwelveKeyDialer(mInCallScreen,
    //                                                  mOtaWidgetData.otaDtmfDialerView);

    // Initialize the new DTMFTwelveKeyDialer instance.  This is
    // needed to play local DTMF tones.
    // mOtaCallCardDtmfDialer.startDialerSession();

    // mOtaWidgetData.otaDtmfDialerView.setDialer(mOtaCallCardDtmfDialer);
}

ECode OtaUtils::CleanOtaScreen(
    /* [in] */ Boolean disableSpeaker)
{
    if (DBG) Log(String("OTA ends, cleanOtaScreen!"));

    mApplication->mCdmaOtaScreenState->mOtaScreenState =
            ICdmaOtaScreenStateOtaScreenState_OTA_STATUS_UNDEFINED;
    mApplication->mCdmaOtaProvisionData->mIsOtaCallCommitted = FALSE;
    mApplication->mCdmaOtaProvisionData->mIsOtaCallIntentProcessed = FALSE;
    mApplication->mCdmaOtaProvisionData->mInOtaSpcState = FALSE;
    mApplication->mCdmaOtaProvisionData->mActivationCount = 0;
    mApplication->mCdmaOtaProvisionData->mOtaSpcUptime = 0;
    mApplication->mCdmaOtaInCallScreenUiState->mState = State.UNDEFINED;

    if (mInteractive && (mOtaWidgetData != NULL)) {
        // if (mInCallTouchUi != null) mInCallTouchUi.setVisibility(View.VISIBLE);
        // if (mCallCard != null) {
        //     mCallCard.setVisibility(View.VISIBLE);
        //     mCallCard.hideCallCardElements();
        // }

        // Free resources from the DTMFTwelveKeyDialer instance we created
        // in initOtaInCallScreen().
        // if (mOtaCallCardDtmfDialer != null) {
        //     mOtaCallCardDtmfDialer.stopDialerSession();
        // }

        mOtaWidgetData->mOtaTextActivate->SetVisibility(IView::GONE);
        mOtaWidgetData->mOtaTextListenProgress->SetVisibility(IView::GONE);
        mOtaWidgetData->mOtaTextProgressBar->SetVisibility(IView::GONE);
        mOtaWidgetData->mOtaTextSuccessFail->SetVisibility(IView::GONE);
        mOtaWidgetData->mCallCardOtaButtonsActivate->SetVisibility(IView::GONE);
        mOtaWidgetData->mCallCardOtaButtonsListenProgress->SetVisibility(IView::GONE);
        mOtaWidgetData->mCallCardOtaButtonsFailSuccess->SetVisibility(IView::GONE);
        mOtaWidgetData->mOtaUpperWidgets->SetVisibility(IView::GONE);
        // mOtaWidgetData.otaDtmfDialerView.setVisibility(View.GONE);
        mOtaWidgetData->mOtaNextButton->SetVisibility(IView::GONE);
        mOtaWidgetData->mOtaTryAgainButton->SetVisibility(IView::GONE);
    }

    // turn off the speaker in case it was turned on
    // but the OTA call could not be completed
    if (disableSpeaker) {
        SetSpeaker(FALSE);
    }
    return NOERROR;
}

ECode OtaUtils::SetCdmaOtaInCallScreenUiState(
    /* [in] */ ICdmaOtaInCallScreenUiStateState state)
{
    if (DBG) Log(String("setCdmaOtaInCallScreenState: ") + StringUtils::ToString(state));
    mApplication->mCdmaOtaInCallScreenUiState->mState = state;
    return NOERROR;
}

ECode OtaUtils::GetCdmaOtaInCallScreenUiState(
        /* [out] */ ICdmaOtaInCallScreenUiStateState* state)
{
    VALIDATE_NOT_NULL(state)

    if (DBG) Log(String("getCdmaOtaInCallScreenState: ")
            + StringUtils::ToString(mApplication->mCdmaOtaInCallScreenUiState->mState));
    *state = mApplication->mCdmaOtaInCallScreenUiState->mState;
    return NOERROR;
}

String OtaUtils::OtaProvisionStatusToString(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IPhone::CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED:
            return String("SPL_UNLOCKED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED:
            return String("SPC_RETRIES_EXCEEDED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED:
            return String("A_KEY_EXCHANGED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_SSD_UPDATED:
            return String("SSD_UPDATED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED:
            return String("NAM_DOWNLOADED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED:
            return String("MDN_DOWNLOADED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED:
            return String("IMSI_DOWNLOADED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED:
            return String("PRL_DOWNLOADED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_COMMITTED:
            return String("COMMITTED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED:
            return String("OTAPA_STARTED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED:
            return String("OTAPA_STOPPED");
        case IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_ABORTED:
            return String("OTAPA_ABORTED");
        default:
            StringBuilder sb;
            sb += "<unknown status";
            sb += status;
            sb += ">";
            return sb.ToString();
    }
}

Int32 OtaUtils::GetLteOnCdmaMode(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj);
    // If the telephony manager is not available yet, or if it doesn't know the answer yet,
    // try falling back on the system property that may or may not be there
    Int32 mode;
    if (telephonyManager == NULL
            || (telephonyManager->GetLteOnCdmaMode(&mode), mode)
            == IPhoneConstants::LTE_ON_CDMA_UNKNOWN) {

        AutoPtr<ISystemProperties> helper;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
        Int32 tmp;
        helper->GetInt32(ITelephonyProperties::PROPERTY_LTE_ON_CDMA_DEVICE,
                IPhoneConstants::LTE_ON_CDMA_UNKNOWN, &tmp);
        return tmp;
    }
    Int32 tmp;
    telephonyManager->GetLteOnCdmaMode(&tmp);
    return tmp;
}

void OtaUtils::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOG_TAG, msg);
}

void OtaUtils::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOG_TAG, msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos