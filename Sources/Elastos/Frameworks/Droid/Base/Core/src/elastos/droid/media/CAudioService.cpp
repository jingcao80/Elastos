
#include "elastos/droid/media/CAudioService.h"
#include "elastos/droid/media/CAudioManager.h"
#include "elastos/droid/media/CAudioRoutesInfo.h"
#include "elastos/droid/media/CMediaPlayer.h"
#include "elastos/droid/media/CSoundPool.h"
#include "elastos/droid/media/CAudioSystem.h"
#include "elastos/droid/media/CAudioSystemHelper.h"
#include "elastos/droid/media/CAudioManagerHelper.h"
//#include "elastos/droid/media/ElAudioSystem.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CRemoteCallbackList.h"
#include "elastos/droid/os/CBinderHelper.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "bluetooth/CBluetoothAdapterHelper.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/widget/CVolumePanel.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include <unistd.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::EIID_IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothClassDevice;
using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::Widget::CVolumePanel;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Media {

const String CAudioService::TAG("AudioService");
const Boolean CAudioService::DEBUG_RC = FALSE;
const Boolean CAudioService::DEBUG_VOL = FALSE;

const Int32 CAudioService::PERSIST_DELAY = 500;

const Int32 CAudioService::SENDMSG_REPLACE = 0;
const Int32 CAudioService::SENDMSG_NOOP = 1;
const Int32 CAudioService::SENDMSG_QUEUE = 2;

const Int32 CAudioService::MSG_SET_DEVICE_VOLUME = 0;
const Int32 CAudioService::MSG_PERSIST_VOLUME = 1;
const Int32 CAudioService::MSG_PERSIST_MASTER_VOLUME = 2;
const Int32 CAudioService::MSG_PERSIST_RINGER_MODE = 3;
const Int32 CAudioService::MSG_MEDIA_SERVER_DIED = 4;
const Int32 CAudioService::MSG_MEDIA_SERVER_STARTED = 5;
const Int32 CAudioService::MSG_PLAY_SOUND_EFFECT = 6;
const Int32 CAudioService::MSG_BTA2DP_DOCK_TIMEOUT = 7;
const Int32 CAudioService::MSG_LOAD_SOUND_EFFECTS = 8;
const Int32 CAudioService::MSG_SET_FORCE_USE = 9;
const Int32 CAudioService::MSG_PERSIST_MEDIABUTTONRECEIVER = 10;
const Int32 CAudioService::MSG_BT_HEADSET_CNCT_FAILED = 11;
const Int32 CAudioService::MSG_RCDISPLAY_CLEAR = 12;
const Int32 CAudioService::MSG_RCDISPLAY_UPDATE = 13;
const Int32 CAudioService::MSG_SET_ALL_VOLUMES = 14;
const Int32 CAudioService::MSG_PERSIST_MASTER_VOLUME_MUTE = 15;
const Int32 CAudioService::MSG_REPORT_NEW_ROUTES = 16;
const Int32 CAudioService::MSG_REEVALUATE_REMOTE = 17;
const Int32 CAudioService::MSG_RCC_NEW_PLAYBACK_INFO = 18;
const Int32 CAudioService::MSG_RCC_NEW_VOLUME_OBS = 19;
const Int32 CAudioService::MSG_SET_FORCE_BT_A2DP_USE = 20;
const Int32 CAudioService::MSG_SET_WIRED_DEVICE_CONNECTION_STATE = 21;
const Int32 CAudioService::MSG_SET_A2DP_CONNECTION_STATE = 22;
const Int32 CAudioService::MSG_SET_RSX_CONNECTION_STATE = 23; // change remote submix connection
const Int32 CAudioService::MSG_CHECK_MUSIC_ACTIVE = 24;
const Int32 CAudioService::MSG_BROADCAST_AUDIO_BECOMING_NOISY = 25;
const Int32 CAudioService::MSG_CONFIGURE_SAFE_MEDIA_VOLUME = 26;
const Int32 CAudioService::MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED = 27;
const Int32 CAudioService::PERSIST_CURRENT = 0x1;
const Int32 CAudioService::PERSIST_LAST_AUDIBLE = 0x2;

const Int32 CAudioService::BTA2DP_DOCK_TIMEOUT_MILLIS = 8000;
const Int32 CAudioService::BT_HEADSET_CNCT_TIMEOUT_MS = 3000;

const Int32 CAudioService::NUM_SOUNDPOOL_CHANNELS = 4;
const Int32 CAudioService::MAX_MASTER_VOLUME = 100;
const Int32 CAudioService::MAX_BATCH_VOLUME_ADJUST_STEPS = 4;

const Int32 CAudioService::SCO_STATE_INACTIVE = 0;
const Int32 CAudioService::SCO_STATE_ACTIVATE_REQ = 1;
const Int32 CAudioService::SCO_STATE_ACTIVE_INTERNAL = 3;
const Int32 CAudioService::SCO_STATE_DEACTIVATE_REQ = 5;
const Int32 CAudioService::SCO_STATE_ACTIVE_EXTERNAL = 2;
const Int32 CAudioService::SCO_STATE_DEACTIVATE_EXT_REQ = 4;

const Int32 CAudioService::DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS = 5000;

const Int32 CAudioService::VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS = 1;
const Int32 CAudioService::VOICEBUTTON_ACTION_START_VOICE_INPUT = 2;
const Int32 CAudioService::VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS = 3;

Int32 CAudioService::sSoundEffectVolumeDb;
Mutex CAudioService::mAudioFocusLock;
Mutex CAudioService::mRingingLock;

const String CAudioService::EXTRA_WAKELOCK_ACQUIRED = String("android.media.AudioService.WAKELOCK_ACQUIRED");

Int32 CAudioService::sLastRccId = 0;

const String CAudioService::SOUND_EFFECTS_PATH = String("/media/audio/ui/");
const String CAudioService::SOUND_EFFECT_FILES[] = {
    String("Effect_Tick.ogg"),
    String("KeypressStandard.ogg"),
    String("KeypressSpacebar.ogg"),
    String("KeypressDelete.ogg"),
    String("KeypressReturn.ogg")
};

String CAudioService::RINGER_MODE_NAMES[3] = {
    String("SILENT"),
    String("VIBRATE"),
    String("NORMAL")
};

//--------------------------------------------------------
//    CAudioService::VolumeStreamState::VolumeDeathHandler
//--------------------------------------------------------

CAR_INTERFACE_IMPL(CAudioService::VolumeStreamState::VolumeDeathHandler, IProxyDeathRecipient);

CAudioService::VolumeStreamState::VolumeDeathHandler::VolumeDeathHandler(
    /* [in] */ IBinder* cb,
    /* [in] */ CAudioService::VolumeStreamState* state,
    /* [in] */ CAudioService* service)
    : mICallback(cb)
    , mMuteCount(0)
    , mVolumeStreamState(state)
    , mAudioService(service)
{}

ECode CAudioService::VolumeStreamState::VolumeDeathHandler::Mute(
    /* [in] */ Boolean state)
{
    ECode ec = NOERROR;
    if (state) {
        if (mMuteCount == 0) {
            do {
                // Register for client death notification
                // mICallback can be 0 if muted by AudioService
                if (mICallback != NULL) {
                    AutoPtr<IProxy> proxy = (IProxy*)mICallback->Probe(EIID_IProxy);
                    if (proxy != NULL) {
                        ec = proxy->LinkToDeath(this, 0);
                        if (FAILED(ec)) break;
                    }
                }

                mVolumeStreamState->mDeathHandlers.PushBack(this);

                // If the stream is not yet muted by any client, set level to 0
                if (mVolumeStreamState->MuteCount() == 0) {
                    {
                        AutoLock lock(&mVolumeStreamState->mIndexLock);
                        HashMap<Int32, Int32>::Iterator it = mVolumeStreamState->mIndex.Begin();
                        Int32 device;
                        Boolean bval;
                        while (it != mVolumeStreamState->mIndex.End()) {
                            device = it->mFirst;
                            ec = mVolumeStreamState->SetIndex(0, device, FALSE /* lastAudible */, &bval);
                            if (FAILED(ec)) break;
                            ++it;
                        }
                    }

                    ec = mAudioService->SendMsg(CAudioService::MSG_SET_ALL_VOLUMES,
                        CAudioService::SENDMSG_QUEUE, 0, 0,
                        (IInterface*)(mVolumeStreamState->Probe(EIID_IInterface)), 0);
                    if (FAILED(ec)) break;
                }
            } while(0);

            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                // Client has died!
                ProxyDied();
                return ec;
            }
        }
        else {
            Logger::W(CAudioService::TAG, "stream: %d was already muted by this client",
                mVolumeStreamState->mStreamType);
        }
        mMuteCount++;
    }
    else {
        if (mMuteCount == 0) {
            Logger::E(CAudioService::TAG, "unexpected unmute for stream: %d",
                mVolumeStreamState->mStreamType);
        }
        else {
            mMuteCount--;
            if (mMuteCount == 0) {
                // Unregister from client death notification
                mVolumeStreamState->mDeathHandlers.Remove(this);

                // mICallback can be 0 if muted by AudioService
                if (mICallback != NULL) {
                    AutoPtr<IProxy> proxy = (IProxy*)mICallback->Probe(EIID_IProxy);
                    if (proxy != NULL) {
                        Boolean result;
                        proxy->UnlinkToDeath(this, 0, &result);
                    }
                }

                if (mVolumeStreamState->MuteCount() == 0) {
                    // If the stream is not muted any more, restore its volume if
                    // ringer mode allows it
                    Boolean bval;
                    mAudioService->IsStreamAffectedByRingerMode(mVolumeStreamState->mStreamType, &bval);
                    if (!bval || mAudioService->mRingerMode == IAudioManager::RINGER_MODE_NORMAL) {
                        AutoLock lock(&mVolumeStreamState->mIndexLock);
                        HashMap<Int32, Int32>::Iterator it = mVolumeStreamState->mIndex.Begin();
                        Int32 device, index;
                        while (it != mVolumeStreamState->mIndex.End()) {
                            device = it->mFirst;
                            mVolumeStreamState->GetIndex(device, TRUE /* lastAudible */, &index);
                            ec = mVolumeStreamState->SetIndex(index, device, FALSE /* lastAudible */, &bval);
                            ++it;
                        }

                        ec = mAudioService->SendMsg(MSG_SET_ALL_VOLUMES, SENDMSG_QUEUE, 0, 0,
                            (IInterface*)(mVolumeStreamState->Probe(EIID_IInterface)), 0);
                    }
                }
            }
        }
    }

    return ec;
}

ECode CAudioService::VolumeStreamState::VolumeDeathHandler::ProxyDied()
{
    Logger::W(TAG, "Volume service client died for stream: %d",
        mVolumeStreamState->mStreamType);
    if (mMuteCount != 0) {
        // Reset all active mute requests from this client.
        mMuteCount = 1;
        return Mute(FALSE);
    }
    return NOERROR;
}

//-------------------------------------------------------
//    CAudioService::VolumeStreamState::VolumeStreamState
//-------------------------------------------------------

PInterface CAudioService::VolumeStreamState::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    return NULL;
}

UInt32 CAudioService::VolumeStreamState::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CAudioService::VolumeStreamState::Release()
{
    return ElRefBase::Release();
}

ECode CAudioService::VolumeStreamState::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IInterface;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

CAudioService::VolumeStreamState::VolumeStreamState(
    /* [in] */ const String& settingName,
    /* [in] */ Int32 streamType,
    /* [in] */ CAudioService* owner)
    : mStreamType(streamType)
    , mMuteCount(0)
    , mAudioService(owner)
{
    mVolumeIndexSettingName = settingName;
    mLastAudibleVolumeIndexSettingName = settingName + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE;
    mIndexMax = mAudioService->MAX_STREAM_VOLUME[streamType];

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->InitStreamVolume(streamType, 0, mIndexMax);
    mIndexMax *= 10;

    ReadSettings();
}

