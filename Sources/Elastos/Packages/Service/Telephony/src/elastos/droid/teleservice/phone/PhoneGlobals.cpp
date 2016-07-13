
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "elastos/droid/teleservice/phone/BluetoothManager.h"
#include "elastos/droid/teleservice/phone/CallLogger.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsPhoneAppBroadcastReceiver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.Core.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUpdateLock;
using Elastos::Droid::Os::CUpdateLock;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::ICallManagerHelper;
using Elastos::Droid::Internal::Telephony::CCallManagerHelper;
using Elastos::Droid::Internal::Telephony::ICallState_DIALING;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::CTelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_IDLE;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_OFFHOOK;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_RINGING;
using Elastos::Droid::Telephony::IServiceStateHelper;
using Elastos::Droid::Telephony::CServiceStateHelper;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

ECode PhoneGlobals::NotificationBroadcastReceiver::constructor(
    /* [in] */ IPhoneGlobals* host)
{
    mHost = (PhoneGlobals*)host;
    return BroadcastReceiver::constructor();
}

ECode PhoneGlobals::NotificationBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    // TODO: use "if (VDBG)" here.
    Logger::D(IPhoneGlobals::TAG, "Broadcast from Notification: %s" ,action.string());

    if (action.Equals(IPhoneGlobals::ACTION_HANG_UP_ONGOING_CALL)) {
        AutoPtr<PhoneGlobals> globals;
        PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
        PhoneUtils::Hangup(globals->mCM);
    }
    else {
        Logger::W(IPhoneGlobals::TAG, "Received hang-up request from notification,"
                " but there's no call the system can hang up.");
    }
    return NOERROR;
}

ECode PhoneGlobals::PhoneAppBroadcastReceiver::constructor(
    /* [in] */ IPhoneGlobals* host)
{
    mHost = (PhoneGlobals*)host;
    return BroadcastReceiver::constructor();
}

