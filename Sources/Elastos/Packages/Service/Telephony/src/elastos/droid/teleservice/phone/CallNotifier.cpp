
#include "elastos/droid/teleservice/phone/CallNotifier.h"
#include "elastos/droid/teleservice/phone/CdmaDisplayInfo.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "elastos/droid/os/AsyncResult.h"
#include "_Elastos.Droid.Core.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringBuilder.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState_ACTIVE;
using Elastos::Droid::Internal::Telephony::ICallState_ALERTING;
using Elastos::Droid::Internal::Telephony::ICallState_DIALING;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_OFFHOOK;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_IDLE;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::CTelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaDisplayInfoRec;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::CDisconnectCause;
using Elastos::Droid::Telephony::EIID_IPhoneStateListener;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CallNotifier::MyPhoneStateListener, Object, IPhoneStateListener)

ECode CallNotifier::MyPhoneStateListener::OnMessageWaitingIndicatorChanged(
    /* [in] */ Boolean mwi)
{
    mHost->OnMwiChanged(mwi);
    return NOERROR;
}

//@Override
ECode CallNotifier::MyPhoneStateListener::OnCallForwardingIndicatorChanged(
    /* [in] */ Boolean cfi)
{
    mHost->OnCfiChanged(cfi);
    return NOERROR;
}

const Int32 CallNotifier::InCallTonePlayer::TONE_NONE = 0;
const Int32 CallNotifier::InCallTonePlayer::TONE_CALL_WAITING = 1;
const Int32 CallNotifier::InCallTonePlayer::TONE_BUSY = 2;
const Int32 CallNotifier::InCallTonePlayer::TONE_CONGESTION = 3;
const Int32 CallNotifier::InCallTonePlayer::TONE_CALL_ENDED = 4;
const Int32 CallNotifier::InCallTonePlayer::TONE_VOICE_PRIVACY = 5;
const Int32 CallNotifier::InCallTonePlayer::TONE_REORDER = 6;
const Int32 CallNotifier::InCallTonePlayer::TONE_INTERCEPT = 7;
const Int32 CallNotifier::InCallTonePlayer::TONE_CDMA_DROP = 8;
const Int32 CallNotifier::InCallTonePlayer::TONE_OUT_OF_SERVICE = 9;
const Int32 CallNotifier::InCallTonePlayer::TONE_REDIAL = 10;
const Int32 CallNotifier::InCallTonePlayer::TONE_OTA_CALL_END = 11;
const Int32 CallNotifier::InCallTonePlayer::TONE_UNOBTAINABLE_NUMBER = 13;

const Int32 CallNotifier::InCallTonePlayer::TONE_RELATIVE_VOLUME_EMERGENCY = 100;
const Int32 CallNotifier::InCallTonePlayer::TONE_RELATIVE_VOLUME_HIPRI = 80;
const Int32 CallNotifier::InCallTonePlayer::TONE_RELATIVE_VOLUME_LOPRI = 50;

const Int32 CallNotifier::InCallTonePlayer::TONE_TIMEOUT_BUFFER = 20;

const Int32 CallNotifier::InCallTonePlayer::TONE_OFF = 0;
const Int32 CallNotifier::InCallTonePlayer::TONE_ON = 1;
const Int32 CallNotifier::InCallTonePlayer::TONE_STOPPED = 2;

CallNotifier::InCallTonePlayer::InCallTonePlayer(
    /* [in] */ CallNotifier* host,
    /* [in] */ Int32 toneId)
    : mHost(host)
    , mToneId(toneId)
    , mState(TONE_OFF)
{
    Thread::constructor();
}

