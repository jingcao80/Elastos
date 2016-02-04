
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/webkit/native/media/AudioManagerElastos.h"
#include "elastos/droid/webkit/native/media/api/AudioManagerElastos_dec.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/content/CIntentFilter.h"
//TODO #include "elastos/droid/provider/Settings.h"
//TODO #include "elastos/droid/media/CAudioRecordHelper.h"
//TODO #include "elastos/droid/media/CAudioTrackHelper.h"
//TODO #include "elastos/droid/media/CAudioTrackHelper.h"
//TODO #include "elastos/droid/media/audiofx/CAcousticEchoCanceler.h"
//TODO #include "elastos/droid/bluetooth/CBluetoothAdapterHelper.h"
//#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/IntegralToString.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/utility/logging/Logger.h>
//#include <elastos/utility/etl/etl_list.h>
//#include "stdlib.h"

//TODO using Elastos::Droid::Bluetooth::IBluetoothAdapter;
//TODO using Elastos::Droid::Bluetooth::IBluetoothManager;
//TODO using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
//TODO using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
//TODO using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::IAudioRecord;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioRecordHelper;
//using Elastos::Droid::Media::IAudioTrack;
using Elastos::Droid::Media::IAudioTrackHelper;
//TODO using Elastos::Droid::Media::CAudioTrackHelper;
//using Elastos::Droid::Media::Audiofx::IAcousticEchoCanceler;
using Elastos::Droid::Media::Audiofx::IAcousticEchoCancelerHelper;
//TODO using Elastos::Droid::Media::Audiofx::CAcousticEchoCancelerHelper;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::ISettings;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringToIntegral;

using Elastos::Core::IThread;
using Elastos::Core::EIID_IThread;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Media {

//===============================================================
//             AudioManagerElastos::NonThreadSafe
//===============================================================

AudioManagerElastos::NonThreadSafe::NonThreadSafe()
{
    if (DEBUG) {
        Thread::GetCurrentThread()->GetId(&mThreadId);
    }
    else {
        // Avoids "Unread field" issue reported by findbugs.
        mThreadId = 0L;
    }
}

/**
 * Checks if the method is called on the valid thread.
 * Assigns the current thread if no thread was assigned.
 */
Boolean AudioManagerElastos::NonThreadSafe::CalledOnValidThread() const
{
    if (DEBUG) {
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        return mThreadId == id;
    }

    return TRUE;
}

//===============================================================
//             AudioManagerElastos::AudioDeviceName
//===============================================================

AudioManagerElastos::AudioDeviceName::AudioDeviceName(
    /* [in] */ Int32 id,
    /* [in] */ const String& name)
    : mId(id)
    , mName(name)
{
}

//@CalledByNative("AudioDeviceName")
String AudioManagerElastos::AudioDeviceName::Id()
{
    return IntegralToString::ToString(mId);
}

//@CalledByNative("AudioDeviceName")
String AudioManagerElastos::AudioDeviceName::Name()
{
    return mName;
}

//callback definition
String AudioManagerElastos::AudioDeviceName::Id(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioDeviceName> amaObj = (AudioDeviceName*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::AudioDeviceName::Id, amaObj is NULL");
        return String(NULL);
    }
    return amaObj->Id();
}

String AudioManagerElastos::AudioDeviceName::Name(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioDeviceName> amaObj = (AudioDeviceName*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::AudioDeviceName::Id, amaObj is NULL");
        return String(NULL);
    }
    return amaObj->Name();
}

//===============================================================
//      AudioManagerElastos::WiredHeadsetBroadcastReceiver
//===============================================================

const Int32 AudioManagerElastos::WiredHeadsetBroadcastReceiver::STATE_UNPLUGGED;
const Int32 AudioManagerElastos::WiredHeadsetBroadcastReceiver::STATE_PLUGGED;
const Int32 AudioManagerElastos::WiredHeadsetBroadcastReceiver::HAS_NO_MIC;
const Int32 AudioManagerElastos::WiredHeadsetBroadcastReceiver::HAS_MIC;

AudioManagerElastos::WiredHeadsetBroadcastReceiver::WiredHeadsetBroadcastReceiver(
    /* [in] */ AudioManagerElastos* owner)
    : mOwner(owner)
{
}

//@Override
ECode AudioManagerElastos::WiredHeadsetBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);

    Int32 state;
    intent->GetInt32Extra(String("state"), STATE_UNPLUGGED, &state);
    if (DEBUG) {
        Int32 microphone;
        intent->GetInt32Extra(String("microphone"), HAS_NO_MIC, &microphone);
        String name;
        intent->GetStringExtra(String("name"), &name);
        String action;
        intent->GetAction(&action);
        Boolean isInitial;
        IsInitialStickyBroadcast(&isInitial);
        Logger::D(TAG, "BroadcastReceiver.onReceive: a=%s, s=%d, m=%d, n=%s, sb=%d",
                action.string(), state, microphone, name.string(), isInitial);
    }
    switch (state) {
        case STATE_UNPLUGGED: {
                AutoLock lock(&(mOwner->mLock));
                // Wired headset and earpiece are mutually exclusive.
                (*(mOwner->mAudioDevices))[DEVICE_WIRED_HEADSET] = FALSE;
                if (mOwner->HasEarpiece()) {
                    (*(mOwner->mAudioDevices))[DEVICE_EARPIECE] = TRUE;
                }
            }
            break;
        case STATE_PLUGGED: {
                AutoLock lock(&(mOwner->mLock));
                // Wired headset and earpiece are mutually exclusive.
                (*(mOwner->mAudioDevices))[DEVICE_WIRED_HEADSET] = TRUE;
                (*(mOwner->mAudioDevices))[DEVICE_EARPIECE] = FALSE;
            }
            break;
        default:
            Loge(String("Invalid state"));
            break;
    }

    // Update the existing device selection, but only if a specific
    // device has already been selected explicitly.
    if (mOwner->DeviceHasBeenRequested()) {
        mOwner->UpdateDeviceActivation();
    }
    else if (DEBUG) {
        mOwner->ReportUpdate();
    }

    return NOERROR;
}

//=========================================================================
//         AudioManagerElastos::BluetoothHeadsetBroadcastReceiver
//=========================================================================

AudioManagerElastos::BluetoothHeadsetBroadcastReceiver::BluetoothHeadsetBroadcastReceiver(
    /* [in] */ AudioManagerElastos* owner)
    : mOwner(owner)
{
}