ECode PhoneGlobals::PhoneAppBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_AIRPLANE_MODE_CHANGED)) {
        AutoPtr<IContentResolver> contentResolver;
        mHost->GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ISettingsSystem> system;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&system);
        Int32 value;
        system->GetInt32(contentResolver, ISettingsSystem::AIRPLANE_MODE_ON, 0, &value);
        Boolean enabled = (value == 0);
        mHost->mPhone->SetRadioPower(enabled);
    }
    else if (action.Equals(ITelephonyIntents::ACTION_ANY_DATA_CONNECTION_STATE_CHANGED)) {
        if (VDBG) Logger::D(IPhoneGlobals::TAG, "mReceiver: ACTION_ANY_DATA_CONNECTION_STATE_CHANGED");
        if (VDBG) {
            String str;
            intent->GetStringExtra(IPhoneConstants::STATE_KEY, &str);
            Logger::D(IPhoneGlobals::TAG, "- state: %s", str.string());
        }
        if (VDBG) {
            String str;
            intent->GetStringExtra(IPhoneConstants::STATE_CHANGE_REASON_KEY, &str);
            Logger::D(IPhoneGlobals::TAG, "- reason: %s", str.string());
        }

        // The "data disconnected due to roaming" notification is shown
        // if (a) you have the "data roaming" feature turned off, and
        // (b) you just lost data connectivity because you're roaming.
        String str;
        intent->GetStringExtra(IPhoneConstants::STATE_KEY, &str);
        String str2;
        intent->GetStringExtra(IPhoneConstants::STATE_CHANGE_REASON_KEY, &str2);
        Boolean res;
        Boolean disconnectedDueToRoaming =
                (mHost->mPhone->GetDataRoamingEnabled(&res), !res)
                && str.Equals("DISCONNECTED")
                && IPhone::REASON_ROAMING_ON.Equals(str2);
        mHost->mHandler->SendEmptyMessage(disconnectedDueToRoaming
                                  ? EVENT_DATA_ROAMING_DISCONNECTED
                                  : EVENT_DATA_ROAMING_OK, &res);
    }
    else if ((action.Equals(ITelephonyIntents::ACTION_SIM_STATE_CHANGED)) &&
            (mHost->mPUKEntryActivity != NULL)) {
        // if an attempt to un-PUK-lock the device was made, while we're
        // receiving this state change notification, notify the handler.
        // NOTE: This is ONLY triggered if an attempt to un-PUK-lock has
        // been attempted.
        String str;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        AutoPtr<IMessage> m;
        mHost->mHandler->ObtainMessage(EVENT_SIM_STATE_CHANGED, TO_IINTERFACE(cchar), (IMessage**)&m);
        Boolean res;
        mHost->mHandler->SendMessage(m, &res);
    }
    else if (action.Equals(ITelephonyIntents::ACTION_RADIO_TECHNOLOGY_CHANGED)) {
        String newPhone;
        intent->GetStringExtra(IPhoneConstants::PHONE_NAME_KEY, &newPhone);
        Logger::D(IPhoneGlobals::TAG, "Radio technology switched. Now %s is active.", newPhone.string());
        mHost->InitForNewRadioTechnology();
    }
    else if (action.Equals(ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED)) {
        mHost->HandleServiceStateChanged(intent);
    }
    else if (action.Equals(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED)) {
        Boolean res;
        AutoPtr<ITelephonyCapabilities> helper;
        CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
        helper->SupportsEcm(mHost->mPhone, &res);
        if (res) {
            Logger::D(IPhoneGlobals::TAG, "Emergency Callback Mode arrived in PhoneApp.");
            // Start Emergency Callback Mode service
            Boolean res;
            if (intent->GetBooleanExtra(String("phoneinECMState"), FALSE, &res), res) {
                AutoPtr<IIntent> intent;
                CIntent::New(context, ECLSID_CEmergencyCallbackModeService, (IIntent**)&intent);
                AutoPtr<IComponentName> name;
                context->StartService(intent, (IComponentName**)&name);
            }
        }
        else {
            // It doesn't make sense to get ACTION_EMERGENCY_CALLBACK_MODE_CHANGED
            // on a device that doesn't support ECM in the first place.
            String name;
            mHost->mPhone->GetPhoneName(&name);
            Logger::E(IPhoneGlobals::TAG, "Got ACTION_EMERGENCY_CALLBACK_MODE_CHANGED, "
                    "but ECM isn't supported for phone: %s", name.string());
        }
    }
    else if (action.Equals(IIntent::ACTION_DOCK_EVENT)) {
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
                IIntent::EXTRA_DOCK_STATE_UNDOCKED, &mDockState);
        if (VDBG) Logger::D(IPhoneGlobals::TAG, "ACTION_DOCK_EVENT -> mDockState = %d", mDockState);
        AutoPtr<IMessage> message;
        mHost->mHandler->ObtainMessage(EVENT_DOCK_STATE_CHANGED, 0, (IMessage**)&message);
        Boolean res;
        mHost->mHandler->SendMessage(message, &res);
    }
    return NOERROR;
}