ECode CallNotifier::InCallTonePlayer::Run()
{
    StringBuilder sb;
    sb += "InCallTonePlayer.run(toneId = ";
    sb += mToneId;
    sb += ")...";
    mHost->Log(sb.ToString());

    Int32 toneType = 0;  // passed to ToneGenerator.startTone()
    Int32 toneVolume;  // passed to the ToneGenerator constructor
    Int32 toneLengthMillis;
    AutoPtr<IPhone> phone;
    mHost->mCM->GetFgPhone((IPhone**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);

    switch (mToneId) {
        case TONE_CALL_WAITING:
            toneType = IToneGenerator::TONE_SUP_CALL_WAITING;
            toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
            // Call waiting tone is stopped by stopTone() method
            toneLengthMillis = Elastos::Core::Math::INT32_MAX_VALUE - TONE_TIMEOUT_BUFFER;
            break;
        case TONE_BUSY:
            if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
                toneType = IToneGenerator::TONE_CDMA_NETWORK_BUSY_ONE_SHOT;
                toneVolume = TONE_RELATIVE_VOLUME_LOPRI;
                toneLengthMillis = 1000;
            }
            else if (phoneType == IPhoneConstants::PHONE_TYPE_GSM
                    || phoneType == IPhoneConstants::PHONE_TYPE_SIP
                    || phoneType == IPhoneConstants::PHONE_TYPE_IMS
                    || phoneType == IPhoneConstants::PHONE_TYPE_THIRD_PARTY) {
                toneType = IToneGenerator::TONE_SUP_BUSY;
                toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 4000;
            }
            else {
                //throw new IllegalStateException("Unexpected phone type: " + phoneType);
                Logger::E("CallNotifier", "Unexpected phone type: %d", phoneType);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            break;
        case TONE_CONGESTION:
            toneType = IToneGenerator::TONE_SUP_CONGESTION;
            toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
            toneLengthMillis = 4000;
            break;

        case TONE_CALL_ENDED:
            toneType = IToneGenerator::TONE_PROP_PROMPT;
            toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
            toneLengthMillis = 200;
            break;
         case TONE_OTA_CALL_END: {
            AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)(mHost->mApplication.Get());
            if (globals->mCdmaOtaConfigData->mOtaPlaySuccessFailureTone ==
                    IOtaUtils::OTA_PLAY_SUCCESS_FAILURE_TONE_ON) {
                toneType = IToneGenerator::TONE_CDMA_ALERT_CALL_GUARD;
                toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 750;
            }
            else {
                toneType = IToneGenerator::TONE_PROP_PROMPT;
                toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 200;
            }
            break;
        }
        case TONE_VOICE_PRIVACY:
            toneType = IToneGenerator::TONE_CDMA_ALERT_NETWORK_LITE;
            toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
            toneLengthMillis = 5000;
            break;
        case TONE_REORDER:
            toneType = IToneGenerator::TONE_CDMA_REORDER;
            toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
            toneLengthMillis = 4000;
            break;
        case TONE_INTERCEPT:
            toneType = IToneGenerator::TONE_CDMA_ABBR_INTERCEPT;
            toneVolume = TONE_RELATIVE_VOLUME_LOPRI;
            toneLengthMillis = 500;
            break;
        case TONE_CDMA_DROP:
        case TONE_OUT_OF_SERVICE:
            toneType = IToneGenerator::TONE_CDMA_CALLDROP_LITE;
            toneVolume = TONE_RELATIVE_VOLUME_LOPRI;
            toneLengthMillis = 375;
            break;
        case TONE_REDIAL:
            toneType = IToneGenerator::TONE_CDMA_ALERT_AUTOREDIAL_LITE;
            toneVolume = TONE_RELATIVE_VOLUME_LOPRI;
            toneLengthMillis = 5000;
            break;
        case TONE_UNOBTAINABLE_NUMBER:
            toneType = IToneGenerator::TONE_SUP_ERROR;
            toneVolume = TONE_RELATIVE_VOLUME_HIPRI;
            toneLengthMillis = 4000;
            break;
        default:
            //throw new IllegalArgumentException("Bad toneId: " + mToneId);
            Logger::E("CallNotifier", "Bad toneId:  %d", mToneId);
            return E_ILLEGAL_STATE_EXCEPTION;
    }

    // If the mToneGenerator creation fails, just continue without it.  It is
    // a local audio signal, and is not as important.
    AutoPtr<IToneGenerator> toneGenerator;
    //try
    ECode ec = NOERROR;
    {
        Int32 stream;
        if (mHost->mBluetoothHeadset != NULL) {
            Boolean res;
            FAIL_GOTO(ec = mHost->mBluetoothHeadset->IsAudioOn(&res), ERROR)
            stream = res ? IAudioManager::STREAM_BLUETOOTH_SCO:
                IAudioManager::STREAM_VOICE_CALL;
        }
        else {
            stream = IAudioManager::STREAM_VOICE_CALL;
        }
        CToneGenerator::New(stream, toneVolume, (IToneGenerator**)&toneGenerator);
        // if (DBG) log("- created toneGenerator: " + toneGenerator);
    }
    // catch (RuntimeException e) {
ERROR:
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Logger::W(TAG,
              "InCallTonePlayer: Exception caught while creating ToneGenerator: %d", ec);
        toneGenerator = NULL;
    }
    //}

    // Using the ToneGenerator (with the CALL_WAITING / BUSY /
    // CONGESTION tones at least), the ToneGenerator itself knows
    // the right pattern of tones to play; we do NOT need to
    // manually start/stop each individual tone, or manually
    // insert the correct delay between tones.  (We just start it
    // and let it run for however long we want the tone pattern to
    // continue.)
    //
    // TODO: When we stop the ToneGenerator in the middle of a
    // "tone pattern", it sounds bad if we cut if off while the
    // tone is actually playing.  Consider adding API to the
    // ToneGenerator to say "stop at the next silent part of the
    // pattern", or simply "play the pattern N times and then
    // stop."
    Boolean needToStopTone = TRUE;
    Boolean okToPlayTone = FALSE;

    if (toneGenerator != NULL) {
        Int32 ringerMode;
        mHost->mAudioManager->GetRingerMode(&ringerMode);
        if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
            if (toneType == IToneGenerator::TONE_CDMA_ALERT_CALL_GUARD) {
                if ((ringerMode != IAudioManager::RINGER_MODE_SILENT) &&
                        (ringerMode != IAudioManager::RINGER_MODE_VIBRATE)) {
                    if (DBG) mHost->Log(String("- InCallTonePlayer: start playing call tone=") + StringUtils::ToString(toneType));
                    okToPlayTone = TRUE;
                    needToStopTone = FALSE;
                }
            }
            else if ((toneType == IToneGenerator::TONE_CDMA_NETWORK_BUSY_ONE_SHOT) ||
                    (toneType == IToneGenerator::TONE_CDMA_REORDER) ||
                    (toneType == IToneGenerator::TONE_CDMA_ABBR_REORDER) ||
                    (toneType == IToneGenerator::TONE_CDMA_ABBR_INTERCEPT) ||
                    (toneType == IToneGenerator::TONE_CDMA_CALLDROP_LITE)) {
                if (ringerMode != IAudioManager::RINGER_MODE_SILENT) {
                    if (DBG) mHost->Log(String("InCallTonePlayer:playing call fail tone:") +  StringUtils::ToString(toneType));
                    okToPlayTone = TRUE;
                    needToStopTone = FALSE;
                }
            }
            else if ((toneType == IToneGenerator::TONE_CDMA_ALERT_AUTOREDIAL_LITE) ||
                       (toneType == IToneGenerator::TONE_CDMA_ALERT_NETWORK_LITE)) {
                if ((ringerMode != IAudioManager::RINGER_MODE_SILENT) &&
                        (ringerMode != IAudioManager::RINGER_MODE_VIBRATE)) {
                    if (DBG) mHost->Log(String("InCallTonePlayer:playing tone for toneType=") +  StringUtils::ToString(toneType));
                    okToPlayTone = TRUE;
                    needToStopTone = FALSE;
                }
            }
            else { // For the rest of the tones, always OK to play.
                okToPlayTone = TRUE;
            }
        }
        else {  // Not "CDMA"
            okToPlayTone = TRUE;
        }

        {
            AutoLock syncLock(this);
            if (okToPlayTone && mState != TONE_STOPPED) {
                mState = TONE_ON;
                Boolean tmp = FALSE;
                toneGenerator->StartTone(toneType, &tmp);
                //try
                ECode ec = NOERROR;
                {
                    ec = Wait(toneLengthMillis + TONE_TIMEOUT_BUFFER);
                }
                // catch  (InterruptedException e) {
                if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    Logger::W(TAG,
                          "InCallTonePlayer stopped: %d", ec);
                }
                if (needToStopTone) {
                    toneGenerator->StopTone();
                }
            }
            // if (DBG) log("- InCallTonePlayer: done playing.");
            toneGenerator->ReleaseResources();
            mState = TONE_OFF;
        }
    }

    // Finally, do the same cleanup we otherwise would have done
    // in onDisconnect().
    //
    // (But watch out: do NOT do this if the phone is in use,
    // since some of our tones get played *during* a call (like
    // CALL_WAITING) and we definitely *don't*
    // want to reset the audio mode / speaker / bluetooth after
    // playing those!
    // This call is really here for use with tones that get played
    // *after* a call disconnects, like "busy" or "congestion" or
    // "call ended", where the phone has already become idle but
    // we need to defer the resetAudioStateAfterDisconnect() call
    // till the tone finishes playing.)
    Int32 state;
    mHost->mCM->GetState(&state);
    if (state == PhoneConstantsState_IDLE) {
        mHost->ResetAudioStateAfterDisconnect();
    }
    return NOERROR;
}