//@Override
ECode AudioManagerElastos::BluetoothHeadsetBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);

    // A change in connection state of the Headset profile has
    // been detected, e.g. BT headset has been connected or
    // disconnected. This broadcast is *not* sticky.
    Int32 profileState;
    intent->GetInt32Extra(
        String("android.bluetooth.profile.extra.STATE"),//TODO IBluetoothProfile::EXTRA_STATE,//IBluetoothHeadset
        0,//IBluetoothProfile::STATE_DISCONNECTED,//IBluetoothHeadset::STATE_DISCONNECTED,
        &profileState);

    if (DEBUG) {
        String action;
        intent->GetAction(&action);
        Boolean isInitial;
        IsInitialStickyBroadcast(&isInitial);
        Logger::D(TAG, "BroadcastReceiver.onReceive: a=%s, s=%d, sb=%d", action.string(), profileState, isInitial);
    }

    switch (profileState) {
        case 0://IBluetoothProfile::STATE_DISCONNECTED:
            // We do not have to explicitly call stopBluetoothSco()
            // since BT SCO will be disconnected automatically when
            // the BT headset is disabled.
            {
                AutoLock lock(&(mOwner->mLock));
                // Remove the BT device from the list of devices.
                (*(mOwner->mAudioDevices))[DEVICE_BLUETOOTH_HEADSET] = FALSE;
            }
            break;
        case 2: {//IBluetoothProfile::STATE_CONNECTED: {
                AutoLock lock(&(mOwner->mLock));
                // Add the BT device to the list of devices.
                (*(mOwner->mAudioDevices))[DEVICE_BLUETOOTH_HEADSET] = TRUE;
            }
            break;
        case 1://IBluetoothProfile::STATE_CONNECTING:
            // Bluetooth service is switching from off to on.
            break;
        case 3://IBluetoothProfile::STATE_DISCONNECTING:
            // Bluetooth service is switching from on to off.
            break;
        default:
            Loge(String("Invalid state"));
            break;
    }

    // Update the existing device selection, but only if a specific
    // device has already been selected explicitly.
    if (mOwner->DeviceHasBeenRequested()) {
        mOwner->UpdateDeviceActivation();
    }
    else if (DEBUG) {
        mOwner->ReportUpdate();
    }

    return NOERROR;
}

//===========================================================================
//         AudioManagerElastos::BluetoothScoIntentBroadcastReceiver
//===========================================================================

AudioManagerElastos::BluetoothScoIntentBroadcastReceiver::BluetoothScoIntentBroadcastReceiver(
    /* [in] */ AudioManagerElastos* owner)
    : mOwner(owner)
{
}

//@Override
ECode AudioManagerElastos::BluetoothScoIntentBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);

    Int32 state;
    intent->GetInt32Extra(
        IAudioManager::EXTRA_SCO_AUDIO_STATE,
        IAudioManager::SCO_AUDIO_STATE_DISCONNECTED,
        &state);

    if (DEBUG) {
        String action;
        intent->GetAction(&action);
        Boolean isInitial;
        IsInitialStickyBroadcast(&isInitial);
        Logger::D(TAG, "BroadcastReceiver.onReceive: a=%s, s=%d, sb=%d", action.string(), state,isInitial);
    }

    switch (state) {
        case IAudioManager::SCO_AUDIO_STATE_CONNECTED:
            mOwner->mBluetoothScoState = STATE_BLUETOOTH_SCO_ON;
            break;
        case IAudioManager::SCO_AUDIO_STATE_DISCONNECTED:
            mOwner->mBluetoothScoState = STATE_BLUETOOTH_SCO_OFF;
            break;
        case IAudioManager::SCO_AUDIO_STATE_CONNECTING:
            // do nothing
            break;
        default:
            Loge(String("Invalid state"));
    }

    if (DEBUG) {
        mOwner->ReportUpdate();
    }

    return NOERROR;
}


//===============================================================
//           AudioManagerElastos::InnerContentObserver
//===============================================================

AudioManagerElastos::InnerContentObserver::InnerContentObserver(
    /* [in] */ AudioManagerElastos* owner)
    : mOwner(owner)
{
}

ECode AudioManagerElastos::InnerContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (DEBUG) Logger::D(TAG, "SettingsObserver.onChange: %d", selfChange);
    ContentObserver::OnChange(selfChange);

    // Ensure that the observer is activated during communication mode.
    Int32 mode;
    mOwner->mAudioManager->GetMode(&mode);
    if (mode != IAudioManager::MODE_IN_COMMUNICATION) {
        // throw new IllegalStateException(
        //         "Only enable SettingsObserver in COMM mode");
        assert(0);
    }

    // Get stream volume for the voice stream and deliver callback if
    // the volume index is zero. It is not possible to move the volume
    // slider all the way down in communication mode but the callback
    // implementation can ensure that the volume is completely muted.
    Int32 volume;
    mOwner->mAudioManager->GetStreamVolume(
        IAudioManager::STREAM_VOICE_CALL,
        &volume);
    if (DEBUG) Logger::D(TAG, "nativeSetMute: %d", (volume == 0));
    mOwner->NativeSetMute(mOwner->mNativeAudioManagerElastos, (volume == 0));

    return NOERROR;
}

//===============================================================
//                    AudioManagerElastos
//===============================================================

template<typename T>
static AutoPtr< ArrayOf<T> > ArrayOf_Init(T array[])
{
    Int32 size = sizeof(T)/sizeof(T[0]);
    AutoPtr< ArrayOf<T> > retArray = ArrayOf<T>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        (*retArray)[i] = array[i];
    }

    return retArray;
}

const String AudioManagerElastos::TAG("AudioManagerElastos");
const Boolean AudioManagerElastos::DEBUG = FALSE;

static String models[] = {
    String("GT-I9300"),  // Galaxy S3
    String("GT-I9500"),  // Galaxy S4
    String("GT-N7105"),  // Galaxy Note 2
    String("Nexus 4"),   // Nexus 4
    String("Nexus 5"),   // Nexus 5
    String("Nexus 7"),   // Nexus 7
    String("SM-N9005"),  // Galaxy Note 3
    String("SM-T310"),   // Galaxy Tab 3 8.0 (WiFi)
};
//TODO
const AutoPtr< ArrayOf<String> > AudioManagerElastos::SUPPORTED_AEC_MODELS;// = ArrayOf_Init(models);

// Supported audio device types.
const Int32 AudioManagerElastos::DEVICE_DEFAULT;
const Int32 AudioManagerElastos::DEVICE_INVALID;
const Int32 AudioManagerElastos::DEVICE_SPEAKERPHONE;
const Int32 AudioManagerElastos::DEVICE_WIRED_HEADSET;
const Int32 AudioManagerElastos::DEVICE_EARPIECE;
const Int32 AudioManagerElastos::DEVICE_BLUETOOTH_HEADSET;
const Int32 AudioManagerElastos::DEVICE_COUNT;