PhoneGlobals::MyHandler::MyHandler(
    /* [in] */ PhoneGlobals* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode PhoneGlobals::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    PhoneConstantsState phoneState;

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        // Starts the SIP service. It's a no-op if SIP API is not supported
        // on the deivce.
        // TODO: Having the phone process host the SIP service is only
        // temporary. Will move it to a persistent communication process
        // later.
        case EVENT_START_SIP_SERVICE:
        {
            AutoPtr<IContext> ctx;
            mHost->GetApplicationContext((IContext**)&ctx);
            Logger::E("PhoneGlobals", "TODO MyHandler::HandleMessage SipService is not ready!");
            //assert(0);
            //SipService::Start(ctx);
            break;
        }
        // TODO: This event should be handled by the lock screen, just
        // like the "SIM missing" and "Sim locked" cases (bug 1804111).
        case EVENT_SIM_NETWORK_LOCKED:
        {
            AutoPtr<IResources> resources;
            mHost->GetResources((IResources**)&resources);
            Boolean res;
            if (resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::ignore_sim_network_locked_events, &res), res) {
                // Some products don't have the concept of a "SIM network lock"
                Logger::I(IPhoneGlobals::TAG, "Ignoring EVENT_SIM_NETWORK_LOCKED event; "
                        "not showing 'SIM network unlock' PIN entry screen");
            }
            else {
                // Normal case: show the "SIM network unlock" PIN entry screen.
                // The user won't be able to do anything else until
                // they enter a valid SIM network PIN.
                Logger::I(IPhoneGlobals::TAG, "show sim depersonal panel");
                AutoPtr<PhoneGlobals> globals;
                PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
                assert(0);
                // AutoPtr<IccNetworkDepersonalizationPanel> ndpPanel;
                // CIccNetworkDepersonalizationPanel::New(globals, (IIccNetworkDepersonalizationPanel**)&&ndpPanel);
                // ndpPanel->Show();
            }
            break;
        }
        case EVENT_DATA_ROAMING_DISCONNECTED:
            mHost->mNotificationMgr->ShowDataDisconnectedRoaming();
            break;
        case EVENT_DATA_ROAMING_OK:
            mHost->mNotificationMgr->HideDataDisconnectedRoaming();
            break;

        case IPhoneGlobals::MMI_COMPLETE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IObject::Probe(obj);
            mHost->OnMMIComplete(result);
            break;
        }
        case IPhoneGlobals::MMI_CANCEL:
            PhoneUtils::CancelMmiCode(mHost->mPhone);
            break;

        case EVENT_SIM_STATE_CHANGED:
        {
            // Marks the event where the SIM goes into ready state.
            // Right now, this is only used for the PUK-unlocking
            // process.
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
            String str;
            cchar->ToString(&str);
            if (str.Equals(IIccCardConstants::INTENT_VALUE_ICC_READY)) {
                // when the right event is triggered and there
                // are UI objects in the foreground, we close
                // them to display the lock panel.
                if (mHost->mPUKEntryActivity != NULL) {
                    mHost->mPUKEntryActivity->Finish();
                    mHost->mPUKEntryActivity = NULL;
                }
                if (mHost->mPUKEntryProgressDialog != NULL) {
                    IDialogInterface::Probe(mHost->mPUKEntryProgressDialog)->Dismiss();
                    mHost->mPUKEntryProgressDialog = NULL;
                }
            }
            break;
        }
        case EVENT_UNSOL_CDMA_INFO_RECORD:
            //TODO: handle message here;
            break;

        case EVENT_DOCK_STATE_CHANGED:
        {
            // If the phone is docked/undocked during a call, and no wired or BT headset
            // is connected: turn on/off the speaker accordingly.
            Boolean inDockMode = FALSE;
            if (mDockState != IIntent::EXTRA_DOCK_STATE_UNDOCKED) {
                inDockMode = TRUE;
            }
            if (VDBG) Logger::D(IPhoneGlobals::TAG, "received EVENT_DOCK_STATE_CHANGED. Phone inDock = %d"
                    , inDockMode);

            mHost->mCM->GetState(&phoneState);
            Boolean res;
            if (phoneState == PhoneConstantsState_OFFHOOK &&
                    (mHost->mBluetoothManager->IsBluetoothHeadsetAudioOn(&res), !res)) {
                AutoPtr<IContext> context;
                mHost->GetApplicationContext((IContext**)&context);
                PhoneUtils::TurnOnSpeaker(context, inDockMode, TRUE);
            }
            break;
        }
    }
    return NOERROR;
}

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);

    return (IPhoneGlobals::DBG_LEVEL >= 1) && (value == 1);
}

const Boolean PhoneGlobals::DBG = initDBG();

const Boolean PhoneGlobals::VDBG = (IPhoneGlobals::DBG_LEVEL >= 2);

AutoPtr<PhoneGlobals> PhoneGlobals::sMe;

Int32 PhoneGlobals::mDockState = IIntent::EXTRA_DOCK_STATE_UNDOCKED;
Boolean PhoneGlobals::sVoiceCapable = TRUE;

const String PhoneGlobals::DEFAULT_CALL_ORIGIN_PACKAGE("com.android.dialer");
const String PhoneGlobals::ALLOWED_EXTRA_CALL_ORIGIN("com.android.dialer.DialtactsActivity");

const Int64 PhoneGlobals::CALL_ORIGIN_EXPIRATION_MILLIS = 30 * 1000;

CAR_INTERFACE_IMPL(PhoneGlobals, ContextWrapper, IPhoneGlobals)

ECode PhoneGlobals::SetRestoreMuteOnInCallResume(
    /* [in] */ Boolean mode)
{
    mShouldRestoreMuteOnInCallResume = mode;
    return NOERROR;
}

PhoneGlobals::PhoneGlobals(
    /* [in] */ IContext* context)
    : mIsSimPinEnabled(FALSE)
    , mCachedSimPin(NULL)
    , mBeginningCall(FALSE)
    , mLastPhoneState(PhoneConstantsState_IDLE)
    , mWakeState(PhoneGlobalsWakeState_SLEEP)
    , mShouldRestoreMuteOnInCallResume(FALSE)
{
    CPhoneGlobalsPhoneAppBroadcastReceiver::New((IPhoneGlobals*)this, (IBroadcastReceiver**)&mReceiver);

    mHandler = new MyHandler(this);

    ContextWrapper::constructor(context);
    sMe = this;
}