ECode CallNotifier::InCallTonePlayer::StopTone()
{
    {
        AutoLock syncLock(this);
        if (mState == TONE_ON) {
            Notify();
        }
        mState = TONE_STOPPED;
    }
    return NOERROR;
}

CallNotifier::SignalInfoTonePlayer::SignalInfoTonePlayer(
    /* [in] */ CallNotifier* host,
    /* [in] */ Int32 toneId)
    : mHost(host)
    , mToneId(toneId)
{
    Thread::constructor();
}

ECode CallNotifier::SignalInfoTonePlayer::Run()
{
    StringBuilder sb;
    sb += "SignalInfoTonePlayer.run(toneId = ";
    sb += mToneId;
    sb += ")...";
    mHost->Log(sb.ToString());
    mHost->CreateSignalInfoToneGenerator();
    if (mHost->mSignalInfoToneGenerator != NULL) {
        //First stop any ongoing SignalInfo tone
        mHost->mSignalInfoToneGenerator->StopTone();

        //Start playing the new tone if its a valid tone
        Boolean tmp = FALSE;
        mHost->mSignalInfoToneGenerator->StartTone(mToneId, &tmp);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CallNotifier::MyBluetoothProfileServiceListener, Object,
        IBluetoothProfileServiceListener)

ECode CallNotifier::MyBluetoothProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    mHost->mBluetoothHeadset = IBluetoothHeadset::Probe(proxy);
    if (VDBG) mHost->Log(String("- Got BluetoothHeadset: ") + TO_CSTR(mHost->mBluetoothHeadset));
    return NOERROR;
}

ECode CallNotifier::MyBluetoothProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    mHost->mBluetoothHeadset = NULL;
    return NOERROR;
}

const String CallNotifier::TAG("CallNotifier");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);

    return (IPhoneGlobals::DBG_LEVEL >= 1) && (value == 1);
}

const Boolean CallNotifier::DBG = initDBG();
const Boolean CallNotifier::VDBG = (IPhoneGlobals::DBG_LEVEL >= 2);

// Time to display the  DisplayInfo Record sent by CDMA network
const Int32 CallNotifier::DISPLAYINFO_NOTIFICATION_TIME = 2000; // msec

static AutoPtr<IAudioAttributes> initVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SPEECH);
    builder->SetUsage(IAudioAttributes::USAGE_VOICE_COMMUNICATION);

    AutoPtr<IAudioAttributes> tmp;
    builder->Build((IAudioAttributes**)&tmp);
    return tmp;
}

AutoPtr<IAudioAttributes> CallNotifier::VIBRATION_ATTRIBUTES = initVIBRATION_ATTRIBUTES();

/** The singleton instance. */
AutoPtr<CallNotifier> CallNotifier::sInstance;

// values used to track the query state
const Int32 CallNotifier::CALLERINFO_QUERY_READY = 0;
const Int32 CallNotifier::CALLERINFO_QUERYING = -1;

const Int32 CallNotifier::PHONE_MWI_CHANGED = 21;
const Int32 CallNotifier::DISPLAYINFO_NOTIFICATION_DONE = 24;
const Int32 CallNotifier::UPDATE_IN_CALL_NOTIFICATION = 27;

const Int32 CallNotifier::TONE_RELATIVE_VOLUME_SIGNALINFO = 80;
Object CallNotifier::THIS;