// Maps audio device types to string values. This map must be in sync
// with the device types above.
// TODO(henrika): add support for proper detection of device names and
// localize the name strings by using resource strings.
// See http://crbug.com/333208 for details.
static String names[] = {
    String("Speakerphone"),
    String("Wired headset"),      // With or without microphone.
    String("Headset earpiece"),   // Only available on mobile phones.
    String("Bluetooth headset"),  // Requires BLUETOOTH permission.
};

//TODO
const AutoPtr< ArrayOf<String> > AudioManagerElastos::DEVICE_NAMES;// = ArrayOf_Init(names);

Int32 AudioManagerElastos::devices[4] = {
    AudioManagerElastos::DEVICE_SPEAKERPHONE,
    AudioManagerElastos::DEVICE_WIRED_HEADSET,
    AudioManagerElastos::DEVICE_EARPIECE,
    AudioManagerElastos::DEVICE_BLUETOOTH_HEADSET,
};

// List of valid device types.
//TODO
const AutoPtr< ArrayOf<Int32> > AudioManagerElastos::VALID_DEVICES;// = ArrayOf_Init(devices);

// Bluetooth audio SCO states. Example of valid state sequence:
// SCO_INVALID -> SCO_TURNING_ON -> SCO_ON -> SCO_TURNING_OFF -> SCO_OFF.
const Int32 AudioManagerElastos::STATE_BLUETOOTH_SCO_INVALID;
const Int32 AudioManagerElastos::STATE_BLUETOOTH_SCO_OFF;
const Int32 AudioManagerElastos::STATE_BLUETOOTH_SCO_ON;
const Int32 AudioManagerElastos::STATE_BLUETOOTH_SCO_TURNING_ON;
const Int32 AudioManagerElastos::STATE_BLUETOOTH_SCO_TURNING_OFF;

// Use 44.1kHz as the default sampling rate.
const Int32 AudioManagerElastos::DEFAULT_SAMPLING_RATE;
// Randomly picked up frame size which is close to return value on N4.
// Return this value when getProperty(PROPERTY_OUTPUT_FRAMES_PER_BUFFER)
// fails.
const Int32 AudioManagerElastos::DEFAULT_FRAME_PER_BUFFER;

AutoPtr< ArrayOf<Boolean> > AudioManagerElastos::mAudioDevices_Init()
{
    AutoPtr< ArrayOf<Boolean> > array = ArrayOf<Boolean>::Alloc(DEVICE_COUNT);
    for (Int32 i = 0; i < DEVICE_COUNT; ++i) {
        (*array)[i] = FALSE;
    }

    return array;
}

AudioManagerElastos::AudioManagerElastos(
    /* [in] */ IContext* context,
    /* [in] */ Int64 nativeAudioManagerElastos)
    : mContext(context)
    , mNativeAudioManagerElastos(nativeAudioManagerElastos)
    , mHasModifyAudioSettingsPermission(FALSE)
    , mHasRecordAudioPermission(FALSE)
    , mHasBluetoothPermission(FALSE)
    , mSavedAudioMode(IAudioManager::MODE_INVALID)
    , mBluetoothScoState(STATE_BLUETOOTH_SCO_INVALID)
    , mIsInitialized(FALSE)
    , mSavedIsSpeakerphoneOn(FALSE)
    , mSavedIsMicrophoneMute(FALSE)
    , mRequestedAudioDevice(DEVICE_INVALID)
    , mAudioDevices(mAudioDevices_Init())
    , mCurrentVolume(0)
{
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&mAudioManager);
    mContext->GetContentResolver((IContentResolver**)&mContentResolver);
}

Boolean AudioManagerElastos::RunningOnJellyBeanOrHigher()
{
    return Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN;
}

Boolean AudioManagerElastos::RunningOnJellyBeanMR1OrHigher()
{
    return Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1;
}

Boolean AudioManagerElastos::RunningOnJellyBeanMR2OrHigher()
{
    return Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR2;
}

/** Construction */
//@CalledByNative return AudioManagerElastos
AutoPtr<AudioManagerElastos> AudioManagerElastos::CreateAudioManagerElastos(
    /* [in] */ IContext* context,
    /* [in] */ Int64 nativeAudioManagerElastos)
{
    AutoPtr<AudioManagerElastos> ret(new AudioManagerElastos(context, nativeAudioManagerElastos));
    return ret;
}

/**
 * Saves the initial speakerphone and microphone state.
 * Populates the list of available audio devices and registers receivers
 * for broadcast intents related to wired headset and Bluetooth devices.
 */
//@CalledByNative
void AudioManagerElastos::Init()
{
    CheckIfCalledOnValidThread();

    if (DEBUG) {
        Logd(String("init"));
    }

    if (DEBUG)  {
        LogDeviceInfo();
    }

    if (mIsInitialized) {
        return;
    }

    // Check if process has MODIFY_AUDIO_SETTINGS and RECORD_AUDIO
    // permissions. Both are required for full functionality.
    assert(0 && "TODO");
    // mHasModifyAudioSettingsPermission = HasPermission(
    //         Manifest::permission::MODIFY_AUDIO_SETTINGS);
    if (DEBUG && !mHasModifyAudioSettingsPermission) {
        Logd(String("MODIFY_AUDIO_SETTINGS permission is missing"));
    }
    assert(0 && "TODO");
    // mHasRecordAudioPermission = HasPermission(
    //         Manifest::permission::RECORD_AUDIO);
    if (DEBUG && !mHasRecordAudioPermission) {
        Logd(String("RECORD_AUDIO permission is missing"));
    }

    // Initialize audio device list with things we know is always available.
    (*mAudioDevices)[DEVICE_EARPIECE] = HasEarpiece();
    (*mAudioDevices)[DEVICE_WIRED_HEADSET] = HasWiredHeadset();
    (*mAudioDevices)[DEVICE_SPEAKERPHONE] = TRUE;

    // Register receivers for broadcast intents related to Bluetooth device
    // and Bluetooth SCO notifications. Requires BLUETOOTH permission.
    RegisterBluetoothIntentsIfNeeded();

    // Register receiver for broadcast intents related to adding/
    // removing a wired headset (Intent.ACTION_HEADSET_PLUG).
    RegisterForWiredHeadsetIntentBroadcast();

    mIsInitialized = TRUE;

    if (DEBUG) {
        ReportUpdate();
    }
}