ECode CAudioService::VolumeStreamState::GetSettingNameForDevice(
    /* [in] */ Boolean lastAudible,
    /* [in] */ Int32 device,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String name = lastAudible ?
        mLastAudibleVolumeIndexSettingName :
        mVolumeIndexSettingName;

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    String suffix;
    audioSystemHelper->GetDeviceName(device, &suffix);

    if (suffix.IsNullOrEmpty()) {
        *result = name;
        return NOERROR;
    }
    *result = name + "_" + suffix;
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::ReadSettings()
{
    AutoLock lock(&mThisLock);

    Int32 remainingDevices = IAudioSystem::DEVICE_OUT_ALL;

    // do not read system stream volume from settings: this stream is always aliased
    // to another stream type and its volume is never persisted. Values in settings can
    // only be stale values
    // on first call to readSettings() at init time, muteCount() is always 0 so we will
    // always create entries for default device
    if ((mStreamType == IAudioSystem::STREAM_SYSTEM) ||
            (mStreamType == IAudioSystem::STREAM_SYSTEM_ENFORCED)) {
        Int32 index = 10 * CAudioManager::DEFAULT_STREAM_VOLUME[mStreamType];

        {
            AutoLock cameraSoundLock(&mAudioService->mCameraSoundForcedLock);
            if (mAudioService->mCameraSoundForced) {
                index = mIndexMax;
            }
        }
        if (MuteCount() == 0) {
            AutoLock indexLock(&mIndexLock);
            mIndex[IAudioSystem::DEVICE_OUT_DEFAULT] = index;
        }

        AutoLock lastIndexLock(&mLastAudibleIndexLock);
        mLastAudibleIndex[IAudioSystem::DEVICE_OUT_DEFAULT] = index;
        return NOERROR;
    }

    Int32 device, defaultIndex, index, lastAudibleIndex;
    String name;
    for (Int32 i = 0; remainingDevices != 0; i++) {
        device = (1 << i);
        if ((device & remainingDevices) == 0) {
            continue;
        }
        remainingDevices &= ~device;

        // ignore settings for fixed volume devices: volume should always be at max
        if (((*(mAudioService->mStreamVolumeAlias))[mStreamType] == IAudioSystem::STREAM_MUSIC) &&
                ((device & mAudioService->mFixedVolumeDevices) != 0)) {
            if (MuteCount() == 0) {
                AutoLock indexLock(&mIndexLock);
                mIndex[device] = index;
            }
            AutoLock lastIndexLock(&mLastAudibleIndexLock);
            mLastAudibleIndex[device] = index;
            continue;
        }

        // retrieve current volume for device
        GetSettingNameForDevice(FALSE /* lastAudible */, device, &name);
        // if no volume stored for current stream and device, use default volume if default
        // device, continue otherwise
        defaultIndex = (device == IAudioSystem::DEVICE_OUT_DEFAULT) ?
            CAudioManager::DEFAULT_STREAM_VOLUME[mStreamType] : -1;
        Settings::System::GetInt32ForUser(
                mAudioService->mContentResolver, name, defaultIndex,
                IUserHandle::USER_CURRENT, &index);
        if (index == -1) {
            continue;
        }

        // retrieve last audible volume for device
        GetSettingNameForDevice(TRUE  /* lastAudible */, device, &name);
        // use stored last audible index if present, otherwise use current index if not 0
        // or default index
        defaultIndex = (index > 0) ?
            index : CAudioManager::DEFAULT_STREAM_VOLUME[mStreamType];
        Settings::System::GetInt32ForUser(
                mAudioService->mContentResolver, name, defaultIndex,
                IUserHandle::USER_CURRENT, &lastAudibleIndex);

        // a last audible index of 0 should never be stored for ring and notification
        // streams on phones (voice capable devices).
        if ((lastAudibleIndex == 0) && mAudioService->mVoiceCapable &&
            ((*(mAudioService->mStreamVolumeAlias))[mStreamType]
                == IAudioSystem::STREAM_RING)) {
            lastAudibleIndex = CAudioManager::DEFAULT_STREAM_VOLUME[mStreamType];
            // Correct the data base
            mAudioService->SendMsg(MSG_PERSIST_VOLUME, SENDMSG_QUEUE, PERSIST_LAST_AUDIBLE,
                device, (IInterface*)(this->Probe(EIID_IInterface)), PERSIST_DELAY);
        }

        {
            AutoLock lastIndexLock(&mLastAudibleIndexLock);
            mLastAudibleIndex[device] = GetValidIndex(10 * lastAudibleIndex);
        }

        // the initial index should never be 0 for ring and notification streams on phones
        // (voice capable devices) if not in silent or vibrate mode.
        if ((index == 0) && (mAudioService->mRingerMode == IAudioManager::RINGER_MODE_NORMAL)
            && mAudioService->mVoiceCapable && ((*(mAudioService->mStreamVolumeAlias))[mStreamType]
                == IAudioSystem::STREAM_RING)) {
            index = lastAudibleIndex;
            // Correct the data base
            mAudioService->SendMsg(MSG_PERSIST_VOLUME, SENDMSG_QUEUE, PERSIST_CURRENT,
                device, (IInterface*)(this->Probe(EIID_IInterface)), PERSIST_DELAY);
        }

        if (MuteCount() == 0) {
            AutoLock indexLock(&mIndexLock);
            mIndex[device] = GetValidIndex(10 * index);
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::ApplyDeviceVolume(
    /* [in] */ Int32 device)
{
    Int32 index;
    FAIL_RETURN(GetIndex(device, FALSE  /* lastAudible */, &index));
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    return audioSystemHelper->SetStreamVolumeIndex(
        mStreamType, (index + 5)/10,  device);
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::ApplyAllVolumes()
{
    AutoLock lock (mThisLock);
    // apply default volume first: by convention this will reset all
    // devices volumes in audio policy manager to the supplied value
    Int32 index;
    FAIL_RETURN(GetIndex(IAudioSystem::DEVICE_OUT_DEFAULT, FALSE /* lastAudible */, &index));
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetStreamVolumeIndex(mStreamType,
        (index + 5)/10, IAudioSystem::DEVICE_OUT_DEFAULT);

    // then apply device specific volumes
    {
        AutoLock lock(&mIndexLock);
        HashMap<Int32, Int32>::Iterator it = mIndex.Begin();
        Int32 device, value;
        while (it != mIndex.End()) {
            device = it->mFirst;
            if (device != IAudioSystem::DEVICE_OUT_DEFAULT) {
                value = it->mSecond;
                audioSystemHelper->SetStreamVolumeIndex(mStreamType,
                    (value + 5)/10, device);
            }
            ++it;
        }
    }

    return NOERROR;
}

ECode CAudioService::VolumeStreamState::AdjustIndex(
     /* [in] */ Int32 deltaIndex,
     /* [in] */ Int32 device,
     /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 index;
    FAIL_RETURN(GetIndex(device, FALSE   /* lastAudible */, &index));
    return SetIndex(index + deltaIndex, device, TRUE  /* lastAudible */, result);
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::SetIndex(
    /* [in] */ Int32 index,
    /* [in] */ Int32 device,
    /* [in] */ Boolean lastAudible,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoLock lock(mThisLock);

    Int32 oldIndex;
    FAIL_RETURN(GetIndex(device, FALSE  /* lastAudible */, &oldIndex));
    index = GetValidIndex(index);

    {
        AutoLock lock(mAudioService->mCameraSoundForcedLock);
        if ((mStreamType == IAudioSystem::STREAM_SYSTEM_ENFORCED)
            && mAudioService->mCameraSoundForced) {
            index = mIndexMax;
        }
    }

    {
        AutoLock lock(&mIndexLock);
        mIndex[device] = GetValidIndex(index);
    }

    if (oldIndex != index) {
        if (lastAudible) {
            AutoLock lastIndexLock(&mLastAudibleIndexLock);
            mLastAudibleIndex[device] = index;
        }

        // Apply change to all streams using this one as alias
        // if changing volume of current device, also change volume of current
        // device on aliased stream
        Boolean currentDevice = (device == mAudioService->GetDeviceForStream(mStreamType));
        Int32 numStreamTypes;
        AutoPtr<IAudioSystemHelper> audioSystemHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
        audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

        Int32 scaledIndex;
        Boolean bval;
        for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
            if (streamType != mStreamType
                && (*(mAudioService->mStreamVolumeAlias))[streamType] == mStreamType) {
                scaledIndex = mAudioService->RescaleIndex(index, mStreamType, streamType);
                (*(mAudioService->mStreamStates))[streamType]->SetIndex(
                    scaledIndex, device, lastAudible, &bval);
                if (currentDevice) {
                    (*(mAudioService->mStreamStates))[streamType]->SetIndex(
                        scaledIndex, mAudioService->GetDeviceForStream(streamType),
                        lastAudible, &bval);
                }
            }
        }

        *result = TRUE;
    }

    return NOERROR;
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::GetIndex(
    /* [in] */ Int32 device,
    /* [in] */ Boolean lastAudible,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mThisLock);
    Int32 index = -1;
    if (lastAudible) {
        AutoLock lock(mLastAudibleIndexLock);

        HashMap<Int32, Int32>::Iterator it = mLastAudibleIndex.Find(device);
        if (it != mLastAudibleIndex.End()) {
            index = it->mSecond;
        }
        else {
            // there is always an entry for AudioSystem.DEVICE_OUT_DEFAULT
            it = mLastAudibleIndex.Find(IAudioSystem::DEVICE_OUT_DEFAULT);
            if (it != mLastAudibleIndex.End()) {
                index = it->mSecond;
            }
        }
    }
    else {
        AutoLock lock(&mIndexLock);

        HashMap<Int32, Int32>::Iterator it = mIndex.Find(device);
        if (it != mIndex.End()) {
            index = it->mSecond;
        }
        else {
            // there is always an entry for AudioSystem.DEVICE_OUT_DEFAULT
            it = mIndex.Find(IAudioSystem::DEVICE_OUT_DEFAULT);
            if (it != mIndex.End()) {
                index = it->mSecond;
            }
        }
    }

    *result = index;
    return NOERROR;
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::SetLastAudibleIndex(
    /* [in] */ Int32 index,
    /* [in] */ Int32 device)
{
    AutoLock lock(mThisLock);

    // Apply change to all streams using this one as alias
    // if changing volume of current device, also change volume of current
    // device on aliased stream
    Boolean currentDevice = (device == mAudioService->GetDeviceForStream(mStreamType));

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 numStreamTypes;
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

    Int32 scaledIndex;
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (streamType != mStreamType &&
                (*(mAudioService->mStreamVolumeAlias))[streamType] == mStreamType) {
            scaledIndex = mAudioService->RescaleIndex(index, mStreamType, streamType);
            (*(mAudioService->mStreamStates))[streamType]->SetLastAudibleIndex(scaledIndex, device);
            if (currentDevice) {
                (*(mAudioService->mStreamStates))[streamType]->SetLastAudibleIndex(
                    scaledIndex, mAudioService->GetDeviceForStream(streamType));
            }
        }
    }

    {
        AutoLock lock(mAudioService->mCameraSoundForcedLock);
        mLastAudibleIndex[device] = GetValidIndex(index);
    }
    return NOERROR;
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::AdjustLastAudibleIndex(
    /* [in] */ Int32 deltaIndex,
    /* [in] */ Int32 device)
{
    AutoLock lock(mThisLock);

    Int32 result;
    FAIL_RETURN(GetIndex(device, TRUE  /* lastAudible */, &result));
    return SetLastAudibleIndex(result + deltaIndex, device);
}

ECode CAudioService::VolumeStreamState::GetMaxIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIndexMax;
    return NOERROR;
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::SetAllIndexes(
    /* [in] */ VolumeStreamState* srcStream,
    /* [in] */ Boolean lastAudible)
{
    AutoLock lock(mThisLock);

    if (lastAudible) {
        AutoLock lock(mLastAudibleIndexLock);

        Int32 device, index, streamType;
        HashMap<Int32, Int32>::Iterator it = mLastAudibleIndex.Begin();
        for (; it != mLastAudibleIndex.End(); ++it) {
            device = it->mFirst;
            index = it->mSecond;
            srcStream->GetStreamType(&streamType);
            index = mAudioService->RescaleIndex(index, streamType, mStreamType);
            SetLastAudibleIndex(index, device);
        }
    }
    else {
        AutoLock lock(&mIndexLock);

        Boolean bval;
        Int32 device, index, streamType;
        HashMap<Int32, Int32>::Iterator it = mIndex.Begin();
        for (; it != mIndex.End(); ++it) {
            device = it->mFirst;
            index = it->mSecond;
            srcStream->GetStreamType(&streamType);
            index = mAudioService->RescaleIndex(index, streamType, mStreamType);
            SetIndex(index, device, FALSE /* lastAudible */, &bval);
        }
    }

    return NOERROR;
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::SetAllIndexesToMax()
{
    AutoLock lock(mThisLock);

    {
        AutoLock lock(&mIndexLock);
        HashMap<Int32, Int32>::Iterator it = mIndex.Begin();
        for (; it != mIndex.End(); ++it) {
            it->mSecond = mIndexMax;
        }
    }

    {
        AutoLock lock(&mLastAudibleIndexLock);
        HashMap<Int32, Int32>::Iterator it = mLastAudibleIndex.Begin();
        for (; it != mLastAudibleIndex.End(); ++it) {
            it->mSecond = mIndexMax;
        }
    }

    return NOERROR;
}

/*synchronized*/
ECode CAudioService::VolumeStreamState::Mute(
    /* [in] */ IBinder* cb,
    /* [in] */ Boolean state)
{
    AutoLock lock(mThisLock);

    AutoPtr<VolumeDeathHandler> handler = GetDeathHandler(cb, state);
    if (handler == NULL) {
        Logger::E(CAudioService::TAG, "Could not get client death handler for stream: %d", mStreamType);
        return NOERROR;
    }
    handler->Mute(state);
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::GetStreamType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStreamType;
    return NOERROR;
}

Int32 CAudioService::VolumeStreamState::GetValidIndex(
    /* [in] */ Int32 index)
{
    if (index < 0) {
        return 0;
    }
    else if (index > mIndexMax) {
        return mIndexMax;
    }

    return index;
}

/*synchronized*/
Int32 CAudioService::VolumeStreamState::MuteCount()
{
    AutoLock lock(mThisLock);
    return mMuteCount;
}

AutoPtr<CAudioService::VolumeStreamState::VolumeDeathHandler>
CAudioService::VolumeStreamState::GetDeathHandler(
    /* [in] */ IBinder* cb,
    /* [in] */ Boolean state)
{
    AutoPtr<VolumeDeathHandler> handler;
    List< AutoPtr<VolumeDeathHandler> >::Iterator it = mDeathHandlers.Begin();
    for (; it != mDeathHandlers.End(); ++it) {
        handler = *it;
        if (cb == handler->mICallback) {
            return handler;
        }
    }

    // If this is the first mute request for this client, create a new
    // client death handler. Otherwise, it is an out of sequence unmute request.
    if (state) {
        handler = new VolumeDeathHandler(cb, this, mAudioService);
    }
    else {
        handler = new VolumeDeathHandler(cb, this, mAudioService);
        // try{
            if (cb != NULL){
                AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
                if (proxy != NULL) proxy->LinkToDeath(handler, 0);
            }

            mDeathHandlers.PushBack(handler);
        // }catch (RemoteException e) {
        //     return null;
        // }
    }
    return handler;
}

String CAudioService::VolumeStreamState::ToString()
{
    StringBuilder sb("   Mute count: ");
    sb += MuteCount();

    sb += "   Current: ";
    {
        AutoLock lock(&mIndexLock);
        HashMap<Int32, Int32>::Iterator it = mIndex.Begin();
        for (; it != mIndex.End(); ++it) {
            sb += StringUtils::Int32ToHexString(it->mFirst);
            sb += ": ";
            sb += StringUtils::Int32ToHexString((it->mSecond + 5)/10);
            sb += ",";
        }
    }

    sb += "\n   Last audible: ";
    {
        AutoLock lock(&mLastAudibleIndexLock);
        HashMap<Int32, Int32>::Iterator it = mLastAudibleIndex.Begin();
        for (; it != mLastAudibleIndex.End(); ++it) {
            sb += StringUtils::Int32ToHexString(it->mFirst);
            sb += ": ";
            sb += StringUtils::Int32ToHexString((it->mSecond + 5)/10);
            sb += ",";
        }
    }

    return sb.ToString();
}

//-------------------------------------------
//    CAudioService::ForceControlStreamClient
//-------------------------------------------

CAR_INTERFACE_IMPL(CAudioService::ForceControlStreamClient, IProxyDeathRecipient);

CAudioService::ForceControlStreamClient::ForceControlStreamClient(
    /* [in] */ IBinder* cb,
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{
    mCb = cb;
    if (mCb != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
        if (proxy != NULL) {
            ECode ec = proxy->LinkToDeath(this, 0);
            if (FAILED(ec)) {
                // Client has died!
                Logger::W(TAG, "ForceControlStreamClient() could not link to %p binder death", mCb.Get());
                mCb = NULL;
            }
        }
    }
}

ECode CAudioService::ForceControlStreamClient::ProxyDied()
{
    AutoLock lock(mAudioService->mForceControlStreamLock);
    Logger::W(TAG, "SCO client died");
    if (mAudioService->mForceControlStreamClient.Get() != this) {
        Logger::W(TAG, "unregistered control stream client died");
    }
    else {
        mAudioService->mForceControlStreamClient = NULL;
        mAudioService->mVolumeControlStream = -1;
    }

    return NOERROR;
}

void CAudioService::ForceControlStreamClient::ReleaseResources()
{
    if (mCb != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
        if (proxy) {
            Boolean result;
            proxy->UnlinkToDeath(this, 0, &result);
        }
        mCb = NULL;
    }
}

//--------------------------------------
//    CAudioService::SetModeDeathHandler
//--------------------------------------

CAR_INTERFACE_IMPL(CAudioService::SetModeDeathHandler, IProxyDeathRecipient);

CAudioService::SetModeDeathHandler::SetModeDeathHandler(
     /* [in] */ IBinder* cb,
     /* [in] */ Int32 pid,
     /* [in] */ CAudioService* owner)
     : mCb(cb)
     , mPid(pid)
     , mMode(IAudioSystem::MODE_NORMAL)
     , mAudioService(owner)
{}

ECode CAudioService::SetModeDeathHandler::ProxyDied()
{
    Int32 newModeOwnerPid = 0;
    {
        AutoLock lock(mAudioService->mSetModeDeathHandlersLock);

        Logger::W(TAG, "setMode() client died");

        AutoPtr<SetModeDeathHandler> target = this;
        List< AutoPtr<SetModeDeathHandler> >::Iterator it =
            Find(mAudioService->mSetModeDeathHandlers.Begin(),
                mAudioService->mSetModeDeathHandlers.End(), target);

        if (it == mAudioService->mSetModeDeathHandlers.End()) {
            Logger::W(TAG, "unregistered setMode() client died");
        }
        else {
            newModeOwnerPid = mAudioService->SetModeInt(IAudioSystem::MODE_NORMAL, mCb, mPid);
        }
    }

    // when entering RINGTONE, IN_CALL or IN_COMMUNICATION mode, clear all
    // SCO connections not started by the application changing the mode
    if (newModeOwnerPid != 0) {
        mAudioService->DisconnectBluetoothSco(newModeOwnerPid);
    }
    return NOERROR;
}

Int32 CAudioService::SetModeDeathHandler::GetPid()
{
    return mPid;
}

void CAudioService::SetModeDeathHandler::SetMode(
    /* [in] */ Int32 mode)
{
    mMode = mode;
}

Int32 CAudioService::SetModeDeathHandler::GetMode()
{
    return mMode;
}

AutoPtr<IBinder> CAudioService::SetModeDeathHandler::GetBinder()
{
    return mCb;
}

//------------------------------------------
//    CAudioService::SoundPoolListenerThread
//------------------------------------------

CAudioService::SoundPoolListenerThread::SoundPoolListenerThread(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{
    Thread::constructor(String("SoundPoolListenerThread"));
}

ECode CAudioService::SoundPoolListenerThread::Run()
{
    Looper::Prepare();

    AutoPtr<ILooperHelper> looperHelper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    looperHelper->MyLooper((ILooper**)(&(mAudioService->mSoundPoolLooper)));
    {
        AutoLock lock(mAudioService->mSoundEffectsLock);

        if (mAudioService->mSoundPool != NULL) {
            mAudioService->mSoundPoolCallBack = new SoundPoolCallback(mAudioService);
            mAudioService->mSoundPool->SetOnLoadCompleteListener(
                mAudioService->mSoundPoolCallBack);
        }

        mAudioService->mSoundEffectsLock.Notify();
    }

    Looper::Loop();
    return NOERROR;
}

//------------------------------------
//    CAudioService::SoundPoolCallback
//------------------------------------

CAR_INTERFACE_IMPL(CAudioService::SoundPoolCallback, IOnLoadCompleteListener);

CAudioService::SoundPoolCallback::SoundPoolCallback(
    /* [in] */ CAudioService* owner)
    : mStatus(0)
    , mLastSample(0)
    , mAudioService(owner)
{
}

Int32 CAudioService::SoundPoolCallback::Status()
{
     return mStatus;
}

void CAudioService::SoundPoolCallback::SetLastSample(
    /* [in] */ Int32 sample)
{
    mLastSample = sample;
}

ECode CAudioService::SoundPoolCallback::OnLoadComplete(
    /* [in] */ ISoundPool* soundPool,
    /* [in] */ Int32 sampleId,
    /* [in] */ Int32 status)
{
    if (CAudioService::DEBUG_VOL)
        Logger::D("CAudioService::SoundPoolCallback", "OnLoadComplete sampleId %d, status %d", sampleId, status);

    AutoLock lock(mAudioService->mSoundEffectsLock);
    if (status != 0) {
        mStatus = status;
    }

    if (sampleId == mLastSample) {
        mAudioService->mSoundEffectsLock.Notify();
    }

    return NOERROR;
}

//----------------------------
//    CAudioService::ScoClient
//----------------------------

CAR_INTERFACE_IMPL(CAudioService::ScoClient, IProxyDeathRecipient);

CAudioService::ScoClient::ScoClient(
    /* [in] */ IBinder* cb,
    /* [in] */ CAudioService* owner)
    : mCb(cb)
    , mStartcount(0)
    , mAudioService(owner)
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    binderHelper->GetCallingPid(&mCreatorPid);
}

ECode CAudioService::ScoClient::ProxyDied()
{
    AutoLock lock(mAudioService->mScoClientsLock);

    Logger::W(TAG, "SCO client died");

    AutoPtr<ScoClient> target = this;
    List< AutoPtr<ScoClient> >::Iterator it =
        Find(mAudioService->mScoClients.Begin(),
            mAudioService->mScoClients.End(), target);

    if (it == mAudioService->mScoClients.End()) {
        Logger::W(TAG, "unregistered SCO client died");
    }
    else {
        ClearCount(TRUE);
        mAudioService->mScoClients.Erase(it);
    }
    return NOERROR;
}

void CAudioService::ScoClient::IncCount()
{
    {
        AutoLock lock(mAudioService->mScoClientsLock);

        RequestScoState(IBluetoothHeadset::STATE_AUDIO_CONNECTED);
        if (mStartcount == 0) {
            AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
            if (proxy != NULL) {
                ECode ec = proxy->LinkToDeath(this, 0);
                if (FAILED(ec)) {
                    // client has already died!
                    Logger::W(TAG, "ScoClient  incCount() could not link to %p binder death", mCb.Get());
                }
            }
        }
        mStartcount++;
    }
}

void CAudioService::ScoClient::DecCount()
{
    AutoLock lock(mAudioService->mScoClientsLock);

    if (mStartcount == 0) {
        Logger::W(TAG, "ScoClient.decCount() already 0");
    }
    else {
        mStartcount--;
        if (mStartcount == 0) {
            AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
            if (proxy) {
                Boolean result;
                ECode ec = proxy->UnlinkToDeath(this, 0, &result);
                if (FAILED(ec)) {
                    Logger::W(TAG, "decCount() going to 0 but not registered to binder");
                }
            }
        }
        RequestScoState(IBluetoothHeadset::STATE_AUDIO_DISCONNECTED);
    }
}

void CAudioService::ScoClient::ClearCount(
    /* [in] */ Boolean stopSco)
{
    {
        AutoLock lock(mAudioService->mScoClientsLock);
        if (mStartcount != 0) {
            AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
            if (proxy) {
                Boolean result;
                ECode ec = proxy->UnlinkToDeath(this, 0, &result);
                if (FAILED(ec)) {
                    Logger::W(TAG, "clearCount() mStartcount: %d != 0 but not registered to binder", mStartcount);
                }
            }
        }
        mStartcount = 0;
        if (stopSco) {
            RequestScoState(IBluetoothHeadset::STATE_AUDIO_DISCONNECTED);
        }
    }
}

Int32 CAudioService::ScoClient::GetCount()
{
    return mStartcount;
}

AutoPtr<IBinder> CAudioService::ScoClient::GetBinder()
{
    return mCb;
}

Int32 CAudioService::ScoClient::GetPid()
{
    return mCreatorPid;
}

Int32 CAudioService::ScoClient::TotalCount()
{
    AutoLock lock(mAudioService->mScoClientsLock);
    Int32 count = 0;
    List< AutoPtr<ScoClient> >::Iterator it = mAudioService->mScoClients.Begin();
    for (; it != mAudioService->mScoClients.End(); ++it) {
        count += (*it)->GetCount();
    }
    return count;
}

void CAudioService::ScoClient::RequestScoState(
    /* [in] */ Int32 state)
{
    mAudioService->CheckScoAudioState();
    if (TotalCount() == 0) {
        if (state == IBluetoothHeadset::STATE_AUDIO_CONNECTED) {
            // Make sure that the state transitions to CONNECTING even if we cannot initiate
            // the connection.
            mAudioService->BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_CONNECTING);

            // Accept SCO audio activation only in NORMAL audio mode or if the mode is
            // currently controlled by the same client process.
            {
                AutoLock lock(mAudioService->mSetModeDeathHandlersLock);

                if ((mAudioService->mSetModeDeathHandlers.IsEmpty()
                    || mAudioService->mSetModeDeathHandlers[0]->GetPid() == mCreatorPid)
                    && (mAudioService->mScoAudioState == SCO_STATE_INACTIVE
                        || mAudioService->mScoAudioState == SCO_STATE_DEACTIVATE_REQ)) {
                    if (mAudioService->mScoAudioState == SCO_STATE_INACTIVE) {
                        if (mAudioService->mBluetoothHeadset != NULL
                            && mAudioService->mBluetoothHeadsetDevice != NULL) {
                            Boolean state;
                            mAudioService->mBluetoothHeadset->StartScoUsingVirtualVoiceCall(
                                    mAudioService->mBluetoothHeadsetDevice, &state);
                            if (state) {
                                mAudioService->mScoAudioState = SCO_STATE_ACTIVE_INTERNAL;
                            }
                            else {
                                mAudioService->BroadcastScoConnectionState(
                                        IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
                            }
                        }
                        else if (mAudioService->GetBluetoothHeadset()) {
                            mAudioService->mScoAudioState = SCO_STATE_ACTIVATE_REQ;
                        }
                    }
                    else {
                        mAudioService->mScoAudioState = SCO_STATE_ACTIVE_INTERNAL;
                        mAudioService->BroadcastScoConnectionState(
                            IAudioManager::SCO_AUDIO_STATE_CONNECTED);
                    }
                }
                else {
                    mAudioService->BroadcastScoConnectionState(
                        IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
                }
            }
        }
        else if (state == IBluetoothHeadset::STATE_AUDIO_DISCONNECTED
            && (mAudioService->mScoAudioState == SCO_STATE_ACTIVE_INTERNAL
                || mAudioService->mScoAudioState == SCO_STATE_ACTIVATE_REQ)) {
            if (mAudioService->mScoAudioState == SCO_STATE_ACTIVE_INTERNAL) {
                if (mAudioService->mBluetoothHeadset != NULL
                    && mAudioService->mBluetoothHeadsetDevice != NULL) {
                    Boolean state;
                    mAudioService->mBluetoothHeadset->StopScoUsingVirtualVoiceCall(
                            mAudioService->mBluetoothHeadsetDevice, &state);
                    if (!state) {
                        mAudioService->mScoAudioState = SCO_STATE_INACTIVE;
                        mAudioService->BroadcastScoConnectionState(
                                IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
                    }
                }
                else if (mAudioService->GetBluetoothHeadset()) {
                    mAudioService->mScoAudioState = SCO_STATE_DEACTIVATE_REQ;
                }
            }
            else {
                mAudioService->mScoAudioState = SCO_STATE_INACTIVE;
                mAudioService->BroadcastScoConnectionState(
                    IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
            }
        }
    }
}

//------------------------------------
//    CAudioService::AudioSystemThread
//------------------------------------

CAudioService::AudioSystemThread::AudioSystemThread(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{
    Init(String("AudioService"));
}

ECode CAudioService::AudioSystemThread::Run()
{
    // Set this thread up so the handler will work on it
    Looper::Prepare();

    {
        AutoLock lock(mAudioService->mThisLock);
        mAudioService->mAudioHandler = new AudioHandler(mAudioService);

        //Notify that the handler has been created
        mAudioService->mThisLock.Notify();
    }

    // Listen for volume change requests that are set by VolumePanel
    Looper::Loop();
    return NOERROR;
}

//-------------------------------
//    CAudioService::AudioHandler
//-------------------------------

CAudioService::AudioHandler::AudioHandler(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{
}

CAudioService::AudioHandler::~AudioHandler()
{
}

ECode CAudioService::AudioHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    if (DEBUG_VOL) {
        static HashMap<Int32, String> infoMap;
        if (infoMap.IsEmpty()) {
            infoMap[MSG_SET_DEVICE_VOLUME] = String("MSG_SET_DEVICE_VOLUME");
            infoMap[MSG_PERSIST_VOLUME] = String("MSG_PERSIST_VOLUME");
            infoMap[MSG_PERSIST_MASTER_VOLUME] = String("MSG_PERSIST_MASTER_VOLUME");
            infoMap[MSG_PERSIST_RINGER_MODE] = String("MSG_PERSIST_RINGER_MODE");
            infoMap[MSG_MEDIA_SERVER_DIED] = String("MSG_MEDIA_SERVER_DIED");
            infoMap[MSG_MEDIA_SERVER_STARTED] = String("MSG_MEDIA_SERVER_STARTED");
            infoMap[MSG_PLAY_SOUND_EFFECT] = String("MSG_PLAY_SOUND_EFFECT");
            infoMap[MSG_BTA2DP_DOCK_TIMEOUT] = String("MSG_BTA2DP_DOCK_TIMEOUT");
            infoMap[MSG_LOAD_SOUND_EFFECTS] = String("MSG_LOAD_SOUND_EFFECTS");
            infoMap[MSG_SET_FORCE_USE] = String("MSG_SET_FORCE_USE");
            infoMap[MSG_PERSIST_MEDIABUTTONRECEIVER] = String("MSG_PERSIST_MEDIABUTTONRECEIVER");
            infoMap[MSG_BT_HEADSET_CNCT_FAILED] = String("MSG_BT_HEADSET_CNCT_FAILED");
            infoMap[MSG_RCDISPLAY_CLEAR] = String("MSG_RCDISPLAY_CLEAR");
            infoMap[MSG_RCDISPLAY_UPDATE] = String("MSG_RCDISPLAY_UPDATE");
            infoMap[MSG_SET_ALL_VOLUMES] = String("MSG_SET_ALL_VOLUMES");
            infoMap[MSG_PERSIST_MASTER_VOLUME_MUTE] = String("MSG_PERSIST_MASTER_VOLUME_MUTE");
            infoMap[MSG_REPORT_NEW_ROUTES] = String("MSG_REPORT_NEW_ROUTES");
            infoMap[MSG_REEVALUATE_REMOTE] = String("MSG_REEVALUATE_REMOTE");
            infoMap[MSG_RCC_NEW_PLAYBACK_INFO] = String("MSG_RCC_NEW_PLAYBACK_INFO");
            infoMap[MSG_RCC_NEW_VOLUME_OBS] = String("MSG_RCC_NEW_VOLUME_OBS");
            infoMap[MSG_SET_FORCE_BT_A2DP_USE] = String("MSG_SET_FORCE_BT_A2DP_USE");
        }

        Logger::D(TAG, "CAudioService::AudioHandler::HandleMessage: what:%s arg1:%d, arg2:%d",
            infoMap[what].string(), arg1, arg2);
    }

    switch (what) {
        case CAudioService::MSG_SET_DEVICE_VOLUME:
            SetDeviceVolume((VolumeStreamState*)(obj.Get()), arg1);
            break;

        case CAudioService::MSG_SET_ALL_VOLUMES:
            SetAllVolumes((VolumeStreamState*)(obj.Get()));
            break;

        case CAudioService::MSG_PERSIST_VOLUME:
            PersistVolume((VolumeStreamState*)(obj.Get()), arg1, arg2);
            break;

        case CAudioService::MSG_PERSIST_MASTER_VOLUME: {
            Boolean result;
            Settings::System::PutFloatForUser(
                    mAudioService->mContentResolver, ISettingsSystem::VOLUME_MASTER,
                    (Float)arg1 / (Float)1000.0, IUserHandle::USER_CURRENT, &result);
            break;
        }

        case CAudioService::MSG_PERSIST_MASTER_VOLUME_MUTE: {
            Boolean result;
            Settings::System::PutInt32ForUser(
                    mAudioService->mContentResolver, ISettingsSystem::VOLUME_MASTER_MUTE,
                    arg1, IUserHandle::USER_CURRENT, &result);
            break;
        }

        case CAudioService::MSG_PERSIST_RINGER_MODE: {
            // note that the value persisted is the current ringer mode, not the
            // value of ringer mode as of the time the request was made to persist
            Int32 value;
            mAudioService->GetRingerMode(&value);
            PersistRingerMode(value);
            break;
        }

        case CAudioService::MSG_MEDIA_SERVER_DIED: {
            if (!mAudioService->mMediaServerOk) {
                Logger::D(TAG, "Media server died.");
                // Force creation of new IAudioFlinger interface so that we are notified
                // when new media_server process is back to life.
                AutoPtr<IAudioSystemHelper> audioSystemHelper;
                CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
                audioSystemHelper->SetErrorCallback(mAudioService->mAudioSystemCallback);
                mAudioService->SendMsg(MSG_MEDIA_SERVER_DIED, SENDMSG_NOOP,
                    0, 0, NULL, 500);
            }
            break;
        }

        case CAudioService::MSG_MEDIA_SERVER_STARTED: {
            Logger::D(TAG, "Media server started.");
            // indicate to audio HAL that we start the reconfiguration phase after a media
            // server crash
            // Note that MSG_MEDIA_SERVER_STARTED message is only received when the media server
            // process restarts after a crash, not the first time it is started.
            AutoPtr<IAudioSystemHelper> audioSystemHelper;
            CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
            audioSystemHelper->SetParameters(String("restarting=true"));

            Int32 temp;
            // Restore device connection states
            {
                AutoLock lock(mAudioService->mConnectedDevicesLock);
                HashMap<Int32, String>::Iterator it = mAudioService->mConnectedDevices.Begin();
                while (it != mAudioService->mConnectedDevices.End()) {
                    audioSystemHelper->SetDeviceConnectionState(
                        it->mFirst, IAudioSystem::DEVICE_STATE_AVAILABLE, it->mSecond);
                   ++it;
                }
            }

            // Restore call state
            audioSystemHelper->SetPhoneState(mAudioService->mMode);

            // Restore forced usage for communcations and record
            audioSystemHelper->SetForceUse(
                IAudioSystem::FOR_COMMUNICATION, mAudioService->mForcedUseForComm);
            audioSystemHelper->SetForceUse(
                IAudioSystem::FOR_RECORD, mAudioService->mForcedUseForComm);
            audioSystemHelper->SetForceUse(
                IAudioSystem::FOR_SYSTEM, mAudioService->mCameraSoundForced ?
                IAudioSystem::FORCE_SYSTEM_ENFORCED : IAudioSystem::FORCE_NONE);

            // Restore stream volumes
            Int32 numStreamTypes;
            audioSystemHelper->GetNumStreamTypes(&numStreamTypes);
            for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
                VolumeStreamState* streamState = (*(mAudioService->mStreamStates))[streamType];
                audioSystemHelper->InitStreamVolume(streamType, 0,
                    (streamState->mIndexMax + 5) / 10);

                streamState->ApplyAllVolumes();
            }

            // Restore ringer mode
            mAudioService->GetRingerMode(&temp);
            mAudioService->SetRingerModeInt(temp, FALSE);

            // Restore master volume
            mAudioService->RestoreMasterVolume();

            // Reset device orientation (if monitored for this device)
            if (mAudioService->mMonitorOrientation) {
                mAudioService->SetOrientationForAudioSystem();
            }

            {
                AutoLock lock(mAudioService->mBluetoothA2dpEnabledLock);
                audioSystemHelper->SetForceUse(IAudioSystem::FOR_MEDIA,
                    mAudioService->mBluetoothA2dpEnabled ?
                    IAudioSystem::FORCE_NONE : IAudioSystem::FORCE_NO_BT_A2DP);
            }

            {
                AutoLock lock(mAudioService->mSettingsLock);
                audioSystemHelper->SetForceUse(IAudioSystem::FOR_DOCK,
                    mAudioService->mDockAudioMediaEnabled ?
                    IAudioSystem::FORCE_ANALOG_DOCK : IAudioSystem::FORCE_NONE);
            }

            // indicate the end of reconfiguration phase to audio HAL
            audioSystemHelper->SetParameters(String("restarting=false"));
            break;
        }

        case CAudioService::MSG_LOAD_SOUND_EFFECTS: {
            Boolean state;
            mAudioService->LoadSoundEffects(&state);
            break;
        }

        case CAudioService::MSG_PLAY_SOUND_EFFECT: {
            PlaySoundEffect(arg1, arg2);
            break;
        }

        case CAudioService::MSG_BTA2DP_DOCK_TIMEOUT: {
            //msg.obj  == address of BTA2DP device
            {
                AutoLock lock(mAudioService->mConnectedDevicesLock);
                String text;
                ICharSequence* seq = ICharSequence::Probe(obj);
                if (seq != NULL) {
                    seq->ToString(&text);
                }

                mAudioService->MakeA2dpDeviceUnavailableNow( text );
            }
            break;
        }

        case CAudioService::MSG_SET_FORCE_USE:
        case CAudioService::MSG_SET_FORCE_BT_A2DP_USE: {
            SetForceUse(arg1, arg2);
            break;
        }

        case CAudioService::MSG_PERSIST_MEDIABUTTONRECEIVER: {
            OnHandlePersistMediaButtonReceiver(IComponentName::Probe(obj));
            break;
        }

        case CAudioService::MSG_RCDISPLAY_CLEAR: {
            mAudioService->OnRcDisplayClear();
            break;
        }

        case CAudioService::MSG_RCDISPLAY_UPDATE: {
            // msg.obj is guaranteed to be non null
            mAudioService->OnRcDisplayUpdate((RemoteControlStackEntry*)(obj.Get()), arg1);
            break;
        }

        case CAudioService::MSG_BT_HEADSET_CNCT_FAILED: {
            mAudioService->ResetBluetoothSco();
            break;
        }

        case CAudioService::MSG_SET_WIRED_DEVICE_CONNECTION_STATE: {
            String text;
            ICharSequence* seq = ICharSequence::Probe(obj);
            if (seq) seq->ToString(&text);
            mAudioService->OnSetWiredDeviceConnectionState(arg1, arg2, text);
            mAudioService->mMediaEventWakeLock->ReleaseLock();
            break;
        }

        case CAudioService::MSG_SET_A2DP_CONNECTION_STATE: {
            mAudioService->OnSetA2dpConnectionState(IBluetoothDevice::Probe(obj), arg1);
            mAudioService->mMediaEventWakeLock->ReleaseLock();
            break;
        }

        case CAudioService::MSG_REPORT_NEW_ROUTES: {
            Int32 N;
            mAudioService->mRoutesObservers->BeginBroadcast(&N);
            if (N > 0) {
                AutoPtr<IAudioRoutesInfo> routes;
                {
                    AutoLock lock(mAudioService->mCurAudioRoutesLock);
                    CAudioRoutesInfo::New(mAudioService->mCurAudioRoutes,
                        (IAudioRoutesInfo**)&routes);
                }

                while (N > 0) {
                    N--;
                    AutoPtr<IInterface> tmp;
                    mAudioService->mRoutesObservers->GetBroadcastItem(N, (IInterface**)&tmp);
                    AutoPtr<IIAudioRoutesObserver> obs = IIAudioRoutesObserver::Probe(tmp.Get());
                    // try {
                    obs->DispatchAudioRoutesChanged(routes);
                    // } catch (RemoteException e) {
                    // }
                }
            }
            mAudioService->mRoutesObservers->FinishBroadcast();
            break;
        }

        case CAudioService::MSG_REEVALUATE_REMOTE: {
            mAudioService->OnReevaluateRemote();
            break;
        }

        case CAudioService::MSG_RCC_NEW_PLAYBACK_INFO: {
            Int32 value = 0;
            IInteger32* tmp = IInteger32::Probe(obj);
            if (tmp) tmp->GetValue(&value);
            mAudioService->OnNewPlaybackInfoForRcc(arg1 /* rccId */, arg2 /* key */,
                    value /* value */);
            break;
        }

        case CAudioService::MSG_RCC_NEW_VOLUME_OBS: {
            mAudioService->OnRegisterVolumeObserverForRcc(arg1 /* rccId */,
                IIRemoteVolumeObserver::Probe(obj) /* rvo */);
            break;
        }

        case CAudioService::MSG_SET_RSX_CONNECTION_STATE: {
            mAudioService->OnSetRsxConnectionState(arg1/*available*/, arg2/*address*/);
            break;
        }

        case CAudioService::MSG_CHECK_MUSIC_ACTIVE: {
            mAudioService->OnCheckMusicActive();
            break;
        }

        case CAudioService::MSG_BROADCAST_AUDIO_BECOMING_NOISY: {
            mAudioService->OnSendBecomingNoisyIntent();
            break;
        }

        case CAudioService::MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED:
        case CAudioService::MSG_CONFIGURE_SAFE_MEDIA_VOLUME: {
            mAudioService->OnConfigureSafeVolume((what == MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED));
            break;
        }
    }
    return NOERROR;
}

void CAudioService::AudioHandler::SetDeviceVolume(
     /* [in] */ VolumeStreamState* streamState,
     /* [in] */ Int32 device)
{
    if (streamState == NULL) return;

    // Apply volume
    streamState->ApplyDeviceVolume(device);

     // Apply change to all streams using this one as alias
    Int32 numStreamTypes;
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (streamType != streamState->mStreamType
            && (*(mAudioService->mStreamVolumeAlias))[streamType] == streamState->mStreamType) {
            (*(mAudioService->mStreamStates))[streamType]->ApplyDeviceVolume(
                mAudioService->GetDeviceForStream(streamType));
        }
    }

    // Post a persist volume msg
    mAudioService->SendMsg(MSG_PERSIST_VOLUME,
        SENDMSG_QUEUE,
        PERSIST_CURRENT|PERSIST_LAST_AUDIBLE,
        device,
        (IInterface*)(streamState->Probe(EIID_IInterface)),
        PERSIST_DELAY);
}

void CAudioService::AudioHandler::SetAllVolumes(
    /* [in] */ VolumeStreamState* streamState)
{
    if (streamState == NULL) return;

    // Apply volume
    streamState->ApplyAllVolumes();

    // Apply change to all streams using this one as alias
    Int32 numStreamTypes;
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (streamType != streamState->mStreamType
            && (*(mAudioService->mStreamVolumeAlias))[streamType] == streamState->mStreamType) {
          (*(mAudioService->mStreamStates))[streamType]->ApplyAllVolumes();
        }
    }
}

void CAudioService::AudioHandler::PersistVolume(
    /* [in] */ VolumeStreamState* streamState,
    /* [in] */ Int32 persistType,
    /* [in] */ Int32 device)
{
    if (streamState == NULL) return;

    if ((persistType & PERSIST_CURRENT) != 0) {
        String text;
        streamState->GetSettingNameForDevice(FALSE /* lastAudible */, device, &text);
        Int32 value;
        streamState->GetIndex(device, FALSE /* lastAudible */, &value);
        // Logger::D("PersistVolume", "TODO PersistVolume device %s, volume %d", text.string(), (value + 5)/ 10);
        Boolean result;
        Settings::System::PutInt32ForUser(mAudioService->mContentResolver,
                text, (value + 5)/ 10, IUserHandle::USER_CURRENT, &result);
    }

    if ((persistType & PERSIST_LAST_AUDIBLE) != 0) {
        String text;
        streamState->GetSettingNameForDevice(TRUE /* lastAudible */, device, &text);
        Int32 value;
        streamState->GetIndex(device, TRUE  /* lastAudible */, &value);
        Boolean result;
        Settings::System::PutInt32ForUser(mAudioService->mContentResolver,
                text, (value + 5) / 10, IUserHandle::USER_CURRENT, &result);
    }
}

void CAudioService::AudioHandler::PersistRingerMode(
    /* [in] */ Int32 ringerMode)
{
    Boolean result;
    Settings::Global::PutInt32(mAudioService->mContentResolver,
            ISettingsGlobal::MODE_RINGER, ringerMode, &result);
}

void CAudioService::AudioHandler::PlaySoundEffect(
    /* [in] */ Int32 effectType,
    /* [in] */ Int32 volume)
{
    Logger::D("CAudioService::AudioHandler", "+ PlaySoundEffect effectType %d, volume %d", effectType, volume);
    using Elastos::Core::Math;
    {
        AutoLock lock(mAudioService->mSoundEffectsLock);

        if (mAudioService->mSoundPool == NULL) {
            Logger::D("CAudioService::AudioHandler", "- PlaySoundEffect mSoundPool is null.");
            return;
        }

        Float volFloat;
        //use default if volume is not specified by caller
        if (volume < 0) {
            volFloat = (Float)Math::Pow(10, (Float)sSoundEffectVolumeDb/20);
        } else {
            volFloat = (Float) volume / 1000.0f;
        }

        Int32 soundFileIndex = (mAudioService->SOUND_EFFECT_FILES_MAP)[effectType][0];
        Int32 soundId = (mAudioService->SOUND_EFFECT_FILES_MAP)[effectType][1];
        if (soundId > 0) {
            Int32 value;
            mAudioService->mSoundPool->Play(soundId, volFloat, volFloat, 0, 0, 1.0f, &value);
            if (DEBUG_VOL) Logger::D("CAudioService::AudioHandler", "Play sound %d result %d", soundId, value);
        }
        else {
            AutoPtr<IMediaPlayer> mediaPlayer;
            CMediaPlayer::New((IMediaPlayer**)&mediaPlayer);
//            try {
                AutoPtr<IFile> file = Environment::GetRootDirectory();
                String text;
                file->ToString(&text);
                String filePath = text +
                    mAudioService->SOUND_EFFECTS_PATH +
                    mAudioService->SOUND_EFFECT_FILES[soundFileIndex];
                mediaPlayer->SetDataSource(filePath);
                mediaPlayer->SetAudioStreamType(IAudioSystem::STREAM_SYSTEM);
                mediaPlayer->Prepare();
                mediaPlayer->SetVolume(volFloat, volFloat);

                AutoPtr<MediaPlayerOnCompletionListener> OnCompletionListener
                        = new MediaPlayerOnCompletionListener(this);
                mediaPlayer->SetOnCompletionListener(OnCompletionListener);

                AutoPtr<MediaPlayerOnErrorListener> OnErrorListener
                        = new MediaPlayerOnErrorListener(this);
                mediaPlayer->SetOnErrorListener(OnErrorListener);

                mediaPlayer->Start();
//            } catch (IOException ex) {
//                Logger::W(TAG, "MediaPlayer IOException: "+ex);
//            } catch (IllegalArgumentException ex) {
//                Logger::W(TAG, "MediaPlayer IllegalArgumentException: "+ex);
//            } catch (IllegalStateException ex) {
//                Logger::W(TAG, "MediaPlayer IllegalStateException: "+ex);
//            }
        }
    }
    Logger::D("CAudioService::AudioHandler", "- PlaySoundEffect");
}

void CAudioService::AudioHandler::OnHandlePersistMediaButtonReceiver(
    /* [in] */ IComponentName* receiver)
{
    assert(receiver != NULL);
    String text;
    receiver->FlattenToString(&text);
    Boolean result;
    Settings::System::PutStringForUser(
        mAudioService->mContentResolver, ISettingsSystem::MEDIA_BUTTON_RECEIVER,
        receiver == NULL ? String("") : text, IUserHandle::USER_CURRENT, &result);
}

void CAudioService::AudioHandler::CleanupPlayer(
    /* [in] */ IMediaPlayer* mp)
{
    if (mp != NULL) {
    // try {
    FAIL_GOTO(mp->Stop(), _EXIT_);
    FAIL_GOTO(mp->ReleaseResources(), _EXIT_);
    // } catch (IllegalStateException ex) {
    //     Logger::W(TAG, "MediaPlayer IllegalStateException: "+ex);
    // }
    }
_EXIT_:
    Logger::W(TAG, "MediaPlayer IllegalStateException: "/*+ex*/);
    return;
}

void CAudioService::AudioHandler::SetForceUse(
    /* [in] */ Int32 usage,
    /* [in] */ Int32 config)
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetForceUse(usage, config);
}

//-----------------------------------
//    CAudioService::SettingsObserver
//-----------------------------------

CAudioService::SettingsObserver::SettingsObserver(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{
    CHandler::New((IHandler**)&mHandler);

    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED, (IUri**)&uri);
    mAudioService->mContentResolver->RegisterContentObserver(uri, FALSE, this);

    uri = NULL;
    Settings::Global::GetUriFor(ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED, (IUri**)&uri);
    mAudioService->mContentResolver->RegisterContentObserver(uri, FALSE, this);
}

ECode CAudioService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);

    // FIXME This synchronized is not necessary if mSettingsLock only protects mRingerMode.
    //       However there appear to be some missing locks around mRingerModeMutedStreams
    //       and mRingerModeAffectedStreams, so will leave this synchronized for now.
    //       mRingerModeMutedStreams and mMuteAffectedStreams are safe (only accessed once).
    {
        AutoLock lock(mAudioService->mSettingsLock);

        Int32 ringerModeAffectedStreams;
        Settings::System::GetInt32ForUser(mAudioService->mContentResolver,
                ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
                ((1 << IAudioSystem::STREAM_RING)|(1 << IAudioSystem::STREAM_NOTIFICATION)|
                (1 <<IAudioSystem::STREAM_SYSTEM)|(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED)),
                IUserHandle::USER_CURRENT, &ringerModeAffectedStreams);
        if (mAudioService->mVoiceCapable) {
            ringerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_MUSIC);
        }
        else {
            ringerModeAffectedStreams |= (1 << IAudioSystem::STREAM_MUSIC);
        }

        {
            AutoLock lock(mAudioService->mCameraSoundForcedLock);

            if (mAudioService->mCameraSoundForced) {
                ringerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
            }
            else {
                ringerModeAffectedStreams |= (1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
            }
        }

        if (ringerModeAffectedStreams != mAudioService->mRingerModeAffectedStreams) {
           /*
            * Ensure all stream types that should be affected by ringer mode
            * are in the proper state.
            */
            mAudioService->mRingerModeAffectedStreams = ringerModeAffectedStreams;
            Int32 value;
            mAudioService->GetRingerMode(&value);
            mAudioService->SetRingerModeInt(value, FALSE);
        }
        mAudioService->ReadDockAudioSettings(mAudioService->mContentResolver);
    }
    return NOERROR;
}

//------------------------------------------------
//    CAudioService::AudioServiceBroadcastReceiver
//------------------------------------------------

CAudioService::AudioServiceBroadcastReceiver::AudioServiceBroadcastReceiver(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{}

ECode CAudioService::AudioServiceBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 device, state;

    Logger::D("CAudioService::AudioServiceBroadcastReceiver", "OnReceive %s", action.string());

    if (action.Equals(IIntent::ACTION_DOCK_EVENT)) {
        Int32 dockState, config;
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
            IIntent::EXTRA_DOCK_STATE_UNDOCKED, &dockState);
        switch (dockState) {
            case IIntent::EXTRA_DOCK_STATE_DESK: {
                config = IAudioSystem::FORCE_BT_DESK_DOCK;
                break;
            }

            case IIntent::EXTRA_DOCK_STATE_CAR: {
                config = IAudioSystem::FORCE_BT_CAR_DOCK;
                break;
            }

            case IIntent::EXTRA_DOCK_STATE_LE_DESK: {
                AutoLock lock(mAudioService->mSettingsLock);
                if (mAudioService->mDockAudioMediaEnabled) {
                    config = IAudioSystem::FORCE_ANALOG_DOCK;
                }
                else {
                    config = IAudioSystem::FORCE_NONE;
                }
                break;
            }

            case IIntent::EXTRA_DOCK_STATE_HE_DESK: {
                config = IAudioSystem::FORCE_DIGITAL_DOCK;
                break;
            }

            case IIntent::EXTRA_DOCK_STATE_UNDOCKED:
            default:
                config = IAudioSystem::FORCE_NONE;
                break;
        }

        AutoPtr<IAudioSystemHelper> audioSystemHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
        audioSystemHelper->SetForceUse(IAudioSystem::FOR_DOCK, config);
    }
    else if (action.Equals(IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED)) {
        intent->GetInt32Extra(
            IBluetoothProfile::EXTRA_STATE, IBluetoothProfile::STATE_DISCONNECTED, &state);
        device = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
        String address;

        AutoPtr<IBluetoothDevice> btDevice;
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IBluetoothDevice::EXTRA_DEVICE, (IParcelable**)&parcelable);
        btDevice = (IBluetoothDevice*)parcelable->Probe(EIID_IBluetoothDevice);
        if (btDevice == NULL) {
            return NOERROR;
        }

        btDevice->GetAddress(&address);
        AutoPtr<IBluetoothClass> btClass;
        btDevice->GetBluetoothClass((IBluetoothClass**)&btClass);
        if (btClass != NULL) {
            Int32 value;
            btClass->GetDeviceClass(&value);
            switch (value) {
                case IBluetoothClassDevice::AUDIO_VIDEO_WEARABLE_HEADSET:
                case IBluetoothClassDevice::AUDIO_VIDEO_HANDSFREE: {
                    device = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                    break;
                }
                case IBluetoothClassDevice::AUDIO_VIDEO_CAR_AUDIO: {
                    device = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                    break;
                }
            }
        }

        AutoPtr<IBluetoothAdapterHelper> bluetoothAdapterHelper;
        Boolean bVal;
        bluetoothAdapterHelper->CheckBluetoothAddress(address, &bVal);
        if (!bVal) {
            address = "";
        }

        Boolean connected = (state == IBluetoothProfile::STATE_CONNECTED);
        if (mAudioService->HandleDeviceConnection(connected, device, address)) {
            AutoLock lock(mAudioService->mScoClientsLock);
            if (connected) {
                mAudioService->mBluetoothHeadsetDevice = btDevice;
            }
            else {
                mAudioService->mBluetoothHeadsetDevice = NULL;
                mAudioService->ResetBluetoothSco();
            }
        }
    }
    else if (action.Equals(IIntent::ACTION_USB_AUDIO_ACCESSORY_PLUG)
        || action.Equals(IIntent::ACTION_USB_AUDIO_DEVICE_PLUG)) {
        intent->GetInt32Extra(String("state"), 0, &state);

        Int32 alsaCard, alsaDevice;
        intent->GetInt32Extra(String("card"), -1, &alsaCard);
        intent->GetInt32Extra(String("device"), -1, &alsaDevice);

        String params = (alsaCard == -1 &&
            alsaDevice == -1 ? String("") : String("card=")
            + StringUtils::Int32ToString(alsaCard)
            + String(";device=")
            + StringUtils::Int32ToString(alsaDevice));

        device = action.Equals(IIntent::ACTION_USB_AUDIO_ACCESSORY_PLUG) ?
                IAudioSystem::DEVICE_OUT_USB_ACCESSORY : IAudioSystem::DEVICE_OUT_USB_DEVICE;

        Logger::V(TAG, String("Broadcast Receiver: Got %s, , state = %d, card: %d, device: %d")
            , (action.Equals(IIntent::ACTION_USB_AUDIO_ACCESSORY_PLUG) ?
                  "ACTION_USB_AUDIO_ACCESSORY_PLUG" : "ACTION_USB_AUDIO_DEVICE_PLUG")
            , state, alsaCard, alsaDevice);

        mAudioService->SetWiredDeviceConnectionState(device, state, params);
    }
    else if (action.Equals(IBluetoothHeadset::ACTION_AUDIO_STATE_CHANGED)) {
        Boolean broadcast = FALSE;
        Int32 scoAudioState = IAudioManager::SCO_AUDIO_STATE_ERROR;
        {
            AutoLock lock(mAudioService->mScoClientsLock);

            Int32 btState;
            intent->GetInt32Extra(IBluetoothProfile::EXTRA_STATE, -1, &btState);
            // broadcast intent if the connection was initated by AudioService
            if (!mAudioService->mScoClients.IsEmpty()
                && (mAudioService->mScoAudioState == SCO_STATE_ACTIVE_INTERNAL ||
                    mAudioService->mScoAudioState == SCO_STATE_ACTIVATE_REQ ||
                    mAudioService->mScoAudioState == SCO_STATE_DEACTIVATE_REQ)) {
                broadcast = TRUE;
            }

            switch (btState) {
                case IBluetoothHeadset::STATE_AUDIO_CONNECTED: {
                    scoAudioState = IAudioManager::SCO_AUDIO_STATE_CONNECTED;
                    if (mAudioService->mScoAudioState != SCO_STATE_ACTIVE_INTERNAL &&
                        mAudioService->mScoAudioState != SCO_STATE_DEACTIVATE_REQ &&
                        mAudioService->mScoAudioState != SCO_STATE_DEACTIVATE_EXT_REQ) {
                        mAudioService->mScoAudioState = SCO_STATE_ACTIVE_EXTERNAL;
                    }
                    break;
                }

                case IBluetoothHeadset::STATE_AUDIO_DISCONNECTED: {
                    scoAudioState = IAudioManager::SCO_AUDIO_STATE_DISCONNECTED;
                    mAudioService->mScoAudioState = SCO_STATE_INACTIVE;
                    mAudioService->ClearAllScoClients(0, FALSE);
                    break;
                }

                case IBluetoothHeadset::STATE_AUDIO_CONNECTING: {
                    if (mAudioService->mScoAudioState != SCO_STATE_ACTIVE_INTERNAL &&
                        mAudioService->mScoAudioState != SCO_STATE_DEACTIVATE_REQ &&
                        mAudioService->mScoAudioState != SCO_STATE_DEACTIVATE_EXT_REQ) {
                        mAudioService->mScoAudioState = SCO_STATE_ACTIVE_EXTERNAL;
                    }
                }

                default: {
                    // do not broadcast CONNECTING or invalid state
                    broadcast = FALSE;
                    break;
                }
            }
        }

        if (broadcast) {
            mAudioService->BroadcastScoConnectionState(scoAudioState);
            //FIXME: this is to maintain compatibility with deprecated intent
            // IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED. Remove when appropriate.
            AutoPtr<IIntent> newIntent;
            CIntent::New(IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED,(IIntent**)&newIntent);

            newIntent->PutExtra(IAudioManager::EXTRA_SCO_AUDIO_STATE, scoAudioState);
            mAudioService->SendStickyBroadcastToAll(newIntent);
        }
    }
    else if (action.Equals(IIntent::ACTION_BOOT_COMPLETED)) {
        mAudioService->HandleBootCompleted(context);
    }
    else if (action.Equals(IIntent::ACTION_PACKAGE_REMOVED)) {
        Boolean boolean;
        intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &boolean);
        if (!boolean) {
            // a package is being removed, not replaced
            AutoPtr<IUri> uri;
            intent->GetData((IUri**)&uri);
            String packageName;
            uri->GetSchemeSpecificPart(&packageName);
            if (packageName != NULL) {
                mAudioService->RemoveMediaButtonReceiverForPackage(packageName);
            }
        }
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        AutoPtr<IAudioSystemHelper> audioSystemHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
        audioSystemHelper->SetParameters(String("screen_state=on"));
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        AutoPtr<IAudioSystemHelper> audioSystemHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
        audioSystemHelper->SetParameters(String("screen_state=off"));
    }
    else if (action.EqualsIgnoreCase(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        mAudioService->HandleConfigurationChanged(context);
    }
    else if (action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        // attempt to stop music playback for background user
        mAudioService->SendMsg(MSG_BROADCAST_AUDIO_BECOMING_NOISY,
            SENDMSG_REPLACE, 0, 0, NULL, 0);
        // the current audio focus owner is no longer valid
        mAudioService->DiscardAudioFocusOwner();

        // load volume settings for new user
        mAudioService->ReadAudioSettings(TRUE /*userSwitch*/);
        VolumeStreamState* state = (*(mAudioService->mStreamStates))[IAudioSystem::STREAM_MUSIC];
        // preserve STREAM_MUSIC volume from one user to the next.
        mAudioService->SendMsg(MSG_SET_ALL_VOLUMES,
            SENDMSG_QUEUE, 0, 0, (IInterface*)(state->Probe(EIID_IInterface)), 0);
    }
    return NOERROR;
}

//------------------------------------------------
//    CAudioService::KeyEventDoneBroadcastReceiver
//------------------------------------------------

CAudioService::KeyEventDoneBroadcastReceiver::KeyEventDoneBroadcastReceiver(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{}

ECode CAudioService::KeyEventDoneBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (extras == NULL) {
        return NOERROR;
    }
    Boolean bval;
    if (extras->ContainsKey(mAudioService->EXTRA_WAKELOCK_ACQUIRED, &bval), bval) {
        mAudioService->mMediaEventWakeLock->ReleaseLock();
    }
    return NOERROR;
}

//-----------------------------------------
//    CAudioService::AudioFocusDeathHandler
//-----------------------------------------

CAR_INTERFACE_IMPL(CAudioService::AudioFocusDeathHandler, IProxyDeathRecipient);

CAudioService::AudioFocusDeathHandler::AudioFocusDeathHandler(
    /* [in] */ IBinder* cb,
    /* [in] */ CAudioService* owner)
    : mCb(cb)
    , mAudioService(owner)
{}

ECode CAudioService::AudioFocusDeathHandler::ProxyDied()
{
    AutoLock lock(mAudioFocusLock);

    Logger::W(TAG, "  AudioFocus   audio focus client died");
    mAudioService->RemoveFocusStackEntryForClient(mCb);
    return NOERROR;
}

AutoPtr<IBinder> CAudioService::AudioFocusDeathHandler::GetBinder()
{
    return mCb;
}

//-------------------------------------------
//    CAudioService::FocusStackEntry (static)
//-------------------------------------------

CAudioService::FocusStackEntry::FocusStackEntry()
    : mStreamType(-1)
    , mFocusChangeType(0)
    , mCallingUid(0)
{}

CAudioService::FocusStackEntry::FocusStackEntry(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 duration,
    /* [in] */ IIAudioFocusDispatcher* afl,
    /* [in] */ IBinder* source,
    /* [in] */ const String& id,
    /* [in] */ AudioFocusDeathHandler* hdlr,
    /* [in] */ const String& pn,
    /* [in] */ Int32 uid)
    : mStreamType(streamType)
    , mFocusDispatcher(afl)
    , mSourceRef(source)
    , mClientId(id)
    , mFocusChangeType(duration)
    , mHandler(hdlr)
    , mPackageName(pn)
    , mCallingUid(uid)
{}

CAudioService::FocusStackEntry::~FocusStackEntry()
{
    Finalize();
}

void CAudioService::FocusStackEntry::UnlinkToDeath()
{
//    try {
        if (mSourceRef != NULL && mHandler != NULL) {
            AutoPtr<IProxy> proxy = (IProxy*)mSourceRef->Probe(EIID_IProxy);
            if (proxy) {
                Boolean result;
                ECode ec = proxy->UnlinkToDeath(mHandler, 0, &result);
                mHandler = NULL;
                if (FAILED(ec)) {
                    Logger::E(TAG, "Encountered %08x in FocusStackEntry.unlinkToDeath()", ec);
                }
            }
        }
//    } catch (java.util.NoSuchElementException e) {
//        Logger::E(TAG, "Encountered " /*+ e +*/ " in FocusStackEntry.unlinkToDeath()");
//    }
}

void CAudioService::FocusStackEntry::Finalize()
{
     UnlinkToDeath(); // unlink exception handled inside method
/*Eddie(E_NOT_IMPLEMENTED)*/
//     super.finalize();
}

//---------------------------------------
//    CAudioService::RcClientDeathHandler
//---------------------------------------

CAR_INTERFACE_IMPL(CAudioService::RcClientDeathHandler, IProxyDeathRecipient);

CAudioService::RcClientDeathHandler::RcClientDeathHandler(
    /* [in] */ IBinder* cb,
    /* [in] */ IPendingIntent* pi,
    /* [in] */ CAudioService* owner)
    : mCb(cb)
    , mMediaIntent(pi)
    , mAudioService(owner)
{}

ECode CAudioService::RcClientDeathHandler::ProxyDied()
{
    Logger::W(TAG, "  RemoteControlClient died");
    // remote control client died, make sure the displays don't use it anymore
    //  by setting its remote control client to null
    Int32 value;
    String text;
    mAudioService->RegisterRemoteControlClient(
        mMediaIntent, NULL/*rcClient*/, text/*ignored*/, &value);
    // the dead client was maybe handling remote playback, reevaluate
    mAudioService->PostReevaluateRemote();
    return NOERROR;
}

AutoPtr<IBinder> CAudioService::RcClientDeathHandler::GetBinder()
{
    return mCb;
}

//--------------------------------------
//    CAudioService::RemotePlaybackState
//--------------------------------------

CAudioService::RemotePlaybackState::RemotePlaybackState(
    /* [in] */ Int32 id,
    /* [in] */ Int32 vol,
    /* [in] */ Int32 volMax)
    : mRccId(id)
    , mVolume(vol)
    , mVolumeMax(volMax)
    , mVolumeHandling(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING)
{}

//---------------------------------------------------
//    CAudioService::RemoteControlStackEntry (static)
//---------------------------------------------------

PInterface CAudioService::RemoteControlStackEntry::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    return NULL;
}

UInt32 CAudioService::RemoteControlStackEntry::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CAudioService::RemoteControlStackEntry::Release()
{
    return ElRefBase::Release();
}

ECode CAudioService::RemoteControlStackEntry::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IInterface;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

CAudioService::RemoteControlStackEntry::RemoteControlStackEntry(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* eventReceiver)
    : mRccId(IRemoteControlClient::RCSE_ID_UNREGISTERED)
    , mMediaIntent(mediaIntent)
    , mReceiverComponent(eventReceiver)
    , mCallingUid(-1)
    , mPlaybackType(0)
    , mPlaybackVolume(0)
    , mPlaybackVolumeMax(0)
    , mPlaybackVolumeHandling(0)
    , mPlaybackStream(0)
    , mPlaybackState(0)
{
    mRccId = ++CAudioService::sLastRccId;
    ResetPlaybackInfo();
}

CAudioService::RemoteControlStackEntry::~RemoteControlStackEntry()
{
    UnlinkToRcClientDeath();// unlink exception handled inside method
    Finalize();
}

void CAudioService::RemoteControlStackEntry::ResetPlaybackInfo()
{
    mPlaybackType = IRemoteControlClient::PLAYBACK_TYPE_LOCAL;
    mPlaybackVolume = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME;
    mPlaybackVolumeMax = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME;
    mPlaybackVolumeHandling = IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING;
    mPlaybackStream = IAudioManager::STREAM_MUSIC;
    mPlaybackState = IRemoteControlClient::PLAYSTATE_STOPPED;
    mRemoteVolumeObs = NULL;
}

void CAudioService::RemoteControlStackEntry::UnlinkToRcClientDeath()
{
    if ((mRcClientDeathHandler != NULL) && (mRcClientDeathHandler->mCb != NULL)) {
//        try {
            AutoPtr<IProxy> proxy = (IProxy*)mRcClientDeathHandler->mCb->Probe(EIID_IProxy);
            if (proxy) {
                Boolean result;
                ECode ec = proxy->UnlinkToDeath(mRcClientDeathHandler, 0, &result);
                mRcClientDeathHandler = NULL;
                if (FAILED(ec)) {
                    // not much we can do here
                    Logger::E(TAG, "Encountered %08x in unlinkToRcClientDeath()", ec);
                }
            }
//        } catch (java.util.NoSuchElementException e) {
            // not much we can do here
//            Logger::E(TAG, "Encountered " /*+ e +*/ " in unlinkToRcClientDeath()");
//            e.printStackTrace();
//        }
    }
}

void CAudioService::RemoteControlStackEntry::Finalize()
{
    UnlinkToRcClientDeath();// unlink exception handled inside method
}

//----------------------------------------
//    CAudioService::RcDisplayDeathHandler
//----------------------------------------

CAR_INTERFACE_IMPL(CAudioService::RcDisplayDeathHandler, IProxyDeathRecipient);

CAudioService::RcDisplayDeathHandler::RcDisplayDeathHandler(
    /* [in] */ IBinder* b,
    /* [in] */ CAudioService* owner)
    : mCb(b)
    , mAudioService(owner)
{
    if (DEBUG_RC) {
        Logger::I(TAG, "new RcDisplayDeathHandler for "/*+b*/);
    }
}

ECode CAudioService::RcDisplayDeathHandler::ProxyDied()
{
    {
        AutoLock lock(mAudioService->mRCStackLock);
        Logger::W(TAG, "RemoteControl: display died");
        mAudioService->mRcDisplay = NULL;
    }
    return NOERROR;
}

void CAudioService::RcDisplayDeathHandler::UnlinkToRcDisplayDeath()
{
    if (DEBUG_RC) {
        Logger::I(TAG, "unlinkToRcDisplayDeath for "/*+mCb*/);
    }

//    try {
    AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
    if (proxy) {
        Boolean result;
        ECode ec = proxy->UnlinkToDeath(this, 0, &result);
        if (ec == (ECode)E_NO_SUCH_ELEMENT_EXCEPTION) {
            // not much we can do here, the display was being unregistered anyway
            Logger::E(TAG, "Encountered %08x in unlinkToRcDisplayDeath()", ec);
        }
    }
//    } catch (java.util.NoSuchElementException e) {
//        // not much we can do here, the display was being unregistered anyway
//        Logger::E(TAG, "Encountered " /*+ e +*/ " in unlinkToRcDisplayDeath()");
//        e.printStackTrace();
//    }
}

//-----------------------------------------------------------------------------
//    CAudioService::MediaPlayerOnCompletionListener (for OnCompletionListener)
//-----------------------------------------------------------------------------

CAR_INTERFACE_IMPL(CAudioService::MediaPlayerOnCompletionListener, IMediaPlayerOnCompletionListener);

CAudioService::MediaPlayerOnCompletionListener::MediaPlayerOnCompletionListener(
    /* [in] */ AudioHandler* owner)
    : mAudioService(owner)
{}

ECode CAudioService::MediaPlayerOnCompletionListener::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    mAudioService->CleanupPlayer(mp);
    return NOERROR;
}

//-------------------------------------------------------------------
//    CAudioService::MediaPlayerOnErrorListener (for OnErrorListener)
//-------------------------------------------------------------------

CAR_INTERFACE_IMPL(CAudioService::MediaPlayerOnErrorListener, IMediaPlayerOnErrorListener);

CAudioService::MediaPlayerOnErrorListener::MediaPlayerOnErrorListener(
    /* [in] */ AudioHandler* owner)
    : mAudioHandler(owner)
{}

ECode CAudioService::MediaPlayerOnErrorListener::OnError(
    /* [in] */ IMediaPlayer* mp,
    /* [in] */ Int32 what,
    /* [in] */ Int32 extra,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mAudioHandler->CleanupPlayer(mp);
    *result = TRUE;
    return NOERROR;
}

//-------------------------------------------------------------------
//    CAudioService::AudioSystemErrorCallback
//-------------------------------------------------------------------
CAR_INTERFACE_IMPL(CAudioService::AudioSystemErrorCallback, IAudioSystemErrorCallback);

CAudioService::AudioSystemErrorCallback::AudioSystemErrorCallback(
    /* [in] */ CAudioService* owner)
    : mAudioService(owner)
{}

ECode CAudioService::AudioSystemErrorCallback::OnError(
   /* [in] */ Int32 error)
{
    switch (error) {
        case IAudioSystem::AUDIO_STATUS_SERVER_DIED:
            if (mAudioService->mMediaServerOk) {
                mAudioService->SendMsg(CAudioService::MSG_MEDIA_SERVER_DIED,
                    CAudioService::SENDMSG_NOOP, 0, 0, NULL, 1500);
                mAudioService->mMediaServerOk = FALSE;
            }
        break;

        case IAudioSystem::AUDIO_STATUS_OK:
            if (!mAudioService->mMediaServerOk) {
                mAudioService->SendMsg(CAudioService::MSG_MEDIA_SERVER_STARTED,
                    CAudioService::SENDMSG_NOOP, 0, 0, NULL, 0);
                mAudioService->mMediaServerOk = TRUE;
            }
        break;

        default:
        break;
    }

    return NOERROR;
}

//--------------------------------
//    CAudioService::CAudioService
//--------------------------------

CAudioService::CAudioService()
    : mVoiceCapable(FALSE)
    , mMode(0)
    , mMediaServerOk(FALSE)
    , mRingerMode(0)
    , mRingerModeAffectedStreams(0)
    , mRingerModeMutedStreams(0)
    , mMuteAffectedStreams(0)
    , mVibrateSetting(0)
    , mHasVibrator(FALSE)
    , mIsRinging(FALSE)
    , mForcedUseForComm(0)
    , mUseMasterVolume(FALSE)
    , mScoAudioState(0)
    , mScoConnectionState(0)
    , mBootCompleted(FALSE)
    , mVolumeControlStream(-1)
    , mDeviceOrientation(IConfiguration::ORIENTATION_UNDEFINED)
    , mBluetoothA2dpEnabled(FALSE)
    , mFixedVolumeDevices(IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET |
        IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET |
        IAudioSystem::DEVICE_OUT_ALL_USB)
    , mMonitorOrientation(FALSE)
    , mDockAudioMediaEnabled(TRUE)
    , mDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mBecomingNoisyIntentDevices(0)
    , mVoiceButtonDown(FALSE)
    , mVoiceButtonHandled(FALSE)
    , mCurrentRcClientGen(0)
    , mMainRemoteIsActive(FALSE)
    , mHasRemotePlayback(FALSE)
    , mArtworkExpectedWidth(-1)
    , SAFE_MEDIA_VOLUME_NOT_CONFIGURED(0)
    , SAFE_MEDIA_VOLUME_DISABLED(1)
    , SAFE_MEDIA_VOLUME_INACTIVE(2)
    , SAFE_MEDIA_VOLUME_ACTIVE(3)
    , mSafeMediaVolumeState(0)
    , mMcc(0)
    , mSafeMediaVolumeIndex(0)
    , mSafeMediaVolumeDevices(0)
    , mMusicActiveMs(0)
    , mCameraSoundForced(FALSE)
{
    SOUND_EFFECT_FILES_MAP[0][0] = 0; SOUND_EFFECT_FILES_MAP[0][1] = -1; // FX_KEY_CLICK
    SOUND_EFFECT_FILES_MAP[1][0] = 0; SOUND_EFFECT_FILES_MAP[1][1] = -1; // FX_FOCUS_NAVIGATION_UP
    SOUND_EFFECT_FILES_MAP[2][0] = 0; SOUND_EFFECT_FILES_MAP[2][1] = -1; // FX_FOCUS_NAVIGATION_DOWN
    SOUND_EFFECT_FILES_MAP[3][0] = 0; SOUND_EFFECT_FILES_MAP[3][1] = -1; // FX_FOCUS_NAVIGATION_LEFT
    SOUND_EFFECT_FILES_MAP[4][0] = 0; SOUND_EFFECT_FILES_MAP[4][1] = -1; // FX_FOCUS_NAVIGATION_RIGHT
    SOUND_EFFECT_FILES_MAP[5][0] = 1; SOUND_EFFECT_FILES_MAP[5][1] = -1; // FX_KEYPRESS_STANDARD
    SOUND_EFFECT_FILES_MAP[6][0] = 2; SOUND_EFFECT_FILES_MAP[6][1] = -1; // FX_KEYPRESS_SPACEBAR
    SOUND_EFFECT_FILES_MAP[7][0] = 3; SOUND_EFFECT_FILES_MAP[7][1] = -1; // FX_FOCUS_DELETE
    SOUND_EFFECT_FILES_MAP[8][0] = 4; SOUND_EFFECT_FILES_MAP[8][1] = -1; // FX_FOCUS_RETURN

    MAX_STREAM_VOLUME[0] = 5;  // STREAM_VOICE_CALL
    MAX_STREAM_VOLUME[1] = 7;  // STREAM_SYSTEM
    MAX_STREAM_VOLUME[2] = 7;  // STREAM_RING
    MAX_STREAM_VOLUME[3] = 15; // STREAM_MUSIC
    MAX_STREAM_VOLUME[4] = 7;  // STREAM_ALARM
    MAX_STREAM_VOLUME[5] = 7;  // STREAM_NOTIFICATION
    MAX_STREAM_VOLUME[6] = 15; // STREAM_BLUETOOTH_SCO
    MAX_STREAM_VOLUME[7] = 7;  // STREAM_SYSTEM_ENFORCED
    MAX_STREAM_VOLUME[8] = 15; // STREAM_DTMF
    MAX_STREAM_VOLUME[9] = 15; // STREAM_TTS

    STREAM_VOLUME_ALIAS[0] = IAudioSystem::STREAM_VOICE_CALL;      // STREAM_VOICE_CALL
    STREAM_VOLUME_ALIAS[1] = IAudioSystem::STREAM_RING;            // STREAM_SYSTEM
    STREAM_VOLUME_ALIAS[2] = IAudioSystem::STREAM_RING;            // STREAM_RING
    STREAM_VOLUME_ALIAS[3] = IAudioSystem::STREAM_MUSIC;           // STREAM_MUSIC
    STREAM_VOLUME_ALIAS[4] = IAudioSystem::STREAM_ALARM;           // STREAM_ALARM
    STREAM_VOLUME_ALIAS[5] = IAudioSystem::STREAM_RING;            // STREAM_NOTIFICATION
    STREAM_VOLUME_ALIAS[6] = IAudioSystem::STREAM_BLUETOOTH_SCO;   // STREAM_BLUETOOTH_SCO
    STREAM_VOLUME_ALIAS[7] = IAudioSystem::STREAM_RING;            // STREAM_SYSTEM_ENFORCED
    STREAM_VOLUME_ALIAS[8] = IAudioSystem::STREAM_RING;            // STREAM_DTMF
    STREAM_VOLUME_ALIAS[9] = IAudioSystem::STREAM_MUSIC;           // STREAM_TTS

    STREAM_VOLUME_ALIAS_NON_VOICE[0] = IAudioSystem::STREAM_VOICE_CALL;      // STREAM_VOICE_CALL
    STREAM_VOLUME_ALIAS_NON_VOICE[1] = IAudioSystem::STREAM_MUSIC;           // STREAM_SYSTEM
    STREAM_VOLUME_ALIAS_NON_VOICE[2] = IAudioSystem::STREAM_RING;            // STREAM_RING
    STREAM_VOLUME_ALIAS_NON_VOICE[3] = IAudioSystem::STREAM_MUSIC;           // STREAM_MUSIC
    STREAM_VOLUME_ALIAS_NON_VOICE[4] = IAudioSystem::STREAM_ALARM;           // STREAM_ALARM
    STREAM_VOLUME_ALIAS_NON_VOICE[5] = IAudioSystem::STREAM_RING;            // STREAM_NOTIFICATION
    STREAM_VOLUME_ALIAS_NON_VOICE[6] = IAudioSystem::STREAM_BLUETOOTH_SCO;   // STREAM_BLUETOOTH_SCO
    STREAM_VOLUME_ALIAS_NON_VOICE[7] = IAudioSystem::STREAM_MUSIC;           // STREAM_SYSTEM_ENFORCED
    STREAM_VOLUME_ALIAS_NON_VOICE[8] = IAudioSystem::STREAM_MUSIC;           // STREAM_DTMF
    STREAM_VOLUME_ALIAS_NON_VOICE[9] = IAudioSystem::STREAM_MUSIC;           // STREAM_TTS

    STREAM_NAMES[0] = "STREAM_VOICE_CALL";
    STREAM_NAMES[1] = "STREAM_SYSTEM";
    STREAM_NAMES[2] = "STREAM_RING";
    STREAM_NAMES[3] = "STREAM_MUSIC";
    STREAM_NAMES[4] = "STREAM_ALARM";
    STREAM_NAMES[5] = "STREAM_NOTIFICATION";
    STREAM_NAMES[6] = "STREAM_BLUETOOTH_SCO";
    STREAM_NAMES[7] = "STREAM_SYSTEM_ENFORCED";
    STREAM_NAMES[8] = "STREAM_DTMF";
    STREAM_NAMES[9] = "STREAM_TTS";

    mReceiver = new AudioServiceBroadcastReceiver(this);

    CAudioRoutesInfo::New((IAudioRoutesInfo**)&mCurAudioRoutes);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mRoutesObservers);

    mFixedVolumeDevices = IAudioSystem::DEVICE_OUT_AUX_DIGITAL
        | IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET
        | IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET
        | IAudioSystem::DEVICE_OUT_ALL_USB;

//    CBluetoothProfileServiceListener::New((IBluetoothProfileServiceListener**)&mBluetoothProfileServiceListener);

    mBecomingNoisyIntentDevices =
        IAudioSystem::DEVICE_OUT_WIRED_HEADSET
        | IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE
        | IAudioSystem::DEVICE_OUT_ALL_A2DP
        | IAudioSystem::DEVICE_OUT_AUX_DIGITAL
        | IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET
        | IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET
        | IAudioSystem::DEVICE_OUT_ALL_USB;

    mKeyEventDone = new KeyEventDoneBroadcastReceiver(this);

    mSafeMediaVolumeDevices = IAudioSystem::DEVICE_OUT_WIRED_HEADSET
        | IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE;

    mAudioSystemCallback = new AudioSystemErrorCallback(this);
//    pthread_cond_init(&mWaitCond, NULL);
}

CAudioService::~CAudioService()
{
    mStreamStates = NULL;
//
//    pthread_cond_destroy(&mWaitCond);
}

ECode CAudioService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    context->GetContentResolver((IContentResolver**)&mContentResolver);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::config_voice_capable, &mVoiceCapable);

    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(tmpObj.Get());
    if (pm != NULL) {
        pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("handleMediaEvent"), (IPowerManagerWakeLock**)&mMediaEventWakeLock);
    }

    mHasVibrator = FALSE;
    tmpObj = NULL;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IVibrator> vibrator = IVibrator::Probe(tmpObj.Get());
    if (vibrator != NULL) {
        vibrator->HasVibrator(&mHasVibrator);
    }

    // Intialized volume
    MAX_STREAM_VOLUME[IAudioSystem::STREAM_VOICE_CALL] = SystemProperties::GetInt32(
        String("ro.config.vc_call_vol_steps"),
        MAX_STREAM_VOLUME[IAudioSystem::STREAM_VOICE_CALL]);

    resources->GetInteger(R::integer::config_soundEffectVolumeDb, &sSoundEffectVolumeDb);

    ECode ec = CVolumePanel::New(context, THIS_PROBE(IAudioService), (IVolumePanel**)&mVolumePanel);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to create volume panel! %08x", ec);
        return ec;
    }

    mMode = IAudioSystem::MODE_NORMAL;
    mForcedUseForComm = IAudioSystem::FORCE_NONE;

    CreateAudioSystemThread();

    Boolean cameraSoundForced;
    resources->GetBoolean(R::bool_::config_camera_sound_forced, &cameraSoundForced);

    mCameraSoundForced = cameraSoundForced;

    SendMsg(MSG_SET_FORCE_USE,
        SENDMSG_QUEUE,
        IAudioSystem::FOR_SYSTEM,
        cameraSoundForced ? IAudioSystem::FORCE_SYSTEM_ENFORCED : IAudioSystem::FORCE_NONE,
        NULL,
        0);

    ReadPersistedSettings();
    mSettingsObserver = new SettingsObserver(this);

    UpdateStreamVolumeAlias(FALSE /*updateVolumes*/);

    CreateStreamStates();

    mMediaServerOk = TRUE;

    // Call setRingerModeInt() to apply correct mute
    // state on streams affected by ringer mode.
    mRingerModeMutedStreams = 0;

    Int32 value;
    GetRingerMode(&value);
    SetRingerModeInt(value, FALSE);

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetErrorCallback(mAudioSystemCallback);

    // Register for device connection intent broadcasts.
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IBluetoothHeadset::ACTION_AUDIO_STATE_CHANGED, (IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED);
    intentFilter->AddAction(IIntent::ACTION_DOCK_EVENT);
    intentFilter->AddAction(IIntent::ACTION_USB_AUDIO_ACCESSORY_PLUG);
    intentFilter->AddAction(IIntent::ACTION_USB_AUDIO_DEVICE_PLUG);
    intentFilter->AddAction(IIntent::ACTION_BOOT_COMPLETED);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    intentFilter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);

    // Register a configuration change listener only if requested by system properties
    // to monitor orientation changes (off by default)
    mMonitorOrientation = SystemProperties::GetBoolean(String("ro.audio.monitorOrientation"), FALSE);
    if (mMonitorOrientation) {
        Logger::V(TAG, "monitoring device orientation");
        // initialize orientation in AudioSystem
        SetOrientationForAudioSystem();
    }

    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&intent);

    // Register for package removal intent broadcasts for media button receiver persistence
    AutoPtr<IIntentFilter> pkgFilter;
    CIntentFilter::New((IIntentFilter**)&pkgFilter);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    pkgFilter->AddDataScheme(String("package"));
    intent = NULL;
    context->RegisterReceiver(mReceiver, pkgFilter, (IIntent**)&intent);

    // Register for phone state monitoring
    // TODO
    // tmpObj = NULL;
    // context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&tmpObj);
    // AutoPtr<ITelephonyManager> tmgr = ITelephonyManager::Probe(tmpObj.Get());
    // if (tmgr != NULL) {
    //     tmgr->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_CALL_STATE);
    // }

    resources->GetBoolean(R::bool_::config_useMasterVolume, &mUseMasterVolume);
    RestoreMasterVolume();
    resources->GetInt32Array(R::array::config_masterVolumeRamp, (ArrayOf<Int32>**)&mMasterVolumeRamp);

    mMainRemote = new RemotePlaybackState(-1,
        MAX_STREAM_VOLUME[IAudioManager::STREAM_MUSIC],
        MAX_STREAM_VOLUME[IAudioManager::STREAM_MUSIC]);

    mHasRemotePlayback = FALSE;
    mMainRemoteIsActive = FALSE;
    PostReevaluateRemote();

    return NOERROR;
}