AutoPtr<CallNotifier> CallNotifier::Init(
    /* [in] */ IPhoneGlobals* app,
    /* [in] */ IPhone* phone,
    /* [in] */ CallLogger* callLogger,
    /* [in] */ CallStateMonitor* callStateMonitor,
    /* [in] */ IBluetoothManager* bluetoothManager)
{
    {
        AutoLock syncLock(THIS);
        if (sInstance == NULL) {
            sInstance = new CallNotifier(app, phone, callLogger, callStateMonitor,
                    bluetoothManager);
        }
        else {
            Logger::D(TAG, "init() called multiple times!  sInstance = %s", TO_CSTR(sInstance));
        }
        return sInstance;
    }
}

CallNotifier::CallNotifier(
    /* [in] */ IPhoneGlobals* app,
    /* [in] */ IPhone* phone,
    /* [in] */ CallLogger* callLogger,
    /* [in] */ CallStateMonitor* callStateMonitor,
    /* [in] */ IBluetoothManager* bluetoothManager)
    : mCallerInfoQueryState(0)
    , mApplication(app)
    , mCallLogger(callLogger)
    , mPreviousCdmaCallState(0)
    , mVoicePrivacyState(FALSE)
    , mIsCdmaRedialCall(FALSE)
    , mBluetoothManager(bluetoothManager)
{
    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)app;
    mCM = globals->mCM;
    Handler::constructor();

    mPhoneStateListener = new MyPhoneStateListener(this);

    mBluetoothProfileServiceListener = new MyBluetoothProfileServiceListener(this);

    AutoPtr<IInterface> obj;
    IContext::Probe(mApplication)->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    callStateMonitor->AddListener(this);

    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    AutoPtr<IBluetoothAdapter> adapter;
    helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    if (adapter != NULL) {
        AutoPtr<IContext> context;
        IContext::Probe(mApplication)->GetApplicationContext((IContext**)&context);
        Boolean tmp = FALSE;
        adapter->GetProfileProxy(context, mBluetoothProfileServiceListener,
                IBluetoothProfile::HEADSET, &tmp);
    }

    AutoPtr<IInterface> obj2;
    IContext::Probe(app)->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj2);
    AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj2);
    telephonyManager->Listen(mPhoneStateListener,
            IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR
            | IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR);
}

void CallNotifier::CreateSignalInfoToneGenerator()
{
    // Instantiate the ToneGenerator for SignalInfo and CallWaiting
    // TODO: We probably don't need the mSignalInfoToneGenerator instance
    // around forever. Need to change it so as to create a ToneGenerator instance only
    // when a tone is being played and releases it after its done playing.
    if (mSignalInfoToneGenerator == NULL) {
        //try
        ECode ec = NOERROR;
        {
            ec = CToneGenerator::New(IAudioManager::STREAM_VOICE_CALL,
                    TONE_RELATIVE_VOLUME_SIGNALINFO, (IToneGenerator**)&mSignalInfoToneGenerator);
            Logger::D(TAG, "CallNotifier: mSignalInfoToneGenerator created when toneplay");
        }
        // catch (RuntimeException e) {
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            Logger::W(TAG, "CallNotifier: Exception caught while creating "
                    "mSignalInfoToneGenerator: %d", ec);
            mSignalInfoToneGenerator = NULL;
        }
        //}
    }
    else {
        Logger::D(TAG, "mSignalInfoToneGenerator created already, hence skipping");
    }
}

ECode CallNotifier::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case CallStateMonitor::PHONE_NEW_RINGING_CONNECTION:
            Log(String("RINGING... (new)"));
            OnNewRingingConnection(IAsyncResult::Probe(obj));
            break;

        case CallStateMonitor::PHONE_STATE_CHANGED:
            OnPhoneStateChanged(IAsyncResult::Probe(obj));
            break;

        case CallStateMonitor::PHONE_DISCONNECT:
            if (DBG) Log(String("DISCONNECT"));
            OnDisconnect(IAsyncResult::Probe(obj));
            break;

        case CallStateMonitor::PHONE_UNKNOWN_CONNECTION_APPEARED:
            OnUnknownConnectionAppeared(IAsyncResult::Probe(obj));
            break;

        case PHONE_MWI_CHANGED:
        {
            AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
            Boolean res;
            globals->mPhone->GetMessageWaitingIndicator(&res);
            OnMwiChanged(res);
            break;
        }

        case CallStateMonitor::PHONE_STATE_DISPLAYINFO:
            if (DBG) Log(String("Received PHONE_STATE_DISPLAYINFO event"));
            OnDisplayInfo(IAsyncResult::Probe(obj));
            break;

        case CallStateMonitor::PHONE_STATE_SIGNALINFO:
            if (DBG) Log(String("Received PHONE_STATE_SIGNALINFO event"));
            OnSignalInfo(IAsyncResult::Probe(obj));
            break;

        case DISPLAYINFO_NOTIFICATION_DONE:
            if (DBG) Log(String("Received Display Info notification done event ..."));
            CdmaDisplayInfo::DismissDisplayInfoRecord();
            break;

        case CallStateMonitor::EVENT_OTA_PROVISION_CHANGE:
        {
            if (DBG) Log(String("EVENT_OTA_PROVISION_CHANGE..."));
            AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
            globals->HandleOtaspEvent(msg);
            break;
        }
        case CallStateMonitor::PHONE_ENHANCED_VP_ON:
        {
            if (DBG) Log(String("PHONE_ENHANCED_VP_ON..."));
            if (!mVoicePrivacyState) {
                Int32 toneToPlay = InCallTonePlayer::TONE_VOICE_PRIVACY;
                AutoPtr<InCallTonePlayer> player = new InCallTonePlayer(this, toneToPlay);
                player->Start();
                mVoicePrivacyState = TRUE;
            }
            break;
        }
        case CallStateMonitor::PHONE_ENHANCED_VP_OFF:
            if (DBG) Log(String("PHONE_ENHANCED_VP_OFF..."));
            if (mVoicePrivacyState) {
                Int32 toneToPlay = InCallTonePlayer::TONE_VOICE_PRIVACY;
                AutoPtr<InCallTonePlayer> player = new InCallTonePlayer(this, toneToPlay);
                player->Start();
                mVoicePrivacyState = FALSE;
            }
            break;

        default: {
            // super.handleMessage(msg);
        }
    }
    return NOERROR;
}