/**
 * Unregister all previously registered intent receivers and restore
 * the stored state (stored in {@link #init()}).
 */
//@CalledByNative
void AudioManagerElastos::Close()
{
    CheckIfCalledOnValidThread();
    if (DEBUG) Logd(String("close"));
    if (!mIsInitialized)
        return;

    StopObservingVolumeChanges();
    UnregisterForWiredHeadsetIntentBroadcast();
    UnregisterBluetoothIntentsIfNeeded();

    mIsInitialized = FALSE;
}

/**
 * Saves current audio mode and sets audio mode to MODE_IN_COMMUNICATION
 * if input parameter is true. Restores saved audio mode if input parameter
 * is false.
 * Required permission: android.Manifest.permission.MODIFY_AUDIO_SETTINGS.
 */
//@CalledByNative
void AudioManagerElastos::SetCommunicationAudioModeOn(Boolean on)
{
    if (DEBUG) {
        Logger::D(TAG,"setCommunicationAudioModeOn(%d)", on);
    }

    // The MODIFY_AUDIO_SETTINGS permission is required to allow an
    // application to modify global audio settings.
    if (!mHasModifyAudioSettingsPermission) {
        Logger::W(TAG, "MODIFY_AUDIO_SETTINGS is missing => client will run with reduced functionality");
        return;
    }

    if (on) {
        if (mSavedAudioMode != IAudioManager::MODE_INVALID) {
            // throw new IllegalStateException("Audio mode has already been set");
            assert(0);
        }

        // Store the current audio mode the first time we try to
        // switch to communication mode.
        // try {
            mAudioManager->GetMode(&mSavedAudioMode);
        // } catch (SecurityException e) {
        //     logDeviceInfo();
        //     throw e;

        // }

        // Store microphone mute state and speakerphone state so it can
        // be restored when closing.
        mAudioManager->IsSpeakerphoneOn(&mSavedIsSpeakerphoneOn);
        mAudioManager->IsMicrophoneMute(&mSavedIsMicrophoneMute);

        // try {
            mAudioManager->SetMode(IAudioManager::MODE_IN_COMMUNICATION);
        // } catch (SecurityException e) {
        //     logDeviceInfo();
        //     throw e;
        // }

        // Start observing volume changes to detect when the
        // voice/communication stream volume is at its lowest level.
        // It is only possible to pull down the volume slider to about 20%
        // of the absolute minimum (slider at far left) in communication
        // mode but we want to be able to mute it completely.
        StartObservingVolumeChanges();
    }
    else {
        if (mSavedAudioMode == IAudioManager::MODE_INVALID) {
            // throw new IllegalStateException("Audio mode has not yet been set");
            assert(0);
        }

        StopObservingVolumeChanges();

        // Restore previously stored audio states.
        SetMicrophoneMute(mSavedIsMicrophoneMute);
        SetSpeakerphoneOn(mSavedIsSpeakerphoneOn);

        // Restore the mode that was used before we switched to
        // communication mode.
        // try {
            mAudioManager->SetMode(mSavedAudioMode);
        // } catch (SecurityException e) {
        //     logDeviceInfo();
        //     throw e;
        // }
        mSavedAudioMode = IAudioManager::MODE_INVALID;
    }
}

/**
 * Activates, i.e., starts routing audio to, the specified audio device.
 *
 * @param deviceId Unique device ID (integer converted to string)
 * representing the selected device. This string is empty if the so-called
 * default device is requested.
 * Required permissions: android.Manifest.permission.MODIFY_AUDIO_SETTINGS
 * and android.Manifest.permission.RECORD_AUDIO.
 */
//@CalledByNative
Boolean AudioManagerElastos::SetDevice(
    /* [in] */ const String& deviceId)
{
    if (DEBUG) Logger::D(TAG,"setDevice: %s", deviceId.string());
    if (!mIsInitialized)
        return FALSE;
    if (!mHasModifyAudioSettingsPermission || !mHasRecordAudioPermission) {
        Logger::W(TAG, "Requires MODIFY_AUDIO_SETTINGS and RECORD_AUDIO");
        Logger::W(TAG, "Selected device will not be available for recording");
        return FALSE;
    }


    //Int32 intDeviceId = deviceId.IsNullOrEmpty() ?  DEVICE_DEFAULT : atoi(deviceId.string());
    Int32 intDeviceId = DEVICE_DEFAULT;
    if (!deviceId.IsNullOrEmpty()) {
        StringToIntegral::Parse(deviceId, &intDeviceId);
    }

    if (intDeviceId == DEVICE_DEFAULT) {
        AutoPtr< ArrayOf<Boolean> > devices;
        {
            AutoLock lock(&mLock);
            devices = mAudioDevices->Clone();
            mRequestedAudioDevice = DEVICE_DEFAULT;
        }
        Int32 defaultDevice = SelectDefaultDevice(devices);
        SetAudioDevice(defaultDevice);
        return TRUE;
    }

    // A non-default device is specified. Verify that it is valid
    // device, and if so, start using it.
    //List<Integer> validIds = Arrays.asList(VALID_DEVICES);
    if (!VALID_DEVICES->Contains(intDeviceId) || !(*mAudioDevices)[intDeviceId]) {
        return FALSE;
    }

    {
        AutoLock lock(&mLock);
        mRequestedAudioDevice = intDeviceId;
    }

    SetAudioDevice(intDeviceId);

    return TRUE;
}

/**
 * @return the current list of available audio devices.
 * Note that this call does not trigger any update of the list of devices,
 * it only copies the current state in to the output array.
 * Required permissions: android.Manifest.permission.MODIFY_AUDIO_SETTINGS
 * and android.Manifest.permission.RECORD_AUDIO.
 */
//@CalledByNative
AutoPtr< ArrayOf<IInterface*> > AudioManagerElastos::GetAudioInputDeviceNames()
{
    if (DEBUG) Logd(String("getAudioInputDeviceNames"));
    if (!mIsInitialized)
        return NULL;
    if (!mHasModifyAudioSettingsPermission || !mHasRecordAudioPermission) {
        Logger::W(TAG, "Requires MODIFY_AUDIO_SETTINGS and RECORD_AUDIO");
        Logger::W(TAG, "No audio device will be available for recording");
        return NULL;
    }

    AutoPtr< ArrayOf<Boolean> > devices;
    {
        AutoLock lock(&mLock);
        devices = mAudioDevices->Clone();
    }
    //TODO List<String> list;// = new ArrayList<String>();
    AutoPtr< ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(GetNumOfAudioDevices(devices));
    Int32 i = 0;
    for (Int32 id = 0; id < DEVICE_COUNT; ++id) {
        if ((*devices)[id]) {
            AutoPtr<AudioDeviceName> dn = new AudioDeviceName(id, (*DEVICE_NAMES)[id]);
            array->Set(i, TO_IINTERFACE(dn));
            //TODO list.Pushback(DEVICE_NAMES[id]);
            i++;
        }
    }

    if (DEBUG) Logger::D(TAG, "getAudioInputDeviceNames: %s", "xxx");//TODO print the value in the list

    return array;
}