void CAudioService::CreateAudioSystemThread()
{
    mAudioSystemThread = new AudioSystemThread(this);
    mAudioSystemThread->Start();
    WaitForAudioHandlerCreation();
}

void CAudioService::WaitForAudioHandlerCreation()
{
    AutoLock lock(mThisLock);
    ECode ec = NOERROR;
    while (mAudioHandler == NULL) {
        // try {
        // Wait for mAudioHandler to be set by the other thread
        ec = mThisLock.Wait();
        if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
            Logger::E(TAG, "Interrupted while waiting on volume handler.");
        }
        // } catch (InterruptedException e) {
        // Logger::E(TAG, "Interrupted while waiting on volume handler.");
        // }
    }
}

void CAudioService::CheckAllAliasStreamVolumes()
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 numStreamTypes;
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

    for (Int32 streamType = 0; streamType < numStreamTypes; streamType++) {
        if (streamType != (*mStreamVolumeAlias)[streamType]) {
            (*mStreamStates)[streamType]->SetAllIndexes(
                (*mStreamStates)[(*mStreamVolumeAlias)[streamType]],
                FALSE /*lastAudible*/);

            (*mStreamStates)[streamType]->SetAllIndexes(
                (*mStreamStates)[(*mStreamVolumeAlias)[streamType]],
                TRUE /*lastAudible*/);
        }

        // apply stream volume
        if ((*mStreamStates)[streamType]->MuteCount() == 0) {
            (*mStreamStates)[streamType]->ApplyAllVolumes();
        }
    }
}