void CallNotifier::OnNewRingingConnection(
    /* [in] */ IAsyncResult* r)
{
    AutoPtr<IConnection> c = IConnection::Probe(((AsyncResult*)r)->mResult);
    Int32 state;
    mCM->GetState(&state);
    StringBuilder sb;
    sb += "onNewRingingConnection(): state = ";
    sb += state;
    sb += ", conn = { ";
    sb += TO_CSTR(c);
    sb += " }";
    Log(sb.ToString());

    AutoPtr<ICall> ringing;
    c->GetCall((ICall**)&ringing);
    AutoPtr<IPhone> phone;
    ringing->GetPhone((IPhone**)&phone);

    // Check for a few cases where we totally ignore incoming calls.
    if (IgnoreAllIncomingCalls(phone)) {
        // Immediately reject the call, without even indicating to the user
        // that an incoming call occurred.  (This will generally send the
        // caller straight to voicemail, just as if we *had* shown the
        // incoming-call UI and the user had declined the call.)
        PhoneUtils::HangupRingingCall(ringing);
        return;
    }

    Boolean res;
    if (c->IsRinging(&res), !res) {
        Logger::I(TAG, "CallNotifier.onNewRingingConnection(): connection not ringing!");
        // This is a very strange case: an incoming call that stopped
        // ringing almost instantly after the onNewRingingConnection()
        // event.  There's nothing we can do here, so just bail out
        // without doing anything.  (But presumably we'll log it in
        // the call log when the disconnect event comes in...)
        return;
    }

    // Stop any signalInfo tone being played on receiving a Call
    StopSignalInfoTone();

    ICallState s;
    c->GetState(&s);
    // State will be either INCOMING or WAITING.
    if (VDBG) Log(String("- connection is ringing!  state = ") + StringUtils::ToString(s));
    // if (DBG) PhoneUtils::DumpCallState(mPhone);

    // No need to do any service state checks here (like for
    // "emergency mode"), since in those states the SIM won't let
    // us get incoming connections in the first place.

    // TODO: Consider sending out a serialized broadcast Intent here
    // (maybe "ACTION_NEW_INCOMING_CALL"), *before* starting the
    // ringer and going to the in-call UI.  The intent should contain
    // the caller-id info for the current connection, and say whether
    // it would be a "call waiting" call or a regular ringing call.
    // If anybody consumed the broadcast, we'd bail out without
    // ringing or bringing up the in-call UI.
    //
    // This would give 3rd party apps a chance to listen for (and
    // intercept) new ringing connections.  An app could reject the
    // incoming call by consuming the broadcast and doing nothing, or
    // it could "pick up" the call (without any action by the user!)
    // via some future TelephonyManager API.
    //
    // See bug 1312336 for more details.
    // We'd need to protect this with a new "intercept incoming calls"
    // system permission.

    // Obtain a partial wake lock to make sure the CPU doesn't go to
    // sleep before we finish bringing up the InCallScreen.
    // (This will be upgraded soon to a full wake lock; see
    // showIncomingCall().)
    if (VDBG) Log(String("Holding wake lock on new incoming connection."));
    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
    globals->RequestWakeState(PhoneGlobalsWakeState_PARTIAL);

    // Note we *don't* post a status bar notification here, since
    // we're not necessarily ready to actually show the incoming call
    // to the user.  (For calls in the INCOMING state, at least, we
    // still need to run a caller-id query, and we may not even ring
    // at all if the "send directly to voicemail" flag is set.)
    //
    // Instead, we update the notification (and potentially launch the
    // InCallScreen) from the showIncomingCall() method, which runs
    // when the caller-id query completes or times out.

    if (VDBG) Log(String("- onNewRingingConnection() done."));
}

Boolean CallNotifier::IgnoreAllIncomingCalls(
    /* [in] */ IPhone* phone)
{
    // Incoming calls are totally ignored on non-voice-capable devices.
    if (!PhoneGlobals::sVoiceCapable) {
        // ...but still log a warning, since we shouldn't have gotten this
        // event in the first place!  (Incoming calls *should* be blocked at
        // the telephony layer on non-voice-capable capable devices.)
        Logger::W(TAG, "Got onNewRingingConnection() on non-voice-capable device! Ignoring...");
        return TRUE;
    }

    // In ECM (emergency callback mode), we ALWAYS allow incoming calls
    // to get through to the user.  (Note that ECM is applicable only to
    // voice-capable CDMA devices).
    if (PhoneUtils::IsPhoneInEcm(phone)) {
        if (DBG) Log(String("Incoming call while in ECM: always allow..."));
        return FALSE;
    }

    // Incoming calls are totally ignored if the device isn't provisioned yet.
    AutoPtr<IContentResolver> cr;
    IContext::Probe(mApplication)->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 tmp;
    helper->GetInt32(cr, ISettingsGlobal::DEVICE_PROVISIONED, 0, &tmp);
    Boolean provisioned = tmp != 0;
    if (!provisioned) {
        Logger::I(TAG, "Ignoring incoming call: not provisioned");
        return TRUE;
    }

    // Incoming calls are totally ignored if an OTASP call is active.
    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
    AutoPtr<ITelephonyCapabilities> helper2;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper2);
    Boolean res;
    if (helper2->SupportsOtasp(phone, &res), res) {
        Boolean activateState = (globals->mCdmaOtaScreenState->mOtaScreenState
                == OtaUtils::CdmaOtaScreenState::OTA_STATUS_ACTIVATION);
        Boolean dialogState = (globals->mCdmaOtaScreenState->mOtaScreenState
                == OtaUtils::CdmaOtaScreenState::OTA_STATUS_SUCCESS_FAILURE_DLG);
        Boolean spcState = globals->mCdmaOtaProvisionData->mInOtaSpcState;

        if (spcState) {
            Logger::I(TAG, "Ignoring incoming call: OTA call is active");
            return TRUE;
        }
        else if (activateState || dialogState) {
            // We *are* allowed to receive incoming calls at this point.
            // But clear out any residual OTASP UI first.
            // TODO: It's an MVC violation to twiddle the OTA UI state here;
            // we should instead provide a higher-level API via OtaUtils.
            if (dialogState) globals->DismissOtaDialogs();
            globals->ClearOtaState();
            return FALSE;
        }
    }

    // Normal case: allow this call to be presented to the user.
    return FALSE;
}