//@CalledByNative
Int32 AudioManagerElastos::GetNativeOutputSampleRate()
{
    if (RunningOnJellyBeanMR1OrHigher()) {
        String sampleRateString;
        mAudioManager->GetProperty(
                IAudioManager::PROPERTY_OUTPUT_SAMPLE_RATE, &sampleRateString);
        //return (sampleRateString == NULL ?  DEFAULT_SAMPLING_RATE : atoi(sampleRateString.string()));
        Int32 result = DEFAULT_SAMPLING_RATE;
        if (!sampleRateString.IsNullOrEmpty()) {
            StringToIntegral::Parse(sampleRateString, &result);
        }
        return result;
    }
    else {
        return DEFAULT_SAMPLING_RATE;
    }
}

/**
* Returns the minimum frame size required for audio input.
*
* @param sampleRate sampling rate
* @param channels number of channels
*/
//@CalledByNative
Int32 AudioManagerElastos::GetMinInputFrameSize(
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channels)
{
    Int32 channelConfig;
    if (channels == 1) {
        channelConfig = IAudioFormat::CHANNEL_IN_MONO;
    }
    else if (channels == 2) {
        channelConfig = IAudioFormat::CHANNEL_IN_STEREO;
    }
    else {
        return -1;
    }

    Int32 result;
    AutoPtr<IAudioRecordHelper> helper;
    //TODO
    //CAudioRecordHelper::AcquireSingleton((IAudioRecordHelper**)&helper);
    helper->GetMinBufferSize(sampleRate, channelConfig, IAudioFormat::ENCODING_PCM_16BIT, &result);
    result = result / 2 / channels;

    return result;
}

/**
* Returns the minimum frame size required for audio output.
*
* @param sampleRate sampling rate
* @param channels number of channels
*/
//@CalledByNative
Int32 AudioManagerElastos::GetMinOutputFrameSize(
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channels)
{
    Int32 channelConfig;
    if (channels == 1) {
        channelConfig = IAudioFormat::CHANNEL_OUT_MONO;
    }
    else if (channels == 2) {
        channelConfig = IAudioFormat::CHANNEL_OUT_STEREO;
    }
    else {
        return -1;
    }


    AutoPtr<IAudioTrackHelper> helper;
    //TODO CAudioTrackHelper::AcquireSingleton((IAudioTrackHelper**)&helper);
    Int32 result;
    helper->GetMinBufferSize(sampleRate, channelConfig, IAudioFormat::ENCODING_PCM_16BIT, &result);
    return result/2/channels;
}

//@CalledByNative
Boolean AudioManagerElastos::IsAudioLowLatencySupported()
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Boolean result = FALSE;
    pm->HasSystemFeature(IPackageManager::FEATURE_AUDIO_LOW_LATENCY, &result);
    return result;
}

//@CalledByNative
Int32 AudioManagerElastos::GetAudioLowLatencyOutputFrameSize()
{
    String framesPerBuffer;
    mAudioManager->GetProperty(IAudioManager::PROPERTY_OUTPUT_FRAMES_PER_BUFFER, &framesPerBuffer);
    //return (framesPerBuffer == NULL ?  DEFAULT_FRAME_PER_BUFFER : atoi(framesPerBuffer));
    Int32 result = DEFAULT_FRAME_PER_BUFFER;
    if (!framesPerBuffer.IsNullOrEmpty()) {
        StringToIntegral::Parse(framesPerBuffer, &result);
    }
    return result;
}

//@CalledByNative
Boolean AudioManagerElastos::ShouldUseAcousticEchoCanceler()
{
    // AcousticEchoCanceler was added in API level 16 (Jelly Bean).
    if (!RunningOnJellyBeanOrHigher()) {
        return FALSE;
    }

    // Verify that this device is among the supported/tested models.
    //List<String> supportedModels = Arrays.asList(SUPPORTED_AEC_MODELS);
    if (!SUPPORTED_AEC_MODELS->Contains(Build::MODEL)) {
        return FALSE;
    }

    AutoPtr<IAcousticEchoCancelerHelper> helper;
    //TODO CAcousticEchoCancelerHelper::AcquireSingleton((IAcousticEchoCancelerHelper**)&helper);
    Boolean isAvailable = FALSE;
    helper->IsAvailable(&isAvailable);
    if (DEBUG && isAvailable) {
        Logger::D(TAG,"Approved for use of hardware acoustic echo canceler.");
    }

    // As a final check, verify that the device supports acoustic echo
    // cancellation.
    return isAvailable;
}

/**
 * Helper method for debugging purposes. Ensures that method is
 * called on same thread as this object was created on.
 */
void AudioManagerElastos::CheckIfCalledOnValidThread()
{
    if (DEBUG && !mNonThreadSafe.CalledOnValidThread()) {
        // throw new IllegalStateException("Method is not called on valid thread");
        assert(0);
    }
}

/**
 * Register for BT intents if we have the BLUETOOTH permission.
 * Also extends the list of available devices with a BT device if one exists.
 */
void AudioManagerElastos::RegisterBluetoothIntentsIfNeeded()
{
    // Check if this process has the BLUETOOTH permission or not.
    assert(0 && "TODO");
    // mHasBluetoothPermission = HasPermission(
    //         Manifest::permission::BLUETOOTH);

    // Add a Bluetooth headset to the list of available devices if a BT
    // headset is detected and if we have the BLUETOOTH permission.
    // We must do this initial check using a dedicated method since the
    // broadcasted intent BluetoothHeadset.ACTION_CONNECTION_STATE_CHANGED
    // is not sticky and will only be received if a BT headset is connected
    // after this method has been called.
    if (!mHasBluetoothPermission) {
//        Log.w(TAG, "Requires BLUETOOTH permission");
        return;
    }
    (*mAudioDevices)[DEVICE_BLUETOOTH_HEADSET] = HasBluetoothHeadset();

    // Register receivers for broadcast intents related to changes in
    // Bluetooth headset availability and usage of the SCO channel.
    RegisterForBluetoothHeadsetIntentBroadcast();
    RegisterForBluetoothScoIntentBroadcast();
}