void CAudioService::CreateStreamStates()
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 numStreamTypes;
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

    mStreamStates = ArrayOf<VolumeStreamState*>::Alloc(numStreamTypes);

    for (Int32 i = 0; i < numStreamTypes; i++) {
        AutoPtr<VolumeStreamState> state = new VolumeStreamState(
            (*Settings::System::VOLUME_SETTINGS)[(*mStreamVolumeAlias)[i]], i, this);
        mStreamStates->Set(i, state);
    }

    CheckAllAliasStreamVolumes();
}

String CAudioService::DumpStreamStates()
{
    StringBuilder sb("\nStream volumes (device: index)");

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 numStreamTypes;
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

    for (Int32 i = 0; i < numStreamTypes; i++) {
        sb += "- ";
        sb += STREAM_NAMES[i];
        sb += ":";
        sb += (*mStreamStates)[i]->ToString();
        sb += "\n";
    }

    sb += "\n- mute affected streams = 0x";
    sb += StringUtils::Int32ToHexString(mMuteAffectedStreams);
    sb += "\n";
    return sb.ToString();
}

void CAudioService::UpdateStreamVolumeAlias(
    /* [in] */ Boolean updateVolumes)
{
    Int32 dtmfStreamAlias;
    if (mVoiceCapable) {
        mStreamVolumeAlias = ArrayOf<Int32>::Alloc(10);
        mStreamVolumeAlias->Copy(STREAM_VOLUME_ALIAS, 10);
        dtmfStreamAlias = IAudioSystem::STREAM_RING;
    }
    else {
        mStreamVolumeAlias = ArrayOf<Int32>::Alloc(10);
        mStreamVolumeAlias->Copy(STREAM_VOLUME_ALIAS_NON_VOICE, 10);
        dtmfStreamAlias = IAudioSystem::STREAM_MUSIC;
    }

    if (IsInCommunication()) {
        dtmfStreamAlias = IAudioSystem::STREAM_VOICE_CALL;
    }

    (*mStreamVolumeAlias)[IAudioSystem::STREAM_DTMF] = dtmfStreamAlias;
    if (updateVolumes) {
        (*mStreamStates)[IAudioSystem::STREAM_DTMF]->SetAllIndexes(
            (*mStreamStates)[dtmfStreamAlias], FALSE /*lastAudible*/);
        (*mStreamStates)[IAudioSystem::STREAM_DTMF]->SetAllIndexes(
            (*mStreamStates)[dtmfStreamAlias], TRUE /*lastAudible*/);
        SendMsg(MSG_SET_ALL_VOLUMES,
            SENDMSG_QUEUE,
            0,
            0,
            (IInterface*)((*mStreamStates)[IAudioSystem::STREAM_DTMF]->Probe(EIID_IInterface)), 0);
    }
}

void CAudioService::ReadDockAudioSettings(
    /* [in] */ IContentResolver* cr)
{
   Int32 tempValue = Settings::Global::GetInt32(cr, ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED, 0);
   mDockAudioMediaEnabled = tempValue == 1;

    if (mDockAudioMediaEnabled) {
        mBecomingNoisyIntentDevices |= IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET;
    }
    else {
        mBecomingNoisyIntentDevices &= ~IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET;
    }

    SendMsg(MSG_SET_FORCE_USE,
        SENDMSG_QUEUE,
        IAudioSystem::FOR_DOCK,
        mDockAudioMediaEnabled ? IAudioSystem::FORCE_ANALOG_DOCK : IAudioSystem::FORCE_NONE,
        NULL,
        0);
}

void CAudioService::ReadPersistedSettings()
{
    AutoPtr<IContentResolver> cr = mContentResolver;

    Int32 ringerModeFromSettings;
    Settings::Global::GetInt32(cr, ISettingsGlobal::MODE_RINGER,
            IAudioManager::RINGER_MODE_NORMAL, &ringerModeFromSettings);

    Int32 ringerMode = ringerModeFromSettings;
    // sanity check in case the settings are restored from a device with incompatible
    // ringer modes
    AutoPtr<IAudioManagerHelper> audioManagerSystemHelper;
    CAudioManagerHelper::AcquireSingleton((IAudioManagerHelper**)&audioManagerSystemHelper);
    Boolean tempState;
    audioManagerSystemHelper->IsValidRingerMode(ringerMode, &tempState);
    if (!tempState) {
        ringerMode = IAudioManager::RINGER_MODE_NORMAL;
    }
    if ((ringerMode == IAudioManager::RINGER_MODE_VIBRATE) && !mHasVibrator) {
        ringerMode = IAudioManager::RINGER_MODE_SILENT;
    }
    if (ringerMode != ringerModeFromSettings) {
        Boolean result;
        Settings::Global::PutInt32(cr, ISettingsGlobal::MODE_RINGER, ringerMode, &result);
    }

    {
        AutoLock lock(mSettingsLock);

        mRingerMode = ringerMode;

        // System.VIBRATE_ON is not used any more but defaults for mVibrateSetting
        // are still needed while setVibrateSetting() and getVibrateSetting() are being
        // deprecated.
        GetValueForVibrateSetting(0,
            IAudioManager::VIBRATE_TYPE_NOTIFICATION,
            mHasVibrator ? IAudioManager::VIBRATE_SETTING_ONLY_SILENT
                : IAudioManager::VIBRATE_SETTING_OFF, &mVibrateSetting);

        GetValueForVibrateSetting(mVibrateSetting,
            IAudioManager::VIBRATE_TYPE_RINGER,
            mHasVibrator ? IAudioManager::VIBRATE_SETTING_ONLY_SILENT
                : IAudioManager::VIBRATE_SETTING_OFF, &mVibrateSetting);

        // make sure settings for ringer mode are consistent with device type: non voice capable
        // devices (tablets) include media stream in silent mode whereas phones don't.
        Settings::System::GetInt32ForUser(cr,
                ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
                ((1 << IAudioSystem::STREAM_RING)|(1 << IAudioSystem::STREAM_NOTIFICATION)|
                (1 << IAudioSystem::STREAM_SYSTEM)|(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED)),
                IUserHandle::USER_CURRENT, &mRingerModeAffectedStreams);

        // ringtone, notification and system streams are always affected by ringer mode
        mRingerModeAffectedStreams |=
            (1 << IAudioSystem::STREAM_RING)
            | (1 << IAudioSystem::STREAM_NOTIFICATION)
            | (1 << IAudioSystem::STREAM_SYSTEM);

        if (mVoiceCapable) {
            mRingerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_MUSIC);
        }
        else {
            mRingerModeAffectedStreams |= (1 << IAudioSystem::STREAM_MUSIC);
        }

        {
            AutoLock lock(mCameraSoundForcedLock);

            if (mCameraSoundForced) {
                mRingerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
            }
            else {
                mRingerModeAffectedStreams |= (1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
            }
        }

        Boolean result;
        Settings::System::PutInt32ForUser(cr,
                ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
                mRingerModeAffectedStreams,
                IUserHandle::USER_CURRENT, &result);

        ReadDockAudioSettings(cr);
    }

    Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::MUTE_STREAMS_AFFECTED,
            ((1 << IAudioSystem::STREAM_MUSIC)
                | (1 << IAudioSystem::STREAM_RING)
                | (1 << IAudioSystem::STREAM_SYSTEM)),
            IUserHandle::USER_CURRENT, &mMuteAffectedStreams);

    Int32 tempValue;
    Settings::System::GetInt32ForUser(
            cr, ISettingsSystem::VOLUME_MASTER_MUTE,
            0, IUserHandle::USER_CURRENT, &tempValue);
    Boolean masterMute = tempValue == 1;

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetMasterMute(masterMute);

    BroadcastMasterMuteStatus(masterMute);

    // Each stream will read its own persisted settings

    // Broadcast the sticky intent
    BroadcastRingerMode(ringerMode);

    // Broadcast vibrate settings
    BroadcastVibrateSetting(IAudioManager::VIBRATE_TYPE_RINGER);
    BroadcastVibrateSetting(IAudioManager::VIBRATE_TYPE_NOTIFICATION);

    // Restore the default media button receiver from the system settings
    RestoreMediaButtonReceiver();
}

Int32 CAudioService::RescaleIndex(
    /* [in] */ Int32 index,
    /* [in] */ Int32 srcStream,
    /* [in] */ Int32 dstStream)
{
    Int32 tempValue1;
    Int32 tempValue2;
    (*mStreamStates)[dstStream]->GetMaxIndex(&tempValue1);
    (*mStreamStates)[srcStream]->GetMaxIndex(&tempValue2);
    return (index *  tempValue1 +  tempValue2 / 2) / tempValue2;
}

///////////////////////////////////////////////////////////////////////////
// IPC methods
///////////////////////////////////////////////////////////////////////////

ECode CAudioService::AdjustVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    return AdjustSuggestedStreamVolume(direction, IAudioManager::USE_DEFAULT_STREAM_TYPE, flags);
}

ECode CAudioService::AdjustLocalOrRemoteStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction)
{
    if (DEBUG_VOL){
        Logger::D(TAG, "adjustLocalOrRemoteStreamVolume(dir=%d)", direction);
    }

    if (CheckUpdateRemoteStateIfActive(IAudioSystem::STREAM_MUSIC)) {
        AdjustRemoteVolume(IAudioSystem::STREAM_MUSIC, direction, 0);
    }
    else {
        AutoPtr<IAudioSystemHelper> audioSystemHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
        Boolean state;
        audioSystemHelper->IsStreamActive(IAudioSystem::STREAM_MUSIC, 0, &state);
        if (state) {
            AdjustStreamVolume(IAudioSystem::STREAM_MUSIC, direction, 0);
        }
    }
    return NOERROR;
}

ECode CAudioService::AdjustSuggestedStreamVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 suggestedStreamType,
    /* [in] */ Int32 flags)
{
    if (DEBUG_VOL){
        Logger::D(TAG, "adjustSuggestedStreamVolume() direction=%d, suggestedStreamType=%d flags=%d",
            direction, suggestedStreamType, flags);
    }

    Int32 streamType;
    if (mVolumeControlStream != -1) {
        streamType = mVolumeControlStream;
    }
    else {
        streamType = GetActiveStreamType(suggestedStreamType);
    }

    // Play sounds on STREAM_RING only and if lock screen is not on.

    Boolean state = FALSE;
    if (mKeyguardManager != NULL) {
        mKeyguardManager->IsKeyguardLocked(&state);
    }

    if ((streamType != STREAM_REMOTE_MUSIC)
        && (flags & IAudioManager::FLAG_PLAY_SOUND) != 0
        && (((*mStreamVolumeAlias)[streamType] != IAudioSystem::STREAM_RING) || state)) {
        flags &= ~IAudioManager::FLAG_PLAY_SOUND;
    }

    if (streamType == STREAM_REMOTE_MUSIC) {
        // don't play sounds for remote
        flags &= ~(IAudioManager::FLAG_PLAY_SOUND | IAudioManager::FLAG_FIXED_VOLUME);
        if (DEBUG_VOL) Logger::I(TAG, "Need to adjust remote volume: calling adjustRemoteVolume()");
        AdjustRemoteVolume(IAudioSystem::STREAM_MUSIC, direction, flags);
    }
    else {
        AdjustStreamVolume(streamType, direction, flags);
    }

    return NOERROR;
}

ECode CAudioService::AdjustStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    if (DEBUG_VOL){
        Logger::D(TAG, "adjustStreamVolume() stream=%d, dir=%d, flags=%d",
            streamType, direction, flags);
    }

    FAIL_RETURN(EnsureValidDirection(direction));
    FAIL_RETURN(EnsureValidStreamType(streamType));

    // use stream type alias here so that streams with same alias have the same behavior,
    // including with regard to silent mode control (e.g the use of STREAM_RING below and in
    // checkForRingerModeChange() in place of STREAM_RING or STREAM_NOTIFICATION)
    Int32 streamTypeAlias = (*mStreamVolumeAlias)[streamType];
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamTypeAlias];

    const Int32 device = GetDeviceForStream(streamTypeAlias);
    // get last audible index if stream is muted, current index otherwise
    Int32 tempValue;
    streamState->GetIndex(
        device, (streamState->MuteCount() != 0) /* lastAudible */, &tempValue);
    const Int32 aliasIndex = tempValue;

    Boolean adjustVolume = TRUE;

    // convert one UI step (+/-1) into a number of internal units on the stream alias
    Int32 step = RescaleIndex(10, streamType, streamTypeAlias);
    Int32 index, oldIndex;

    if ((direction == IAudioManager::ADJUST_RAISE)
        && !CheckSafeMediaVolume(streamTypeAlias, aliasIndex + step, device)) {
        (*mStreamStates)[streamType]->GetIndex(
            device, (streamState->MuteCount() != 0)  /* lastAudible */, &index);
        oldIndex = index;
    }
    else {
        flags &= ~IAudioManager::FLAG_FIXED_VOLUME;

        if ((streamTypeAlias == IAudioSystem::STREAM_MUSIC)
            && ((device & mFixedVolumeDevices) != 0)) {
            flags |= IAudioManager::FLAG_FIXED_VOLUME;
            (*mStreamStates)[streamType]->GetMaxIndex(&index);
            oldIndex = index;
        }
        else {
            // If either the client forces allowing ringer modes for this adjustment,
            // or the stream type is one that is affected by ringer modes
            GetMasterStreamType(&tempValue);
            if (((flags & IAudioManager::FLAG_ALLOW_RINGER_MODES) != 0) ||
                    (streamTypeAlias == tempValue)) {
                Int32 ringerMode;
                GetRingerMode(&ringerMode);
                // do not vibrate if already in vibrate mode
                if (ringerMode == IAudioManager::RINGER_MODE_VIBRATE) {
                    flags &= ~IAudioManager::FLAG_VIBRATE;
                }

                // Check if the ringer mode changes with this volume adjustment. If
                // it does, it will handle adjusting the volume, so we won't below
                adjustVolume = CheckForRingerModeChange(aliasIndex, direction, step);
                GetMasterStreamType(&tempValue);
                if ((streamTypeAlias == tempValue)
                    && (mRingerMode == IAudioManager::RINGER_MODE_SILENT)) {
                    streamState->SetLastAudibleIndex(0, device);
                }
            }

            // If stream is muted, adjust last audible index only
            (*mStreamStates)[streamType]->GetIndex(device,
                ((*mStreamStates)[streamType]->MuteCount() != 0) /* lastAudible */, &oldIndex);

            if (streamState->MuteCount() != 0) {
                if (adjustVolume) {
                    // Post a persist volume msg
                    // no need to persist volume on all streams sharing the same alias
                    streamState->AdjustLastAudibleIndex(direction * step, device);
                    SendMsg(MSG_PERSIST_VOLUME,
                        SENDMSG_QUEUE,
                        PERSIST_LAST_AUDIBLE,
                        device,
                        (IInterface*)(streamState->Probe(EIID_IInterface)),
                        PERSIST_DELAY);
                }
                (*mStreamStates)[streamType]->GetIndex(device, TRUE  /* lastAudible */, &index);
            }
            else {
                Boolean tempState;
                streamState->AdjustIndex(direction * step, device, &tempState);
                if (adjustVolume && tempState) {
                    // Post message to set system volume (it in turn will post a message
                    // to persist). Do not change volume if stream is muted.
                    SendMsg(MSG_SET_DEVICE_VOLUME,
                        SENDMSG_QUEUE,
                        device,
                        0,
                        (IInterface*)(streamState->Probe(EIID_IInterface)),
                        0);
                }
                (*mStreamStates)[streamType]->GetIndex(device, FALSE  /* lastAudible */, &index);
            }
        }
    }

    SendVolumeUpdate(streamType, oldIndex, index, flags);
    return NOERROR;
}

ECode CAudioService::AdjustMasterVolume(
    /* [in] */ Int32 steps,
    /* [in] */ Int32 flags)
{
    using Elastos::Core::Math;
    EnsureValidSteps(steps);
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Float tempValue;
    audioSystemHelper->GetMasterVolume(&tempValue);
    Int32 volume = Math::Round(tempValue * MAX_MASTER_VOLUME);
    Int32 delta = 0;
    Int32 numSteps = Math::Abs(steps);
    Int32 direction = steps > 0 ? IAudioManager::ADJUST_RAISE : IAudioManager::ADJUST_LOWER;
    for (Int32 i = 0; i < numSteps; ++i) {
        delta = FindVolumeDelta(direction, volume);
        volume += delta;
    }

    if (DEBUG_VOL){
        Logger::D(TAG, "adjustMasterVolume volume: %d steps: %d", volume, steps);
    }
    SetMasterVolume(volume, flags);

    return NOERROR;
}

ECode CAudioService::SetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    EnsureValidStreamType(streamType);
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[(*mStreamVolumeAlias)[streamType]];

    const Int32 device = GetDeviceForStream(streamType);
    Int32 oldIndex;

    flags &= ~IAudioManager::FLAG_FIXED_VOLUME;
    if (((*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC)
        && ((device & mFixedVolumeDevices) != 0)) {
        flags |= IAudioManager::FLAG_FIXED_VOLUME;
        (*mStreamStates)[streamType]->GetMaxIndex(&index);
        oldIndex = index;
    }
    else {
        // get last audible index if stream is muted, current index otherwise
        streamState->GetIndex(
            device, (streamState->MuteCount() != 0) /* lastAudible */, &oldIndex);

        index = RescaleIndex(index * 10, streamType, (*mStreamVolumeAlias)[streamType]);

        if (!CheckSafeMediaVolume((*mStreamVolumeAlias)[streamType], index, device)) {
            return NOERROR;
        }

        // setting volume on master stream type also controls silent mode
        Int32 tempValue;
        GetMasterStreamType(&tempValue);
        if (((flags & IAudioManager::FLAG_ALLOW_RINGER_MODES) != 0)
            || ((*mStreamVolumeAlias)[streamType] == tempValue)) {
            Int32 newRingerMode;
            if (index == 0) {
                newRingerMode = mHasVibrator ?
                    IAudioManager::RINGER_MODE_VIBRATE : IAudioManager::RINGER_MODE_SILENT;
                SetStreamVolumeInt(
                    (*mStreamVolumeAlias)[streamType],
                    index,
                    device,
                    FALSE,
                    TRUE);
            }
            else {
                newRingerMode = IAudioManager::RINGER_MODE_NORMAL;
            }
            SetRingerMode(newRingerMode);
        }

        SetStreamVolumeInt((*mStreamVolumeAlias)[streamType], index, device, FALSE, TRUE);
        // get last audible index if stream is muted, current index otherwise
        (*mStreamStates)[streamType]->GetIndex(
            device, ((*mStreamStates)[streamType]->MuteCount() != 0) /* lastAudible */, &index);
    }

    SendVolumeUpdate(streamType, oldIndex, index, flags);
    return NOERROR;
}

ECode CAudioService::ForceVolumeControlStream(
    /* [in] */ Int32 streamType,
    /* [in] */ IBinder* cb)
{
    AutoLock lock(mForceControlStreamLock);

    mVolumeControlStream = streamType;
    if (mVolumeControlStream == -1) {
        if (mForceControlStreamClient != NULL) {
            mForceControlStreamClient->ReleaseResources();
            mForceControlStreamClient = NULL;
        }
    }
    else {
        mForceControlStreamClient = new ForceControlStreamClient(cb, this);
    }
    return NOERROR;
}

Int32 CAudioService::FindVolumeDelta(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 volume)
{
    Int32 delta = 0;
    if (direction == IAudioManager::ADJUST_RAISE) {
        if (volume == MAX_MASTER_VOLUME) {
            return 0;
        }
        // This is the default value if we make it to the end
        delta = (*mMasterVolumeRamp)[1];
        // If we're raising the volume move down the ramp array until we
        // find the volume we're above and use that groups delta.
        for (Int32 i = mMasterVolumeRamp->GetLength() - 1; i > 1; i -= 2) {
            if (volume >= (*mMasterVolumeRamp)[i - 1]) {
                delta = (*mMasterVolumeRamp)[i];
                break;
            }
        }
    }
    else if (direction == IAudioManager::ADJUST_LOWER){
        if (volume == 0) {
            return 0;
        }
        Int32 length = mMasterVolumeRamp->GetLength();
        // This is the default value if we make it to the end
        delta = -(*mMasterVolumeRamp)[length - 1];
        // If we're lowering the volume move up the ramp array until we
        // find the volume we're below and use the group below it's delta
        for (Int32 i = 2; i < length; i += 2) {
            if (volume <= (*mMasterVolumeRamp)[i]) {
                delta = -(*mMasterVolumeRamp)[i - 1];
                break;
            }
        }
    }
    return delta;
}

void CAudioService::SendBroadcastToAll(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 tempValue;
    binderHelper->ClearCallingIdentity(&tempValue);
    const Int64 ident = tempValue;

//    try {
        AutoPtr<IUserHandleHelper> userHandleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
        AutoPtr<IUserHandle> userHandle;
        userHandleHelper->GetALL((IUserHandle**)&userHandle);
        mContext->SendBroadcastAsUser(intent, userHandle);
//    } finally {
        binderHelper->RestoreCallingIdentity(ident);
//    }
}

void CAudioService::SendStickyBroadcastToAll(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 tempValue;
    binderHelper->ClearCallingIdentity(&tempValue);
    const Int64 ident = tempValue;
//    try {
        AutoPtr<IUserHandleHelper> userHandleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
        AutoPtr<IUserHandle> userHandle;
        userHandleHelper->GetALL((IUserHandle**)&userHandle);
        mContext->SendStickyBroadcastAsUser(intent, userHandle);
//    } finally {
        binderHelper->RestoreCallingIdentity(ident);
//    }
}

void CAudioService::SendVolumeUpdate(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    if (DEBUG_VOL) {
        Logger::D(TAG, "SendVolumeUpdate: streamType=%d oldIndex=%d index=%d flags=%08x",
            streamType, oldIndex, index, flags);
    }

    if (!mVoiceCapable && (streamType == IAudioSystem::STREAM_RING)) {
        streamType = IAudioSystem::STREAM_NOTIFICATION;
    }

    if (mVolumePanel != NULL) {
        mVolumePanel->PostVolumeChanged(streamType, flags);
    }

    if ((flags & IAudioManager::FLAG_FIXED_VOLUME) == 0) {
        oldIndex = (oldIndex + 5) / 10;
        index = (index + 5) / 10;

        AutoPtr<IIntent> intent;
        CIntent::New(IAudioManager::VOLUME_CHANGED_ACTION, (IIntent**)&intent);
        intent->PutExtra(IAudioManager::EXTRA_VOLUME_STREAM_TYPE, streamType);
        intent->PutExtra(IAudioManager::EXTRA_VOLUME_STREAM_VALUE, index);
        intent->PutExtra(IAudioManager::EXTRA_PREV_VOLUME_STREAM_VALUE, oldIndex);
        SendBroadcastToAll(intent);
    }
}

void CAudioService::SendMasterVolumeUpdate(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 oldVolume,
    /* [in] */ Int32 newVolume)
{
    if (mVolumePanel != NULL) {
        mVolumePanel->PostMasterVolumeChanged(flags);
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::MASTER_VOLUME_CHANGED_ACTION, (IIntent**)&intent);
    intent->PutExtra(IAudioManager::EXTRA_PREV_MASTER_VOLUME_VALUE, oldVolume);
    intent->PutExtra(IAudioManager::EXTRA_MASTER_VOLUME_VALUE, newVolume);
    SendBroadcastToAll(intent);
}

void CAudioService::SendMasterMuteUpdate(
    /* [in] */ Boolean muted,
    /* [in] */ Int32 flags)
{
    if (mVolumePanel != NULL) {
        mVolumePanel->PostMasterMuteChanged(flags);
    }

    BroadcastMasterMuteStatus(muted);
}

void CAudioService::BroadcastMasterMuteStatus(
    /* [in] */ Boolean muted)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::MASTER_MUTE_CHANGED_ACTION, (IIntent**)&intent);

    intent->PutExtra(IAudioManager::EXTRA_MASTER_VOLUME_MUTED, muted);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
        | IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    SendStickyBroadcastToAll(intent);
}

void CAudioService::SetStreamVolumeInt(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 device,
    /* [in] */ Boolean force,
    /* [in] */ Boolean lastAudible)
{
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamType];

    // If stream is muted, set last audible index only
    if (streamState->MuteCount() != 0) {
        // Do not allow last audible index to be 0
        if (index != 0) {
            streamState->SetLastAudibleIndex(index, device);
            // Post a persist volume msg
            SendMsg(MSG_PERSIST_VOLUME,
                SENDMSG_QUEUE,
                PERSIST_LAST_AUDIBLE,
                device,
                (IInterface*)(streamState->Probe(EIID_IInterface)),
                PERSIST_DELAY);
        }
    }
    else {
        Boolean tempState;
        streamState->SetIndex(index, device, lastAudible, &tempState);
        if (tempState || force) {
            // Post message to set system volume (it in turn will post a message
            // to persist).
            SendMsg(MSG_SET_DEVICE_VOLUME,
                SENDMSG_QUEUE,
                device,
                0,
                (IInterface*)(streamState->Probe(EIID_IInterface)),
                0);
        }
    }
}

ECode CAudioService::SetStreamSolo(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state,
    /* [in] */ IBinder* cb)
{
    Boolean tempState;
    for (Int32 stream = 0; stream < mStreamStates->GetLength(); stream++) {
        IsStreamAffectedByMute(stream, &tempState);
        if (!tempState || stream == streamType) {
            continue;
        }
        // Bring back last audible volume
        (*mStreamStates)[stream]->Mute(cb, state);
    }

    return NOERROR;
}

ECode CAudioService::SetStreamMute(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state,
    /* [in] */ IBinder* cb)
{
    Boolean tempState;
    IsStreamAffectedByMute(streamType, &tempState);
    if (tempState) {
        (*mStreamStates)[streamType]->Mute(cb, state);
        /* add by Gary. start {{----------------------------------- */
        /* 2011-10-27 */
        /* support mute */
        if (mVolumePanel != NULL) {
            mVolumePanel->PostVolumeChanged(streamType, IAudioManager::FLAG_SHOW_UI);
        }
        /* add by Gary. end   -----------------------------------}} */
    }

    return NOERROR;
}

ECode CAudioService::IsStreamMute(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
     VALIDATE_NOT_NULL(result);
     *result = ((*mStreamStates)[streamType]->MuteCount() != 0);
     return NOERROR;
}