void CallNotifier::OnUnknownConnectionAppeared(
    /* [in] */ IAsyncResult* r)
{
    PhoneConstantsState state;
    mCM->GetState(&state);

    if (state == PhoneConstantsState_OFFHOOK) {
        if (DBG) Log(String("unknown connection appeared..."));

        OnPhoneStateChanged(r);
    }
}

void CallNotifier::OnPhoneStateChanged(
    /* [in] */ IAsyncResult* r)
{
    PhoneConstantsState state;
    mCM->GetState(&state);
    if (VDBG) Log(String("onPhoneStateChanged: state = ") + StringUtils::ToString(state));

    // Turn status bar notifications on or off depending upon the state
    // of the phone.  Notification Alerts (audible or vibrating) should
    // be on if and only if the phone is IDLE.
    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
    globals->mNotificationMgr->mStatusBarHelper->EnableNotificationAlerts(state == PhoneConstantsState_IDLE);

    AutoPtr<IPhone> fgPhone;
    mCM->GetFgPhone((IPhone**)&fgPhone);
    Int32 type;
    fgPhone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        AutoPtr<ICall> call;
        fgPhone->GetForegroundCall((ICall**)&call);
        Int32 state;
        call->GetState(&state);
        if ((state == ICallState_ACTIVE)
                && ((mPreviousCdmaCallState == ICallState_DIALING)
                ||  (mPreviousCdmaCallState == ICallState_ALERTING))) {
            if (mIsCdmaRedialCall) {
                Int32 toneToPlay = InCallTonePlayer::TONE_REDIAL;
                AutoPtr<InCallTonePlayer> player = new InCallTonePlayer(this, toneToPlay);
                player->Start();
            }
            // Stop any signal info tone when call moves to ACTIVE state
            StopSignalInfoTone();
        }
        mPreviousCdmaCallState = state;
    }

    // Have the PhoneApp recompute its mShowBluetoothIndication
    // flag based on the (new) telephony state.
    // There's no need to force a UI update since we update the
    // in-call notification ourselves (below), and the InCallScreen
    // listens for phone state changes itself.
    mBluetoothManager->UpdateBluetoothIndication();

    // Update the phone state and other sensor/lock.
    globals->UpdatePhoneState(state);

    if (state == PhoneConstantsState_OFFHOOK) {
        if (VDBG) Log(String("onPhoneStateChanged: OFF HOOK"));
        // make sure audio is in in-call mode now
        PhoneUtils::SetAudioMode(mCM);
    }
}

ECode CallNotifier::UpdateCallNotifierRegistrationsAfterRadioTechnologyChange()
{
    if (DBG) Logger::D(TAG, "updateCallNotifierRegistrationsAfterRadioTechnologyChange...");

    // Instantiate mSignalInfoToneGenerator
    CreateSignalInfoToneGenerator();
    return NOERROR;
}