/** Unregister for BT intents if a registration has been made. */
void AudioManagerElastos::UnregisterBluetoothIntentsIfNeeded()
{
    if (mHasBluetoothPermission) {
        mAudioManager->StopBluetoothSco();
        UnregisterForBluetoothHeadsetIntentBroadcast();
        UnregisterForBluetoothScoIntentBroadcast();
    }
}

/** Sets the speaker phone mode. */
void AudioManagerElastos::SetSpeakerphoneOn(
    /* [in] */ Boolean on)
{
    Boolean wasOn;
    mAudioManager->IsSpeakerphoneOn(&wasOn);
    if (wasOn == on) {
        return;
    }

    mAudioManager->SetSpeakerphoneOn(on);
}

/** Sets the microphone mute state. */
void AudioManagerElastos::SetMicrophoneMute(
    /* [in] */ Boolean on)
{
    Boolean wasMuted;
    mAudioManager->IsMicrophoneMute(&wasMuted);
    if (wasMuted == on) {
        return;
    }

    mAudioManager->SetMicrophoneMute(on);
}

/** Gets  the current microphone mute state. */
Boolean AudioManagerElastos::IsMicrophoneMute()
{
    Boolean result = FALSE;
    mAudioManager->IsMicrophoneMute(&result);
    return result;
}

/** Gets the current earpiece state. */
Boolean AudioManagerElastos::HasEarpiece()
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Boolean result = FALSE;
    pm->HasSystemFeature(
        IPackageManager::FEATURE_TELEPHONY, &result);
    return result;
}

/**
 * Checks whether a wired headset is connected or not.
 * This is not a valid indication that audio playback is actually over
 * the wired headset as audio routing depends on other conditions. We
 * only use it as an early indicator (during initialization) of an attached
 * wired headset.
 */
//@Deprecated
Boolean AudioManagerElastos::HasWiredHeadset()
{
    Boolean result = FALSE;
    mAudioManager->IsWiredHeadsetOn(&result);
    return result;
}

/** Checks if the process has as specified permission or not. */
Boolean AudioManagerElastos::HasPermission(
    /* [in] */ const String& permission)
{
    Int32 result;
    mContext->CheckPermission(
            permission,
            Process::MyPid(),
            Process::MyUid(), &result);

    return result == IPackageManager::PERMISSION_GRANTED;
}

/**
 * Gets the current Bluetooth headset state.
 * android.bluetooth.BluetoothAdapter.getProfileConnectionState() requires
 * the BLUETOOTH permission.
 */
Boolean AudioManagerElastos::HasBluetoothHeadset()
{
    if (!mHasBluetoothPermission) {
        Logger::W(TAG, "hasBluetoothHeadset() requires BLUETOOTH permission");
        return FALSE;
    }

    // To get a BluetoothAdapter representing the local Bluetooth adapter,
    // when running on JELLY_BEAN_MR1 (4.2) and below, call the static
    // getDefaultAdapter() method; when running on JELLY_BEAN_MR2 (4.3) and
    // higher, retrieve it through getSystemService(String) with
    // BLUETOOTH_SERVICE.
    /* TODO
    AutoPtr<IBluetoothAdapter> btAdapter;
    if (RunningOnJellyBeanMR2OrHigher()) {
        // Use BluetoothManager to get the BluetoothAdapter for
        // Android 4.3 and above.
        AutoPtr<IBluetoothManager> btManager;
        mContext->GetSystemService(IContext::BLUETOOTH_SERVICE, (IInterface**)&btManager);
        btManager->GetAdapter((IBluetoothManager**)&btAdapter);
    }
    else {
        // Use static method for Android 4.2 and below to get the
        // BluetoothAdapter.
        AutoPtr<IBluetoothAdapterHelper> helper;
        CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
        helper->GetDefaultAdapter((IBluetoothAdapter**)&btAdapter);
    }

    if (btAdapter == NULL) {
        // Bluetooth not supported on this platform.
        return FALSE;
    }
    */

    Int32 profileConnectionState = 0;//IBluetoothProfile::STATE_DISCONNECTED
    Boolean isEnabled = FALSE;
    //TODO btAdapter->GetProfileConnectionState(1/*TODO IBluetoothProfile::HEADSET*/, &profileConnectionState);

    // Ensure that Bluetooth is enabled and that a device which supports the
    // headset and handsfree profile is connected.
    // TODO(henrika): it is possible that btAdapter.isEnabled() is
    // redundant. It might be sufficient to only check the profile state.
    //TODO btAdapter->IsEnabled(&isEnabled);
    return isEnabled && profileConnectionState ==
        2/*TODO IBluetoothProfile::STATE_CONNECTED*/;
}

/**
 * Registers receiver for the broadcasted intent when a wired headset is
 * plugged in or unplugged. The received intent will have an extra
 * 'state' value where 0 means unplugged, and 1 means plugged.
 */
void AudioManagerElastos::RegisterForWiredHeadsetIntentBroadcast()
{
    AutoPtr<IIntentFilter> filter;
    AutoPtr<IIntent> intent;
    CIntentFilter::New(IIntent::ACTION_HEADSET_PLUG, (IIntentFilter**)&filter);

    /** Receiver which handles changes in wired headset availability. */
    mWiredHeadsetReceiver = new WiredHeadsetBroadcastReceiver(this);

    // Note: the intent we register for here is sticky, so it'll tell us
    // immediately what the last action was (plugged or unplugged).
    // It will enable us to set the speakerphone correctly.
    mContext->RegisterReceiver(mWiredHeadsetReceiver, filter, (IIntent**)&intent);
}

/** Unregister receiver for broadcasted ACTION_HEADSET_PLUG intent. */
void AudioManagerElastos::UnregisterForWiredHeadsetIntentBroadcast()
{
    mContext->UnregisterReceiver(mWiredHeadsetReceiver);
    mWiredHeadsetReceiver = NULL;
}

/**
 * Registers receiver for the broadcasted intent related to BT headset
 * availability or a change in connection state of the local Bluetooth
 * adapter. Example: triggers when the BT device is turned on or off.
 * BLUETOOTH permission is required to receive this one.
 */