ECode PhoneGlobals::OnCreate()
{
    if (VDBG) Logger::V(IPhoneGlobals::TAG, "onCreate()...");

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);

    // Cache the "voice capable" flag.
    // This flag currently comes from a resource (which is
    // overrideable on a per-product basis):
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &sVoiceCapable);


    // ...but this might eventually become a PackageManager "system
    // feature" instead, in which case we'd do something like:
    // sVoiceCapable =
    //   getPackageManager().hasSystemFeature(PackageManager.FEATURE_TELEPHONY_VOICE_CALLS);

    if (mPhone == NULL) {
        // Initialize the telephony framework
        AutoPtr<IPhoneFactory> helper;
        CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
        helper->MakeDefaultPhones((IContext*)this);

        // Get the default phone
        helper->GetDefaultPhone((IPhone**)&mPhone);

        // Start TelephonyDebugService After the default phone is created.
        AutoPtr<IIntent> intent;
        CIntent::New(this, ECLSID_CTelephonyDebugService, (IIntent**)&intent);
        AutoPtr<IComponentName> name;
        StartService(intent, (IComponentName**)&name);

        Boolean res = FALSE;
        //Logger::D(TAG, "TODO Need CallManager::GetInstance");
        AutoPtr<ICallManagerHelper> cmHelper;
        CCallManagerHelper::AcquireSingleton((ICallManagerHelper**)&cmHelper);
        cmHelper->GetInstance((ICallManager**)&mCM);
        mCM->RegisterPhone(mPhone, &res);

        // Create the NotificationMgr singleton, which is used to display
        // status bar icons and control other status bar behavior.
        //Logger::D(TAG, "TODO Need NotificationMgr::Init");
        mNotificationMgr = NotificationMgr::Init(this);

        mHandler->SendEmptyMessage(EVENT_START_SIP_SERVICE, &res);

        Int32 phoneType;
        mPhone->GetPhoneType(&phoneType);

        Logger::D(TAG, "OnCreate phoneType: %d", phoneType);
        if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
            // Create an instance of CdmaPhoneCallState and initialize it to IDLE
            mCdmaPhoneCallState = new CdmaPhoneCallState();
            mCdmaPhoneCallState->CdmaPhoneCallStateInit();
        }

        // before registering for phone state changes
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
        mPowerManager = IPowerManager::Probe(obj);
        mPowerManager->NewWakeLock(IPowerManager::FULL_WAKE_LOCK, IPhoneGlobals::TAG,
                (IPowerManagerWakeLock**)&mWakeLock);
        // lock used to keep the processor awake, when we don't care for the display.
        mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK
                | IPowerManager::ON_AFTER_RELEASE, IPhoneGlobals::TAG, (IPowerManagerWakeLock**)&mPartialWakeLock);

        AutoPtr<IInterface> obj2;
        GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&obj2);
        mKeyguardManager = IKeyguardManager::Probe(obj2);

        // get a handle to the service so that we can use it later when we
        // want to set the poke lock.
        AutoPtr<IInterface> obj3 = ServiceManager::GetService(String("power"));
        mPowerManagerService = IIPowerManager::Probe(obj3);

        // Get UpdateLock to suppress system-update related events (e.g. dialog show-up)
        // during phone calls.
        CUpdateLock::New(String("phone"), (IUpdateLock**)&mUpdateLock);

        if (DBG) Logger::D(IPhoneGlobals::TAG, "onCreate: mUpdateLock: %s", TO_CSTR(mUpdateLock));