void CallNotifier::OnDisconnect(
    /* [in] */ IAsyncResult* r)
{
    Int32 state;
    mCM->GetState(&state);
    if (VDBG) Log(String("onDisconnect()...  CallManager state: ") + StringUtils::ToString(state));

    mVoicePrivacyState = FALSE;
    AutoPtr<IConnection> c = IConnection::Probe(((AsyncResult*)r)->mResult);
    if (c != NULL) {
        AutoPtr<IDisconnectCause> helper;
        CDisconnectCause::AcquireSingleton((IDisconnectCause**)&helper);
        Int32 dc = 0;
        c->GetDisconnectCause(&dc);
        String s;
        helper->ToString(dc, &s);
        Boolean tmp = FALSE;
        c->IsIncoming(&tmp);
        Int64 t = 0;
        c->GetCreateTime(&t);
        Log(String("onDisconnect: cause = ") + s
              + ", incoming = " + StringUtils::ToString(tmp)
              + ", date = " + StringUtils::ToString(t));
    }
    else {
        Logger::W(TAG, "onDisconnect: null connection");
    }

    Int32 autoretrySetting = 0;
    AutoPtr<IPhone> phone;
    Int32 type;
    if (c != NULL) {
        AutoPtr<ICall> call;
        c->GetCall((ICall**)&call);
        call->GetPhone((IPhone**)&phone);
        phone->GetPhoneType(&type);
    }
    if ((c != NULL) && (type == IPhoneConstants::PHONE_TYPE_CDMA)) {
        AutoPtr<IContentResolver> cr;
        IContext::Probe(mApplication)->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        helper->GetInt32(cr, ISettingsGlobal::CALL_AUTO_RETRY, 0, &autoretrySetting);
    }

    // Stop any signalInfo tone being played when a call gets ended
    StopSignalInfoTone();

    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
    if ((c != NULL) && (type == IPhoneConstants::PHONE_TYPE_CDMA)) {
        // Resetting the CdmaPhoneCallState members
        globals->mCdmaPhoneCallState->ResetCdmaPhoneCallState();
    }

    // If this is the end of an OTASP call, pass it on to the PhoneApp.
    AutoPtr<ITelephonyCapabilities> helper;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper);
    Boolean res;
    helper->SupportsOtasp(phone, &res);
    if (c != NULL && res) {
        String number;
        c->GetAddress(&number);
        if (phone->IsOtaSpNumber(number, &res), res) {
            if (DBG) Log(String("onDisconnect: this was an OTASP call!"));
            globals->HandleOtaspDisconnect();
        }
    }

    // Check for the various tones we might need to play (thru the
    // earpiece) after a call disconnects.
    Int32 toneToPlay = InCallTonePlayer::TONE_NONE;

    // If we don't need to play BUSY or CONGESTION, then play the
    // "call ended" tone if this was a "regular disconnect" (i.e. a
    // normal call where one end or the other hung up) *and* this
    // disconnect event caused the phone to become idle.  (In other
    // words, we *don't* play the sound if one call hangs up but
    // there's still an active call on the other line.)
    // TODO: We may eventually want to disable this via a preference.
    if ((toneToPlay == InCallTonePlayer::TONE_NONE)
        && (state == PhoneConstantsState_IDLE)
        && (c != NULL)) {
        Int32 cause;
        c->GetDisconnectCause(&cause);
        if ((cause == IDisconnectCause::NORMAL)  // remote hangup
            || (cause == IDisconnectCause::LOCAL)) {  // local hangup
            if (VDBG) Log(String("- need to play CALL_ENDED tone!"));
            toneToPlay = InCallTonePlayer::TONE_CALL_ENDED;
            mIsCdmaRedialCall = FALSE;
        }
    }

    // All phone calls are disconnected.
    if (state == PhoneConstantsState_IDLE) {
        // Don't reset the audio mode or bluetooth/speakerphone state
        // if we still need to let the user hear a tone through the earpiece.
        if (toneToPlay == InCallTonePlayer::TONE_NONE) {
            ResetAudioStateAfterDisconnect();
        }
    }

    if (c != NULL) {
        mCallLogger->LogCall(c);

        String number;
        c->GetAddress(&number);
        AutoPtr<ICall> call;
        c->GetCall((ICall**)&call);
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);

        Boolean isEmergencyNumber = FALSE;
        AutoPtr<IPhoneNumberUtils> helper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
        helper->IsLocalEmergencyNumber(IContext::Probe(mApplication), number, &isEmergencyNumber);

        // Possibly play a "post-disconnect tone" thru the earpiece.
        // We do this here, rather than from the InCallScreen
        // activity, since we need to do this even if you're not in
        // the Phone UI at the moment the connection ends.
        if (toneToPlay != InCallTonePlayer::TONE_NONE) {
            if (VDBG) {
                StringBuilder sb;
                sb += "- starting post-disconnect tone (";
                sb += toneToPlay;
                sb += ")...";
                Log(sb.ToString());
            }
            AutoPtr<InCallTonePlayer> player = new InCallTonePlayer(this, toneToPlay);
            player->Start();

            // TODO: alternatively, we could start an InCallTonePlayer
            // here with an "unlimited" tone length,
            // and manually stop it later when this connection truly goes
            // away.  (The real connection over the network was closed as soon
            // as we got the BUSY message.  But our telephony layer keeps the
            // connection open for a few extra seconds so we can show the
            // "busy" indication to the user.  We could stop the busy tone
            // when *that* connection's "disconnect" event comes in.)
        }

        Int32 cause;
        c->GetDisconnectCause(&cause);
        if (((mPreviousCdmaCallState == ICallState_DIALING)
                || (mPreviousCdmaCallState == ICallState_ALERTING))
                && (!isEmergencyNumber)
                && (cause != IDisconnectCause::INCOMING_MISSED )
                && (cause != IDisconnectCause::NORMAL)
                && (cause != IDisconnectCause::LOCAL)
                && (cause != IDisconnectCause::INCOMING_REJECTED)) {
            if (!mIsCdmaRedialCall) {
                if (autoretrySetting == IInCallScreen::AUTO_RETRY_ON) {
                    // TODO: (Moto): The contact reference data may need to be stored and use
                    // here when redialing a call. For now, pass in NULL as the URI parameter.
                    Int32 status = 0;
                    status = PhoneUtils::PlaceCall(IContext::Probe(mApplication), phone, number, NULL, FALSE);
                    if (status != PhoneUtils::CALL_STATUS_FAILED) {
                        mIsCdmaRedialCall = TRUE;
                    }
                } else {
                    mIsCdmaRedialCall = FALSE;
                }
            } else {
                mIsCdmaRedialCall = FALSE;
            }
        }
    }
}

void CallNotifier::ResetAudioStateAfterDisconnect()
{
    if (VDBG) Log(String("resetAudioStateAfterDisconnect()..."));

    if (mBluetoothHeadset != NULL) {
        Boolean tmp = FALSE;
        mBluetoothHeadset->DisconnectAudio(&tmp);
    }

    // call turnOnSpeaker() with state=false and store=true even if speaker
    // is already off to reset user requested speaker state.
    PhoneUtils::TurnOnSpeaker(IContext::Probe(mApplication), FALSE, TRUE);

    PhoneUtils::SetAudioMode(mCM);
}