void AudioManagerElastos::RegisterForBluetoothHeadsetIntentBroadcast()
{
    AutoPtr<IIntentFilter> filter;
    AutoPtr<IIntent> intent;
    CIntentFilter::New(
        String("android.bluetooth.headset.profile.action.CONNECTION_STATE_CHANGED"),//TODO IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED,
        (IIntentFilter**)&filter);

    /** Receiver which handles changes in BT headset availability. */
    mBluetoothHeadsetReceiver = new BluetoothHeadsetBroadcastReceiver(this);

    mContext->RegisterReceiver(mBluetoothHeadsetReceiver, filter, (IIntent**)&intent);
}

void AudioManagerElastos::UnregisterForBluetoothHeadsetIntentBroadcast()
{
    mContext->UnregisterReceiver(mBluetoothHeadsetReceiver);
    mBluetoothHeadsetReceiver = NULL;
}

/**
 * Registers receiver for the broadcasted intent related the existence
 * of a BT SCO channel. Indicates if BT SCO streaming is on or off.
 */
void AudioManagerElastos::RegisterForBluetoothScoIntentBroadcast()
{
    AutoPtr<IIntentFilter> filter;
    AutoPtr<IIntent> intent;
    CIntentFilter::New(
        IAudioManager::ACTION_SCO_AUDIO_STATE_UPDATED,
        (IIntentFilter**)&filter);

    /** BroadcastReceiver implementation which handles changes in BT SCO. */
    mBluetoothScoReceiver = new BluetoothScoIntentBroadcastReceiver(this);

    mContext->RegisterReceiver(mBluetoothScoReceiver, filter, (IIntent**)&intent);
}

void AudioManagerElastos::UnregisterForBluetoothScoIntentBroadcast()
{
    mContext->UnregisterReceiver(mBluetoothScoReceiver);
    mBluetoothScoReceiver = NULL;
}

/** Enables BT audio using the SCO audio channel. */
void AudioManagerElastos::StartBluetoothSco()
{
    if (!mHasBluetoothPermission) {
        return;
    }

    if (mBluetoothScoState == STATE_BLUETOOTH_SCO_ON ||
        mBluetoothScoState == STATE_BLUETOOTH_SCO_TURNING_ON) {
        // Unable to turn on BT in this state.
        return;
    }

    // Check if audio is already routed to BT SCO; if so, just update
    // states but don't try to enable it again.
    Boolean isBluetoothScoOn;
    mAudioManager->IsBluetoothScoOn(&isBluetoothScoOn);
    if (isBluetoothScoOn) {
        mBluetoothScoState = STATE_BLUETOOTH_SCO_ON;
        return;
    }

    if (DEBUG) Logd(String("startBluetoothSco: turning BT SCO on..."));
    mBluetoothScoState = STATE_BLUETOOTH_SCO_TURNING_ON;
    mAudioManager->StartBluetoothSco();
}

/** Disables BT audio using the SCO audio channel. */
void AudioManagerElastos::StopBluetoothSco()
{
    if (!mHasBluetoothPermission) {
        return;
    }

    if (mBluetoothScoState != STATE_BLUETOOTH_SCO_ON &&
        mBluetoothScoState != STATE_BLUETOOTH_SCO_TURNING_ON) {
        // No need to turn off BT in this state.
        return;
    }

    Boolean isBluetoothScoOn = FALSE;
    mAudioManager->IsBluetoothScoOn(&isBluetoothScoOn);
    if (!isBluetoothScoOn) {
        // TODO(henrika): can we do anything else than logging here?
        Loge(String("Unable to stop BT SCO since it is already disabled"));
        return;
    }

    if (DEBUG) Logd(String("stopBluetoothSco: turning BT SCO off..."));
    mBluetoothScoState = STATE_BLUETOOTH_SCO_TURNING_OFF;
    mAudioManager->StopBluetoothSco();
}

/**
 * Changes selection of the currently active audio device.
 *
 * @param device Specifies the selected audio device.
 */
void AudioManagerElastos::SetAudioDevice(
    /* [in] */ Int32 device)
{
    if (DEBUG) Logger::D(TAG, "setAudioDevice(device=%d", device);

    // Ensure that the Bluetooth SCO audio channel is always disabled
    // unless the BT headset device is selected.
    if (device == DEVICE_BLUETOOTH_HEADSET) {
        StartBluetoothSco();
    }
    else {
        StopBluetoothSco();
    }

    switch (device) {
        case DEVICE_BLUETOOTH_HEADSET:
            break;
        case DEVICE_SPEAKERPHONE:
            SetSpeakerphoneOn(TRUE);
            break;
        case DEVICE_WIRED_HEADSET:
            SetSpeakerphoneOn(FALSE);
            break;
        case DEVICE_EARPIECE:
            SetSpeakerphoneOn(FALSE);
            break;
        default:
            Loge(String("Invalid audio device selection"));
            break;
    }

    ReportUpdate();
}

/**
 * Use a special selection scheme if the default device is selected.
 * The "most unique" device will be selected; Wired headset first,
 * then Bluetooth and last the speaker phone.
 */
Int32 AudioManagerElastos::SelectDefaultDevice(
    /* [in] */ ArrayOf<Boolean>* devices)
{
    if ((*devices)[DEVICE_WIRED_HEADSET]) {
        return DEVICE_WIRED_HEADSET;
    }
    else if ((*devices)[DEVICE_BLUETOOTH_HEADSET]) {
        // TODO(henrika): possibly need improvements here if we are
        // in a state where Bluetooth is turning off.
        return DEVICE_BLUETOOTH_HEADSET;
    }

    return DEVICE_SPEAKERPHONE;
}

/** Returns true if setDevice() has been called with a valid device id. */
Boolean AudioManagerElastos::DeviceHasBeenRequested()
{
    AutoLock lock(&mLock);
    return (mRequestedAudioDevice != DEVICE_INVALID);
}

/**
 * Updates the active device given the current list of devices and
 * information about if a specific device has been selected or if
 * the default device is selected.
 */
void AudioManagerElastos::UpdateDeviceActivation()
{
    AutoPtr< ArrayOf<Boolean> > devices;
    Int32 requested = DEVICE_INVALID;

    {
        AutoLock lock(&mLock);
        requested = mRequestedAudioDevice;
        devices = mAudioDevices->Clone();
    }

    if (requested == DEVICE_INVALID) {
        Loge(String("Unable to activate device since no device is selected"));
        return;
    }

    // Update default device if it has been selected explicitly, or
    // the selected device has been removed from the list.
    if (requested == DEVICE_DEFAULT || !(*devices)[requested]) {
        // Get default device given current list and activate the device.
        Int32 defaultDevice = SelectDefaultDevice(devices);
        SetAudioDevice(defaultDevice);
    }
    else {
        // Activate the selected device since we know that it exists in
        // the list.
        SetAudioDevice(requested);
    }
}