#if 1
        Logger::D(TAG, "TODO Need CallLogAsync");
        //TODO AutoPtr<ICallLogAsync> callLogAsync;
        //TODO CCallLogAsync::New((ICallLogAsync**)&callLogAsync);
        AutoPtr<CallLogger> callLogger = new CallLogger(this/*TODO , callLogAsync*/);

        mCallGatewayManager = CallGatewayManager::GetInstance();

        // Create the CallController singleton, which is the interface
        // to the telephony layer for user-initiated telephony functionality
        // (like making outgoing calls.)
        //Logger::D(TAG, "TODO Need CallLogAsync");
        CallController::Init(this, callLogger, mCallGatewayManager, (CallController**)&mCallController);

        // Create the CallerInfoCache singleton, which remembers custom ring tone and
        // send-to-voicemail settings.
        //
        // The asynchronous caching will start just after this call.
        mCallerInfoCache = CallerInfoCache::Init(this);

        // Monitors call activity from the telephony layer
        mCallStateMonitor = new CallStateMonitor(mCM);

        // Bluetooth manager
        mBluetoothManager = new BluetoothManager();

        mPhoneMgr = CPhoneInterfaceManager::Init(this, mPhone);

        // Create the CallNotifer singleton, which handles
        // asynchronous events from the telephony layer (like
        // launching the incoming-call UI when an incoming call comes
        // in.)
        //Logger::D(TAG, "TODO Need callLogger");
         mNotifier = CallNotifier::Init(this, mPhone, callLogger, mCallStateMonitor,
                 mBluetoothManager);
#endif

        // register for ICC status
        AutoPtr<IIccCard> sim;
        mPhone->GetIccCard((IIccCard**)&sim);
        if (sim != NULL) {
            if (VDBG) Logger::V(IPhoneGlobals::TAG, "register for ICC status");
            Logger::D(TAG, "TODO IccCard is not ready!");
            //TODO sim->RegisterForPersoLocked(mHandler, EVENT_SIM_NETWORK_LOCKED, NULL);
        }

        // register for MMI/USSD
        mCM->RegisterForMmiComplete(mHandler, MMI_COMPLETE, NULL);

        // register connection tracking to PhoneUtils
        PhoneUtils::InitializeConnectionHandler(mCM);

        // Register for misc other intent broadcasts.
        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntentFilter**)&intentFilter);
        intentFilter->AddAction(ITelephonyIntents::ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
        intentFilter->AddAction(IIntent::ACTION_DOCK_EVENT);
        intentFilter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
        intentFilter->AddAction(ITelephonyIntents::ACTION_RADIO_TECHNOLOGY_CHANGED);
        intentFilter->AddAction(ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED);
        intentFilter->AddAction(ITelephonyIntents::ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
        AutoPtr<IIntent> tmp;
        RegisterReceiver(mReceiver, intentFilter, (IIntent**)&tmp);

        //set the default values for the preferences in the phone.
        AutoPtr<IPreferenceManagerHelper> managerHelper;
        CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&managerHelper);
        managerHelper->SetDefaultValues(this,
                Elastos::Droid::TeleService::R::xml::network_setting, FALSE);

        managerHelper->SetDefaultValues(this,
                Elastos::Droid::TeleService::R::xml::call_feature_setting, FALSE);

        // Make sure the audio mode (along with some
        // audio-mode-related state of our own) is initialized
        // correctly, given the current state of the phone.
        PhoneUtils::SetAudioMode(mCM);
    }

    mCdmaOtaProvisionData = new OtaUtils::CdmaOtaProvisionData();
    mCdmaOtaConfigData = new OtaUtils::CdmaOtaConfigData();
    mCdmaOtaScreenState = new OtaUtils::CdmaOtaScreenState();
    mCdmaOtaInCallScreenUiState = new OtaUtils::CdmaOtaInCallScreenUiState();

    // XXX pre-load the SimProvider so that it's ready
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> _data;
    helper->Parse(String("content://icc/adn"), (IUri**)&_data);
    String tmp;
    resolver->GetType(_data, &tmp);

    // start with the default value to set the mute state.
    mShouldRestoreMuteOnInCallResume = FALSE;

    // TODO: Register for Cdma Information Records
    // phone.registerCdmaInformationRecord(mHandler, EVENT_UNSOL_CDMA_INFO_RECORD, null);

    // Read HAC settings and configure audio hardware
    Boolean res;
    resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::hac_enabled, &res);
    if (res) {
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsSystem> helper;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);

        Int32 hac;
        helper->GetInt32(contentResolver, ISettingsSystem::HEARING_AID, 0, &hac);

        AutoPtr<IInterface> obj;
        GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
        AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
        audioManager->SetParameter(ICallFeaturesSetting::HAC_KEY, hac != 0 ?
                ICallFeaturesSetting::HAC_VAL_ON :
                ICallFeaturesSetting::HAC_VAL_OFF);
    }
    return NOERROR;
}