ECode CAudioService::SetMasterMute(
    /* [in] */ Boolean state,
    /* [in] */ Int32 flags,
    /* [in] */ IBinder* cb)
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Boolean tempState;
    audioSystemHelper->GetMasterMute(&tempState);
    if (state != tempState) {
        audioSystemHelper->SetMasterMute(state);
        // Post a persist master volume msg
        SendMsg(MSG_PERSIST_MASTER_VOLUME_MUTE, SENDMSG_REPLACE,
            state ? 1 : 0, 0, NULL, PERSIST_DELAY);
        SendMasterMuteUpdate(state, flags);
    }

    return NOERROR;
}

ECode CAudioService::IsMasterMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->GetMasterMute(result);
    return NOERROR;
}

ECode CAudioService::GetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    EnsureValidStreamType(streamType);
    Int32 device = GetDeviceForStream(streamType);
    Int32 index;

    if (((*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC)
        && (device & mFixedVolumeDevices) != 0) {
        (*mStreamStates)[streamType]->GetMaxIndex(&index);
    }
    else {
        (*mStreamStates)[streamType]->GetIndex(device, FALSE  /* lastAudible */, &index);
    }
    *result = (index + 5) / 10;
    return NOERROR;
}

ECode CAudioService::GetMasterVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Boolean tempState;
    IsMasterMute(&tempState);
    if (tempState){
        return NOERROR;
    }

    GetLastAudibleMasterVolume(result);
    return NOERROR;
}

ECode CAudioService::SetMasterVolume(
    /* [in] */ Int32 volume,
    /* [in] */ Int32 flags)
{
    if (volume < 0) {
        volume = 0;
    }
    else if (volume > MAX_MASTER_VOLUME) {
        volume = MAX_MASTER_VOLUME;
    }
    DoSetMasterVolume((Float)volume / MAX_MASTER_VOLUME, flags);
    return NOERROR;
}

void CAudioService::DoSetMasterVolume(
    /* [in] */ Float volume,
    /* [in] */ Int32 flags)
{
    using Elastos::Core::Math;

    // don't allow changing master volume when muted
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Boolean tempState;
    audioSystemHelper->GetMasterMute(&tempState);
    if (!tempState) {
        Int32 oldVolume;
        GetMasterVolume(&oldVolume);

        audioSystemHelper->SetMasterVolume(volume);

        Int32 newVolume;
        GetMasterVolume(&newVolume);

        if (newVolume != oldVolume) {
            // Post a persist master volume msg
            SendMsg(MSG_PERSIST_MASTER_VOLUME, SENDMSG_REPLACE,
                Math::Round(volume * (Float)1000.0), 0, NULL, PERSIST_DELAY);
        }
        // Send the volume update regardless whether there was a change.
        SendMasterVolumeUpdate(flags, oldVolume, newVolume);
    }
}

ECode CAudioService::GetStreamMaxVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    EnsureValidStreamType(streamType);
    Int32 tempValue;
    (*mStreamStates)[streamType]->GetMaxIndex(&tempValue);
    *result = (tempValue + 5) / 10;

    return NOERROR;
}

ECode CAudioService::GetMasterMaxVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = MAX_MASTER_VOLUME;
    return NOERROR;
}

ECode CAudioService::GetLastAudibleStreamVolume(
     /* [in] */ Int32 streamType,
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    EnsureValidStreamType(streamType);
    Int32 device = GetDeviceForStream(streamType);
    Int32 tempValue;
    (*mStreamStates)[streamType]->GetIndex(device, TRUE  /* lastAudible */, &tempValue);
    *result = (tempValue + 5) / 10;
    return NOERROR;
}

ECode CAudioService::GetLastAudibleMasterVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Float tempValue;
    audioSystemHelper->GetMasterVolume(&tempValue);
    *result = Elastos::Core::Math::Round(tempValue * MAX_MASTER_VOLUME);
    return NOERROR;
}

ECode CAudioService::GetMasterStreamType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mVoiceCapable) {
        *result = IAudioSystem::STREAM_RING;
    }
    else {
        *result = IAudioSystem::STREAM_MUSIC;
    }
    return NOERROR;
}

ECode CAudioService::GetRingerMode(
    /* [out] */ Int32* result)
{
    AutoLock lock(mSettingsLock);
    *result = mRingerMode;
    return NOERROR;
}

ECode CAudioService::EnsureValidRingerMode(
    /* [in] */ Int32 ringerMode)
{
    AutoPtr<IAudioManagerHelper> audioManagerHelper;
    Boolean tempState;
    audioManagerHelper->IsValidRingerMode(ringerMode, &tempState);
    if (!tempState) {
        Logger::D(TAG, "Bad ringer mode %d", ringerMode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("Bad ringer mode " + ringerMode);
    }
    return NOERROR;
}

ECode CAudioService::SetRingerMode(
    /* [in] */ Int32 ringerMode)
{
    if ((ringerMode == IAudioManager::RINGER_MODE_VIBRATE) && !mHasVibrator) {
        ringerMode = IAudioManager::RINGER_MODE_SILENT;
    }

    Int32 tempValue;
    GetRingerMode(&tempValue);
    if (ringerMode != tempValue) {
        SetRingerModeInt(ringerMode, TRUE);
        // Send sticky broadcast
        BroadcastRingerMode(ringerMode);
    }

    return NOERROR;
}

void CAudioService::SetRingerModeInt(
    /* [in] */ Int32 ringerMode,
    /* [in] */ Boolean persist)
{
    {
        AutoLock lock(mSettingsLock);
        mRingerMode = ringerMode;
    }

    // Mute stream if not previously muted by ringer mode and ringer mode
    // is not RINGER_MODE_NORMAL and stream is affected by ringer mode.
    // Unmute stream if previously muted by ringer mode and ringer mode
    // is RINGER_MODE_NORMAL or stream is not affected by ringer mode.
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 numStreamTypes;
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

    Boolean tempState;
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (IsStreamMutedByRingerMode(streamType)) {
            IsStreamAffectedByRingerMode(streamType, &tempState);
            if (!tempState ||
                ringerMode == IAudioManager::RINGER_MODE_NORMAL) {
                // ring and notifications volume should never be 0 when not silenced
                // on voice capable devices
                if (mVoiceCapable &&
                    (*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_RING)
                {
                    AutoLock lock(&mStreamStatesLock);
                    VolumeStreamState* vsState = (*mStreamStates)[streamType];

                    AutoLock lastIndexLock(&vsState->mLastAudibleIndexLock);
                    HashMap<Int32, Int32>::Iterator it = vsState->mLastAudibleIndex.Begin();
                    while (it != vsState->mLastAudibleIndex.End()) {
                        if (it->mSecond == 0) it->mSecond = 10;
                        ++it;
                    }
                }

                (*mStreamStates)[streamType]->Mute(NULL, FALSE);
                mRingerModeMutedStreams &= ~(1 << streamType);
            }
        }
        else {
            IsStreamAffectedByRingerMode(streamType, &tempState);
            if (tempState &&
                ringerMode != IAudioManager::RINGER_MODE_NORMAL) {
               (*mStreamStates)[streamType]->Mute(NULL, TRUE);
               mRingerModeMutedStreams |= (1 << streamType);
            }
        }
    }

    // Post a persist ringer mode msg
    if (persist) {
        SendMsg(MSG_PERSIST_RINGER_MODE,
            SENDMSG_REPLACE, 0, 0, NULL, PERSIST_DELAY);
    }
}

void CAudioService::RestoreMasterVolume()
{
    if (mUseMasterVolume) {
        Float volume;
        Settings::System::GetFloatForUser(mContentResolver,
                ISettingsSystem::VOLUME_MASTER, -1.0f, IUserHandle::USER_CURRENT, &volume);
        if (volume >= 0.0f) {
            AutoPtr<IAudioSystemHelper> audioSystemHelper;
            CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
            audioSystemHelper->SetMasterVolume(volume);
        }
    }
}

ECode CAudioService::ShouldVibrate(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (!mHasVibrator){
        return NOERROR;
    }

    Int32 setting, tempValue;
    GetVibrateSetting(vibrateType, &setting);
    switch (setting) {
        case IAudioManager::VIBRATE_SETTING_ON: {
            GetRingerMode(&tempValue);
            *result = tempValue != IAudioManager::RINGER_MODE_SILENT;
            return NOERROR;
        }

        case IAudioManager::VIBRATE_SETTING_ONLY_SILENT: {
            GetRingerMode(&tempValue);
            *result = tempValue == IAudioManager::RINGER_MODE_VIBRATE;
            return NOERROR;
        }

        case IAudioManager::VIBRATE_SETTING_OFF:  {
            // return FALSE, even for incoming calls
            *result = FALSE;
            return NOERROR;
        }

        default: {
            *result = FALSE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAudioService::GetVibrateSetting(
     /* [in] */ Int32 vibrateType,
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (!mHasVibrator){
        *result = IAudioManager::VIBRATE_SETTING_OFF;
        return NOERROR;
    }

    *result = (mVibrateSetting >> (vibrateType * 2)) & 3;
    return NOERROR;
}

ECode CAudioService::SetVibrateSetting(
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting)
{
    if (!mHasVibrator) {
        return NOERROR;
    }

    GetValueForVibrateSetting(mVibrateSetting, vibrateType, vibrateSetting, &mVibrateSetting);

    // Broadcast change
    BroadcastVibrateSetting(vibrateType);

    return NOERROR;
}

/*static*/
ECode CAudioService::GetValueForVibrateSetting(
    /* [in] */ Int32 existingValue,
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // First clear the existing setting. Each vibrate type has two bits in
    // the value. Note '3' is '11' in binary.
    existingValue &= ~(3 << (vibrateType * 2));

    // Set into the old value
    existingValue |= (vibrateSetting & 3) << (vibrateType * 2);
    *result = existingValue;

    return NOERROR;
}

ECode CAudioService::SetMode(
    /* [in] */ Int32 mode,
    /* [in] */ IBinder* cb)
{
    if (!CheckAudioSettingsPermission(String("setMode()"))) {
        return NOERROR;
    }

    if (mode < IAudioSystem::MODE_CURRENT || mode >= IAudioSystem::NUM_MODES) {
        return NOERROR;
    }

    Int32 newModeOwnerPid = 0;

    {
        AutoLock lock(mSetModeDeathHandlersLock);

        if (mode == IAudioSystem::MODE_CURRENT) {
            mode = mMode;
        }
        AutoPtr<IBinderHelper> binderHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
        Int32 tempValue;
        binderHelper->GetCallingPid(&tempValue);
        newModeOwnerPid = SetModeInt(mode, cb, tempValue);
    }

    // when entering RINGTONE, IN_CALL or IN_COMMUNICATION mode, clear all
    // SCO connections not started by the application changing the mode
    if (newModeOwnerPid != 0) {
         DisconnectBluetoothSco(newModeOwnerPid);
    }

    return NOERROR;
}

Int32 CAudioService::SetModeInt(
    /* [in] */ Int32 mode,
    /* [in] */ IBinder* callback,
    /* [in] */ Int32 pid)
{
    Int32 newModeOwnerPid = 0;
    if (callback == NULL) {
        Logger::E(TAG, "setModeInt() called with null binder");
        return newModeOwnerPid;
    }

    AutoPtr<SetModeDeathHandler> hdlr;
    List< AutoPtr<SetModeDeathHandler> >::Iterator iter = mSetModeDeathHandlers.Begin();
    while (iter != mSetModeDeathHandlers.End()) {
        AutoPtr<SetModeDeathHandler> h = *iter;
        if (h && h->GetPid() == pid) {
            hdlr = h;
            // Remove from client list so that it is re-inserted at top of list
            mSetModeDeathHandlers.Erase(iter);
            AutoPtr<IBinder> binder = hdlr->GetBinder();
            AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
            if (proxy) {
                Boolean result;
                proxy->UnlinkToDeath(hdlr, 0, &result);
            }
            break;
        }

        ++iter;
    }

    AutoPtr<IBinder> cb = callback;
    Int32 status = IAudioSystem::AUDIO_STATUS_OK;
    do {
        if (mode == IAudioSystem::MODE_NORMAL) {
            // get new mode from client at top the list if any
            if (!mSetModeDeathHandlers.IsEmpty()) {
                hdlr = mSetModeDeathHandlers[0];
                cb = hdlr->GetBinder();
                mode = hdlr->GetMode();
            }
        }
        else {
            if (hdlr == NULL) {
                hdlr = new SetModeDeathHandler(cb, pid, this);
            }
            // Register for client death notification
            // try {
            AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
            if (proxy != NULL) {
                ECode ec = proxy->LinkToDeath(hdlr, 0);
               if (FAILED(ec)) {
                   // Client has died!
                   Logger::W(TAG, "setMode() could not link to %p binder death", cb.Get());
               }
            }
            // } catch (RemoteException e) {
            //    // Client has died!
            //    Logger::W(TAG, "setMode() could not link to "/*+cb+*/" binder death");
            // }

            // Last client to call setMode() is always at top of client list
            // as required by SetModeDeathHandler.binderDied()
            mSetModeDeathHandlers.PushFront(hdlr);
            hdlr->SetMode(mode);
        }

        if (mode != mMode) {
            CAudioSystem::SetPhoneState(mode);
            if (status == IAudioSystem::AUDIO_STATUS_OK) {
                mMode = mode;
            }
            else {
                if (hdlr != NULL) {
                    mSetModeDeathHandlers.Remove(hdlr);
                    AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
                    if (proxy != NULL) {
                        Boolean result;
                        proxy->UnlinkToDeath(hdlr, 0, &result);
                    }
                }
                // force reading new top of mSetModeDeathHandlers stack
                mode = IAudioSystem::MODE_NORMAL;
            }
        }
        else {
            status = IAudioSystem::AUDIO_STATUS_OK;
        }
    } while (status != IAudioSystem::AUDIO_STATUS_OK && !mSetModeDeathHandlers.IsEmpty());

    if (status == IAudioSystem::AUDIO_STATUS_OK) {
        if (mode != IAudioSystem::MODE_NORMAL) {
            if (mSetModeDeathHandlers.IsEmpty()) {
                Logger::E(TAG, "setMode() different from MODE_NORMAL with empty mode client stack");
            }
            else {
                newModeOwnerPid = mSetModeDeathHandlers[0]->GetPid();
            }
        }

        Int32 streamType = GetActiveStreamType(IAudioManager::USE_DEFAULT_STREAM_TYPE);
        if (streamType == STREAM_REMOTE_MUSIC) {
            // here handle remote media playback the same way as local playback
            streamType = IAudioManager::STREAM_MUSIC;
        }
        Int32 device = GetDeviceForStream(streamType);
        Int32 index;
        (*mStreamStates)[(*mStreamVolumeAlias)[streamType]]->GetIndex(device, FALSE, &index);
        SetStreamVolumeInt((*mStreamVolumeAlias)[streamType], index, device, TRUE, FALSE);

        UpdateStreamVolumeAlias(TRUE /*updateVolumes*/);
    }
    return newModeOwnerPid;
}

ECode CAudioService::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMode;
    return NOERROR;
}

ECode CAudioService::PlaySoundEffect(
    /* [in] */ Int32 effectType)
{
    SendMsg(MSG_PLAY_SOUND_EFFECT, SENDMSG_NOOP,
        effectType, -1, NULL, 0);
    return NOERROR;
}

ECode CAudioService::PlaySoundEffectVolume(
    /* [in] */ Int32 effectType,
    /* [in] */ Float volume)
{
    Boolean tempState;
    LoadSoundEffects(&tempState);
    SendMsg(MSG_PLAY_SOUND_EFFECT, SENDMSG_NOOP,
        effectType, (Int32) (volume * 1000), NULL, 0);
    return NOERROR;
}

ECode CAudioService::LoadSoundEffects(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 status;

    {
        AutoLock lock(mSoundEffectsLock);

        if (!mBootCompleted) {
            Logger::W(TAG, "loadSoundEffects() called before boot complete");
            *result = FALSE;
            return NOERROR;
        }

        if (mSoundPool != NULL) {
            Logger::D(TAG, "LoadSoundEffects mSoundPool is not null.");
            *result = TRUE;
            return NOERROR;
        }
        CSoundPool::New(NUM_SOUNDPOOL_CHANNELS, IAudioSystem::STREAM_SYSTEM, 0, (ISoundPool**)&mSoundPool);

//        try {
           mSoundPoolListenerThread = new SoundPoolListenerThread(this);
           mSoundPoolListenerThread->Start();
           // Wait for mSoundPoolCallBack to be set by the other thread
           mSoundEffectsLock.Wait();
//        } catch (InterruptedException e) {
//            Logger::W(TAG, "Interrupted while waiting sound pool listener thread.");
//        }

        if (mSoundPoolCallBack == NULL) {
            Logger::W(TAG, "loadSoundEffects() could not create SoundPool listener or thread");
            if (mSoundPoolLooper != NULL) {
                mSoundPoolLooper->Quit();
                mSoundPoolLooper = NULL;
            }
            mSoundPoolListenerThread = NULL;
            mSoundPool->ReleaseResources();
            mSoundPool = NULL;
            *result = FALSE;
            return NOERROR;
        }

        /*
         * poolId table: The value -1 in this table indicates that corresponding
         * file (same index in SOUND_EFFECT_FILES[] has not been loaded.
         * Once loaded, the value in poolId is the sample ID and the same
         * sample can be reused for another effect using the same file.
         */
        Int32 SOUND_EFFECT_FILES_LENGTH = sizeof(SOUND_EFFECT_FILES)/sizeof(String);

        AutoPtr< ArrayOf<Int32> > poolId = ArrayOf<Int32>::Alloc(SOUND_EFFECT_FILES_LENGTH);
        for (Int32 fileIdx = 0; fileIdx < SOUND_EFFECT_FILES_LENGTH; fileIdx++) {
            (*poolId)[fileIdx] = -1;
        }

        /*
         * Effects whose value in SOUND_EFFECT_FILES_MAP[effect][1] is -1 must be loaded.
         * If load succeeds, value in SOUND_EFFECT_FILES_MAP[effect][1] is > 0:
         * this indicates we have a valid sample loaded for this effect.
         */
        Int32 lastSample = 0;
        for (Int32 effect = 0; effect < IAudioManager::NUM_SOUND_EFFECTS; effect++) {
            // Do not load sample if this effect uses the MediaPlayer
            if (SOUND_EFFECT_FILES_MAP[effect][1] == 0) {
                continue;
            }
            if ((*poolId)[SOUND_EFFECT_FILES_MAP[effect][0]] == -1) {
                AutoPtr<IFile> file = Environment::GetRootDirectory();
                String tempText;
                file->ToString(&tempText);
                String filePath = tempText + SOUND_EFFECTS_PATH + SOUND_EFFECT_FILES[SOUND_EFFECT_FILES_MAP[effect][0]];
                if (DEBUG_VOL) {
                    Logger::D(TAG, " > index %d, loading sound effect: (%d, %d), %s", effect,
                        SOUND_EFFECT_FILES_MAP[effect][1],
                        (*poolId)[SOUND_EFFECT_FILES_MAP[effect][0]],
                        filePath.string());
                }
                Int32 sampleId ;
                mSoundPool->Load(filePath, 0, &sampleId);
                if (sampleId <= 0) {
                    Logger::W(TAG, String("Soundpool could not load file: ") + filePath);
                }
                else {
                    SOUND_EFFECT_FILES_MAP[effect][1] = sampleId;
                    (*poolId)[SOUND_EFFECT_FILES_MAP[effect][0]] = sampleId;
                    lastSample = sampleId;
                }
            } else {
                SOUND_EFFECT_FILES_MAP[effect][1] = (*poolId)[SOUND_EFFECT_FILES_MAP[effect][0]];
            }
        }

        // wait for all samples to be loaded
        if (lastSample != 0) {
            mSoundPoolCallBack->SetLastSample(lastSample);

//            try {
                mSoundEffectsLock.Wait();
                status = mSoundPoolCallBack->Status();
//            } catch (java.lang.InterruptedException e) {
//                Log::W(TAG, "Interrupted while waiting sound pool callback.");
//                status = -1;
//            }
        } else {
            status = -1;
        }

        if (mSoundPoolLooper != NULL) {
            mSoundPoolLooper->Quit();
            mSoundPoolLooper = NULL;
        }
        mSoundPoolListenerThread = NULL;
        if (status != 0) {
            Logger::W(TAG, String("loadSoundEffects(), Error %d while loading samples"),
                (lastSample != 0) ? mSoundPoolCallBack->Status() : -1);
            for (Int32 effect = 0; effect < IAudioManager::NUM_SOUND_EFFECTS; effect++) {
                if (SOUND_EFFECT_FILES_MAP[effect][1] > 0) {
                    SOUND_EFFECT_FILES_MAP[effect][1] = -1;
                }
            }

            mSoundPool->ReleaseResources();
            mSoundPool = NULL;
        }
    }

    *result = (status == 0);
    return NOERROR;
}

ECode CAudioService::UnloadSoundEffects()
{
    AutoLock lock(mSoundEffectsLock);

    if (mSoundPool == NULL) {
        return NOERROR;
    }

    mAudioHandler->RemoveMessages(MSG_LOAD_SOUND_EFFECTS);
    mAudioHandler->RemoveMessages(MSG_PLAY_SOUND_EFFECT);

    Int32 SOUND_EFFECT_FILES_LENGTH = sizeof(SOUND_EFFECT_FILES)/sizeof(String);
    AutoPtr< ArrayOf<Int32> > poolId = ArrayOf<Int32>::Alloc(SOUND_EFFECT_FILES_LENGTH);

    for (Int32 fileIdx = 0; fileIdx < SOUND_EFFECT_FILES_LENGTH; fileIdx++) {
        (*poolId)[fileIdx] = 0;
    }

    Boolean tempState;
    for (Int32 effect = 0; effect < IAudioManager::NUM_SOUND_EFFECTS; effect++) {
        if (SOUND_EFFECT_FILES_MAP[effect][1] <= 0) {
            continue;
        }
        if ((*poolId)[SOUND_EFFECT_FILES_MAP[effect][0]] == 0) {
            mSoundPool->Unload(SOUND_EFFECT_FILES_MAP[effect][1], &tempState);
            SOUND_EFFECT_FILES_MAP[effect][1] = -1;
            (*poolId)[SOUND_EFFECT_FILES_MAP[effect][0]] = -1;
        }
    }
    mSoundPool->ReleaseResources();
    mSoundPool = NULL;

    return NOERROR;
}

ECode CAudioService::ReloadAudioSettings()
{
    ReadAudioSettings(FALSE /*userSwitch*/);

    return NOERROR;
}

void CAudioService::ReadAudioSettings(
    /* [in] */ Boolean userSwitch)
{
    // restore ringer mode, ringer mode affected streams, mute affected streams and vibrate settings
    ReadPersistedSettings();

    // restore volume settings
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 numStreamTypes;
    audioSystemHelper->GetNumStreamTypes(&numStreamTypes);

    for (Int32 streamType = 0; streamType < numStreamTypes; streamType++) {
        AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamType];

        if (userSwitch && (*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC) {
            continue;
        }

        {
            AutoLock lock(mStreamStatesLock);

            streamState->ReadSettings();

            // unmute stream that was muted but is not affect by mute anymore
            Boolean tempState;
            IsStreamAffectedByMute(streamType, &tempState);
            if ((streamState->MuteCount() != 0 && !tempState) &&
                    !IsStreamMutedByRingerMode(streamType)) {
                List< AutoPtr<VolumeStreamState::VolumeDeathHandler> >::Iterator it;
                it = streamState->mDeathHandlers.Begin();
                for (; it != streamState->mDeathHandlers.End(); ++it) {
                    (*it)->mMuteCount = 1;
                    (*it)->Mute(FALSE);

                }
            }
        }
    }

    // apply new ringer mode before checking volume for alias streams so that streams
    // muted by ringer mode have the correct volume
    Int32 tempValue;
    GetRingerMode(&tempValue);
    SetRingerModeInt(tempValue, FALSE);

    CheckAllAliasStreamVolumes();

    {
        AutoLock lock(mSafeMediaVolumeStateLock);

        if (mSafeMediaVolumeState == SAFE_MEDIA_VOLUME_ACTIVE) {
            EnforceSafeMediaVolume();
        }
    }
}

ECode CAudioService::SetSpeakerphoneOn(
    /* [in] */ Boolean on)
{
    if (!CheckAudioSettingsPermission(String("setSpeakerphoneOn()"))) {
         return NOERROR;
    }
    mForcedUseForComm = on ? IAudioSystem::FORCE_SPEAKER : IAudioSystem::FORCE_NONE;

    SendMsg(MSG_SET_FORCE_USE, SENDMSG_QUEUE,
        IAudioSystem::FOR_COMMUNICATION, mForcedUseForComm, NULL, 0);

    return NOERROR;
}

ECode CAudioService::IsSpeakerphoneOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mForcedUseForComm == IAudioSystem::FORCE_SPEAKER);
    return NOERROR;
}

ECode CAudioService::SetBluetoothScoOn(
    /* [in] */ Boolean on)
{
    if (!CheckAudioSettingsPermission(String("setBluetoothScoOn()"))) {
        return NOERROR;
    }
    mForcedUseForComm = on ? IAudioSystem::FORCE_BT_SCO : IAudioSystem::FORCE_NONE;

    SendMsg(MSG_SET_FORCE_USE, SENDMSG_QUEUE,
        IAudioSystem::FOR_COMMUNICATION, mForcedUseForComm, NULL, 0);

    SendMsg(MSG_SET_FORCE_USE, SENDMSG_QUEUE,
        IAudioSystem::FOR_RECORD, mForcedUseForComm, NULL, 0);

    return NOERROR;
}

ECode CAudioService::IsBluetoothScoOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mForcedUseForComm == IAudioSystem::FORCE_BT_SCO);
    return NOERROR;
}

ECode CAudioService::SetBluetoothA2dpOn(
    /* [in] */ Boolean on)
{
    AutoLock lock(mBluetoothA2dpEnabledLock);

    mBluetoothA2dpEnabled = on;

    SendMsg(MSG_SET_FORCE_BT_A2DP_USE, SENDMSG_QUEUE,
        IAudioSystem::FOR_MEDIA,
        mBluetoothA2dpEnabled ? IAudioSystem::FORCE_NONE : IAudioSystem::FORCE_NO_BT_A2DP,
        NULL, 0);
    return NOERROR;
}

ECode CAudioService::IsBluetoothA2dpOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mBluetoothA2dpEnabledLock);
    *result = mBluetoothA2dpEnabled;
    return NOERROR;
}

ECode CAudioService::StartBluetoothSco(
    /* [in] */ IBinder* cb)
{
    if (!CheckAudioSettingsPermission(String("startBluetoothSco()"))
        || !mBootCompleted) {
        return NOERROR;
    }

    AutoPtr<ScoClient> client = GetScoClient(cb, TRUE);
    client->IncCount();

    return NOERROR;
}

ECode CAudioService::StopBluetoothSco(
    /* [in] */ IBinder* cb)
{
    if (!CheckAudioSettingsPermission(String("stopBluetoothSco()"))
        || !mBootCompleted) {
        return NOERROR;
    }

    AutoPtr<ScoClient> client = GetScoClient(cb, FALSE);
    if (client != NULL) {
        client->DecCount();
    }

    return NOERROR;
}

void CAudioService::CheckScoAudioState()
{
    Int32 tempValue;
    mBluetoothHeadset->GetAudioState(mBluetoothHeadsetDevice, &tempValue);
    if (mBluetoothHeadset != NULL && mBluetoothHeadsetDevice != NULL
        && mScoAudioState == SCO_STATE_INACTIVE
        && tempValue != IBluetoothHeadset::STATE_AUDIO_DISCONNECTED) {
        mScoAudioState = SCO_STATE_ACTIVE_EXTERNAL;
    }
}

AutoPtr<CAudioService::ScoClient> CAudioService::GetScoClient(
    /* [in] */ IBinder* cb,
    /* [in] */ Boolean create)
{
    AutoLock lock(mScoClientsLock);

    AutoPtr<ScoClient> client;
    Int32 size = mScoClients.GetSize();
    for (Int32 i = 0; i < size; i++) {
        client = mScoClients[i];
        if (cb == client->GetBinder()) {
            return client;
        }
    }
    if (create) {
        client = new ScoClient(cb, this);
        mScoClients.PushBack(client);
    }
    return client;
}

ECode CAudioService::ClearAllScoClients(
    /* [in] */ Int32 exceptPid,
    /* [in] */ Boolean stopSco)
{
    AutoLock lock(mScoClientsLock);
    AutoPtr<ScoClient> savedClient;
    Int32 size = mScoClients.GetSize();
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<ScoClient> cl = mScoClients[i];
        if (cl->GetPid() != exceptPid) {
            cl->ClearCount(stopSco);
        } else {
            savedClient = cl;
        }
    }
    mScoClients.Clear();
    if (savedClient != NULL) {
        mScoClients.PushBack(savedClient);
    }

    return NOERROR;
}

Boolean CAudioService::GetBluetoothHeadset()
{
    Boolean result = FALSE;
    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    AutoPtr<IBluetoothAdapter> adapter;
    helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);

    if (adapter != NULL) {
        adapter->GetProfileProxy(mContext, mBluetoothProfileServiceListener,
            IBluetoothProfile::HEADSET, &result);
    }
    // If we could not get a bluetooth headset proxy, send a failure message
    // without delay to reset the SCO audio state and clear SCO clients.
    // If we could get a proxy, send a delayed failure message that will reset our state
    // in case we don't receive onServiceConnected().
    SendMsg(MSG_BT_HEADSET_CNCT_FAILED,
        SENDMSG_REPLACE, 0, 0, NULL, result ? BT_HEADSET_CNCT_TIMEOUT_MS : 0);

    return result;
}

void CAudioService::DisconnectBluetoothSco(
    /* [in] */ Int32 exceptPid)
{
    AutoLock lock(mScoClientsLock);

    CheckScoAudioState();
    if (mScoAudioState == SCO_STATE_ACTIVE_EXTERNAL ||
            mScoAudioState == SCO_STATE_DEACTIVATE_EXT_REQ) {
        if (mBluetoothHeadsetDevice != NULL) {
            if (mBluetoothHeadset != NULL) {
                Boolean tempState;
                mBluetoothHeadset->StopVoiceRecognition(mBluetoothHeadsetDevice, &tempState);
                if (!tempState) {
                    SendMsg(MSG_BT_HEADSET_CNCT_FAILED,
                        SENDMSG_REPLACE, 0, 0, NULL, 0);
                }
            } else if (mScoAudioState == SCO_STATE_ACTIVE_EXTERNAL
                && GetBluetoothHeadset()) {
                mScoAudioState = SCO_STATE_DEACTIVATE_EXT_REQ;
            }
        }
    }
    else {
        ClearAllScoClients(exceptPid, TRUE);
    }
}

void CAudioService::ResetBluetoothSco()
{
    AutoLock lock(mScoClientsLock);

    ClearAllScoClients(0, FALSE);
    mScoAudioState = SCO_STATE_INACTIVE;
    BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
}

void CAudioService::BroadcastScoConnectionState(
    /* [in] */ Int32 state)
{
    if (state != mScoConnectionState) {
        AutoPtr<IIntent> newIntent;
        CIntent::New(IAudioManager::ACTION_SCO_AUDIO_STATE_UPDATED, (IIntent**)&newIntent);
        newIntent->PutExtra(IAudioManager::EXTRA_SCO_AUDIO_STATE, state);
        newIntent->PutExtra(IAudioManager::EXTRA_SCO_AUDIO_PREVIOUS_STATE,
            mScoConnectionState);
        SendStickyBroadcastToAll(newIntent);
        mScoConnectionState = state;
    }
}

ECode CAudioService::SetRemoteSubmixOn(
    /* [in] */ Boolean on,
    /* [in] */ Int32 address)
{
    SendMsg(MSG_SET_RSX_CONNECTION_STATE,
        SENDMSG_REPLACE /* replace with QUEUE when multiple addresses are supported */,
        on ? 1 : 0 /*arg1*/,
        address /*arg2*/,
        NULL/*obj*/, 0/*delay*/);

    return NOERROR;
}

void CAudioService::OnSetRsxConnectionState(
    /* [in] */ Int32 available,
    /* [in] */ Int32 address)
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetDeviceConnectionState(IAudioSystem::DEVICE_IN_REMOTE_SUBMIX,
        available == 1 ?
            IAudioSystem::DEVICE_STATE_AVAILABLE : IAudioSystem::DEVICE_STATE_UNAVAILABLE,
        StringUtils::Int32ToString(address) /*device_address*/);
    audioSystemHelper->SetDeviceConnectionState(IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX,
        available == 1 ?
            IAudioSystem::DEVICE_STATE_AVAILABLE : IAudioSystem::DEVICE_STATE_UNAVAILABLE,
        StringUtils::Int32ToString(address) /*device_address*/);
}

void CAudioService::OnCheckMusicActive()
{
    AutoLock lock(mSafeMediaVolumeStateLock);

    if (mSafeMediaVolumeState == SAFE_MEDIA_VOLUME_INACTIVE) {
        Int32 device = GetDeviceForStream(IAudioSystem::STREAM_MUSIC);

        if ((device & mSafeMediaVolumeDevices) != 0) {
            SendMsg(MSG_CHECK_MUSIC_ACTIVE,
                SENDMSG_REPLACE,
                0,
                0,
                NULL,
                MUSIC_ACTIVE_POLL_PERIOD_MS);
            Int32 index;
            (*mStreamStates)[IAudioSystem::STREAM_MUSIC]->GetIndex(
                device, FALSE /*lastAudible*/, &index);
            AutoPtr<IAudioSystemHelper> audioSystemHelper;
            CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
            Boolean tempState;
            audioSystemHelper->IsStreamActive(IAudioSystem::STREAM_MUSIC, 0, &tempState);
            if (tempState && (index > mSafeMediaVolumeIndex)) {
                // Approximate cumulative active music time
                mMusicActiveMs += MUSIC_ACTIVE_POLL_PERIOD_MS;
                if (mMusicActiveMs > UNSAFE_VOLUME_MUSIC_ACTIVE_MS_MAX) {
                    SetSafeMediaVolumeEnabled(TRUE);
                    mMusicActiveMs = 0;
                    if (mVolumePanel != NULL) {
                        mVolumePanel->PostDisplaySafeVolumeWarning();
                    }
                }
            }
        }
    }
}

void CAudioService::OnConfigureSafeVolume(
    /* [in] */ Boolean force)
{
    AutoLock lock(mSafeMediaVolumeStateLock);

    AutoPtr<IResources> resources;
    Int32 mcc;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    configuration->GetMcc(&mcc);

    if ((mMcc != mcc) || ((mMcc == 0) && force)) {
        Int32 tempValue;
        resources->GetInteger(
            R::integer::config_safe_media_volume_index, &tempValue);
        mSafeMediaVolumeIndex = tempValue * 10;

        Boolean safeMediaVolumeEnabled;
        resources->GetBoolean(
            R::bool_::config_safe_media_volume_enabled, &safeMediaVolumeEnabled);

        if (safeMediaVolumeEnabled) {
            mSafeMediaVolumeState = SAFE_MEDIA_VOLUME_ACTIVE;
            EnforceSafeMediaVolume();
        }
        else {
            mSafeMediaVolumeState = SAFE_MEDIA_VOLUME_DISABLED;
        }
        mMcc = mcc;
    }
}

///////////////////////////////////////////////////////////////////////////
// Internal methods
///////////////////////////////////////////////////////////////////////////
Boolean CAudioService::CheckForRingerModeChange(
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 step)
{
    Boolean adjustVolumeIndex = TRUE;
    Int32 ringerMode;
    GetRingerMode(&ringerMode);

    switch (ringerMode) {
        case IAudioManager::RINGER_MODE_NORMAL: {
            if (direction == IAudioManager::ADJUST_LOWER){
                if (mHasVibrator) {
                    // "step" is the delta in internal index units corresponding to a
                    // change of 1 in UI index units.
                    // Because of rounding when rescaling from one stream index range to its alias
                    // index range, we cannot simply test oldIndex == step:
                    //   (step <= oldIndex < 2 * step) is equivalent to: (old UI index == 1)
                    if (step <= oldIndex && oldIndex < 2 * step) {
                        ringerMode = IAudioManager::RINGER_MODE_VIBRATE;
                    }
                }
                else {
                    // (oldIndex < step) is equivalent to (old UI index == 0)
                    if ((oldIndex < step) && mPrevVolDirection != IAudioManager::ADJUST_LOWER) {
                        ringerMode = IAudioManager::RINGER_MODE_SILENT;
                    }
                }
            }
            break;
        }

        case IAudioManager::RINGER_MODE_VIBRATE: {
            if (!mHasVibrator) {
                Logger::E(TAG, "checkForRingerModeChange() current ringer mode is vibrate"
                    "but no vibrator is present");
                break;
            }

            if ((direction == IAudioManager::ADJUST_LOWER)) {
                if (mPrevVolDirection != IAudioManager::ADJUST_LOWER) {
                    ringerMode = IAudioManager::RINGER_MODE_SILENT;
                }
            }
            else if (direction == IAudioManager::ADJUST_RAISE) {
                ringerMode = IAudioManager::RINGER_MODE_NORMAL;
            }
            adjustVolumeIndex = FALSE;
            break;
        }

        case IAudioManager::RINGER_MODE_SILENT: {
            if (direction == IAudioManager::ADJUST_RAISE) {
                if (mHasVibrator) {
                    ringerMode = IAudioManager::RINGER_MODE_VIBRATE;
                }
                else {
                    ringerMode = IAudioManager::RINGER_MODE_NORMAL;
                }
            }
            adjustVolumeIndex = FALSE;
            break;
        }

        default: {
            Logger::E(TAG, "checkForRingerModeChange() wrong ringer mode: %d", ringerMode);
            break;
        }
    }

    SetRingerMode(ringerMode);

    mPrevVolDirection = direction;

    return adjustVolumeIndex;
}

ECode CAudioService::IsStreamAffectedByRingerMode(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mRingerModeAffectedStreams & (1 << streamType)) != 0;
    return NOERROR;
}

Boolean CAudioService::IsStreamMutedByRingerMode(
    /* [in] */ Int32 streamType)
{
    return (mRingerModeMutedStreams & (1 << streamType)) != 0;
}

ECode CAudioService::IsStreamAffectedByMute(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mMuteAffectedStreams & (1 << streamType)) != 0;
    return NOERROR;
}

ECode CAudioService::EnsureValidDirection(
    /* [in] */ Int32 direction)
{
    if (direction < IAudioManager::ADJUST_LOWER || direction > IAudioManager::ADJUST_RAISE) {
        Logger::E(TAG, "Bad direction %d", direction);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        //throw new IllegalArgumentException("Bad direction " + direction);
    }
    return NOERROR;
}

ECode CAudioService::EnsureValidSteps(
    /* [in] */ Int32 steps)
{
    if (Elastos::Core::Math::Abs(steps) > MAX_BATCH_VOLUME_ADJUST_STEPS) {
        Logger::E(TAG, "Bad volume adjust steps %d", steps);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        //throw new IllegalArgumentException("Bad volume adjust steps " + steps);
    }
    return NOERROR;
}

ECode CAudioService::EnsureValidStreamType(
    /* [in] */ Int32 streamType)
{
    if (streamType < 0 || streamType >= mStreamStates->GetLength()) {
        Logger::E(TAG, "Bad stream type %d", streamType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        //throw new IllegalArgumentException("Bad stream type " + streamType);
    }
    return NOERROR;
}

Boolean CAudioService::IsInCommunication()
{
/*Eddie(E_NOT_IMPLEMENTED)*/
    Boolean isOffhook = FALSE;
//
//    if (mVoiceCapable) {
//        try {
//            ITelephony phone = ITelephony.Stub.asInterface(ServiceManager.checkService("phone"));
//            if (phone != NULL) {
//                isOffhook = phone.isOffhook();
//            }
//        } catch (RemoteException e) {
//            Logger::W(TAG, "Couldn't connect to phone service"/*, e*/);
//        }
//    }

    Int32 tempValue;
    GetMode(&tempValue);
    return (isOffhook || tempValue == IAudioManager::MODE_IN_COMMUNICATION);
}

Int32 CAudioService::GetActiveStreamType(
    /* [in] */ Int32 suggestedStreamType)
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 tempValue;
    Boolean tempState1;
    Boolean tempState2;

    if (mVoiceCapable) {
        if (IsInCommunication()) {
            audioSystemHelper->GetForceUse(IAudioSystem::FOR_COMMUNICATION, &tempValue);
            if (tempValue == IAudioSystem::FORCE_BT_SCO) {
                // Logger::V(TAG, "getActiveStreamType: Forcing STREAM_BLUETOOTH_SCO...");
                return IAudioSystem::STREAM_BLUETOOTH_SCO;
            }
            else {
                // Logger::V(TAG, "getActiveStreamType: Forcing STREAM_VOICE_CALL...");
                return IAudioSystem::STREAM_VOICE_CALL;
            }
        }
        else if (suggestedStreamType == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
            // Having the suggested stream be USE_DEFAULT_STREAM_TYPE is how remote control
            // volume can have priority over STREAM_MUSIC
            if (CheckUpdateRemoteStateIfActive(IAudioSystem::STREAM_MUSIC)) {
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_REMOTE_MUSIC");
                }
                return STREAM_REMOTE_MUSIC;
            }
            else if (audioSystemHelper->IsStreamActive(IAudioSystem::STREAM_MUSIC,
                        DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS, &tempState1), tempState1) {
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_MUSIC stream active");
                }
                return IAudioSystem::STREAM_MUSIC;
            }
            else {
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_RING...");
                }
                return IAudioSystem::STREAM_RING;
            }
        }
        else if (audioSystemHelper->IsStreamActive(IAudioSystem::STREAM_MUSIC, 0, &tempState1), tempState1) {
            if (DEBUG_VOL) {
                Logger::V(TAG, "getActiveStreamType: Forcing STREAM_MUSIC stream active");
            }
            return IAudioSystem::STREAM_MUSIC;
        }
        else {
            if (DEBUG_VOL) {
                Logger::V(TAG, "getActiveStreamType: Returning suggested type %d", suggestedStreamType);
            }
            return suggestedStreamType;
        }
    }
    else {
        if (IsInCommunication()) {
            audioSystemHelper->GetForceUse(IAudioSystem::FOR_COMMUNICATION, &tempValue);
            if (tempValue == IAudioSystem::FORCE_BT_SCO) {
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_BLUETOOTH_SCO");
                }
                return IAudioSystem::STREAM_BLUETOOTH_SCO;
            }
            else {
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_VOICE_CALL");
                }
                return IAudioSystem::STREAM_VOICE_CALL;
            }
        }
        else if ((audioSystemHelper->IsStreamActive(IAudioSystem::STREAM_NOTIFICATION,
                DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS, &tempState1), tempState1) ||
                (audioSystemHelper->IsStreamActive(IAudioSystem::STREAM_RING,
                        DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS, &tempState2), tempState2)) {
            if (DEBUG_VOL) {
                Logger::V(TAG, "getActiveStreamType: Forcing STREAM_NOTIFICATION");
            }
            return IAudioSystem::STREAM_NOTIFICATION;
        }
        else if (suggestedStreamType == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
            if (CheckUpdateRemoteStateIfActive(IAudioSystem::STREAM_MUSIC)) {
                // Having the suggested stream be USE_DEFAULT_STREAM_TYPE is how remote control
                // volume can have priority over STREAM_MUSIC
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_REMOTE_MUSIC");
                }
                return STREAM_REMOTE_MUSIC;
            }
            else {
                if (DEBUG_VOL) {
                    Logger::V(TAG, "getActiveStreamType: using STREAM_MUSIC as default");
                }
                return IAudioSystem::STREAM_MUSIC;
            }
        }
        else {
            if (DEBUG_VOL) {
                Logger::V(TAG, "getActiveStreamType: Returning suggested type %d", suggestedStreamType);
            }
            return suggestedStreamType;
        }
    }
}

void CAudioService::BroadcastRingerMode(
    /* [in] */ Int32 ringerMode)
{
    // Send sticky broadcast
    AutoPtr<IIntent> broadcast;
    CIntent::New(IAudioManager::RINGER_MODE_CHANGED_ACTION, (IIntent**)&broadcast);
    broadcast->PutExtra(IAudioManager::EXTRA_RINGER_MODE, mRingerMode);
    broadcast->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    SendStickyBroadcastToAll(broadcast);
}

void CAudioService::BroadcastVibrateSetting(
    /* [in] */ Int32 vibrateType)
{
    // Send broadcast
/*Eddie(E_NOT_IMPLEMENTED)*/
//    if (ActivityManagerNative::IsSystemReady()) {
//        AutoPtr<IIntent> broadcast;
//        CIIntent::New(IAudioManager::VIBRATE_SETTING_CHANGED_ACTION, (IIntent**)&broadcast);
//        broadcast->PutExtra(IAudioManager::EXTRA_VIBRATE_TYPE, vibrateType);
//        Boolean tempState;
//        GetVibrateSetting(vibrateType, &tempState);
//        broadcast->PutBooleanExtra(IAudioManager::EXTRA_VIBRATE_SETTING, tempState);
//        SendBroadcastToAll(broadcast);
//    }
}

// Message helper methods

void CAudioService::QueueMsgUnderWakeLock(
    /* [in] */ Int32 msg,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 delay)
{
    mMediaEventWakeLock->AcquireLock();
    SendMsg(msg, SENDMSG_QUEUE, arg1, arg2, obj, delay);
}

ECode CAudioService::SendMsg(
    /* [in] */ Int32 msg,
    /* [in] */ Int32 existingMsgPolicy,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 delay)
{
    VALIDATE_NOT_NULL(mAudioHandler);

    Boolean bval;
    if (existingMsgPolicy == SENDMSG_REPLACE) {
        mAudioHandler->RemoveMessages(msg);
    }
    else if (existingMsgPolicy == SENDMSG_NOOP && (mAudioHandler->HasMessages(msg, &bval), bval)) {
        return NOERROR;
    }

    AutoPtr<IMessage> message;
    mAudioHandler->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&message);
    return mAudioHandler->SendMessageDelayed(message, delay, &bval);
}

/*package*/
Boolean CAudioService::CheckAudioSettingsPermission(
    /* [in] */ const String& method)
{
    Int32 tempValue;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MODIFY_AUDIO_SETTINGS, &tempValue));
    if (tempValue == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }

    AutoPtr<IBinderHelper> binderHelper;
    Int32 pid, uid;
    binderHelper->GetCallingPid(&pid);
    binderHelper->GetCallingUid(&uid);

    Logger::W(TAG, "Audio Settings Permission Denial: %s from pid=%d, uid=%d",
        method.string(), pid, uid);
    return FALSE;
}

Int32 CAudioService::GetDeviceForStream(
    /* [in] */ Int32 stream)
{
    Int32 device;
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->GetDevicesForStream(stream, &device);
    if ((device & (device - 1)) != 0) {
        // Multiple device selection is either:
        //  - speaker + one other device: give priority to speaker in this case.
        //  - one A2DP device + another device: happens with duplicated output. In this case
        // retain the device on the A2DP output as the other must not correspond to an active
        // selection if not the speaker.
        if (((device & IAudioSystem::DEVICE_OUT_SPEAKER) != 0)
            ||(device & IAudioSystem::DEVICE_OUT_WIRED_HEADSET) != 0) {
            device = IAudioSystem::DEVICE_OUT_SPEAKER;
        }
        else {
            device &= IAudioSystem::DEVICE_OUT_ALL_A2DP;
        }
    }

    if (SystemProperties::GetBoolean(String("sys.audio.headsetConmbine"), FALSE)) {//wxl
        if(device == IAudioSystem::DEVICE_OUT_WIRED_HEADSET) {
            device = IAudioSystem::DEVICE_OUT_SPEAKER;
        }
    }
    return device;
}

ECode CAudioService::SetWiredDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    AutoLock lock(mConnectedDevicesLock);

    AutoPtr<ICharSequence> seq;
    CString::New(name, (ICharSequence**)&seq);
    Int32 delay = CheckSendBecomingNoisyIntent(device, state);
    QueueMsgUnderWakeLock(MSG_SET_WIRED_DEVICE_CONNECTION_STATE,
        device,
        state,
        seq,
        delay);
    return NOERROR;
}

ECode CAudioService::SetBluetoothA2dpDeviceConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 state,
    /* [out] */ Int32* result)
{
    AutoLock lock(mConnectedDevicesLock);
    Int32 delay = CheckSendBecomingNoisyIntent(
        IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
        (state == IBluetoothA2dp::STATE_CONNECTED) ? 1 : 0);
    QueueMsgUnderWakeLock(MSG_SET_A2DP_CONNECTION_STATE,
        state,
        0,
        (IInterface*)device,
        delay);

    *result = delay;
    return NOERROR;
}

void CAudioService::MakeA2dpDeviceAvailable(
    /* [in] */ const String& address)
{
    // enable A2DP before notifying A2DP connection to avoid unecessary processing in
    // audio policy manager
    SetBluetoothA2dpOnInt(TRUE);
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetDeviceConnectionState(
        IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
        IAudioSystem::DEVICE_STATE_AVAILABLE,
        address);
    // Reset A2DP suspend state each time a new sink is connected
    audioSystemHelper->SetParameters(String("A2dpSuspended=FALSE"));
    mConnectedDevices[IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP] = address;
}

void CAudioService::OnSendBecomingNoisyIntent() {
    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::ACTION_AUDIO_BECOMING_NOISY, (IIntent**)&intent);
    SendBroadcastToAll(intent);
}

void CAudioService::MakeA2dpDeviceUnavailableNow(
    /* [in] */ const String& address)
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetDeviceConnectionState(
        IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
        IAudioSystem::DEVICE_STATE_UNAVAILABLE,
        address);
    mConnectedDevices.Erase(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP);
}

void CAudioService::MakeA2dpDeviceUnavailableLater(
    /* [in] */ const String& address)
{
    // prevent any activity on the A2DP audio output to avoid unwanted
    // reconnection of the sink.
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    Int32 tempValue;
    audioSystemHelper->SetParameters(String("A2dpSuspended=TRUE"));

    // the device will be made unavailable later, so consider it disconnected right away
    mConnectedDevices.Erase(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP);

    // send the delayed message to make the device unavailable later
    AutoPtr<ICharSequence> seq;
    CString::New(address, (ICharSequence**)&seq);
    AutoPtr<IMessage> message;
    mAudioHandler->ObtainMessage(MSG_BTA2DP_DOCK_TIMEOUT, seq, (IMessage**)&message);
    Boolean bval;
    mAudioHandler->SendMessageDelayed(message, BTA2DP_DOCK_TIMEOUT_MILLIS, &bval);
}

void CAudioService::CancelA2dpDeviceTimeout()
{
    mAudioHandler->RemoveMessages(MSG_BTA2DP_DOCK_TIMEOUT);
}

Boolean CAudioService::HasScheduledA2dpDockTimeout()
{
    Boolean bval;
    mAudioHandler->HasMessages(MSG_BTA2DP_DOCK_TIMEOUT, &bval);
    return bval;
}

void CAudioService::OnSetA2dpConnectionState(
    /* [in] */ IBluetoothDevice* btDevice,
    /* [in] */ Int32 state)
{
    if (btDevice == NULL) {
        return;
    }
    String address;
    btDevice->GetAddress(&address);
    AutoPtr<IBluetoothAdapterHelper> bluetoothAdapterHelper;
    Boolean tempState;
    bluetoothAdapterHelper->CheckBluetoothAddress(address, &tempState);
    if (!tempState) {
        address = "";
    }
    {
        AutoLock lock(mConnectedDevicesLock);

        Boolean tempState;
        HashMap< Int32, String >::Iterator it = mConnectedDevices.Find(
            IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP);
        if (it != mConnectedDevices.End()) {
            tempState = TRUE;
        } else {
            tempState = FALSE;
        }

        Boolean isConnected = tempState && it->mSecond.Equals(address);

        if (isConnected && state != IBluetoothProfile::STATE_CONNECTED) {
            btDevice->IsBluetoothDock(&tempState);
            if (tempState) {
                if (state == IBluetoothProfile::STATE_DISCONNECTED) {
                    // introduction of a delay for transient disconnections of docks when
                    // power is rapidly turned off/on, this message will be canceled if
                    // we reconnect the dock under a preset delay
                    MakeA2dpDeviceUnavailableLater(address);
                    // the next time isConnected is evaluated, it will be false for the dock
                }
            } else {
                MakeA2dpDeviceUnavailableNow(address);
            }

            {
               AutoLock lock(mCurAudioRoutesLock);
               AutoPtr<ICharSequence> seq;
               mCurAudioRoutes->GetBluetoothName((ICharSequence**)&seq);

               if (seq != NULL) {
                   mCurAudioRoutes->SetBluetoothName(NULL);
                   SendMsg(MSG_REPORT_NEW_ROUTES, SENDMSG_NOOP, 0, 0, NULL, 0);
               }
           }
        }
        else if (!isConnected && state == IBluetoothProfile::STATE_CONNECTED) {
            btDevice->IsBluetoothDock(&tempState);
            if (tempState) {
                // this could be a reconnection after a transient disconnection
                CancelA2dpDeviceTimeout();
                mDockAddress = address;
            } else {
                // this could be a connection of another A2DP device before the timeout of
                // a dock: cancel the dock timeout, and make the dock unavailable now
                if(HasScheduledA2dpDockTimeout()) {
                    CancelA2dpDeviceTimeout();
                    MakeA2dpDeviceUnavailableNow(mDockAddress);
                }
            }
            MakeA2dpDeviceAvailable(address);

            {
                AutoLock lock(mCurAudioRoutesLock);

                AutoPtr<ICharSequence> seq;
                mCurAudioRoutes->GetBluetoothName((ICharSequence**)&seq);
                String bluetoothName;
                if (seq != NULL) {
                    seq->ToString(&bluetoothName);
                }

                String name;
                btDevice->GetAliasName(&name);
                if (!TextUtils::Equals(bluetoothName, name)) {
                    seq = NULL;
                    CString::New(name, (ICharSequence**)&seq);
                    mCurAudioRoutes->SetBluetoothName(seq);
                    SendMsg(MSG_REPORT_NEW_ROUTES, SENDMSG_NOOP, 0, 0, NULL, 0);
                }
            }
        }
    }
}

Boolean CAudioService::HandleDeviceConnection(
    /* [in] */ Boolean connected,
    /* [in] */ Int32 device,
    /* [in] */ const String& params)
{
    {
        AutoLock lock(mConnectedDevicesLock);

        Boolean tempState;
        HashMap< Int32, String >::Iterator it = mConnectedDevices.Find(device);
        if (it != mConnectedDevices.End()) {
            tempState = TRUE;
        } else {
            tempState = FALSE;
        }

        Boolean isConnected = tempState &&
                (params.IsEmpty() || it->mSecond.Equals(params));

        AutoPtr<IAudioSystemHelper> audioSystemHelper;
        CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
        if (isConnected && !connected) {
            audioSystemHelper->SetDeviceConnectionState(
                device, IAudioSystem::DEVICE_STATE_UNAVAILABLE, it->mSecond);
            mConnectedDevices.Erase(device);
            return TRUE;
        }
        else if (!isConnected && connected) {
            audioSystemHelper->SetDeviceConnectionState(
                device, IAudioSystem::DEVICE_STATE_AVAILABLE, params);
            mConnectedDevices[device] = params;
            return TRUE;
        }
    }
    return FALSE;
}

Int32 CAudioService::CheckSendBecomingNoisyIntent(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state)
{
    Int32 delay = 0;
    if ((state == 0) && ((device & mBecomingNoisyIntentDevices) != 0)) {
        Int32 devices = 0;
        HashMap< Int32, String >::Iterator it = mConnectedDevices.Begin();
        for(Int32 dev = 0; it != mConnectedDevices.End(); ++it) {
            dev = it->mFirst;
            if ((dev & mBecomingNoisyIntentDevices) != 0) {
               devices |= dev;
            }
        }

        if (devices == device) {
            SendMsg(MSG_BROADCAST_AUDIO_BECOMING_NOISY,
                SENDMSG_REPLACE,
                0,
                0,
                NULL,
                0);
            delay = 1000;
        }
    }

    Boolean bval;
    if ((mAudioHandler->HasMessages(MSG_SET_A2DP_CONNECTION_STATE, &bval), bval)
        || (mAudioHandler->HasMessages(MSG_SET_WIRED_DEVICE_CONNECTION_STATE, &bval), bval)
    ) {
        delay = 1000;
    }
    return delay;
}

void CAudioService::SendDeviceConnectionIntent(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    intent->PutExtra(String("state"), state);
    intent->PutExtra(String("name"), name);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);

    Int32 connType = 0;

    if (device == IAudioSystem::DEVICE_OUT_WIRED_HEADSET) {
        connType = CAudioRoutesInfo::MAIN_HEADSET;
        intent->SetAction(IIntent::ACTION_HEADSET_PLUG);
        intent->PutExtra(String("microphone"), 1);

    }
    else if (device == IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE) {
        connType = CAudioRoutesInfo::MAIN_HEADPHONES;
        intent->SetAction(IIntent::ACTION_HEADSET_PLUG);
        intent->PutExtra(String("microphone"), 0);

    }
    else if (device == IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET) {
        connType = CAudioRoutesInfo::MAIN_DOCK_SPEAKERS;
        intent->SetAction(IIntent::ACTION_ANALOG_AUDIO_DOCK_PLUG);

    }
    else if (device == IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET) {
        connType = CAudioRoutesInfo::MAIN_DOCK_SPEAKERS;
        intent->SetAction(IIntent::ACTION_DIGITAL_AUDIO_DOCK_PLUG);

    }
    else if (device == IAudioSystem::DEVICE_OUT_AUX_DIGITAL) {
        connType = CAudioRoutesInfo::MAIN_HDMI;
        intent->SetAction(IIntent::ACTION_HDMI_AUDIO_PLUG);

    }

    {
        AutoLock lock(mCurAudioRoutesLock);

        if (connType != 0) {
            Int32 oldConn, newConn;
            mCurAudioRoutes->GetMainType(&oldConn);

            newConn = oldConn;
            if (state != 0) {
                newConn |= connType;
            }
            else {
                newConn &= ~connType;
            }

            if (newConn != oldConn) {
                mCurAudioRoutes->SetMainType(newConn);
                SendMsg(MSG_REPORT_NEW_ROUTES, SENDMSG_NOOP, 0, 0, NULL, 0);
            }
        }
    }

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 tempValue;
    binderHelper->ClearCallingIdentity(&tempValue);
    const Int64 ident = tempValue;

//    try {
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
//    } finally {
    binderHelper->RestoreCallingIdentity(ident);
//    }
}

void CAudioService::OnSetWiredDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    AutoLock lock(mConnectedDevicesLock);

    if ((state == 0) && ((device == IAudioSystem::DEVICE_OUT_WIRED_HEADSET) ||
            (device == IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE))) {
        SetBluetoothA2dpOnInt(TRUE);
    }
    Boolean isUsb = ((device & IAudioSystem::DEVICE_OUT_ALL_USB) != 0);
    HandleDeviceConnection((state == 1), device, (isUsb ? name : String("")));
    if (state != 0) {
        if ((device == IAudioSystem::DEVICE_OUT_WIRED_HEADSET) ||
            (device == IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE)) {
            SetBluetoothA2dpOnInt(FALSE);
        }
        if ((device & mSafeMediaVolumeDevices) != 0) {
            SendMsg(MSG_CHECK_MUSIC_ACTIVE,
                SENDMSG_REPLACE,
                0,
                0,
                NULL,
                MUSIC_ACTIVE_POLL_PERIOD_MS);
        }
    }
    if (!isUsb) {
         SendDeviceConnectionIntent(device, state, name);
    }
}

void CAudioService::DiscardAudioFocusOwner()
{
    // AutoLock lock(mAudioFocusLock);

    // if (!mFocusStack->Empty() && (mFocusStack->Peek().mFocusDispatcher != NULL)) {
    //     // notify the current focus owner it lost focus after removing it from stack
    //     FocusStackEntry focusOwner = mFocusStack->Pop();
    //     try {
    //         focusOwner::mFocusDispatcher->DispatchAudioFocusChange(
    //         IAudioManager::AUDIOFOCUS_LOSS, focusOwner::mClientId);
    //     } catch (RemoteException e) {
    //         Logger::E(TAG, "Failure to signal loss of audio focus due to "/*+ e*/);
    //         e.printStackTrace();
    //     }

    //     focusOwner->UnlinkToDeath();

    //     // clear RCD
    //     {
    //         AutoLock lock(mRCStackLock);

    //         ClearRemoteControlDisplay_syncAfRcs();
    //     }
    // }
}

void CAudioService::NotifyTopOfAudioFocusStack()
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    // notify the top of the stack it gained focus
//    if (!mFocusStack->Empty() && (mFocusStack->Peek().mFocusDispatcher != NULL)) {
//        if (CanReassignAudioFocus()) {
//            try {
//                mFocusStack->Peek().mFocusDispatcher->DispatchAudioFocusChange(
//                        AudioManager_AUDIOFOCUS_GAIN, mFocusStack->Peek().mClientId);
//            } catch (RemoteException e) {
//                Logger::E(TAG, "Failure to signal gain of audio control focus due to "/*+ e*/);
//                e.printStackTrace();
//            }
//         }
//     }
}

String CAudioService::DumpFocusStack()
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    pw->PrintStringln(String("\nAudio Focus stack entries:"));
//    {
//        AutoLock lock(mAudioFocusLock);
//
//        Iterator<FocusStackEntry> stackIterator = mFocusStack->Iterator();
//        while(stackIterator->HasNext()) {
//            FocusStackEntry fse = stackIterator->Next();
//            pw->Println("  source:" + fse.mSourceRef
//                     + " -- pack: " + fse.mPackageName
//                     + " -- client: " + fse.mClientId
//                     + " -- duration: " + fse.mFocusChangeType
//                     + " -- uid: " + fse.mCallingUid
//                     + " -- stream: " + fse.mStreamType);
//        }
//    }
    return String("\nAudio Focus stack entries:");
}

void CAudioService::RemoveFocusStackEntry(
    /* [in] */ const String& clientToRemove,
    /* [in] */ Boolean signal)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    // is the current top of the focus stack abandoning focus? (because of request, not death))
//    if (!mFocusStack->Empty() && mFocusStack->Peek().mClientId->Equals(clientToRemove))
//    {
//        //Logger::i(TAG, "   removeFocusStackEntry() removing top of stack");
//        FocusStackEntry fse = mFocusStack->Pop();
//        fse->UnlinkToDeath();
//        if (signal) {
//            // notify the new top of the stack it gained focus
//            NotifyTopOfAudioFocusStack();
//            // there's a new top of the stack, let the remote control know
//            {
//                AutoLock lock(mRCStackLock);
//
//                CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
//            }
//        }
//    } else {
//        // focus is abandoned by a client that's not at the top of the stack,
//        // no need to update focus.
//        Iterator<FocusStackEntry> stackIterator = mFocusStack->Iterator();
//        while(stackIterator->HasNext()) {
//            FocusStackEntry fse = (FocusStackEntry)stackIterator->Next();
//            if(fse.mClientId->Equals(clientToRemove)) {
//                Logger::I(TAG, " AudioFocus  abandonAudioFocus(): removing entry for "
//                        + fse.mClientId);
//                stackIterator->Remove();
//                fse->UnlinkToDeath();
//            }
//        }
//    }
}

void CAudioService::RemoveFocusStackEntryForClient(
    /* [in] */ IBinder* cb)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    // is the owner of the audio focus part of the client to remove?
//    Boolean isTopOfStackForClientToRemove = !mFocusStack->IsEmpty() &&
//            mFocusStack->Peek().mSourceRef->Equals(cb);
//    Iterator<FocusStackEntry> stackIterator = mFocusStack->Iterator();
//    while(stackIterator->HasNext()) {
//        FocusStackEntry fse = (FocusStackEntry)stackIterator->Next();
//        if(fse.mSourceRef->Equals(cb)) {
//            Logger::I(TAG, " AudioFocus  abandonAudioFocus(): removing entry for "
//                        + fse.mClientId);
//            stackIterator->Remove();
//            // the client just died, no need to unlink to its death
//        }
//    }
//    if (isTopOfStackForClientToRemove) {
//        // we removed an entry at the top of the stack:
//        //  notify the new top of the stack it gained focus.
//        NotifyTopOfAudioFocusStack();
//        // there's a new top of the stack, let the remote control know
//        {
//            AutoLock lock(mRCStackLock);
//
//            CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
//        }
//    }
}

Boolean CAudioService::CanReassignAudioFocus()
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    // focus requests are rejected during a phone call or when the phone is ringing
//    // this is equivalent to IN_VOICE_COMM_FOCUS_ID having the focus
//    if (!mFocusStack->IsEmpty() && IN_VOICE_COMM_FOCUS_ID->Equals(mFocusStack->Peek().mClientId)) {
//        return FALSE;
//    }
    return TRUE;
}

ECode CAudioService::RequestAudioFocus(
    /* [in] */ Int32 mainStreamType,
    /* [in] */ Int32 focusChangeHint,
    /* [in] */ IBinder* cb,
    /* [in] */ IIAudioFocusDispatcher* fd,
    /* [in] */ const String& clientId,
    /* [in] */ const String& callingPackageName,
    /* [out] */ Int32* result)
{
    Logger::I(TAG, String(" AudioFocus  requestAudioFocus() from ") + clientId);
    // the main stream type for the audio focus request is currently not used. It may
    // potentially be used to handle multiple stream type-dependent audio focuses.

    // we need a valid binder callback for clients
/*Eddie(E_NOT_IMPLEMENTED)*/
//    if (!cb->PingBinder()) {
//         Logger::E(TAG, " AudioFocus DOA client for requestAudioFocus(), aborting.");
//         *result = IAudioManager::AUDIOFOCUS_REQUEST_FAILED;
//         return NOERROR;
//    }

    {
        AutoLock lock(mAudioFocusLock);

        if (!CanReassignAudioFocus()) {
            *result = IAudioManager::AUDIOFOCUS_REQUEST_FAILED;
            return NOERROR;
        }

        // handle the potential premature death of the new holder of the focus
        // (premature death == death before abandoning focus)
        // Register for client death notification
        AutoPtr<AudioFocusDeathHandler> afdh = new AudioFocusDeathHandler(cb, this);

/*Eddie(E_NOT_IMPLEMENTED)*/
//        try {
//            cb->LinkToDeath(afdh, 0);
//        } catch (RemoteException e) {
//            // client has already died!
//            Logger::W(TAG, "AudioFocus  requestAudioFocus() could not link to "/*+cb+*/" binder death");
//            *result = IAudioManager::AUDIOFOCUS_REQUEST_FAILED;
//            return NOERROR;
//        }
//
//        if (!mFocusStack->Empty() && mFocusStack->Peek().mClientId->Equals(clientId)) {
//            // if focus is already owned by this client and the reason for acquiring the focus
//            // hasn't changed, don't do anything
//            if (mFocusStack->Peek().mFocusChangeType == focusChangeHint) {
//                // unlink death handler so it can be gc'ed.
//                // linkToDeath() creates a JNI global reference preventing collection.
//                cb->UnlinkToDeath(afdh, 0);
//                *result = IAudioManager::AUDIOFOCUS_REQUEST_GRANTED;
//                return NOERROR;
//            }
//            // the reason for the audio focus request has changed: remove the current top of
//            // stack and respond as if we had a new focus owner
//            AutoPtr<FocusStackEntry> fse = mFocusStack->Pop();
//            fse->UnlinkToDeath();
//        }
//
//        // notify current top of stack it is losing focus
//        if (!mFocusStack->Empty() && (mFocusStack->Peek().mFocusDispatcher != NULL)) {
//            try {
//                mFocusStack->Peek().mFocusDispatcher->DispatchAudioFocusChange(
//                        -1 * focusChangeHint, // loss and gain codes are inverse of each other
//                        mFocusStack->Peek().mClientId);
//            } catch (RemoteException e) {
//                Logger::E(TAG, " Failure to signal loss of focus due to "/*+ e*/);
//                e->PrintStackTrace();
//            }
//        }

        // focus requester might already be somewhere below in the stack, remove it
        RemoveFocusStackEntry(clientId, FALSE /* signal */);

/*Eddie(E_NOT_IMPLEMENTED)*/
//        // push focus requester at the top of the audio focus stack
//        mFocusStack->Push(new FocusStackEntry(mainStreamType, focusChangeHint, fd, cb,
//                clientId, afdh, callingPackageName, Binder->GetCallingUid()));
//
//        // there's a new top of the stack, let the remote control know
//        {
//            AutoLock lock(mRCStack);
//            CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
//        }
    }//synchronized(mAudioFocusLock)

    *result =  IAudioManager::AUDIOFOCUS_REQUEST_GRANTED;
    return NOERROR;
}

ECode CAudioService::AbandonAudioFocus(
    /* [in] */ IIAudioFocusDispatcher* fl,
    /* [in] */ const String& clientId,
    /* [out] */ Int32* result)
{
    Logger::I(TAG, String(" AudioFocus  abandonAudioFocus() from ") + clientId);
//    try {
        // this will take care of notifying the new focus owner if needed
        {
            AutoLock lock(mAudioFocusLock);
            RemoveFocusStackEntry(clientId, TRUE);
        }
//    } catch (java.util.ConcurrentModificationException cme) {
//        // Catching this exception here is temporary. It is here just to prevent
//        // a crash seen when the "Silent" notification is played. This is believed to be fixed
//        // but this try catch block is left just to be safe.
//        Logger::E(TAG, "FATAL EXCEPTION AudioFocus  abandonAudioFocus() caused " /*+ cme*/);
//        cme.printStackTrace();
//    }

    *result = IAudioManager::AUDIOFOCUS_REQUEST_GRANTED;
    return NOERROR;
}

ECode CAudioService::UnregisterAudioFocusClient(
    /* [in] */ const String& clientId)
{
    AutoLock lock(mAudioFocusLock);
    RemoveFocusStackEntry(clientId, FALSE);

    return NOERROR;
}

//==========================================================================================
// RemoteControl
//==========================================================================================

ECode CAudioService::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent)
{
    FilterMediaKeyEvent(keyEvent, FALSE /*needWakeLock*/);

    return NOERROR;
}

ECode CAudioService::DispatchMediaKeyEventUnderWakelock(
    /* [in] */ IKeyEvent* keyEvent)
{
    FilterMediaKeyEvent(keyEvent, TRUE /*needWakeLock*/);

    return NOERROR;
}

void CAudioService::FilterMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    // sanity check on the incoming key event
    if (!IsValidMediaKeyEvent(keyEvent)) {
        Logger::E(TAG, "not dispatching invalid media key event " /*+ keyEvent*/);
        return;
    }
    // event filtering for telephony
    Int32 tempValue;
    {
        AutoLock lock(mRingingLock);
        {
            AutoLock lock(mRCStackLock);

            GetMode(&tempValue);
            if ((mMediaReceiverForCalls != NULL) &&
                    (mIsRinging || (tempValue == IAudioSystem::MODE_IN_CALL))) {
                DispatchMediaKeyEventForCalls(keyEvent, needWakeLock);
                return;
            }
        }
    }
    // event filtering based on voice-based interactions
    keyEvent->GetKeyCode(&tempValue);
    if (IsValidVoiceInputKeyCode(tempValue)) {
        FilterVoiceInputKeyEvent(keyEvent, needWakeLock);
    } else {
        DispatchMediaKeyEvent(keyEvent, needWakeLock);
    }
}

void CAudioService::DispatchMediaKeyEventForCalls(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    AutoPtr<IIntent> keyIntent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, NULL, (IIntent**)&keyIntent);
    keyIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, (IParcelable*)keyEvent);
    String tempText;
    mMediaReceiverForCalls->GetPackageName(&tempText);
    keyIntent->SetPackage(tempText);
    if (needWakeLock) {
        mMediaEventWakeLock->AcquireLock();
        keyIntent->PutExtra(EXTRA_WAKELOCK_ACQUIRED, WAKELOCK_RELEASE_ON_FINISHED);
    }
    AutoPtr<IBinderHelper> binderHelper;
    Int64 tempValue;
    binderHelper->ClearCallingIdentity(&tempValue);
    const Int64 ident = tempValue;
//    try {
        AutoPtr<IUserHandleHelper> userHandleHelper;
        AutoPtr<IUserHandle> userHandle;
        userHandleHelper->GetALL((IUserHandle**)&userHandle);
        mContext->SendOrderedBroadcastAsUser((IIntent*)keyIntent, userHandle,
                String(""), mKeyEventDone, mAudioHandler, IActivity::RESULT_OK, String(""), NULL);
//    } finally {
        binderHelper->RestoreCallingIdentity(ident);
//    }
}

void CAudioService::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    if (needWakeLock) {
        mMediaEventWakeLock->AcquireLock();
    }

    AutoPtr<IIntent> keyIntent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, NULL, (IIntent**)&keyIntent);
    keyIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, (IParcelable*)keyEvent);

    {
        // AutoLock lock(mRCStackLock);

        // if (!mRCStack.IsEmpty()) {
        //    // send the intent that was registered by the client
        //    try {
        //        mRCStack->Peek().mMediaIntent->Send(mContext,
        //                needWakeLock ? WAKELOCK_RELEASE_ON_FINISHED : 0 /*code*/,
        //                keyIntent, AudioService.this, mAudioHandler);
        //    } catch (CanceledException e) {
        //        Logger::E(TAG, "Error sending pending intent " + mRCStack.peek());
        //        e->PrintStackTrace();
        //    }
        // } else {
        //    // legacy behavior when nobody registered their media button event receiver
        //    //    through AudioManager
        //    if (needWakeLock) {
        //        keyIntent->PutExtra(EXTRA_WAKELOCK_ACQUIRED, WAKELOCK_RELEASE_ON_FINISHED);
        //    }
        //    const Int64 ident = Binder->ClearCallingIdentity();
        //    try {
        //        mContext->SendOrderedBroadcastAsUser(keyIntent, UserHandle.ALL,
        //                NULL, mKeyEventDone,
        //                mAudioHandler, IActivity::RESULT_OK, NULL, NULL);
        //    } finally {
        //         Binder->RestoreCallingIdentity(ident);
        //    }
        // }
    }
}

void CAudioService::FilterVoiceInputKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    if (DEBUG_RC) {
        Logger::V(TAG, String("voice input key event: ") + /*keyEvent*/ + String(", needWakeLock=") + StringUtils::BooleanToString(needWakeLock));
    }

    Int32 voiceButtonAction = VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS;
    Int32 keyAction;
    keyEvent->GetAction(&keyAction);
    {
        AutoLock lock(mVoiceEventLock);

        if (keyAction == IKeyEvent::ACTION_DOWN) {
            Int32 tempValue;
            keyEvent->GetRepeatCount(&tempValue);
            if (tempValue == 0) {
                // initial down
                mVoiceButtonDown = TRUE;
                mVoiceButtonHandled = FALSE;
            } else if (mVoiceButtonDown && !mVoiceButtonHandled
                    && ((keyEvent->GetFlags(&tempValue), tempValue) & IKeyEvent::FLAG_LONG_PRESS) != 0) {
                // long-press, start voice-based interactions
                mVoiceButtonHandled = TRUE;
                voiceButtonAction = VOICEBUTTON_ACTION_START_VOICE_INPUT;
            }
        } else if (keyAction == IKeyEvent::ACTION_UP) {
            if (mVoiceButtonDown) {
                // voice button up
                mVoiceButtonDown = FALSE;
                Boolean tempState;
                if (!mVoiceButtonHandled && !(keyEvent->IsCanceled(&tempState), tempState)) {
                    voiceButtonAction = VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS;
                }
            }
        }
    }//synchronized(mVoiceEventLock)

    // take action after media button event filtering for voice-based interactions
    switch (voiceButtonAction) {
        case VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS: {
            if (DEBUG_RC) {
                Logger::V(TAG, "   ignore key event");
            }
            break;
        }

        case VOICEBUTTON_ACTION_START_VOICE_INPUT: {
            if (DEBUG_RC) {
                Logger::V(TAG, "   start voice-based interactions");
            }
            // then start the voice-based interactions
            StartVoiceBasedInteractions(needWakeLock);
            break;
        }

        case VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS: {
            if (DEBUG_RC) {
                Logger::V(TAG, "   send simulated key event, wakelock=%s",
                    needWakeLock ? "TRUE" : "FALSE");
            }
            SendSimulatedMediaButtonEvent(keyEvent, needWakeLock);
            break;
        }
    }
}

void CAudioService::SendSimulatedMediaButtonEvent(
    /* [in] */ IKeyEvent* originalKeyEvent,
    /* [in] */ Boolean needWakeLock)
{
    // send DOWN event
    AutoPtr<IKeyEventHelper> keyEventHelper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&keyEventHelper);
    AutoPtr<IKeyEvent> keyEvent;
    keyEventHelper->ChangeAction(originalKeyEvent, IKeyEvent::ACTION_DOWN, (IKeyEvent**)&keyEvent);
    DispatchMediaKeyEvent(keyEvent, needWakeLock);

    // send UP event
    keyEvent = NULL;
    keyEventHelper->ChangeAction(originalKeyEvent, IKeyEvent::ACTION_UP, (IKeyEvent**)&keyEvent);
    DispatchMediaKeyEvent(keyEvent, needWakeLock);
}

/*static*/
Boolean CAudioService::IsValidMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent)
{
    if (keyEvent == NULL) {
        return FALSE;
    }
    Int32 tempValue;
    keyEvent->GetKeyCode(&tempValue);
    const Int32 keyCode = tempValue;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
        case IKeyEvent::KEYCODE_MEDIA_CLOSE:
        case IKeyEvent::KEYCODE_MEDIA_EJECT:
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

/*static*/
Boolean CAudioService::IsValidVoiceInputKeyCode(
    /* [in] */ Int32 keyCode)
{
    if (keyCode == IKeyEvent::KEYCODE_HEADSETHOOK) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void CAudioService::StartVoiceBasedInteractions(
    /* [in] */ Boolean needWakeLock)
{
    AutoPtr<IIntent> voiceIntent;
    // select which type of search to launch:
    // - screen on and device unlocked: action is ACTION_WEB_SEARCH
    // - device locked or screen off: action is ACTION_VOICE_SEARCH_HANDS_FREE
    //    with EXTRA_SECURE set to true if the device is securely locked
    AutoPtr<IPowerManager> pm;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pm);
    assert(pm != NULL);

    Boolean tempState = FALSE;
    if (mKeyguardManager != NULL) {
        mKeyguardManager->IsKeyguardLocked(&tempState);
    }
    Boolean isLocked = tempState;
    if (!isLocked && (pm->IsScreenOn(&tempState), tempState)) {
        CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&voiceIntent);
    }
    else {
        tempState = FALSE;
        if (mKeyguardManager != NULL) {
            mKeyguardManager->IsKeyguardSecure(&tempState);
        }

        CIntent::New(IRecognizerIntent::ACTION_VOICE_SEARCH_HANDS_FREE, (IIntent**)&voiceIntent);
        voiceIntent->PutBooleanExtra(IRecognizerIntent::EXTRA_SECURE, isLocked && tempState);
    }

    // start the search activity
    if (needWakeLock) {
         mMediaEventWakeLock->AcquireLock();
    }
//    try {
        if (voiceIntent != NULL) {
            voiceIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
            mContext->StartActivity(voiceIntent);
        }
//    } catch (ActivityNotFoundException e) {
//        Logger::W(TAG, "No activity for search: " /*+ e*/);
//    } finally {
        if (needWakeLock) {
            mMediaEventWakeLock->ReleaseLock();
        }
//    }
}

ECode CAudioService::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    if (resultCode == WAKELOCK_RELEASE_ON_FINISHED) {
        mMediaEventWakeLock->ReleaseLock();
    }
    return NOERROR;
}

String CAudioService::DumpRCStack()
{
    StringBuilder sb("\nRemote Control stack entries:");

    // {
    //     AutoLock lock(mRCStackLock);

    //     Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
    //     while(stackIterator->HasNext()) {
    //         RemoteControlStackEntry rcse = stackIterator->Next();
    //         pw->Println("  pi: " + rcse.mMediaIntent +
    //         " -- pack: " + rcse.mCallingPackageName +
    //         "  -- ercvr: " + rcse.mReceiverComponent +
    //         "  -- client: " + rcse.mRcClient +
    //         "  -- uid: " + rcse.mCallingUid +
    //         "  -- type: " + rcse.mPlaybackType +
    //         "  state: " + rcse.mPlaybackState);
    //         }
    // }
    return sb.ToString();
}

String CAudioService::DumpRCCStack()
{
    StringBuilder sb("\nRemote Control Client stack entries:");
    // pw->PrintStringln(String("\nRemote Control Client stack entries:"));
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            pw->Println("  uid: " + rcse.mCallingUid +
//                    "  -- id: " + rcse.mRccId +
//                    "  -- type: " + rcse.mPlaybackType +
//                    "  -- state: " + rcse.mPlaybackState +
//                    "  -- vol handling: " + rcse.mPlaybackVolumeHandling +
//                    "  -- vol: " + rcse.mPlaybackVolume +
//                    "  -- volMax: " + rcse.mPlaybackVolumeMax +
//                    "  -- volObs: " + rcse.mRemoteVolumeObs);
//        }
//    }
//    {
//        AutoLock lock(mMainRemoteLock);
//
//        pw->Println("\nRemote Volume State:");
//        pw->Println("  has remote: " + mHasRemotePlayback);
//        pw->Println("  is remote active: " + mMainRemoteIsActive);
//        pw->Println("  rccId: " + mMainRemote.mRccId);
//        pw->Println("  volume handling: "
//                + ((mMainRemote.mVolumeHandling == RemoteControlClient.PLAYBACK_VOLUME_FIXED) ?
//                        "PLAYBACK_VOLUME_FIXED(0)" : "PLAYBACK_VOLUME_VARIABLE(1)"));
//        pw->Println("  volume: " + mMainRemote.mVolume);
//        pw->Println("  volume steps: " + mMainRemote.mVolumeMax);
//    }
    return sb.ToString();
}

void CAudioService::RemoveMediaButtonReceiverForPackage(
    /* [in] */ const String& packageName)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//
//        if (mRCStack.IsEmpty()) {
//            return;
//        } else {
//            RemoteControlStackEntry oldTop = mRCStack->Peek();
//            Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//            // iterate over the stack entries
//            while(stackIterator->HasNext()) {
//                RemoteControlStackEntry rcse = (RemoteControlStackEntry)stackIterator->Next();
//                if (packageName->EqualsIgnoreCase(rcse.mReceiverComponent->GetPackageName())) {
//                    // a stack entry is from the package being removed, remove it from the stack
//                    stackIterator->Remove();
//                    rcse->UnlinkToRcClientDeath();
//                }
//            }
//            if (mRCStack->Empty()) {
            // AutoPtr<IMessage> message;
            // mAudioHandler->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&message);
//                // no saved media button receiver
//                mAudioHandler->SendMessage(
//                        mAudioHandler->ObtainMessage(MSG_PERSIST_MEDIABUTTONRECEIVER, 0, 0,
//                                NULL));
//            } else if (oldTop != mRCStack->Peek()) {
//                // the top of the stack has changed, save it in the system settings
//                // by posting a message to persist it
//                mAudioHandler->SendMessage(
//                        mAudioHandler->ObtainMessage(MSG_PERSIST_MEDIABUTTONRECEIVER, 0, 0,
//                                mRCStack->Peek().mReceiverComponent));
//            }
//        }
//    }
}

void CAudioService::RestoreMediaButtonReceiver()
{
    String receiverName;
    Settings::System::GetStringForUser(mContentResolver,
        ISettingsSystem::MEDIA_BUTTON_RECEIVER,
        IUserHandle::USER_CURRENT, &receiverName);

   if (!receiverName.IsNullOrEmpty()) {
       AutoPtr<IComponentNameHelper> componentNameHelper;
       CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&componentNameHelper);
       AutoPtr<IComponentName> eventReceiver;
       componentNameHelper->UnflattenFromString(receiverName, (IComponentName**)&eventReceiver);
       // construct a PendingIntent targeted to the restored component name
       // for the media button and register it
       AutoPtr<IIntent> mediaButtonIntent;
       CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&mediaButtonIntent);

       //     the associated intent will be handled by the component being registered
       mediaButtonIntent->SetComponent(eventReceiver);
       AutoPtr<IPendingIntent> pi;
       AutoPtr<IPendingIntentHelper> pendingIntentHelper;
       CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
       pendingIntentHelper->GetBroadcast(mContext,
               0/*requestCode, ignored*/, mediaButtonIntent, 0/*flags*/, (IPendingIntent**)&pi);
       RegisterMediaButtonIntent(pi, eventReceiver);
   }
}

void CAudioService::PushMediaButtonReceiver(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* target)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    // already at top of stack?
//    if (!mRCStack.IsEmpty() && mRCStack->Peek().mMediaIntent->Equals(mediaIntent)) {
//        return;
//    }
//    Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//    RemoteControlStackEntry rcse = NULL;
//    Boolean wasInsideStack = FALSE;
//    while(stackIterator->HasNext()) {
//        rcse = (RemoteControlStackEntry)stackIterator->Next();
//        if(rcse.mMediaIntent->Equals(mediaIntent)) {
//             wasInsideStack = TRUE;
//             stackIterator->Remove();
//             break;
//        }
//    }
//    if (!wasInsideStack) {
//        rcse = new RemoteControlStackEntry(mediaIntent, target);
//    }
//    mRCStack->Push(rcse);

   // post message to persist the default media button receiver
    AutoPtr<IMessage> message;
    mAudioHandler->ObtainMessage(MSG_PERSIST_MEDIABUTTONRECEIVER, target, (IMessage**)&message);
    Boolean bval;
    mAudioHandler->SendMessage(message, &bval);
}

void CAudioService::RemoveMediaButtonReceiver(
    /* [in] */ IPendingIntent* pi)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//    while(stackIterator->HasNext()) {
//        RemoteControlStackEntry rcse = (RemoteControlStackEntry)stackIterator->Next();
//        if(rcse.mMediaIntent->Equals(pi)) {
//            stackIterator->Remove();
//            rcse->UnlinkToRcClientDeath();
//            break;
//        }
//    }
}

Boolean CAudioService::IsCurrentRcController(
    /* [in] */ IPendingIntent* pi)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    if (!mRCStack.IsEmpty() && mRCStack.peek().mMediaIntent.equals(pi)) {
//        return true;
//    }
    return FALSE;
}

void CAudioService::SetNewRcClientOnDisplays_syncRcsCurrc(
    /* [in] */ Int32 newClientGeneration,
    /* [in] */ IPendingIntent* newMediaIntent,
    /* [in] */ Boolean clearing)
{
    // NOTE: Only one IRemoteControlDisplay supported in this implementation
    if (mRcDisplay != NULL) {
        ECode ec = mRcDisplay->SetCurrentClientId(newClientGeneration, newMediaIntent, clearing);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // if we had a display before, stop monitoring its death
           RcDisplay_stopDeathMonitor_syncRcStack();
           mRcDisplay = NULL;
        }
    }
}

void CAudioService::SetNewRcClientGenerationOnClients_syncRcsCurrc(
    /* [in]*/ Int32 newClientGeneration)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//    while(stackIterator->HasNext()) {
//        RemoteControlStackEntry se = stackIterator->Next();
//        if ((se != NULL) && (se.mRcClient != NULL)) {
//            try {
//                se.mRcClient->SetCurrentClientGenerationId(newClientGeneration);
//            } catch (RemoteException e) {
//                Logger::W(TAG, "Dead client in setNewRcClientGenerationOnClients_syncRcsCurrc()"/*+e*/);
//                stackIterator->Remove();
//                se->UnlinkToRcClientDeath();
//            }
//        }
//    }
}

void CAudioService::SetNewRcClient_syncRcsCurrc(
    /* [in] */ Int32 newClientGeneration,
    /* [in] */ IPendingIntent* newMediaIntent,
    /* [in] */ Boolean clearing)
{
    // send the new valid client generation ID to all displays
    SetNewRcClientOnDisplays_syncRcsCurrc(newClientGeneration, newMediaIntent, clearing);
    // send the new valid client generation ID to all clients
    SetNewRcClientGenerationOnClients_syncRcsCurrc(newClientGeneration);
}

void CAudioService::OnRcDisplayClear()
{
    if (DEBUG_RC) {
        Logger::I(TAG, "Clear remote control display");
    }

    {
        AutoLock lock(mRCStackLock);
        {
            AutoLock lock(mCurrentRcLock);

            mCurrentRcClientGen++;
            // synchronously update the displays and clients with the new client generation
            SetNewRcClient_syncRcsCurrc(mCurrentRcClientGen,
                NULL /*newMediaIntent*/, TRUE /*clearing*/);
        }
    }
}

void CAudioService::OnRcDisplayUpdate(
    /* [in] */ RemoteControlStackEntry* rcse,
    /* [in] */ Int32 flags /* USED ?*/)
{
    {
        AutoLock lock(mRCStackLock);
        {
            AutoLock lock(mCurrentRcLock);

            if ((mCurrentRcClient != NULL) && (mCurrentRcClient == rcse->mRcClient)) {
                if (DEBUG_RC) {
                    Logger::I(TAG, "Display/update remote control ");
                }

                mCurrentRcClientGen++;
                // synchronously update the displays and clients with
                //      the new client generation
                SetNewRcClient_syncRcsCurrc(mCurrentRcClientGen,
                        rcse->mMediaIntent /*newMediaIntent*/,
                        FALSE /*clearing*/);

                // tell the current client that it needs to send info
//                try {
                    ECode ec = mCurrentRcClient->OnInformationRequested(mCurrentRcClientGen,
                        flags, mArtworkExpectedWidth, mArtworkExpectedHeight);
                    if (ec == (ECode)E_REMOTE_EXCEPTION) {
                        Logger::E(TAG, "Current valid remote client is dead: %08x", ec);
                        mCurrentRcClient = NULL;
                    }
//                } catch (RemoteException e) {
//                    Logger::E(TAG, "Current valid remote client is dead: "/*+e*/);
//                    mCurrentRcClient = NULL;
//                }
            } else {
                // the remote control display owner has changed between the
                // the message to update the display was sent, and the time it
                // gets to be processed (now)
            }
        }
    }
}

void CAudioService::ClearRemoteControlDisplay_syncAfRcs()
{
    {
        AutoLock lock(mCurrentRcLock);

        mCurrentRcClient = NULL;
    }

    // will cause onRcDisplayClear() to be called in AudioService's handler thread
    AutoPtr<IMessage> message;
    mAudioHandler->ObtainMessage(MSG_RCDISPLAY_CLEAR, (IMessage**)&message);
    Boolean bval;
    mAudioHandler->SendMessage(message, &bval);
}

void CAudioService::UpdateRemoteControlDisplay_syncAfRcs(
    /* [in] */ Int32 infoChangedFlags)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    AutoPtr<IRemoteControlStackEntry> rcse = mRCStack->Peek();
//    Int32 infoFlagsAboutToBeUsed = infoChangedFlags;
//    // this is where we enforce opt-in for information display on the remote controls
//    //   with the new IAudioManager::registerRemoteControlClient() API
//    if (rcse.mRcClient == NULL) {
//        //Logger::w(TAG, "Can't update remote control display with null remote control client");
//        ClearRemoteControlDisplay_syncAfRcs();
//        return;
//    }
//    {
//        AutoLock lock(mCurrentRcLock);
//
//        if (!rcse.mRcClient->Equals(mCurrentRcClient)) {
//           // new RC client, assume every type of information shall be queried
//           infoFlagsAboutToBeUsed = RC_INFO_ALL;
//        }
//        mCurrentRcClient = rcse::mRcClient;
//    }
    // // will cause onRcDisplayUpdate() to be called in AudioService's handler thread
    // AutoPtr<IMessage> message;
    // mAudioHandler->ObtainMessage(MSG_RCDISPLAY_UPDATE, infoFlagsAboutToBeUsed, 0, rcse, (IMessage**)&message);
    // Boolean bval;
    // mAudioHandler->SendMessage(message, &bval);
}

void CAudioService::CheckUpdateRemoteControlDisplay_syncAfRcs(
    /* [in] */ Int32 infoChangedFlags)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    // determine whether the remote control display should be refreshed
//    // if either stack is empty, there is a mismatch, so clear the RC display
//    if (mRCStack->IsEmpty() || mFocusStack->IsEmpty()) {
//        ClearRemoteControlDisplay_syncAfRcs();
//        return;
//    }
//
//    // determine which entry in the AudioFocus stack to consider, and compare against the
//    // top of the stack for the media button event receivers : simply using the top of the
//    // stack would make the entry disappear from the RemoteControlDisplay in conditions such as
//    // notifications playing during music playback.
//    // crawl the AudioFocus stack until an entry is found with the following characteristics:
//    // - focus gain on STREAM_MUSIC stream
//    // - non-transient focus gain on a stream other than music
//    FocusStackEntry af = NULL;
//    Iterator<FocusStackEntry> stackIterator = mFocusStack->Iterator();
//    while(stackIterator->HasNext()) {
//        FocusStackEntry fse = (FocusStackEntry)stackIterator->Next();
//        if ((fse::mStreamType == AudioManager_STREAM_MUSIC)
//                || (fse::mFocusChangeType == AudioManager_AUDIOFOCUS_GAIN)) {
//            af = fse;
//            break;
//        }
//    }
//    if (af == NULL) {
//        ClearRemoteControlDisplay_syncAfRcs();
//        return;
//    }
//
//    // if the audio focus and RC owners belong to different packages, there is a mismatch, clear
//    if ((mRCStack->Peek().mCallingPackageName != NULL)
//            && (af.mPackageName != NULL)
//            && !(mRCStack->Peek().mCallingPackageName->CompareTo(
//                    af.mPackageName) == 0)) {
//        ClearRemoteControlDisplay_syncAfRcs();
//        return;
//    }
//    // if the audio focus didn't originate from the same Uid as the one in which the remote
//    //   control information will be retrieved, clear
//    if (mRCStack->Peek().mCallingUid != af::mCallingUid) {
//        ClearRemoteControlDisplay_syncAfRcs();
//        return;
//    }
//    // refresh conditions were verified: update the remote controls
//    // ok to call: synchronized mAudioFocusLock then on mRCStack, mRCStack is not empty
//    UpdateRemoteControlDisplay_syncAfRcs(infoChangedFlags);
}

ECode CAudioService::RegisterMediaButtonIntent(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* eventReceiver)
{
    Logger::I(TAG, "  Remote Control   registerMediaButtonIntent() for " /*+ mediaIntent*/);

    AutoLock lock(mAudioFocusLock);
    {
        AutoLock lock(mRCStackLock);
        {
            PushMediaButtonReceiver(mediaIntent, eventReceiver);
            // new RC client, assume every type of information shall be queried
            CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
        }
    }
    return NOERROR;
}

ECode CAudioService::UnregisterMediaButtonIntent(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* eventReceiver)
{
    Logger::I(TAG, "  Remote Control   unregisterMediaButtonIntent() for " /*+ mediaIntent*/);

    AutoLock lock(mAudioFocusLock);
    {
        AutoLock lock(mRCStackLock);

        Boolean topOfStackWillChange = IsCurrentRcController(mediaIntent);
        RemoveMediaButtonReceiver(mediaIntent);
        if (topOfStackWillChange) {
            // current RC client will change, assume every type of info needs to be queried
            CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
        }
    }

    return NOERROR;
}

ECode CAudioService::RegisterMediaButtonEventReceiverForCalls(
    /* [in] */ IComponentName* c)
{
    Int32 tempValue;
    FAIL_RETURN(mContext->CheckCallingPermission(Elastos::Droid::Manifest::permission::MODIFY_PHONE_STATE, &tempValue));
    if (tempValue != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Invalid permissions to register media button receiver for calls");
        return NOERROR;
    }

    {
        AutoLock lock(mRCStackLock);
        mMediaReceiverForCalls = c;
    }

    return NOERROR;
}

ECode CAudioService::UnregisterMediaButtonEventReceiverForCalls()
{
    Int32 tempValue;
    FAIL_RETURN(mContext->CheckCallingPermission(Elastos::Droid::Manifest::permission::MODIFY_PHONE_STATE, &tempValue));
    if (tempValue != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Invalid permissions to unregister media button receiver for calls");
        return NOERROR;
    }

    {
        AutoLock lock(mRCStackLock);
        mMediaReceiverForCalls = NULL;
    }

    return NOERROR;
}

ECode CAudioService::RegisterRemoteControlClient(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IIRemoteControlClient* rcClient,
    /* [in] */ const String& callingPackageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG_RC) {
        Logger::I(TAG, "Register remote control client rcClient="/*+rcClient*/);
    }

    // Int32 rccId = IRemoteControlClient::RCSE_ID_UNREGISTERED;

/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mAudioFocusLock);
//        {
//            AutoLock lock(mRCStackLock);
//            // store the new display information
//            Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//            while(stackIterator->HasNext()) {
//                RemoteControlStackEntry rcse = stackIterator->Next();
//                if(rcse.mMediaIntent->Equals(mediaIntent)) {
//                    // already had a remote control client?
//                    if (rcse.mRcClientDeathHandler != NULL) {
//                        // stop monitoring the old client's death
//                        rcse->UnlinkToRcClientDeath();
//                    }
//                    // save the new remote control client
//                    rcse::mRcClient = rcClient;
//                    rcse::mCallingPackageName = callingPackageName;
//                    rcse::mCallingUid = Binder->GetCallingUid();
//                    if (rcClient == NULL) {
//                        // here rcse.mRcClientDeathHandler is null;
//                        rcse->ResetPlaybackInfo();
//                        break;
//                    }
//                    rccId = rcse::mRccId;
//
//                    // there is a new (non-null) client:
//                   // 1/ give the new client the current display (if any)
//                    if (mRcDisplay != NULL) {
//                        try {
//                            rcse.mRcClient.plugRemoteControlDisplay(mRcDisplay);
//                        } catch (RemoteException e) {
//                            Logger::e(TAG, "Error connecting remote control display to client: "+e);
//                            e.printStackTrace();
//                        }
//                    }
//                    // 2/ monitor the new client's death
//                    IBinder b = rcse.mRcClient->AsBinder();
//                    RcClientDeathHandler rcdh =
//                            new RcClientDeathHandler(b, rcse.mMediaIntent);
//                    AutoPtr<IRcClientDeathHandler> rcdh;
//                    CRcClientDeathHandler::New(b, rcse::mMediaIntent,(IRcClientDeathHandler**)&rcdh);
//                    try {
//                        b.linkToDeath(rcdh, 0);
//                    } catch (RemoteException e) {
//                        // remote control client is DOA, disqualify it
//                        Logger::w(TAG, "registerRemoteControlClient() has a dead client " + b);
//                        rcse::mRcClient = NULL;
//                    }
//                    rcse::mRcClientDeathHandler = rcdh;
//                    break;
//                }
//            }
//            // if the eventReceiver is at the top of the stack
//            // then check for potential refresh of the remote controls
//            if (IsCurrentRcController(mediaIntent)) {
//                CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
//            }
//        }
//    }
//    *result = rccId;

    return NOERROR;
}

ECode CAudioService::UnregisterRemoteControlClient(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IIRemoteControlClient* rcClient)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mAudioFocusLock);
//        {
//            AutoLock lock(mRCStackLock);
//
//            Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//            while(stackIterator->HasNext()) {
//                RemoteControlStackEntry rcse = stackIterator->Next();
//                if ((rcse.mMediaIntent->Equals(mediaIntent))
//                        && rcClient->Equals(rcse::mRcClient)) {
//                    // we found the IRemoteControlClient to unregister
//                    // stop monitoring its death
//                    rcse->UnlinkToRcClientDeath();
//                    // reset the client-related fields
//                    rcse::mRcClient = NULL;
//                    rcse::mCallingPackageName = NULL;
//                }
//            }
//        }
//    }

    return NOERROR;
}

void CAudioService::RcDisplay_stopDeathMonitor_syncRcStack()
{
    if (mRcDisplay != NULL) { // implies (mRcDisplayDeathHandler != null)
        // we had a display before, stop monitoring its death
        mRcDisplayDeathHandler->UnlinkToRcDisplayDeath();
    }
}

void CAudioService::RcDisplay_startDeathMonitor_syncRcStack()
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    if (mRcDisplay != NULL) {
//        // new non-null display, monitor its death
//        AutoPtr<IBinder> b = mRcDisplay->AsBinder();
//        mRcDisplayDeathHandler = new RcDisplayDeathHandler(b);
//        AutoPtr<IRcDisplayDeathHandler> mRcDisplayDeathHandler;
//        CRcDisplayDeathHandler::New(b,(IRcDisplayDeathHandler**)&mRcDisplayDeathHandler);
//
//        try {
//            b->LinkToDeath(mRcDisplayDeathHandler, 0);
//        } catch (RemoteException e) {
//            // remote control display is DOA, disqualify it
//            Logger::W(TAG, "registerRemoteControlDisplay() has a dead client " /*+ b*/);
//            mRcDisplay = NULL;
//        }
//    }
}

ECode CAudioService::RegisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    if (DEBUG_RC) {
        Logger::D(TAG, ">>> registerRemoteControlDisplay("/*+rcd+*/")");
    }
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mAudioFocusLock);
//        {
//            AutoLock lock(mRCStackLock);
//
//            if ((mRcDisplay == rcd) || (rcd == NULL)) {
//                return;
//            }
//            // if we had a display before, stop monitoring its death
//            RcDisplay_stopDeathMonitor_syncRcStack();
//            mRcDisplay = rcd;
//            // new display, start monitoring its death
//            RcDisplay_startDeathMonitor_syncRcStack();
//
//            // let all the remote control clients there is a new display
//            // no need to unplug the previous because we only support one display
//            // and the clients don't track the death of the display
//            Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//            while(stackIterator->HasNext()) {
//                RemoteControlStackEntry rcse = stackIterator->Next();
//                if(rcse.mRcClient != NULL) {
//                    try {
//                        rcse.mRcClient->PlugRemoteControlDisplay(mRcDisplay);
//                    } catch (RemoteException e) {
//                        Logger::E(TAG, "Error connecting remote control display to client: " + e);
//                        e.printStackTrace();
//                    }
//                }
//            }
//
//            // we have a new display, of which all the clients are now aware: have it be updated
//            CheckUpdateRemoteControlDisplay_syncAfRcs(RC_INFO_ALL);
//        }
//    }

    return NOERROR;
}

ECode CAudioService::UnregisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    if (DEBUG_RC) {
        Logger::D(TAG, "<<< unregisterRemoteControlDisplay("/*+rcd+*/")");
    }
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//        // only one display here, so you can only unregister the current display
//        if ((rcd == NULL) || (rcd != mRcDisplay)) {
//            if (DEBUG_RC) {
//                Logger::W(TAG, "    trying to unregister unregistered RCD");
//            }
//            return NOERROR;
//        }
//        // if we had a display before, stop monitoring its death
//        rcDisplay_stopDeathMonitor_syncRcStack();
//        mRcDisplay = NULL;
//
//        // disconnect this remote control display from all the clients
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            if(rcse.mRcClient != NULL) {
//                try {
//                    rcse.mRcClient->UnplugRemoteControlDisplay(rcd);
//                } catch (RemoteException e) {
//                    Logger::E(TAG, "Error disconnecting remote control display to client: " /*+ e*/);
//                    e.printStackTrace();
//                }
//            }
//        }
//    }

    return NOERROR;
}

ECode CAudioService::RemoteControlDisplayUsesBitmapSize(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    AutoLock lock(mRCStackLock);
    // NOTE: Only one IRemoteControlDisplay supported in this implementation
    mArtworkExpectedWidth = w;
    mArtworkExpectedHeight = h;

    return NOERROR;
}

ECode CAudioService::SetPlaybackInfoForRcc(
     /* [in] */ Int32 rccId,
     /* [in] */ Int32 what,
     /* [in] */ Int32 value)
{
    AutoPtr<IInteger32> ival;
    CInteger32::New(value, (IInteger32**)&ival);
    SendMsg(MSG_RCC_NEW_PLAYBACK_INFO, SENDMSG_QUEUE,
        rccId /* arg1 */, what /* arg2 */, (IInterface*)ival /* obj */, 0 /* delay */);

    return NOERROR;
}

void CAudioService::OnNewPlaybackInfoForRcc(
    /* [in] */ Int32 rccId,
    /* [in] */ Int32 key,
    /* [in] */ Int32 value)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    if(DEBUG_RC) {
//        Logger::D(TAG, String("onNewPlaybackInfoForRcc(id=%d") + rccId +
//                String(", what=%d") + key + String(",val=%d") +
//                value + String(")"));
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            if (rcse::mRccId == rccId) {
//                switch (key) {
//                    case RemoteControlClient_PLAYBACKINFO_PLAYBACK_TYPE:
//                        rcse::mPlaybackType = value;
//                        PostReevaluateRemote();
//                        break;
//                    case RemoteControlClient_PLAYBACKINFO_VOLUME:
//                        rcse::mPlaybackVolume = value;
//                        {
//                            AutoLock lock(&mMainRemote);
//                            if (rccId == mMainRemote::mRccId) {
//                                mMainRemote::mVolume = value;
//                                mVolumePanel->PostHasNewRemotePlaybackInfo();
//                            }
//                        }
//                        break;
//                    case RemoteControlClient_PLAYBACKINFO_VOLUME_MAX:
//                        rcse::mPlaybackVolumeMax = value;
//                        {
//                            AutoLock lock(&mMainRemote);
//                            if (rccId == mMainRemote::mRccId) {
//                                mMainRemote.mVolumeMax = value;
//                                mVolumePanel->PostHasNewRemotePlaybackInfo();
//                            }
//                        }
//                        break;
//                    case RemoteControlClient_PLAYBACKINFO_VOLUME_HANDLING:
//                        rcse.mPlaybackVolumeHandling = value;
//                        {
//                            AutoLock lock(&mMainRemote);
//                            if (rccId == mMainRemote::mRccId) {
//                                mMainRemote::mVolumeHandling = value;
//                                mVolumePanel->PostHasNewRemotePlaybackInfo();
//                            }
//                       }
//                        break;
//                    case RemoteControlClient_PLAYBACKINFO_USES_STREAM:
//                        rcse::mPlaybackStream = value;
//                        break;
//                    case RemoteControlClient_PLAYBACKINFO_PLAYSTATE:
//                        rcse::mPlaybackState = value;
//                        {
//                            AutoLock lock(&mMainRemote);
//                            if (rccId == mMainRemote::mRccId) {
//                                mMainRemoteIsActive = IsPlaystateActive(value);
//                                PostReevaluateRemote();
//                            }
//                        }
//                        break;
//                    default:
//                        Logger::E(TAG, "unhandled key " + key + " for RCC " + rccId);
//                        break;
//                }
//                return;
//            }
//        }
//    }
}

ECode CAudioService::RegisterRemoteVolumeObserverForRcc(
    /* [in] */ Int32 rccId,
    /* [in] */ IIRemoteVolumeObserver* rvo)
{
    SendMsg(MSG_RCC_NEW_VOLUME_OBS, SENDMSG_QUEUE,
        rccId /* arg1 */, 0, rvo /* obj */, 0 /* delay */);

    return NOERROR;
}

void CAudioService::OnRegisterVolumeObserverForRcc(
    /* [in] */ Int32 rccId,
    /* [in] */ IIRemoteVolumeObserver* rvo)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            if (rcse::mRccId == rccId) {
//                rcse::mRemoteVolumeObs = rvo;
//                break;
//            }
//        }
//    }
}

Boolean CAudioService::CheckUpdateRemoteStateIfActive(
    /* [in] */ Int32 streamType)
{
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            if ((rcse.mPlaybackType == RemoteControlClient.PLAYBACK_TYPE_REMOTE)
//                    && IsPlaystateActive(rcse.mPlaybackState)
//                    && (rcse.mPlaybackStream == streamType)) {
//                if (DEBUG_RC) {
//                    Logger::D(TAG, "remote playback active on stream " + streamType
//                            + ", vol =" + rcse.mPlaybackVolume);
//                {
//                    AutoLock lock(mMainRemoteLock);
//                    mMainRemote.mRccId = rcse.mRccId;
//                    mMainRemote.mVolume = rcse.mPlaybackVolume;
//                    mMainRemote.mVolumeMax = rcse.mPlaybackVolumeMax;
//                    mMainRemote.mVolumeHandling = rcse.mPlaybackVolumeHandling;
//                    mMainRemoteIsActive = TRUE;
//                }
//                return TRUE;
//            }
//        }
//    }
//    {
//        AutoLock lock(mMainRemoteLock);
//        mMainRemoteIsActive = FALSE;
//    }
    return FALSE;
}

/*static*/
Boolean CAudioService::IsPlaystateActive(
    /* [in] */ Int32 playState)
{
    switch (playState) {
        case IRemoteControlClient::PLAYSTATE_PLAYING:
        case IRemoteControlClient::PLAYSTATE_BUFFERING:
        case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
        case IRemoteControlClient::PLAYSTATE_REWINDING:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
            return TRUE;

        default:
            return FALSE;
    }
}

void CAudioService::AdjustRemoteVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    if (DEBUG_VOL){
        Logger::D(TAG, "adjustRemoteVolume() stream=%d, dir=%d, flags=%d",
            streamType, direction, flags);
    }

    Int32 rccId = IRemoteControlClient::RCSE_ID_UNREGISTERED;
    Boolean volFixed = FALSE;
    {
        AutoLock lock(mMainRemoteLock);

        if (!mMainRemoteIsActive) {
            if (DEBUG_VOL) {
                Logger::W(TAG, "adjustRemoteVolume didn't find an active client");
            }
            return;
        }
        rccId = mMainRemote->mRccId;
        volFixed = (mMainRemote->mVolumeHandling ==
                IRemoteControlClient::PLAYBACK_VOLUME_FIXED);
    }
    // unlike "local" stream volumes, we can't compute the new volume based on the direction,
    // we can only notify the remote that volume needs to be updated, and we'll get an async'
    // update through setPlaybackInfoForRcc()
    if (!volFixed) {
        SendVolumeUpdateToRemote(rccId, direction);
    }

    // fire up the UI
    if (mVolumePanel != NULL) {
        mVolumePanel->PostRemoteVolumeChanged(streamType, flags);
    }
}

void CAudioService::SendVolumeUpdateToRemote(
    /* [in] */ Int32 rccId,
    /* [in] */ Int32 direction)
{
    if (DEBUG_VOL) {
        Logger::D(TAG, "sendVolumeUpdateToRemote(rccId=%d, dir=%d", rccId, direction);
    }
    if (direction == 0) {
        // only handling discrete events
        return;
    }
/*Eddie(E_NOT_IMPLEMENTED)*/
//    AutoPtr<IIRemoteVolumeObserver> rvo = NULL;
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            //FIXME OPTIMIZE store this info in mMainRemote so we don't have to iterate?
//            if (rcse::mRccId == rccId) {
//                rvo = rcse::mRemoteVolumeObs;
//                break;
//            }
//        }
//    }
//    if (rvo != NULL) {
//        try {
//            rvo->DispatchRemoteVolumeUpdate(direction, -1);
//        } catch (RemoteException e) {
//            Logger::E(TAG, "Error dispatching relative volume update"/*, e*/);
//        }
//    }
}

ECode CAudioService::GetRemoteStreamMaxVolume(
    /* [out] */ Int32* result)
{
    AutoLock lock(mMainRemoteLock);

    if (mMainRemote->mRccId == IRemoteControlClient::RCSE_ID_UNREGISTERED) {
        *result = 0;
        return NOERROR;
    }
    *result = mMainRemote->mVolumeMax;
    return NOERROR;
}

ECode CAudioService::GetRemoteStreamVolume(
    /* [out]*/ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mMainRemoteLock);

    if (mMainRemote->mRccId == IRemoteControlClient::RCSE_ID_UNREGISTERED) {
        *result = 0;
        return NOERROR;
    }
    *result = mMainRemote->mVolume;
    return NOERROR;
}

ECode CAudioService::SetRemoteStreamVolume(
    /* [in] */ Int32 vol)
{
    if (DEBUG_VOL) {
        Logger::D(TAG, "setRemoteStreamVolume(vol=%d)", vol);
    }

    Int32 rccId = IRemoteControlClient::RCSE_ID_UNREGISTERED;

    {
        AutoLock lock(mMainRemoteLock);

        if (mMainRemote->mRccId == IRemoteControlClient::RCSE_ID_UNREGISTERED) {
            return NOERROR;
        }
        rccId = mMainRemote->mRccId;
    }

    AutoPtr<IIRemoteVolumeObserver> rvo = NULL;

/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            if (rcse::mRccId == rccId) {
//                //FIXME OPTIMIZE store this info in mMainRemote so we don't have to iterate?
//                rvo = rcse::mRemoteVolumeObs;
//                break;
//            }
//        }
//    }
//
//    if (rvo != NULL) {
//        try {
//            rvo->DispatchRemoteVolumeUpdate(0, vol);
//        } catch (RemoteException e) {
//            Logger::E(TAG, "Error dispatching absolute volume update"/*, e*/);
//        }
//    }

    return NOERROR;
}

void CAudioService::PostReevaluateRemote()
{
    SendMsg(MSG_REEVALUATE_REMOTE, SENDMSG_QUEUE, 0, 0, NULL, 0);
}

void CAudioService::OnReevaluateRemote()
{
    if (DEBUG_VOL) {
        Logger::W(TAG, "onReevaluateRemote()");
    }
    // is there a registered RemoteControlClient that is handling remote playback
    Boolean hasRemotePlayback = FALSE;
/*Eddie(E_NOT_IMPLEMENTED)*/
//    {
//        AutoLock lock(mRCStackLock);
//
//        Iterator<RemoteControlStackEntry> stackIterator = mRCStack->Iterator();
//        while(stackIterator->HasNext()) {
//            RemoteControlStackEntry rcse = stackIterator->Next();
//            if (rcse::mPlaybackType == RemoteControlClient_PLAYBACK_TYPE_REMOTE) {
//                hasRemotePlayback = TRUE;
//                break;
//            }
//        }
//    }
   {
       AutoLock lock(mMainRemoteLock);

       if (mHasRemotePlayback != hasRemotePlayback) {
            mHasRemotePlayback = hasRemotePlayback;
            if (mVolumePanel != NULL) {
                mVolumePanel->PostRemoteSliderVisibility(hasRemotePlayback);
            }
       }
   }
}

//==========================================================================================
// Device orientation
//==========================================================================================
void CAudioService::HandleBootCompleted(
    /* [in] */ IContext* context)
{
    if (!mBootCompleted) {
        mBootCompleted = TRUE;
        SendMsg(MSG_LOAD_SOUND_EFFECTS, SENDMSG_NOOP, 0, 0, NULL, 0);

        AutoPtr<IInterface> object;
        mContext->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&object);
        mKeyguardManager = IKeyguardManager::Probe(object.Get());

        mScoConnectionState = IAudioManager::SCO_AUDIO_STATE_ERROR;
        ResetBluetoothSco();
        GetBluetoothHeadset();
        //FIXME: this is to maintain compatibility with deprecated intent
        // IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED. Remove when appropriate.
        AutoPtr<IIntent> newIntent;
        CIntent::New(IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED,(IIntent**)&newIntent);

        newIntent->PutExtra(IAudioManager::EXTRA_SCO_AUDIO_STATE,
            IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
        SendStickyBroadcastToAll(newIntent);

        AutoPtr<IBluetoothAdapter> adapter;
        AutoPtr<IBluetoothAdapterHelper> helper;
        CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
        helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);

        if (adapter != NULL) {
            Boolean boolean;
            adapter->GetProfileProxy(mContext,
                mBluetoothProfileServiceListener,
                IBluetoothProfile::A2DP, &boolean);
        }

        SendMsg(MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED,
            SENDMSG_REPLACE, 0, 0, NULL, SAFE_VOLUME_CONFIGURE_TIMEOUT_MS);
    }
}

void CAudioService::HandleConfigurationChanged(
    /* [in] */ IContext* context)
{
//    try {
        // reading new orientation "safely" (i.e. under try catch) in case anything
        // goes wrong when obtaining resources and configuration
        AutoPtr<IConfiguration> config;
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetConfiguration((IConfiguration**)&config);

        if (mMonitorOrientation) {
            Int32 newOrientation;
            config->GetOrientation(&newOrientation);
            if (newOrientation != mDeviceOrientation) {
                mDeviceOrientation = newOrientation;
                SetOrientationForAudioSystem();
            }
        }
        SendMsg(MSG_CONFIGURE_SAFE_MEDIA_VOLUME,
            SENDMSG_REPLACE,
            0,
            0,
            NULL,
            0);

        Boolean cameraSoundForced;
        resources = NULL;
        mContext->GetResources((IResources**)&resources);
        resources->GetBoolean(R::bool_::config_camera_sound_forced,
                &cameraSoundForced);
        {
            AutoLock lock(mSettingsLock);
            {
                AutoLock lock(mCameraSoundForcedLock);

                if (cameraSoundForced != mCameraSoundForced) {
                    mCameraSoundForced = cameraSoundForced;

                    AutoPtr<VolumeStreamState> s = (*mStreamStates)[IAudioSystem::STREAM_SYSTEM_ENFORCED];
                    if (cameraSoundForced) {
                        s->SetAllIndexesToMax();
                        mRingerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
                    }
                    else {
                        s->SetAllIndexes((*mStreamStates)[IAudioSystem::STREAM_SYSTEM],
                            FALSE /*lastAudible*/);
                        s->SetAllIndexes((*mStreamStates)[IAudioSystem::STREAM_SYSTEM],
                            TRUE /*lastAudible*/);
                        mRingerModeAffectedStreams |= (1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
                    }

                    // take new state into account for streams muted by ringer mode
                    Int32 tempValue;
                    GetRingerMode(&tempValue);
                    SetRingerModeInt(tempValue, FALSE);

                    SendMsg(MSG_SET_FORCE_USE,
                        SENDMSG_QUEUE,
                        IAudioSystem::FOR_SYSTEM,
                        cameraSoundForced ?
                            IAudioSystem::FORCE_SYSTEM_ENFORCED : IAudioSystem::FORCE_NONE,
                        NULL,
                        0);

                    SendMsg(MSG_SET_ALL_VOLUMES,
                        SENDMSG_QUEUE,
                        0,
                        0,
                        (IInterface*)((*mStreamStates)[IAudioSystem::STREAM_SYSTEM_ENFORCED]->Probe(EIID_IInterface)), 0);
                }
            }
        }
//    } catch (Exception e) {
//        Logger::E(TAG, "Error retrieving device orientation: " /*+ e*/);
//    }
}

void CAudioService::SetOrientationForAudioSystem()
{
    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);

    switch (mDeviceOrientation) {
        case IConfiguration::ORIENTATION_LANDSCAPE: {
            //Logger::I(TAG, "orientation is landscape");
            audioSystemHelper->SetParameters(String("orientation=landscape"));
            break;
        }
        case IConfiguration::ORIENTATION_PORTRAIT: {
            //Logger::I(TAG, "orientation is portrait");
            audioSystemHelper->SetParameters(String("orientation=portrait"));
            break;
        }
        case IConfiguration::ORIENTATION_SQUARE: {
            //Logger::I(TAG, "orientation is square");
            audioSystemHelper->SetParameters(String("orientation=square"));
            break;
        }
        case IConfiguration::ORIENTATION_UNDEFINED: {
            //Logger::I(TAG, "orientation is undefined");
            audioSystemHelper->SetParameters(String("orientation=undefined"));
            break;
        }
        default:
        {
            Logger::E(TAG, "Unknown orientation");
        }
    }
}

ECode CAudioService::SetBluetoothA2dpOnInt(
    /* [in] */ Boolean on)
{
    AutoLock lock(mBluetoothA2dpEnabledLock);

    mBluetoothA2dpEnabled = on;
    mAudioHandler->RemoveMessages(MSG_SET_FORCE_BT_A2DP_USE);

    AutoPtr<IAudioSystemHelper> audioSystemHelper;
    CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
    audioSystemHelper->SetForceUse(IAudioSystem::FOR_MEDIA,
        mBluetoothA2dpEnabled ?
        IAudioSystem::FORCE_NONE : IAudioSystem::FORCE_NO_BT_A2DP);

    return NOERROR;
}

ECode CAudioService::SetRingtonePlayer(
    /* [in] */ IIRingtonePlayer* player)
{
    String text;
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::REMOTE_AUDIO_PLAYBACK, text));

    mRingtonePlayer = player;

    return NOERROR;
}

ECode CAudioService::GetRingtonePlayer(
    /* [out] */ IIRingtonePlayer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRingtonePlayer;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAudioService::StartWatchingRoutes(
    /* [in] */ IIAudioRoutesObserver* observer,
    /* [out] */ IAudioRoutesInfo** result)
{
    VALIDATE_NOT_NULL(result);

    {
        AutoLock lock(mCurAudioRoutesLock);

        AutoPtr<IAudioRoutesInfo> routes ;
        CAudioRoutesInfo::New(mCurAudioRoutes,(IAudioRoutesInfo**)&routes);
        Boolean tempState;
        mRoutesObservers->Register(observer, &tempState);
        *result = routes;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

//==========================================================================================
// Safe media volume management.
// MUSIC stream volume level is limited when headphones are connected according to safety
// regulation. When the user attempts to raise the volume above the limit, a warning is
// displayed and the user has to acknowlegde before the volume is actually changed.
// The volume index corresponding to the limit is stored in config_safe_media_volume_index
// property. Platforms with a different limit must set this property accordingly in their
// overlay.
//==========================================================================================

void CAudioService::SetSafeMediaVolumeEnabled(
    /* [in] */ Boolean on)
{
    AutoLock lock(mSafeMediaVolumeStateLock);

    if ((mSafeMediaVolumeState != SAFE_MEDIA_VOLUME_NOT_CONFIGURED) &&
            (mSafeMediaVolumeState != SAFE_MEDIA_VOLUME_DISABLED)) {
        if (on && (mSafeMediaVolumeState == SAFE_MEDIA_VOLUME_INACTIVE)) {
            mSafeMediaVolumeState = SAFE_MEDIA_VOLUME_ACTIVE;
            EnforceSafeMediaVolume();
        } else if (!on && (mSafeMediaVolumeState == SAFE_MEDIA_VOLUME_ACTIVE)) {
            mSafeMediaVolumeState = SAFE_MEDIA_VOLUME_INACTIVE;
            mMusicActiveMs = 0;
            SendMsg(MSG_CHECK_MUSIC_ACTIVE,
                SENDMSG_REPLACE,
                0,
                0,
                NULL,
                MUSIC_ACTIVE_POLL_PERIOD_MS);
        }
    }
}

void CAudioService::EnforceSafeMediaVolume()
{
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[IAudioSystem::STREAM_MUSIC];
    Boolean lastAudible = (streamState->MuteCount() != 0);
    Int32 devices = mSafeMediaVolumeDevices;
    Int32 i = 0;

    while (devices != 0) {
        Int32 device = 1 << i++;
        if ((device & devices) == 0) {
            continue;
        }
        Int32 index;
        streamState->GetIndex(device, lastAudible, &index);
        if (index > mSafeMediaVolumeIndex) {
            if (lastAudible) {
                streamState->SetLastAudibleIndex(mSafeMediaVolumeIndex, device);
                SendMsg(MSG_PERSIST_VOLUME,
                    SENDMSG_QUEUE,
                    PERSIST_LAST_AUDIBLE,
                    device,
                    (IInterface*)(streamState->Probe(EIID_IInterface)),
                    PERSIST_DELAY);
                }
                else {
                    Boolean tempState;
                    streamState->SetIndex(mSafeMediaVolumeIndex, device, TRUE, &tempState);
                    SendMsg(MSG_SET_DEVICE_VOLUME,
                        SENDMSG_QUEUE,
                        device,
                        0,
                        (IInterface*)(streamState->Probe(EIID_IInterface)),
                        0);
                }
        }
        devices &= ~device;
    }
}

Boolean CAudioService::CheckSafeMediaVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 device)
{
    AutoLock lock(mSafeMediaVolumeStateLock);

    if ((mSafeMediaVolumeState == SAFE_MEDIA_VOLUME_ACTIVE) &&
        ((*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC) &&
        ((device & mSafeMediaVolumeDevices) != 0) &&
        (index > mSafeMediaVolumeIndex)) {
        if (mVolumePanel != NULL) {
            mVolumePanel->PostDisplaySafeVolumeWarning();
        }
        return FALSE;
    }
    return TRUE;
}

ECode CAudioService::DisableSafeMediaVolume()
{
    AutoLock lock(mSafeMediaVolumeStateLock);
    SetSafeMediaVolumeEnabled(FALSE);
    return NOERROR;
}

//==========================================================================================
// Camera shutter sound policy.
// config_camera_sound_forced configuration option in config.xml defines if the camera shutter
// sound is forced (sound even if the device is in silent mode) or not. This option is false by
// default and can be overridden by country specific overlay in values-mccXXX/config.xml.
//==========================================================================================

ECode CAudioService::IsCameraSoundForced(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mCameraSoundForcedLock);
    *result = mCameraSoundForced;
    return NOERROR;
}

String CAudioService::DumpRingerMode()
{
    StringBuilder sb("\nRinger mode: ");
    sb += "- mode: ";
    sb += RINGER_MODE_NAMES[mRingerMode];
    sb += "- ringer mode affected streams = 0x";
    sb += StringUtils::Int32ToHexString(mRingerModeAffectedStreams);
    sb += StringUtils::Int32ToHexString(mRingerModeAffectedStreams);
    sb += "- ringer mode muted streams = 0x";
    sb += mRingerModeMutedStreams;
    sb += StringUtils::Int32ToHexString(mRingerModeMutedStreams);
    return sb.ToString();
}

ECode CAudioService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG));

    StringBuilder sb;
    sb += DumpFocusStack();
    sb += DumpRCStack();
    sb += DumpRCCStack();
    sb += DumpStreamStates();
    sb += DumpRingerMode();
    sb += "\nAudio routes:";
    sb += "\n  mMainType=0x";

    Int32 type;
    mCurAudioRoutes->GetMainType(&type);
    sb += StringUtils::Int32ToHexString(type);

    AutoPtr<ICharSequence> seq;
    mCurAudioRoutes->GetBluetoothName((ICharSequence**)&seq);
    String text;
    seq->ToString(&text);
    sb += "\n  mBluetoothName=";
    sb += text;
    *result = sb.ToString();
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