/** Returns number of available devices */
Int32 AudioManagerElastos::GetNumOfAudioDevices(
    /* [in] */ ArrayOf<Boolean>* devices)
{
    Int32 count = 0;
    for (Int32 i = 0; i < DEVICE_COUNT; ++i) {
        if ((*devices)[i])
            ++count;
    }

    return count;
}

/**
 * For now, just log the state change but the idea is that we should
 * notify a registered state change listener (if any) that there has
 * been a change in the state.
 * TODO(henrika): add support for state change listener.
 */
void AudioManagerElastos::ReportUpdate()
{
    AutoLock lock(&mLock);
    List<String> devices;// = new ArrayList<String>();
    for (Int32 i = 0; i < DEVICE_COUNT; ++i) {
        if (mAudioDevices[i])
            devices.PushBack((*DEVICE_NAMES)[i]);
    }

    if (DEBUG) {
        Logger::D(TAG, "reportUpdate: requested=%d, btSco=%d", mRequestedAudioDevice, mBluetoothScoState);
        for (Int32 i = 0; i < DEVICE_COUNT; ++i) {
            Logger::D(TAG, "devices[%d]: %s", i, (*DEVICE_NAMES)[i].string());
        }
    }
}

/** Information about the current build, taken from system properties. */
void AudioManagerElastos::LogDeviceInfo()
{
    Logger::D(TAG,"Android SDK: %d, Release: %s, Brand: %s, CPU_ABI:%s, Device:%s, \
            Id:%s, Hardware:%s, Manufacturer:%s, Model:%s, Product:%s",
            Build::VERSION::SDK_INT, Build::VERSION::RELEASE.string(), Build::BRAND.string(),
        Build::CPU_ABI.string(), Build::DEVICE.string(), Build::ID.string(), Build::HARDWARE.string(),
        Build::MANUFACTURER.string(), Build::MODEL.string(), Build::PRODUCT.string());
}

/** Trivial helper method for debug logging */
void AudioManagerElastos::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg.string());
}

/** Trivial helper method for error logging */
void AudioManagerElastos::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(TAG, msg.string());
}

/** Start thread which observes volume changes on the voice stream. */
void AudioManagerElastos::StartObservingVolumeChanges()
{
    if (DEBUG) Logd(String("startObservingVolumeChanges"));
    if (mSettingsObserverThread != NULL)
        return;

    CHandlerThread::New(String("SettingsObserver"), (IHandlerThread**)&mSettingsObserverThread);
    AutoPtr<IThread> thread = IThread::Probe(mSettingsObserverThread);
    thread->Start();

    mSettingsObserver = new InnerContentObserver(this);

    AutoPtr<IUri> uri;
    //Uri::Parse(String("content://") + ISettings::AUTHORITY + String("/system"), (IUri**)&uri);
    mContentResolver->RegisterContentObserver(
        uri/*TODO Settings::System::CONTENT_URI*/, TRUE, mSettingsObserver);
}

/** Quit observer thread and stop listening for volume changes. */
void AudioManagerElastos::StopObservingVolumeChanges()
{
    if (DEBUG) Logd(String("stopObservingVolumeChanges"));
    if (mSettingsObserverThread == NULL)
        return;

    mContentResolver->UnregisterContentObserver(mSettingsObserver);
    mSettingsObserver = NULL;

    Boolean result;
    mSettingsObserverThread->Quit(&result);
    // try {
    AutoPtr<IThread> thread = IThread::Probe(mSettingsObserverThread);
    thread->Join();
    // } catch (InterruptedException e) {
    //     Log.e(TAG, "Thread.join() exception: ", e);
    // }
    mSettingsObserverThread = NULL;
}

void AudioManagerElastos::NativeSetMute(
    /* [in] */ Int64 nativeAudioManagerElastos,
    /* [in] */ Boolean muted)
{
    AutoPtr<IInterface> iAudioManagerElastos = THIS_PROBE(IInterface);
    Elastos_AudioManagerAndroid_nativeSetMute(iAudioManagerElastos, nativeAudioManagerElastos, muted);
}

//begin callback functions definition
void AudioManagerElastos::Init(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::Init, amaObj is NULL");
        return;
    }
    amaObj->Init();
}

void AudioManagerElastos::Close(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::Close, amaObj is NULL");
        return;
    }
    amaObj->Close();
}

void AudioManagerElastos::SetCommunicationAudioModeOn(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean on)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::SetCommunicationAudioModeOn, amaObj is NULL");
        return;
    }
    amaObj->SetCommunicationAudioModeOn(on);
}

Boolean AudioManagerElastos::SetDevice(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& deviceId)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::SetDevice, amaObj is NULL");
        return FALSE;
    }
    return amaObj->SetDevice(deviceId);
}
AutoPtr<ArrayOf<IInterface*> > AudioManagerElastos::GetAudioInputDeviceNames(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::GetAudioInputDeviceNames, amaObj is NULL");
        return NULL;
    }
    return amaObj->GetAudioInputDeviceNames();
}

Int32 AudioManagerElastos::GetNativeOutputSampleRate(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::GetNativeOutputSampleRate, amaObj is NULL");
        return DEFAULT_SAMPLING_RATE;
    }
    return amaObj->GetNativeOutputSampleRate();
}

Boolean AudioManagerElastos::IsAudioLowLatencySupported(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::IsAudioLowLatencySupported, amaObj is NULL");
        return FALSE;
    }
    return amaObj->IsAudioLowLatencySupported();
}

Int32 AudioManagerElastos::GetAudioLowLatencyOutputFrameSize(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AudioManagerElastos> amaObj = (AudioManagerElastos*)(IObject::Probe(obj));
    if (NULL == amaObj)
    {
        Logger::E(TAG, "AudioManagerElastos::GetAudioLowLatencyOutputFrameSize, amaObj is NULL");
        return DEFAULT_FRAME_PER_BUFFER;
    }
    return amaObj->GetAudioLowLatencyOutputFrameSize();
}

AutoPtr<IInterface> AudioManagerElastos::CreateAudioManagerElastos(
    /* [in] */ IInterface* context,
    /* [in} */ Int64 nativeAudioManagerElastos)
{
    AutoPtr<IContext> cObj = (IContext*)(IContext::Probe(context));
    if (NULL == cObj)
    {
        Logger::E(TAG, "AudioManagerElastos::CreateAudioManagerElastos, amaObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(CreateAudioManagerElastos(cObj, nativeAudioManagerElastos));
}

} // namespace Media
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