ECode PhoneGlobals::GetInstance(
    /* [out] */ PhoneGlobals** global)
{
    VALIDATE_NOT_NULL(global)
    if (sMe == NULL) {
        //throw new IllegalStateException("No PhoneGlobals here!");
        Logger::E(IPhoneGlobals::TAG, "No PhoneGlobals here!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *global = sMe;
    REFCOUNT_ADD(*global);
    return NOERROR;
}

AutoPtr<PhoneGlobals> PhoneGlobals::GetInstanceIfPrimary()
{
    return sMe;
}

AutoPtr<IPhone> PhoneGlobals::GetPhone()
{
    AutoPtr<PhoneGlobals> global;
    GetInstance((PhoneGlobals**)&global);
    return global->mPhone;
}

ECode PhoneGlobals::GetBluetoothManager(
    /* [out] */ IBluetoothManager** manager)
{
    VALIDATE_NOT_NULL(manager)

    *manager = mBluetoothManager;
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

ECode PhoneGlobals::GetCallManager(
    /* [out] */ ICallManager** manager)
{
    VALIDATE_NOT_NULL(manager)

    *manager = mCM;
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

AutoPtr<IPendingIntent> PhoneGlobals::CreateHangUpOngoingCallPendingIntent(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    assert(0);
    // CIntent::New(IPhoneGlobals::ACTION_HANG_UP_ONGOING_CALL, NULL,
    //         context, ECLSID_CPhoneGlobalsNotificationBroadcastReceiver, (IIntent**)&intent);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetBroadcast(context, 0, intent, 0, (IPendingIntent**)&pendingIntent);
    return pendingIntent;
}

ECode PhoneGlobals::IsSimPinEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsSimPinEnabled;
    return NOERROR;
}

ECode PhoneGlobals::AuthenticateAgainstCachedSimPin(
    /* [in] */ const String& pin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (!mCachedSimPin.IsNull() && mCachedSimPin.Equals(pin));
    return NOERROR;
}

ECode PhoneGlobals::SetCachedSimPin(
    /* [in] */ const String& pin)
{
    mCachedSimPin = pin;
    return NOERROR;
}

ECode PhoneGlobals::HandleOtaspEvent(
    /* [in] */ IMessage* msg)
{
    if (DBG) Logger::D(IPhoneGlobals::TAG, "handleOtaspEvent(message %s)...", TO_CSTR(msg));

    if (mOtaUtils == NULL) {
        // We shouldn't be getting OTASP events without ever
        // having started the OTASP call in the first place!
        Logger::W(IPhoneGlobals::TAG, "handleOtaEvents: got an event but otaUtils is null! "
                "message = %s", TO_CSTR(msg));
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    return mOtaUtils->OnOtaProvisionStatusChanged((AsyncResult*)IObject::Probe(obj));
}

ECode PhoneGlobals::HandleOtaspDisconnect()
{
    if (DBG) Logger::D(IPhoneGlobals::TAG, "handleOtaspDisconnect()...");

    if (mOtaUtils == NULL) {
        // We shouldn't be getting OTASP events without ever
        // having started the OTASP call in the first place!
        Logger::W(IPhoneGlobals::TAG, "handleOtaspDisconnect: otaUtils is null!");
        return NOERROR;
    }

    return mOtaUtils->OnOtaspDisconnect();
}

ECode PhoneGlobals::SetPukEntryActivity(
    /* [in] */ IActivity* activity)
{
    mPUKEntryActivity = activity;
    return NOERROR;
}

ECode PhoneGlobals::GetPUKEntryActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)

    *activity = mPUKEntryActivity;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode PhoneGlobals::SetPukEntryProgressDialog(
    /* [in] */ IProgressDialog* dialog)
{
    mPUKEntryProgressDialog = dialog;
    return NOERROR;
}

ECode PhoneGlobals::GetPUKEntryProgressDialog(
    /* [out] */ IProgressDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)

    *dialog = mPUKEntryProgressDialog;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

ECode PhoneGlobals::RequestWakeState(
    /* [in] */ IPhoneGlobalsWakeState ws)
{
    if (VDBG) Logger::D(IPhoneGlobals::TAG, "requestWakeState(%d)...", ws);

    {
        AutoLock syncLock(this);
        if (mWakeState != ws) {
            switch (ws) {
                case PhoneGlobalsWakeState_PARTIAL:
                {
                    // acquire the processor wake lock, and release the FULL
                    // lock if it is being held.
                    mPartialWakeLock->AcquireLock();
                    Boolean res;
                    if (mWakeLock->IsHeld(&res), res) {
                        mWakeLock->ReleaseLock();
                    }
                    break;
                }
                case PhoneGlobalsWakeState_FULL:
                {
                    // acquire the full wake lock, and release the PARTIAL
                    // lock if it is being held.
                    mWakeLock->AcquireLock();
                    Boolean res;
                    if (mPartialWakeLock->IsHeld(&res), res) {
                        mPartialWakeLock->ReleaseLock();
                    }
                    break;
                }
                case PhoneGlobalsWakeState_SLEEP:
                default:
                {
                    // release both the PARTIAL and FULL locks.
                    Boolean res;
                    if (mWakeLock->IsHeld(&res), res) {
                        mWakeLock->ReleaseLock();
                    }
                    if (mPartialWakeLock->IsHeld(&res), res) {
                        mPartialWakeLock->ReleaseLock();
                    }
                    break;
                }
            }
            mWakeState = ws;
        }
    }
    return NOERROR;
}

ECode PhoneGlobals::WakeUpScreen()
{
    {
        AutoLock syncLock(this);
        if (mWakeState == PhoneGlobalsWakeState_SLEEP) {
            if (DBG) Logger::D(IPhoneGlobals::TAG, "pulse screen lock");
            mPowerManager->WakeUp(SystemClock::GetUptimeMillis());
        }
    }
    return NOERROR;
}

ECode PhoneGlobals::UpdateWakeState()
{
    PhoneConstantsState state;
    mCM->GetState(&state);

    // True if the speakerphone is in use.  (If so, we *always* use
    // the default timeout.  Since the user is obviously not holding
    // the phone up to his/her face, we don't need to worry about
    // false touches, and thus don't need to turn the screen off so
    // aggressively.)
    // Note that we need to make a fresh call to this method any
    // time the speaker state changes.  (That happens in
    // PhoneUtils.turnOnSpeaker().)
    // Boolean isSpeakerInUse = (state == PhoneConstantsState_OFFHOOK) && PhoneUtils::IsSpeakerOn(this);

    // TODO (bug 1440854): The screen timeout *might* also need to
    // depend on the bluetooth state, but this isn't as clear-cut as
    // the speaker state (since while using BT it's common for the
    // user to put the phone straight into a pocket, in which case the
    // timeout should probably still be short.)

    // Decide whether to force the screen on or not.
    //
    // Force the screen to be on if the phone is ringing or dialing,
    // or if we're displaying the "Call ended" UI for a connection in
    // the "disconnected" state.
    // However, if the phone is disconnected while the user is in the
    // middle of selecting a quick response message, we should not force
    // the screen to be on.
    //
    Boolean isRinging = (state == PhoneConstantsState_RINGING);
    AutoPtr<ICall> call;
    mPhone->GetForegroundCall((ICall**)&call);
    ICallState _state;
    call->GetState(&_state);
    Boolean isDialing = (_state == ICallState_DIALING);
    Boolean keepScreenOn = isRinging || isDialing;
    // keepScreenOn == true means we'll hold a full wake lock:
    RequestWakeState(keepScreenOn ? PhoneGlobalsWakeState_FULL : PhoneGlobalsWakeState_SLEEP);
    return NOERROR;
}

ECode PhoneGlobals::PokeUserActivity()
{
    if (VDBG) Logger::D(IPhoneGlobals::TAG, "pokeUserActivity()...");
    return mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
}

ECode PhoneGlobals::UpdatePhoneState(
    /* [in] */ PhoneConstantsState state)
{
    if (state != mLastPhoneState) {
        mLastPhoneState = state;

        // Try to acquire or release UpdateLock.
        //
        // Watch out: we don't release the lock here when the screen is still in foreground.
        // At that time InCallScreen will release it on onPause().
        if (state != PhoneConstantsState_IDLE) {
            // UpdateLock is a recursive lock, while we may get "acquire" request twice and
            // "release" request once for a single call (RINGING + OFFHOOK and IDLE).
            // We need to manually ensure the lock is just acquired once for each (and this
            // will prevent other possible buggy situations too).
            Boolean res;
            if (mUpdateLock->IsHeld(&res), !res) {
                mUpdateLock->AcquireLock();
            }
        }
        else {
            Boolean res;
            if (mUpdateLock->IsHeld(&res), res) {
                mUpdateLock->ReleaseLock();
            }
        }
    }
    return NOERROR;
}

ECode PhoneGlobals::GetPhoneState(
    /* [out] */ PhoneConstantsState* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mLastPhoneState;
    return NOERROR;
}

ECode PhoneGlobals::GetKeyguardManager(
    /* [out] */ IKeyguardManager** manager)
{
    VALIDATE_NOT_NULL(manager)

    *manager = mKeyguardManager;
    REFCOUNT_ADD(*manager)
    return NOERROR;
}

ECode PhoneGlobals::OnMMIComplete(
    /* [in] */ AsyncResult* r)
{
    if (VDBG) Logger::D(IPhoneGlobals::TAG, "onMMIComplete()...");
    AutoPtr<IMmiCode> mmiCode = IMmiCode::Probe(r->mResult);

    AutoPtr<PhoneGlobals> global;
    FAIL_RETURN(GetInstance((PhoneGlobals**)&global))
    return PhoneUtils::DisplayMMIComplete(mPhone, global, mmiCode, NULL, NULL);
}

void PhoneGlobals::InitForNewRadioTechnology()
{
    if (DBG) Logger::D(IPhoneGlobals::TAG, "initForNewRadioTechnology...");

    Int32 type;
    mPhone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        // Create an instance of CdmaPhoneCallState and initialize it to IDLE
        mCdmaPhoneCallState = new CdmaPhoneCallState();
        mCdmaPhoneCallState->CdmaPhoneCallStateInit();
    }

    AutoPtr<ITelephonyCapabilities> helper;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
    Boolean res;
    helper->SupportsOtasp(mPhone, &res);
    if (!res) {
        //Clean up OTA data in GSM/UMTS. It is valid only for CDMA
        ClearOtaState();
    }

    mNotifier->UpdateCallNotifierRegistrationsAfterRadioTechnologyChange();
    mCallStateMonitor->UpdateAfterRadioTechnologyChange();

    // Update registration for ICC status after radio technology change
    AutoPtr<IIccCard> sim;
    mPhone->GetIccCard((IIccCard**)&sim);
    if (sim != NULL) {
        if (DBG) Logger::D(IPhoneGlobals::TAG, "Update registration for ICC status...");

        assert(0);
        //Register all events new to the new active phone
        //sim->RegisterForNetworkLocked(mHandler, EVENT_SIM_NETWORK_LOCKED, NULL);
    }
}

void PhoneGlobals::HandleServiceStateChanged(
    /* [in] */ IIntent* intent)
{
    /**
     * This used to handle updating EriTextWidgetProvider this routine
     * and and listening for ACTION_SERVICE_STATE_CHANGED intents could
     * be removed. But leaving just in case it might be needed in the near
     * future.
     */

    // If service just returned, start sending out the queued messages
    AutoPtr<IBundle> obj;
    intent->GetExtras((IBundle**)&obj);
    AutoPtr<IServiceStateHelper> helper;
    CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&helper);

    AutoPtr<IServiceState> ss;
    helper->NewFromBundle(obj, (IServiceState**)&ss);

    if (ss != NULL) {
        Int32 state;
        ss->GetState(&state);
        mNotificationMgr->UpdateNetworkSelection(state);
    }
}

ECode PhoneGlobals::IsOtaCallInActiveState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean otaCallActive = FALSE;
    if (VDBG) Logger::D(IPhoneGlobals::TAG, "- isOtaCallInActiveState %s", otaCallActive);
    *result = otaCallActive;
    return NOERROR;
}

ECode PhoneGlobals::IsOtaCallInEndState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean otaCallEnded = FALSE;
    if (VDBG) Logger::D(IPhoneGlobals::TAG, "- isOtaCallInEndState %d", otaCallEnded);
    *result = otaCallEnded;
    return NOERROR;
}

ECode PhoneGlobals::ClearOtaState()
{
    if (DBG) Logger::D(IPhoneGlobals::TAG, "- clearOtaState ...");
    if (mOtaUtils != NULL) {
        mOtaUtils->CleanOtaScreen(TRUE);
        if (DBG) Logger::D(IPhoneGlobals::TAG, "  - clearOtaState clears OTA screen");
    }
    return NOERROR;
}

ECode PhoneGlobals::DismissOtaDialogs()
{
    if (DBG) Logger::D(IPhoneGlobals::TAG, "- dismissOtaDialogs ...");
    if (mOtaUtils != NULL) {
        mOtaUtils->DismissAllOtaDialogs();
        if (DBG) Logger::D(IPhoneGlobals::TAG, "  - dismissOtaDialogs clears OTA dialogs");
    }
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