void CallNotifier::OnMwiChanged(
    /* [in] */ Boolean visible)
{
    if (VDBG) Log(String("onMwiChanged(): ") + StringUtils::ToString(visible));

    // "Voicemail" is meaningless on non-voice-capable devices,
    // so ignore MWI events.
    if (!PhoneGlobals::sVoiceCapable) {
        // ...but still log a warning, since we shouldn't have gotten this
        // event in the first place!
        // (PhoneStateListener.LISTEN_MESSAGE_WAITING_INDICATOR events
        // *should* be blocked at the telephony layer on non-voice-capable
        // capable devices.)
        Logger::W(TAG, "Got onMwiChanged() on non-voice-capable device! Ignoring...");
        return;
    }

    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
    globals->mNotificationMgr->UpdateMwi(visible);
}

ECode CallNotifier::SendMwiChangedDelayed(
    /* [in] */ Int64 delayMillis)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> message;
    helper->Obtain(this, PHONE_MWI_CHANGED, (IMessage**)&message);
    Boolean tmp = FALSE;
    return SendMessageDelayed(message, delayMillis, &tmp);
}

void CallNotifier::OnCfiChanged(
    /* [in] */ Boolean visible)
{
    if (VDBG) Log(String("onCfiChanged(): ") + StringUtils::ToString(visible));
    AutoPtr<PhoneGlobals> globals = (PhoneGlobals*)mApplication.Get();
    globals->mNotificationMgr->UpdateCfi(visible);
}

void CallNotifier::OnDisplayInfo(
    /* [in] */ IAsyncResult* r)
{
    // Extract the DisplayInfo String from the message
    AutoPtr<ICdmaInformationRecordsCdmaDisplayInfoRec> displayInfoRec
        = ICdmaInformationRecordsCdmaDisplayInfoRec::Probe(((AsyncResult*)r)->mResult);

    if (displayInfoRec != NULL) {
        String displayInfo;
        assert(0 && "TODO");
        // displayInfo = displayInfoRec.alpha;
        if (DBG) Log(String("onDisplayInfo: displayInfo=") + displayInfo);
        CdmaDisplayInfo::DisplayInfoRecord(IContext::Probe(mApplication), displayInfo);

        // start a 2 second timer
        Boolean tmp = FALSE;
        SendEmptyMessageDelayed(DISPLAYINFO_NOTIFICATION_DONE,
                DISPLAYINFO_NOTIFICATION_TIME, &tmp);
    }
}

void CallNotifier::OnSignalInfo(
    /* [in] */ IAsyncResult* r)
{
    // Signal Info are totally ignored on non-voice-capable devices.
    if (!PhoneGlobals::sVoiceCapable) {
        Logger::W(TAG, "Got onSignalInfo() on non-voice-capable device! Ignoring...");
        return;
    }

    AutoPtr<ICall> call;
    mCM->GetFirstActiveRingingCall((ICall**)&call);
    Int32 state;
    call->GetState(&state);
    assert(0 && "TODO : need PhoneUtils");
    // if (PhoneUtils::IsRealIncomingCall(state)) {
    //     // Do not start any new SignalInfo tone when Call state is INCOMING
    //     // and stop any previous SignalInfo tone which is being played
    //     StopSignalInfoTone();
    // }
    // else {
    //     // Extract the SignalInfo String from the message
    //     CdmaSignalInfoRec signalInfoRec = (CdmaSignalInfoRec)(r.result);
    //     // Only proceed if a Signal info is present.
    //     if (signalInfoRec != NULL) {
    //         Boolean isPresent = signalInfoRec.isPresent;
    //         if (DBG) Log(String("onSignalInfo: isPresent=") + StringUtils::ToString(isPresent));
    //         if (isPresent) {// if tone is valid
    //             Int32 uSignalType = signalInfoRec.signalType;
    //             Int32 uAlertPitch = signalInfoRec.alertPitch;
    //             Int32 uSignal = signalInfoRec.signal;

    //             if (DBG) {
    //                 StringBuilder sb;
    //                 sb += "onSignalInfo: uSignalType=";
    //                 sb += uSignalType;
    //                 sb += ", uAlertPitch=";
    //                 sb += uAlertPitch;
    //                 sb += ", uSignal=";
    //                 sb += uSignal;
    //                 Log(sb.ToString());
    //             }
    //             //Map the Signal to a ToneGenerator ToneID only if Signal info is present
    //             Int32 toneID;
    //             SignalToneUtil::GetAudioToneFromSignalInfo(uSignalType, uAlertPitch, uSignal, &toneID);

    //             //Create the SignalInfo tone player and pass the ToneID
    //             AutoPtr<ISignalInfoTonePlayer> player = new SignalInfoTonePlayer(toneID);
    //             player->Start();
    //         }
    //     }
    // }
}

ECode CallNotifier::StopSignalInfoTone()
{
    if (DBG) Log(String("stopSignalInfoTone: Stopping SignalInfo tone player"));
    AutoPtr<SignalInfoTonePlayer> player = new SignalInfoTonePlayer(this, IToneGenerator::TONE_CDMA_SIGNAL_OFF);
    return player->Start();
}

ECode CallNotifier::GetPreviousCdmaCallState(
    /* [out] */ ICallState* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mPreviousCdmaCallState;
    return NOERROR;
}

ECode CallNotifier::GetVoicePrivacyState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mVoicePrivacyState;
    return NOERROR;
}

ECode CallNotifier::GetIsCdmaRedialCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsCdmaRedialCall;
    return NOERROR;
}

void CallNotifier::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos