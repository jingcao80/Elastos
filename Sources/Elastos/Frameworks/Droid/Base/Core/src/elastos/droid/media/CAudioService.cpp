#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/media/CAudioFormat.h"
#include "elastos/droid/media/CAudioManager.h"
#include "elastos/droid/media/CAudioRoutesInfo.h"
#include "elastos/droid/media/CAudioService.h"
#include "elastos/droid/media/CMediaPlayer.h"
#include "elastos/droid/media/CSoundPool.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CRemoteCallbackList.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Bluetooth::CBluetoothAdapter;
using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothClassDevice;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Hardware::Hdmi::EIID_IHdmiPlaybackClientDisplayStatusCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiClient;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioRoutesInfo;
using Elastos::Droid::Media::CMediaPlayer;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityManagerTouchExplorationStateChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::Manifest;
using Elastos::Droid::R;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CAudioService::VolumeStreamState::VolumeDeathHandler, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CAudioService::ScoClient, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CAudioService::SetModeDeathHandler, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CAudioService::VolumeStreamState, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

const String CAudioService::TAG("CAudioService");
const String CAudioService::SOUND_EFFECTS_PATH("/media/audio/ui/");
const String CAudioService::TAG_AUDIO_ASSETS("audio_assets");
const String CAudioService::ATTR_VERSION("version");
const String CAudioService::TAG_GROUP("group");
const String CAudioService::ATTR_GROUP_NAME("name");
const String CAudioService::TAG_ASSET("asset");
const String CAudioService::ATTR_ASSET_ID("id");
const String CAudioService::ATTR_ASSET_FILE("file");
const String CAudioService::ASSET_FILE_VERSION("1.0");
const String CAudioService::GROUP_TOUCH_SOUNDS("touch_sounds");

const Boolean CAudioService::DEBUG_MODE = FALSE; //Log.isLoggable(TAG + ".MOD", Log.DEBUG);
const Boolean CAudioService::DEBUG_VOL = FALSE; //Log.isLoggable(TAG + ".VOL", Log.DEBUG);
const Boolean CAudioService::DEBUG_SESSIONS = FALSE; //Log.isLoggable(TAG + ".SESSIONS", Log.DEBUG);
const Boolean CAudioService::VOLUME_SETS_RINGER_MODE_SILENT = FALSE;
const Boolean CAudioService::PREVENT_VOLUME_ADJUSTMENT_IF_SILENT = TRUE;

const Int32 CAudioService::PERSIST_DELAY = 500;
const Int32 CAudioService::FLAG_ADJUST_VOLUME = 1;
const Int32 CAudioService::PLATFORM_DEFAULT = 0;
const Int32 CAudioService::PLATFORM_VOICE = 1;
const Int32 CAudioService::PLATFORM_TELEVISION = 2;
const Int32 CAudioService::SENDMSG_REPLACE = 0;
const Int32 CAudioService::SENDMSG_NOOP = 1;
const Int32 CAudioService::SENDMSG_QUEUE = 2;
const Int32 CAudioService::MSG_SET_DEVICE_VOLUME = 0;
const Int32 CAudioService::MSG_PERSIST_VOLUME = 1;
const Int32 CAudioService::MSG_PERSIST_MASTER_VOLUME = 2;
const Int32 CAudioService::MSG_PERSIST_RINGER_MODE = 3;
const Int32 CAudioService::MSG_MEDIA_SERVER_DIED = 4;
const Int32 CAudioService::MSG_PLAY_SOUND_EFFECT = 5;
const Int32 CAudioService::MSG_BTA2DP_DOCK_TIMEOUT = 6;
const Int32 CAudioService::MSG_LOAD_SOUND_EFFECTS = 7;
const Int32 CAudioService::MSG_SET_FORCE_USE = 8;
const Int32 CAudioService::MSG_BT_HEADSET_CNCT_FAILED = 9;
const Int32 CAudioService::MSG_SET_ALL_VOLUMES = 10;
const Int32 CAudioService::MSG_PERSIST_MASTER_VOLUME_MUTE = 11;
const Int32 CAudioService::MSG_REPORT_NEW_ROUTES = 12;
const Int32 CAudioService::MSG_SET_FORCE_BT_A2DP_USE = 13;
const Int32 CAudioService::MSG_CHECK_MUSIC_ACTIVE = 14;
const Int32 CAudioService::MSG_BROADCAST_AUDIO_BECOMING_NOISY = 15;
const Int32 CAudioService::MSG_CONFIGURE_SAFE_MEDIA_VOLUME = 16;
const Int32 CAudioService::MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED = 17;
const Int32 CAudioService::MSG_PERSIST_SAFE_VOLUME_STATE = 18;
const Int32 CAudioService::MSG_BROADCAST_BT_CONNECTION_STATE = 19;
const Int32 CAudioService::MSG_UNLOAD_SOUND_EFFECTS = 20;
const Int32 CAudioService::MSG_SYSTEM_READY = 21;
const Int32 CAudioService::MSG_PERSIST_MUSIC_ACTIVE_MS = 22;
const Int32 CAudioService::MSG_PERSIST_MICROPHONE_MUTE = 23;
const Int32 CAudioService::MSG_SET_WIRED_DEVICE_CONNECTION_STATE = 100;
const Int32 CAudioService::MSG_SET_A2DP_SRC_CONNECTION_STATE = 101;
const Int32 CAudioService::MSG_SET_A2DP_SINK_CONNECTION_STATE = 102;
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
const Int32 CAudioService::SCO_MODE_UNDEFINED = -1;
const Int32 CAudioService::SCO_MODE_VIRTUAL_CALL = 0;
const Int32 CAudioService::SCO_MODE_RAW = 1;
const Int32 CAudioService::SCO_MODE_VR = 2;
const Int32 CAudioService::SCO_MODE_MAX = 2;
const Int32 CAudioService::SOUND_EFFECTS_LOAD_TIMEOUT_MS = 5000;
const Int32 CAudioService::SAFE_MEDIA_VOLUME_NOT_CONFIGURED = 0;
const Int32 CAudioService::SAFE_MEDIA_VOLUME_DISABLED = 1;
const Int32 CAudioService::SAFE_MEDIA_VOLUME_INACTIVE = 2;  // confirmed
const Int32 CAudioService::SAFE_MEDIA_VOLUME_ACTIVE = 3;  // unconfirmed
const Int32 CAudioService::UNSAFE_VOLUME_MUSIC_ACTIVE_MS_MAX = (20 * 3600 * 1000); // 20 hours
const Int32 CAudioService::MUSIC_ACTIVE_POLL_PERIOD_MS = 60000;  // 1 minute polling interval
const Int32 CAudioService::SAFE_VOLUME_CONFIGURE_TIMEOUT_MS = 30000;  // 30s after boot completed
Int32 CAudioService::sSoundEffectVolumeDb;

AutoPtr<IList> CAudioService::mMediaPlayers;
AutoPtr<IList> CAudioService::SOUND_EFFECT_FILES;
AutoPtr<ArrayOf<Int32> > CAudioService::MAX_STREAM_VOLUME;
AutoPtr<ArrayOf<Int32> > CAudioService::STEAM_VOLUME_OPS;
AutoPtr<ArrayOf<String> > CAudioService::STREAM_NAMES;
AutoPtr<ArrayOf<String> > CAudioService::RINGER_MODE_NAMES;

Boolean CAudioService::InitStatic()
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    SOUND_EFFECT_FILES = IList::Probe(list);

    MAX_STREAM_VOLUME = ArrayOf<Int32>::Alloc(10);
    MAX_STREAM_VOLUME->Set(0, 5);   // STREAM_VOICE_CALL
    MAX_STREAM_VOLUME->Set(1, 7);   // STREAM_SYSTEM
    MAX_STREAM_VOLUME->Set(2, 7);   // STREAM_RING
    MAX_STREAM_VOLUME->Set(3, 15);  // STREAM_MUSIC
    MAX_STREAM_VOLUME->Set(4, 7);   // STREAM_ALARM
    MAX_STREAM_VOLUME->Set(5, 7);   // STREAM_NOTIFICATION
    MAX_STREAM_VOLUME->Set(6, 15);  // STREAM_BLUETOOTH_SCO
    MAX_STREAM_VOLUME->Set(7, 7);   // STREAM_SYSTEM_ENFORCED
    MAX_STREAM_VOLUME->Set(8, 15);  // STREAM_DTMF
    MAX_STREAM_VOLUME->Set(9, 15);  // STREAM_TTS

    STEAM_VOLUME_OPS = ArrayOf<Int32>::Alloc(10);
    STEAM_VOLUME_OPS->Set(0, IAppOpsManager::OP_AUDIO_VOICE_VOLUME);            // STREAM_VOICE_CALL
    STEAM_VOLUME_OPS->Set(1, IAppOpsManager::OP_AUDIO_MEDIA_VOLUME);            // STREAM_SYSTEM
    STEAM_VOLUME_OPS->Set(2, IAppOpsManager::OP_AUDIO_RING_VOLUME);             // STREAM_RING
    STEAM_VOLUME_OPS->Set(3, IAppOpsManager::OP_AUDIO_MEDIA_VOLUME);            // STREAM_MUSIC
    STEAM_VOLUME_OPS->Set(4, IAppOpsManager::OP_AUDIO_ALARM_VOLUME);            // STREAM_ALARM
    STEAM_VOLUME_OPS->Set(5, IAppOpsManager::OP_AUDIO_NOTIFICATION_VOLUME);     // STREAM_NOTIFICATION
    STEAM_VOLUME_OPS->Set(6, IAppOpsManager::OP_AUDIO_BLUETOOTH_VOLUME);        // STREAM_BLUETOOTH_SCO
    STEAM_VOLUME_OPS->Set(7, IAppOpsManager::OP_AUDIO_MEDIA_VOLUME);            // STREAM_SYSTEM_ENFORCED
    STEAM_VOLUME_OPS->Set(8, IAppOpsManager::OP_AUDIO_MEDIA_VOLUME);            // STREAM_DTMF
    STEAM_VOLUME_OPS->Set(9, IAppOpsManager::OP_AUDIO_MEDIA_VOLUME);            // STREAM_TTS

    STREAM_NAMES = ArrayOf<String>::Alloc(10);
    STREAM_NAMES->Set(0, String("STREAM_VOICE_CALL"));
    STREAM_NAMES->Set(1, String("STREAM_SYSTEM"));
    STREAM_NAMES->Set(2, String("STREAM_RING"));
    STREAM_NAMES->Set(3, String("STREAM_MUSIC"));
    STREAM_NAMES->Set(4, String("STREAM_ALARM"));
    STREAM_NAMES->Set(5, String("STREAM_NOTIFICATION"));
    STREAM_NAMES->Set(6, String("STREAM_BLUETOOTH_SCO"));
    STREAM_NAMES->Set(7, String("STREAM_SYSTEM_ENFORCED"));
    STREAM_NAMES->Set(8, String("STREAM_DTMF"));
    STREAM_NAMES->Set(9, String("STREAM_TTS"));

    RINGER_MODE_NAMES = ArrayOf<String>::Alloc(3);
    RINGER_MODE_NAMES->Set(0, String("SILENT"));
    RINGER_MODE_NAMES->Set(1, String("VIBRATE"));
    RINGER_MODE_NAMES->Set(2, String("NORMAL"));

    CArrayList::New((IList**)&mMediaPlayers);

    return TRUE;
}

Boolean CAudioService::sInit = InitStatic();

//==============================================================================
//  CAudioService::StreamVolumeCommand
//==============================================================================

ECode CAudioService::StreamVolumeCommand::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("{streamType=");
    sb.Append(mStreamType);
    sb.Append(",index=");
    sb.Append(mIndex);
    sb.Append(",flags=");
    sb.Append(mFlags);
    sb.Append(",device=");
    sb.Append(mDevice);
    sb.AppendChar('}');
    return sb.ToString(result);
}

//==============================================================================
//  CAudioService::SoundPoolListenerThread
//==============================================================================

ECode CAudioService::SoundPoolListenerThread::Run()
{
    Looper::Prepare();
    mHost->mSoundPoolLooper = Looper::GetMyLooper();

    synchronized(mHost->mSoundEffectsLock) {
        if (mHost->mSoundPool != NULL) {
            mHost->mSoundPoolCallBack = new SoundPoolCallback(mHost);
            mHost->mSoundPool->SetOnLoadCompleteListener(mHost->mSoundPoolCallBack);
        }
        mHost->mSoundEffectsLock.Notify();
    }
    Looper::Loop();
    return NOERROR;
}

//==============================================================================
//  CAudioService::VolumeStreamState::VolumeDeathHandler
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::VolumeStreamState::VolumeDeathHandler,
        Object, IProxyDeathRecipient)

ECode CAudioService::VolumeStreamState::VolumeDeathHandler::ProxyDied()
{
    Logger::W(TAG, "Volume service client died for stream: %d", mHost->mStreamType);
    if (mMuteCount != 0) {
        // Reset all active mute requests from this client.
        mMuteCount = 1;
        Mute(FALSE);
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::VolumeDeathHandler::Mute(
    /* [in] */ Boolean state)
{
    Boolean updateVolume = FALSE;
    if (state) {
        if (mMuteCount == 0) {
            // Register for client death notification
            // try {
            // mCallback can be 0 if muted by AudioService
            if (mCallback != NULL) {
                AutoPtr<IProxy> proxy = (IProxy*)mCallback->Probe(EIID_IProxy);
                if (proxy != NULL) proxy->LinkToDeath(this, 0);
            }
            mHost->mDeathHandlers->Add(TO_IINTERFACE(this));
            // If the stream is not yet muted by any client, set level to 0
            if (!mHost->IsMuted()) {
                updateVolume = TRUE;
            }
            // } catch (RemoteException e) {
            //     // Client has died!
            //     binderDied();
            //     return;
            // }
        }
        else {
            Logger::W(TAG, "stream: %d was already muted by this client", mHost->mStreamType);
        }
        mMuteCount++;
    }
    else {
        if (mMuteCount == 0) {
            Logger::E(TAG, "unexpected unmute for stream: %d", mHost->mStreamType);
        }
        else {
            mMuteCount--;
            if (mMuteCount == 0) {
                // Unregister from client death notification
                mHost->mDeathHandlers->Remove(TO_IINTERFACE(this));
                // mCallback can be 0 if muted by AudioService
                if (mCallback != NULL) {
                    AutoPtr<IProxy> proxy = (IProxy*)mCallback->Probe(EIID_IProxy);
                    Boolean b;
                    if (proxy != NULL) proxy->UnlinkToDeath(this, 0, &b);
                }
                if (!mHost->IsMuted()) {
                    updateVolume = TRUE;
                }
            }
        }
    }
    if (updateVolume) {
        SendMsg(mHost->mHost->mAudioHandler, MSG_SET_ALL_VOLUMES,
            SENDMSG_QUEUE, 0, 0, (IInterface*)(IObject*)mHost, 0);
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::VolumeStreamState
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::VolumeStreamState,
        Object, IAudioServiceVolumeStreamState)

CAudioService::VolumeStreamState::VolumeStreamState(
    /* [in] */ CAudioService* host,
    /* [in] */ const String& settingName,
    /* [in] */ Int32 streamType)
    : mHost(host)
{
    Logger::W(TAG, "TODO: CConcurrentHashMap is not completed");
    // CConcurrentHashMap::New(8, 0.75f, 4, (IConcurrentHashMap**)&mIndex);
    CHashMap::New((IHashMap**)&mIndex);

    mVolumeIndexSettingName = settingName;

    mStreamType = streamType;
    mIndexMax = (*MAX_STREAM_VOLUME)[streamType];
    AudioSystem::InitStreamVolume(streamType, 0, mIndexMax);
    mIndexMax *= 10;

    // mDeathHandlers must be created before calling readSettings()
    CArrayList::New((IArrayList**)&mDeathHandlers);

    ReadSettings();
}

ECode CAudioService::VolumeStreamState::GetSettingNameForDevice(
    /* [in] */ Int32 device,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String name = mVolumeIndexSettingName;
    String suffix;
    AudioSystem::GetOutputDeviceName(device, &suffix);
    if (suffix.IsEmpty()) {
        *result = name;
        return NOERROR;
    }
    *result = name + "_" + suffix;
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::ReadSettings()
{
    synchronized(this) {
        // force maximum volume on all streams if fixed volume property is set
        if (mHost->mUseFixedVolume) {
            AutoPtr<IInteger32> iKey;
            CInteger32::New(IAudioSystem::DEVICE_OUT_DEFAULT, (IInteger32**)&iKey);
            AutoPtr<IInteger32> iValue;
            CInteger32::New(mIndexMax, (IInteger32**)&iValue);
            //TODO: delete this lock when mIndex is IConcurrentHashMap
            AutoLock lock(mIndex);
            mIndex->Put(iKey, iValue);
            return NOERROR;
        }
        // do not read system stream volume from settings: this stream is always aliased
        // to another stream type and its volume is never persisted. Values in settings can
        // only be stale values
        if ((mStreamType == IAudioSystem::STREAM_SYSTEM) ||
                (mStreamType == IAudioSystem::STREAM_SYSTEM_ENFORCED)) {
            Int32 index = 10 * CAudioManager::DEFAULT_STREAM_VOLUME[mStreamType];
            AutoPtr<IBoolean> lock = mHost->mCameraSoundForced;
            synchronized(lock) {
                Boolean b;
                mHost->mCameraSoundForced->GetValue(&b);
                if (b) {
                    index = mIndexMax;
                }
            }
            AutoPtr<IInteger32> iKey;
            CInteger32::New(IAudioSystem::DEVICE_OUT_DEFAULT, (IInteger32**)&iKey);
            AutoPtr<IInteger32> iValue;
            CInteger32::New(index, (IInteger32**)&iValue);
            //TODO: delete this lock when mIndex is IConcurrentHashMap
            AutoLock _lock(mIndex);
            mIndex->Put(iKey, iValue);
            return NOERROR;
        }

        Int32 remainingDevices = IAudioSystem::DEVICE_OUT_ALL;

        for (Int32 i = 0; remainingDevices != 0; i++) {
            Int32 device = (1 << i);
            if ((device & remainingDevices) == 0) {
                continue;
            }
            remainingDevices &= ~device;

            // retrieve current volume for device
            String name;
            GetSettingNameForDevice(device, &name);
            // if no volume stored for current stream and device, use default volume if default
            // device, continue otherwise
            Int32 defaultIndex = (device == IAudioSystem::DEVICE_OUT_DEFAULT) ?
                                    CAudioManager::DEFAULT_STREAM_VOLUME[mStreamType] : -1;
            Int32 index;
            Settings::System::GetInt32ForUser(
                mHost->mContentResolver, name, defaultIndex, IUserHandle::USER_CURRENT, &index);
            if (index == -1) {
                continue;
            }

            AutoPtr<IInteger32> iKey;
            CInteger32::New(device, (IInteger32**)&iKey);
            AutoPtr<IInteger32> iValue;
            CInteger32::New(GetValidIndex(10 * index), (IInteger32**)&iValue);
            //TODO: delete this lock when mIndex is IConcurrentHashMap
            AutoLock lock(mIndex);
            mIndex->Put(iKey, iValue);
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::ApplyDeviceVolume(
    /* [in] */ Int32 device)
{
    Int32 index;
    if (IsMuted()) {
        index = 0;
    }
    else if (((device & IAudioSystem::DEVICE_OUT_ALL_A2DP) != 0 && mHost->mAvrcpAbsVolSupported)
            || ((device & mHost->mFullVolumeDevices) != 0)) {
        index = (mIndexMax + 5)/10;
    }
    else {
        Int32 tmp;
        GetIndex(device, &tmp);
        index = (tmp + 5)/10;
    }
    return AudioSystem::SetStreamVolumeIndex(mStreamType, index, device);
}

ECode CAudioService::VolumeStreamState::ApplyAllVolumes()
{
    synchronized(this) {
        // apply default volume first: by convention this will reset all
        // devices volumes in audio policy manager to the supplied value
        Int32 index;
        if (IsMuted()) {
            index = 0;
        }
        else {
            Int32 tmp;
            GetIndex(IAudioSystem::DEVICE_OUT_DEFAULT, &tmp);
            index = (tmp + 5)/10;
        }
        AudioSystem::SetStreamVolumeIndex(mStreamType, index, IAudioSystem::DEVICE_OUT_DEFAULT);

        //TODO: delete this lock when mIndex is IConcurrentHashMap
        AutoLock lock(mIndex);
        // then apply device specific volumes
        AutoPtr<ISet> set;

        mIndex->GetEntrySet((ISet**)&set);

        AutoPtr<ArrayOf<IInterface*> > array;
        set->ToArray((ArrayOf<IInterface*>**)&array);

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IMapEntry> entry;
            entry = IMapEntry::Probe((*array)[i]);
            AutoPtr<IInterface> iKey;
            entry->GetKey((IInterface**)&iKey);

            Int32 device;
            IInteger32::Probe(iKey)->GetValue(&device);

            if (device != IAudioSystem::DEVICE_OUT_DEFAULT) {
                if (IsMuted()) {
                    index = 0;
                }
                else if (
                    ((device & IAudioSystem::DEVICE_OUT_ALL_A2DP) != 0
                        && mHost->mAvrcpAbsVolSupported)
                    || ((device & mHost->mFullVolumeDevices) != 0))
                {
                    index = (mIndexMax + 5)/10;
                }
                else {
                    Int32 val;
                    AutoPtr<IInterface> iValue;
                    entry->GetValue((IInterface**)&iValue);
                    IInteger32::Probe(iValue)->GetValue(&val);
                    index = (val + 5)/10;
                }
                AudioSystem::SetStreamVolumeIndex(mStreamType, index, device);
            }
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::AdjustIndex(
    /* [in] */ Int32 deltaIndex,
    /* [in] */ Int32 device,
    /* [out] */ Boolean* result)
{
    Int32 index;
    GetIndex(device, &index);
    return SetIndex(index + deltaIndex, device, result);
}

ECode CAudioService::VolumeStreamState::SetIndex(
    /* [in] */ Int32 index,
    /* [in] */ Int32 device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    synchronized(this) {
        Int32 oldIndex;
        GetIndex(device, &oldIndex);
        index = GetValidIndex(index);
        AutoPtr<IBoolean> lock = mHost->mCameraSoundForced;
        synchronized(lock) {
            Boolean b;
            mHost->mCameraSoundForced->GetValue(&b);
            if ((mStreamType == IAudioSystem::STREAM_SYSTEM_ENFORCED) && b) {
                index = mIndexMax;
            }
        }
        AutoPtr<IInteger32> iKey;
        CInteger32::New(device, (IInteger32**)&iKey);
        AutoPtr<IInteger32> iValue;
        CInteger32::New(index, (IInteger32**)&iValue);
        {
            //TODO: delete this lock when mIndex is IConcurrentHashMap
            AutoLock lock(mIndex);
            mIndex->Put(iKey, iValue);
        }

        if (oldIndex != index) {
            // Apply change to all streams using this one as alias
            // if changing volume of current device, also change volume of current
            // device on aliased stream
            Boolean currentDevice = (device == mHost->GetDeviceForStream(mStreamType));
            Int32 numStreamTypes;
            AudioSystem::GetNumStreamTypes(&numStreamTypes);
            for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
                if (streamType != mStreamType &&
                        (*mHost->mStreamVolumeAlias)[streamType] == mStreamType) {
                    Int32 scaledIndex = mHost->RescaleIndex(index, mStreamType, streamType);
                    Boolean b;
                    (*mHost->mStreamStates)[streamType]->SetIndex(scaledIndex, device, &b);
                    if (currentDevice) {
                        (*mHost->mStreamStates)[streamType]->SetIndex(scaledIndex,
                            mHost->GetDeviceForStream(streamType), &b);
                    }
                }
            }
            *result = TRUE;
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::GetIndex(
    /* [in] */ Int32 device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    synchronized(this) {
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger32> iKey;
        CInteger32::New(device, (IInteger32**)&iKey);
        //TODO: delete this lock when mIndex is IConcurrentHashMap
        AutoLock lock(mIndex);
        mIndex->Get(iKey, (IInterface**)&obj);
        AutoPtr<IInteger32> index = IInteger32::Probe(obj);
        if (index == NULL) {
            // there is always an entry for IAudioSystem::DEVICE_OUT_DEFAULT
            obj = NULL;
            AutoPtr<IInteger32> iKey;
            CInteger32::New(IAudioSystem::DEVICE_OUT_DEFAULT, (IInteger32**)&iKey);
            mIndex->Get(iKey, (IInterface**)&obj);
            index = IInteger32::Probe(obj);
        }
        index->GetValue(result);
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::GetMaxIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndexMax;
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::SetAllIndexes(
    /* [in] */ IAudioServiceVolumeStreamState* srcStream)
{
    synchronized(this) {
        Int32 srcStreamType;
        srcStream->GetStreamType(&srcStreamType);
        // apply default device volume from source stream to all devices first in case
        // some devices are present in this stream state but not in source stream state
        Int32 index;
        srcStream->GetIndex(IAudioSystem::DEVICE_OUT_DEFAULT, &index);
        index = mHost->RescaleIndex(index, srcStreamType, mStreamType);

        AutoPtr<ISet> set;
        AutoPtr<IIterator> i;
        Boolean b;
        {
            //TODO: delete this lock when mIndex is IConcurrentHashMap
            AutoLock lock(mIndex);
            mIndex->GetEntrySet((ISet**)&set);
            set->GetIterator((IIterator**)&i);
            while (i->HasNext(&b), b) {
                AutoPtr<IInterface> obj;
                i->GetNext((IInterface**)&obj);
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
                AutoPtr<IInteger32> i32;
                CInteger32::New(index, (IInteger32**)&i32);
                obj = NULL;
                entry->SetValue(i32, (IInterface**)&obj);
            }
        }

        //TODO: delete this lock when mIndex is IConcurrentHashMap
        AutoLock lock(((VolumeStreamState*)srcStream)->mIndex);
        // Now apply actual volume for devices in source stream state
        set = NULL;
        IMap::Probe(((VolumeStreamState*)srcStream)->mIndex)->GetEntrySet((ISet**)&set);
        i = NULL;
        set->GetIterator((IIterator**)&i);
        while (i->HasNext(&b), b) {
            AutoPtr<IInterface> obj;
            i->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetKey((IInterface**)&obj);
            Int32 device = IInteger32::Probe(obj)->GetValue(&device);
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            IInteger32::Probe(obj)->GetValue(&index);
            index = mHost->RescaleIndex(index, srcStreamType, mStreamType);

            SetIndex(index, device, &b);
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::SetAllIndexesToMax()
{
    synchronized(this) {
        //TODO: delete this lock when mIndex is IConcurrentHashMap
        AutoLock lock(mIndex);
        AutoPtr<ISet> set;
        mIndex->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> i;
        set->GetIterator((IIterator**)&i);
        Boolean b;
        while (i->HasNext(&b), b) {
            AutoPtr<IInterface> obj;
            i->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            AutoPtr<IInteger32> i32;
            CInteger32::New(mIndexMax, (IInteger32**)&i32);
            obj = NULL;
            entry->SetValue(i32, (IInterface**)&obj);
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::Mute(
    /* [in] */ IBinder* cb,
    /* [in] */ Boolean state)
{
    synchronized(this) {
        AutoPtr<VolumeDeathHandler> handler = GetDeathHandler(cb, state);
        if (handler == NULL) {
            Logger::E(TAG, "Could not get client death handler for stream: %d", mStreamType);
            return NOERROR;
        }
        handler->Mute(state);
    }
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::GetStreamType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStreamType;
    return NOERROR;
}

ECode CAudioService::VolumeStreamState::CheckFixedVolumeDevices()
{
    synchronized(this) {
        // ignore settings for fixed volume devices: volume should always be at max or 0
        if ((*mHost->mStreamVolumeAlias)[mStreamType] == IAudioSystem::STREAM_MUSIC) {
            //TODO: delete this lock when mIndex is IConcurrentHashMap
            AutoLock lock(mIndex);
            AutoPtr<ISet> set;
            mIndex->GetEntrySet((ISet**)&set);

            AutoPtr<ArrayOf<IInterface*> > array;
            set->ToArray((ArrayOf<IInterface*>**)&array);

            for (Int32 i = 0; i < array->GetLength(); ++i) {
                AutoPtr<IMapEntry> entry;
                entry = IMapEntry::Probe((*array)[i]);
                AutoPtr<IInterface> iKey;
                entry->GetKey((IInterface**)&iKey);
                Int32 device;
                IInteger32::Probe(iKey)->GetValue(&device);
                AutoPtr<IInterface> iValue;
                entry->GetValue((IInterface**)&iValue);
                Int32 index;
                IInteger32::Probe(iValue)->GetValue(&index);

                if (((device & mHost->mFullVolumeDevices) != 0)
                        || (((device & mHost->mFixedVolumeDevices) != 0) && index != 0)) {
                    AutoPtr<IInteger32> iV;
                    CInteger32::New(mIndexMax, (IInteger32**)&iV);
                    AutoPtr<IInterface> tmp;
                    entry->SetValue(iV, (IInterface**)&tmp);
                }
                ApplyDeviceVolume(device);
            }
        }
    }
    return NOERROR;
}

Int32 CAudioService::VolumeStreamState::GetValidIndex(
    /* [in] */ Int32 index)
{
    if (index < 0) {
        return 0;
    }
    else if (mHost->mUseFixedVolume || index > mIndexMax) {
        return mIndexMax;
    }

    return index;
}

Int32 CAudioService::VolumeStreamState::MuteCount()
{
    Int32 count = 0;
    Int32 size;
    mDeathHandlers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDeathHandlers->Get(i, (IInterface**)&obj);
        count += ((VolumeDeathHandler*)(IObject*)obj.Get())->mMuteCount;
    }
    return count;
}

Boolean CAudioService::VolumeStreamState::IsMuted()
{
    return MuteCount() != 0;
}

AutoPtr<CAudioService::VolumeStreamState::VolumeDeathHandler> CAudioService::VolumeStreamState::GetDeathHandler(
    /* [in] */ IBinder* cb,
    /* [in] */ Boolean state)
{
    AutoPtr<VolumeDeathHandler> handler;
    Int32 size;
    mDeathHandlers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDeathHandlers->Get(i, (IInterface**)&obj);
        handler = ((VolumeDeathHandler*)(IObject*)obj.Get());

        if (cb == handler->mCallback) {
            return handler;
        }
    }
    // If this is the first mute request for this client, create a new
    // client death handler. Otherwise, it is an out of sequence unmute request.
    if (state) {
        handler = new VolumeDeathHandler(this, cb);
    }
    else {
        Logger::W(TAG, "stream was not muted by this client");
        handler = NULL;
    }
    return handler;
}

void CAudioService::VolumeStreamState::Dump(
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(String("   Mute count: "));
    pw->Println(MuteCount());
    pw->Print(String("   Max: "));
    pw->Println((mIndexMax + 5) / 10);
    pw->Print(String("   Current: "));

    //TODO: delete this lock when mIndex is IConcurrentHashMap
    AutoLock lock(mIndex);
    AutoPtr<ISet> set;
    mIndex->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> i;
    set->GetIterator((IIterator**)&i);
    Boolean b;
    while (i->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        i->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        obj = NULL;
        entry->GetKey((IInterface**)&obj);
        Int32 device;
        IInteger32::Probe(obj)->GetValue(&device);
        obj = NULL;
        entry->GetValue((IInterface**)&obj);
        Int32 value;
        IInteger32::Probe(obj)->GetValue(&value);

        pw->Print(StringUtils::ToHexString(device));
        String deviceName;
        if (device == IAudioSystem::DEVICE_OUT_DEFAULT) {
            deviceName = String("default");
        }
        else {
            AudioSystem::GetOutputDeviceName(device, &deviceName);
        }

        if (!deviceName.IsEmpty()) {
            pw->Print(String(" ("));
            pw->Print(deviceName);
            pw->Print(String(")"));
        }
        pw->Print(String(": "));
        Int32 index = (value + 5) / 10;
        pw->Print(index);
        if (i->HasNext(&b), b) {
            pw->Print(String(", "));
        }
    }
}

//==============================================================================
//  CAudioService::VolumeController
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::VolumeController,
        Object, IAudioServiceVolumeController)

const String CAudioService::VolumeController::TAG("VolumeController");

ECode CAudioService::VolumeController::SetController(
    /* [in] */ IIVolumeController* controller)
{
    mController = controller;
    mVisible = FALSE;
    return NOERROR;
}

ECode CAudioService::VolumeController::LoadSettings(
    /* [in] */ IContentResolver* cr)
{
    return Settings::System::GetInt32ForUser(cr,
            ISettingsSecure::LONG_PRESS_TIMEOUT, 500, IUserHandle::USER_CURRENT, &mLongPressTimeout);
}

ECode CAudioService::VolumeController::SuppressAdjustment(
    /* [in] */ Int32 resolvedStream,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (resolvedStream == IAudioSystem::STREAM_RING && mController != NULL) {
        Int64 now = SystemClock::GetUptimeMillis();
        if ((flags & IAudioManager::FLAG_SHOW_UI) != 0 && !mVisible) {
            // ui will become visible
            if (mNextLongPress < now) {
                mNextLongPress = now + mLongPressTimeout;
            }
            *result = TRUE;
        }
        else if (mNextLongPress > 0) {  // in a long-press
            if (now > mNextLongPress) {
                // Int64 press triggered, no more suppression
                mNextLongPress = 0;
            }
            else {
                // keep suppressing until the Int64 press triggers
                *result = TRUE;
            }
        }
    }
    return NOERROR;
}

ECode CAudioService::VolumeController::SetVisible(
    /* [in] */ Boolean visible)
{
    mVisible = visible;
    return NOERROR;
}

ECode CAudioService::VolumeController::IsSameBinder(
    /* [in] */ IIVolumeController* controller,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBinder> binder;
    AsBinder((IBinder**)&binder);
    return IObject::Probe(binder)->Equals(Binder(controller), result);
}

ECode CAudioService::VolumeController::AsBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBinder> temp = Binder(mController);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioService::VolumeController::PostDisplaySafeVolumeWarning(
    /* [in] */ Int32 flags)
{
    if (mController == NULL)
        return NOERROR;
    // try {
    return mController->DisplaySafeVolumeWarning(flags);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling displaySafeVolumeWarning", e);
    // }
}

ECode CAudioService::VolumeController::PostVolumeChanged(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 flags)
{
    if (mController == NULL)
        return NOERROR;
    // try {
    return mController->VolumeChanged(streamType, flags);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling volumeChanged", e);
    // }
}

ECode CAudioService::VolumeController::PostMasterVolumeChanged(
    /* [in] */ Int32 flags)
{
    if (mController == NULL)
        return NOERROR;
    // try {
    return mController->MasterVolumeChanged(flags);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling masterVolumeChanged", e);
    // }
}

ECode CAudioService::VolumeController::PostMasterMuteChanged(
    /* [in] */ Int32 flags)
{
    if (mController == NULL)
        return NOERROR;
    // try {
    return mController->MasterMuteChanged(flags);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling masterMuteChanged", e);
    // }
}

ECode CAudioService::VolumeController::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    if (mController == NULL)
        return NOERROR;
    // try {
    return mController->SetLayoutDirection(layoutDirection);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling setLayoutDirection", e);
    // }
}

ECode CAudioService::VolumeController::PostDismiss()
{
    if (mController == NULL)
        return NOERROR;
    // try {
    return mController->Dismiss();
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling dismiss", e);
    // }
}

ECode CAudioService::VolumeController::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBinder> binder;
    AsBinder((IBinder**)&binder);
    String str;
    binder->ToString(&str);
    *result = String("VolumeController(") + str + ",mVisible="
            + StringUtils::BooleanToString(mVisible) + ")";
    return NOERROR;
}

AutoPtr<IBinder> CAudioService::VolumeController::Binder(
    /* [in] */ IIVolumeController* controller)
{
    if (controller == NULL) {
        return  NULL;
    }
    else {
        return IBinder::Probe(controller);
    }
}

//==============================================================================
//  CAudioService::AudioServiceInternal
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::AudioServiceInternal,
        Object, IAudioManagerInternal)

ECode CAudioService::AudioServiceInternal::AdjustSuggestedStreamVolumeForUid(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 uid)
{
    // direction and stream type swap here because the public
    // adjustSuggested has a different order than the other methods.
    mHost->AdjustSuggestedStreamVolume(direction, streamType, flags, callingPackage, uid);
    return NOERROR;
}

ECode CAudioService::AudioServiceInternal::AdjustStreamVolumeForUid(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 uid)
{
    mHost->AdjustStreamVolume(streamType, direction, flags, callingPackage, uid);
    return NOERROR;
}

ECode CAudioService::AudioServiceInternal::SetStreamVolumeForUid(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 uid)
{
    mHost->SetStreamVolume(streamType, direction, flags, callingPackage, uid);
    return NOERROR;
}

//==============================================================================
//  CAudioService::AudioPolicyProxy
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::AudioPolicyProxy,
        Object, IProxyDeathRecipient)

const String CAudioService::AudioPolicyProxy::TAG("AudioPolicyProxy");

ECode CAudioService::AudioPolicyProxy::ProxyDied()
{
    AutoPtr<IHashMap> lock = mHost->mAudioPolicies;
    synchronized(lock) {
        String str;
        mToken->ToString(&str);
        Logger::V(TAG, "audio policy %s died", str.string());
        mHost->mAudioPolicies->Remove(mToken);
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::AudioOrientationEventListener
//==============================================================================

ECode CAudioService::AudioOrientationEventListener::Run()
{
    //Even though we're responding to phone orientation events,
    //use display rotation so audio stays in sync with video/dialogs
    AutoPtr<IInterface> service;
    mHost->mContext->GetSystemService(IContext::WINDOW_SERVICE, ((IInterface**)&service));
    AutoPtr<IDisplay> disp;

    IWindowManager::Probe(service)->GetDefaultDisplay((IDisplay**)&disp);
    Int32 newRotation;
    disp->GetRotation(&newRotation);

    if (newRotation != mHost->mDeviceRotation) {
        mHost->mDeviceRotation = newRotation;
        mHost->SetRotationForAudioSystem();
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::ForceControlStreamClient
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::ForceControlStreamClient,
        Object, IProxyDeathRecipient)

CAudioService::ForceControlStreamClient::ForceControlStreamClient(
    /* [in] */ CAudioService* host,
    /* [in] */ IBinder* cb)
    : mHost(host)
    , mCb(cb)
{
    if (cb != NULL) {
        // try {
        AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
        if (proxy != NULL) proxy->LinkToDeath(this, 0);
        // } catch (RemoteException e) {
        //     // Client has died!
        //     Log.w(TAG, "ForceControlStreamClient() could not link to "+cb+" binder death");
        //     cb = null;
        // }
    }
}

ECode CAudioService::ForceControlStreamClient::ProxyDied()
{
    Object& lock = mHost->mForceControlStreamLock;
    synchronized(lock) {
        Logger::W(TAG, "SCO client died");
        if (mHost->mForceControlStreamClient.Get() != this) {
            Logger::W(TAG, "unregistered control stream client died");
        }
        else {
            mHost->mForceControlStreamClient = NULL;
            mHost->mVolumeControlStream = -1;
        }
    }
    return NOERROR;
}

ECode CAudioService::ForceControlStreamClient::ReleaseResources()
{
    if (mCb != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
        Boolean b;
        if (proxy != NULL) proxy->UnlinkToDeath(this, 0, &b);
        mCb = NULL;
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::RmtSbmxFullVolDeathHandler
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::RmtSbmxFullVolDeathHandler,
        Object, IProxyDeathRecipient)

CAudioService::RmtSbmxFullVolDeathHandler::RmtSbmxFullVolDeathHandler(
    /* [in] */ CAudioService* host,
    /* [in] */ IBinder* cb)
    : mHost(host)
    , mICallback(cb)
{
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
    if (proxy != NULL) proxy->LinkToDeath(this, 0);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "can't link to death", e);
    // }
}

ECode CAudioService::RmtSbmxFullVolDeathHandler::ProxyDied()
{
    String str;
    mICallback->ToString(&str);
    Logger::W(TAG, "Recorder with remote submix at full volume died %s", str.string());
    return mHost->ForceRemoteSubmixFullVolume(FALSE, mICallback);
}

ECode CAudioService::RmtSbmxFullVolDeathHandler::IsHandlerFor(
    /* [in] */ IBinder* cb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return IObject::Probe(mICallback)->Equals(cb, result);
}

ECode CAudioService::RmtSbmxFullVolDeathHandler::Forget()
{
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)mICallback->Probe(EIID_IProxy);
    Boolean b;
    if (proxy != NULL) proxy->UnlinkToDeath(this, 0, &b);
    // } catch (NoSuchElementException e) {
    //     Log.e(TAG, "error unlinking to death", e);
    // }
    return NOERROR;
}

//==============================================================================
//  CAudioService::SetModeDeathHandler
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::SetModeDeathHandler,
        Object, IProxyDeathRecipient)

ECode CAudioService::SetModeDeathHandler::ProxyDied()
{
    Int32 newModeOwnerPid = 0;
    AutoPtr<IArrayList> lock = mHost->mSetModeDeathHandlers;
    synchronized(lock) {
        Logger::W(TAG, "setMode() client died");
        Int32 index;
        mHost->mSetModeDeathHandlers->IndexOf(TO_IINTERFACE(this), &index);
        if (index < 0) {
            Logger::W(TAG, "unregistered setMode() client died");
        }
        else {
            newModeOwnerPid = mHost->SetModeInt(IAudioSystem::MODE_NORMAL, mCb, mPid);
        }
    }
    // when entering RINGTONE, IN_CALL or IN_COMMUNICATION mode, clear all
    // SCO connections not started by the application changing the mode
    if (newModeOwnerPid != 0) {
        Int64 ident = Binder::ClearCallingIdentity();
        mHost->DisconnectBluetoothSco(newModeOwnerPid);
        Binder::RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode CAudioService::SetModeDeathHandler::GetPid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPid;
    return NOERROR;
}

ECode CAudioService::SetModeDeathHandler::SetMode(
    /* [in] */ Int32 mode)
{
    mMode = mode;
    return NOERROR;
}

ECode CAudioService::SetModeDeathHandler::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMode;
    return NOERROR;
}

ECode CAudioService::SetModeDeathHandler::GetBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCb;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//==============================================================================
//  CAudioService::SoundPoolCallback
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::SoundPoolCallback, Object, ISoundPoolOnLoadCompleteListener)

CAudioService::SoundPoolCallback::SoundPoolCallback(
    /* [in] */ CAudioService* host)
    : mHost(host)
    , mStatus(1)
{
    CArrayList::New((IList**)&mSamples);
}

ECode CAudioService::SoundPoolCallback::OnLoadComplete(
    /* [in] */ ISoundPool* soundPool,
    /* [in] */ Int32 sampleId,
    /* [in] */ Int32 status)
{
    synchronized(mHost->mSoundEffectsLock) {
        Int32 i;
        AutoPtr<IInteger32> i32;
        CInteger32::New(sampleId, (IInteger32**)&i32);
        mSamples->IndexOf(i32, &i);
        if (i >= 0) {
            mSamples->Remove(i);
        }
        Boolean b;
        if ((status != 0) || (mSamples->IsEmpty(&b), b)) {
            mStatus = status;
            mHost->mSoundEffectsLock.Notify();
        }
    }
    return NOERROR;
}

ECode CAudioService::SoundPoolCallback::Status(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatus;
    return NOERROR;
}

ECode CAudioService::SoundPoolCallback::SetSamples(
    /* [in] */ ArrayOf<Int32>* samples)
{
    for (Int32 i = 0; i < samples->GetLength(); i++) {
        // do not wait ack for samples rejected upfront by SoundPool
        if ((*samples)[i] > 0) {
            AutoPtr<IInteger32> i32;
            CInteger32::New((*samples)[i], (IInteger32**)&i32);
            mSamples->Add(i32);
        }
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::ScoClient
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::ScoClient,
        Object, IProxyDeathRecipient)

CAudioService::ScoClient::ScoClient(
    /* [in] */ CAudioService* host,
    /* [in] */ IBinder* cb)
    : mHost(host)
    , mCb(cb)
    , mStartcount(0)
{
    mCreatorPid = Binder::GetCallingPid();
}

ECode CAudioService::ScoClient::ProxyDied()
{
    synchronized(mHost->mScoClients.Get()) {
        Logger::W(TAG, "SCO client died");
        Int32 index;
        mHost->mScoClients->IndexOf(TO_IINTERFACE(this), &index);
        if (index < 0) {
            Logger::W(TAG, "unregistered SCO client died");
        }
        else {
            ClearCount(TRUE);
            mHost->mScoClients->Remove(TO_IINTERFACE(this));
        }
    }
    return NOERROR;
}

ECode CAudioService::ScoClient::IncCount(
    /* [in] */ Int32 scoAudioMode)
{
    synchronized(mHost->mScoClients.Get()) {
        RequestScoState(IBluetoothHeadset::STATE_AUDIO_CONNECTED, scoAudioMode);
        if (mStartcount == 0) {
            // try {
            AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
            if (proxy != NULL) proxy->LinkToDeath(this, 0);
            // } catch (RemoteException e) {
            //     // client has already died!
            //     Log.w(TAG, "ScoClient  incCount() could not link to "+mCb+" binder death");
            // }
        }
        mStartcount++;
    }
    return NOERROR;
}

ECode CAudioService::ScoClient::DecCount()
{
    synchronized(mHost->mScoClients.Get()) {
        if (mStartcount == 0) {
            Logger::W(TAG, "ScoClient.decCount() already 0");
        }
        else {
            mStartcount--;
            if (mStartcount == 0) {
                // try {
                AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
                Boolean b;
                if (proxy != NULL) proxy->UnlinkToDeath(this, 0, &b);
                // } catch (NoSuchElementException e) {
                //     Log.w(TAG, "decCount() going to 0 but not registered to binder");
                // }
            }
            RequestScoState(IBluetoothHeadset::STATE_AUDIO_DISCONNECTED, 0);
        }
    }
    return NOERROR;
}

ECode CAudioService::ScoClient::ClearCount(
    /* [in] */ Boolean stopSco)
{
    synchronized(mHost->mScoClients.Get()) {
        if (mStartcount != 0) {
            // try {
            AutoPtr<IProxy> proxy = (IProxy*)mCb->Probe(EIID_IProxy);
            Boolean b;
            if (proxy != NULL) proxy->UnlinkToDeath(this, 0, &b);
            // } catch (NoSuchElementException e) {
            //     Log.w(TAG, "clearCount() mStartcount: "+mStartcount+" != 0 but not registered to binder");
            // }
        }
        mStartcount = 0;
        if (stopSco) {
            RequestScoState(IBluetoothHeadset::STATE_AUDIO_DISCONNECTED, 0);
        }
    }
    return NOERROR;
}

ECode CAudioService::ScoClient::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStartcount;
    return NOERROR;
}

ECode CAudioService::ScoClient::GetBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCb;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioService::ScoClient::GetPid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCreatorPid;
    return NOERROR;
}

ECode CAudioService::ScoClient::TotalCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(mHost->mScoClients.Get()) {
        Int32 count = 0;
        Int32 size;
        mHost->mScoClients->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mHost->mScoClients->Get(i, (IInterface**)&obj);
            Int32 tmp;
            ((ScoClient*)(IObject*)obj.Get())->GetCount(&tmp);
            count += tmp;
        }
        *result = count;
    }
    return NOERROR;
}

void CAudioService::ScoClient::RequestScoState(
    /* [in] */ Int32 state,
    /* [in] */ Int32 scoAudioMode)
{
    mHost->CheckScoAudioState();
    Int32 count;
    TotalCount(&count);
    if (count == 0) {
        if (state == IBluetoothHeadset::STATE_AUDIO_CONNECTED) {
            // Make sure that the state transitions to CONNECTING even if we cannot initiate
            // the connection.
            mHost->BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_CONNECTING);
            // Accept SCO audio activation only in NORMAL audio mode or if the mode is
            // currently controlled by the same client process.
            AutoPtr<IArrayList> lock = mHost->mSetModeDeathHandlers;
            synchronized(lock) {
                AutoPtr<IInterface> obj;
                mHost->mSetModeDeathHandlers->Get(0, (IInterface**)&obj);
                Int32 pid;
                ((SetModeDeathHandler*)(IObject*)obj.Get())->GetPid(&pid);
                Boolean b;
                if (((mHost->mSetModeDeathHandlers->IsEmpty(&b), b) ||
                        pid == mCreatorPid) &&
                        (mHost->mScoAudioState == SCO_STATE_INACTIVE ||
                         mHost->mScoAudioState == SCO_STATE_DEACTIVATE_REQ)) {
                    if (mHost->mScoAudioState == SCO_STATE_INACTIVE) {
                        mHost->mScoAudioMode = scoAudioMode;
                        if (scoAudioMode == SCO_MODE_UNDEFINED) {
                            String addr;
                            mHost->mBluetoothHeadsetDevice->GetAddress(&addr);
                            Int32 value;
                            Settings::Global::GetInt32(mHost->mContentResolver,
                                    String("bluetooth_sco_channel_") + addr,
                                    SCO_MODE_VIRTUAL_CALL, &value);
                            AutoPtr<IInteger32> i32;
                            CInteger32::New(value, (IInteger32**)&i32);
                            i32->GetValue(&mHost->mScoAudioMode);

                            if (mHost->mScoAudioMode > SCO_MODE_MAX || mHost->mScoAudioMode < 0) {
                                mHost->mScoAudioMode = SCO_MODE_VIRTUAL_CALL;
                            }
                        }
                        if (mHost->mBluetoothHeadset != NULL && mHost->mBluetoothHeadsetDevice != NULL) {
                            Boolean status = FALSE;
                            if (mHost->mScoAudioMode == SCO_MODE_VR) {
                                mHost->mBluetoothHeadset->StartVoiceRecognition(mHost->mBluetoothHeadsetDevice, &status);
                            }
                            else {
                                mHost->mBluetoothHeadset->StartScoUsingVirtualVoiceCall(
                                        mHost->mBluetoothHeadsetDevice, &status);
                            }
                            if (status) {
                                mHost->mScoAudioState = SCO_STATE_ACTIVE_INTERNAL;
                            }
                            else {
                                mHost->BroadcastScoConnectionState(
                                        IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
                            }
                        }
                        else if (mHost->GetBluetoothHeadset()) {
                            mHost->mScoAudioState = SCO_STATE_ACTIVATE_REQ;
                        }
                    }
                    else {
                        mHost->mScoAudioState = SCO_STATE_ACTIVE_INTERNAL;
                        mHost->BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_CONNECTED);
                    }
                }
                else {
                    mHost->BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
                }
            }
        }
        else if (state == IBluetoothHeadset::STATE_AUDIO_DISCONNECTED &&
                      (mHost->mScoAudioState == SCO_STATE_ACTIVE_INTERNAL ||
                       mHost->mScoAudioState == SCO_STATE_ACTIVATE_REQ)) {
            if (mHost->mScoAudioState == SCO_STATE_ACTIVE_INTERNAL) {
                if (mHost->mBluetoothHeadset != NULL && mHost->mBluetoothHeadsetDevice != NULL) {
                    Boolean status = FALSE;
                    if (mHost->mScoAudioMode == SCO_MODE_VR) {
                        mHost->mBluetoothHeadset->StopVoiceRecognition(mHost->mBluetoothHeadsetDevice, &status);
                    }
                    else {
                        mHost->mBluetoothHeadset->StopScoUsingVirtualVoiceCall(
                                mHost->mBluetoothHeadsetDevice, &status);
                    }
                    if (!status) {
                        mHost->mScoAudioState = SCO_STATE_INACTIVE;
                        mHost->BroadcastScoConnectionState(
                                IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
                    }
                }
                else if (mHost->GetBluetoothHeadset()) {
                    mHost->mScoAudioState = SCO_STATE_DEACTIVATE_REQ;
                }
            }
            else {
                mHost->mScoAudioState = SCO_STATE_INACTIVE;
                mHost->BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
            }
        }
    }
}

//==============================================================================
//  CAudioService::AudioSystemThread
//==============================================================================

ECode CAudioService::AudioSystemThread::Run()
{
    // Set this thread up so the handler will work on it
    Looper::Prepare();

    synchronized(mHost) {
        mHost->mAudioHandler = new AudioHandler(mHost);

        // Notify that the handler has been created
        mHost->Notify();
    }

    // Listen for volume change requests that are set by VolumePanel
    Looper::Loop();
    return NOERROR;
}

//==============================================================================
//  CAudioService::AudioHandler
//==============================================================================

ECode CAudioService::AudioHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg1;
    msg->GetArg1(&arg1);
    Int32 arg2;
    msg->GetArg2(&arg2);

    switch (what) {

        case MSG_SET_DEVICE_VOLUME:
            SetDeviceVolume((VolumeStreamState*)(IObject*)obj.Get(), arg1);
            break;

        case MSG_SET_ALL_VOLUMES:
            SetAllVolumes((VolumeStreamState*)(IObject*)obj.Get());
            break;

        case MSG_PERSIST_VOLUME:
            PersistVolume((VolumeStreamState*)(IObject*)obj.Get(), arg1);
            break;

        case MSG_PERSIST_MASTER_VOLUME: {
            if (mHost->mUseFixedVolume) {
                return NOERROR;
            }
            Boolean b;
            Settings::System::PutFloatForUser(mHost->mContentResolver,
                                            ISettingsSystem::VOLUME_MASTER,
                                            arg1 / (Float)1000.0,
                                            IUserHandle::USER_CURRENT, &b);
            break;
        }
        case MSG_PERSIST_MASTER_VOLUME_MUTE: {
            if (mHost->mUseFixedVolume) {
                return NOERROR;
            }
            Boolean b;
            Settings::System::PutInt32ForUser(mHost->mContentResolver,
                                         ISettingsSystem::VOLUME_MASTER_MUTE,
                                         arg1, arg2, &b);
            break;
        }
        case MSG_PERSIST_RINGER_MODE: {
            // note that the value persisted is the current ringer mode, not the
            // value of ringer mode as of the time the request was made to persist
            Int32 mode;
            mHost->GetRingerMode(&mode);
            PersistRingerMode(mode);
            break;
        }
        case MSG_MEDIA_SERVER_DIED: {
            Int32 result;
            AudioSystem::CheckAudioFlinger(&result);
            if (!mHost->mSystemReady ||
                    (result != IAudioSystem::AUDIO_STATUS_OK)) {
                Logger::E(TAG, "Media server died.");
                SendMsg(mHost->mAudioHandler, MSG_MEDIA_SERVER_DIED, SENDMSG_NOOP, 0, 0,
                        NULL, 500);
                break;
            }
            Logger::E(TAG, "Media server started.");

            // indicate to audio HAL that we start the reconfiguration phase after a media
            // server crash
            // Note that we only execute this when the media server
            // process restarts after a crash, not the first time it is started.
            AudioSystem::SetParameters(String("restarting=TRUE"));

            ReadAndSetLowRamDevice();

            // Restore device connection states
            AutoPtr<IHashMap> lock = mHost->mConnectedDevices;
            synchronized(lock) {
                AutoPtr<ISet> entrySet;
                mHost->mConnectedDevices->GetEntrySet((ISet**)&entrySet);
                AutoPtr<ArrayOf<IInterface*> > array;
                entrySet->ToArray((ArrayOf<IInterface*>**)&array);

                for (Int32 i = 0; i < array->GetLength(); ++i) {
                    AutoPtr<IMapEntry> entry;
                    entry = IMapEntry::Probe((*array)[i]);
                    AutoPtr<IInterface> iKey;
                    entry->GetKey((IInterface**)&iKey);
                    Int32 key;
                    IInteger32::Probe(iKey)->GetValue(&key);

                    AutoPtr<IInterface> iValue;
                    entry->GetValue((IInterface**)&iValue);
                    String value;
                    ICharSequence::Probe(iValue)->ToString(&value);

                    AudioSystem::SetDeviceConnectionState(
                            key,
                            IAudioSystem::DEVICE_STATE_AVAILABLE,
                            value);
                }
            }
            // Restore call state
            AudioSystem::SetPhoneState(mHost->mMode, &result);

            // Restore forced usage for communcations and record
            AudioSystem::SetForceUse(IAudioSystem::FOR_COMMUNICATION, mHost->mForcedUseForComm);
            AudioSystem::SetForceUse(IAudioSystem::FOR_RECORD, mHost->mForcedUseForComm);
            Boolean b;
            mHost->mCameraSoundForced->GetValue(&b);
            AudioSystem::SetForceUse(IAudioSystem::FOR_SYSTEM, b ?
                            IAudioSystem::FORCE_SYSTEM_ENFORCED : IAudioSystem::FORCE_NONE);

            // Restore stream volumes
            Int32 numStreamTypes;
            AudioSystem::GetNumStreamTypes(&numStreamTypes);
            for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
                AutoPtr<VolumeStreamState> streamState = (*mHost->mStreamStates)[streamType];
                AudioSystem::InitStreamVolume(streamType, 0, (streamState->mIndexMax + 5) / 10);

                streamState->ApplyAllVolumes();
            }

            // Restore ringer mode
            Int32 mode;
            mHost->GetRingerMode(&mode);
            mHost->SetRingerModeInt(mode, FALSE);

            // Restore master volume
            mHost->RestoreMasterVolume();

            // Reset device orientation (if monitored for this device)
            if (mHost->mMonitorOrientation) {
                mHost->SetOrientationForAudioSystem();
            }
            if (mHost->mMonitorRotation) {
                mHost->SetRotationForAudioSystem();
            }

            Object& eLock = mHost->mBluetoothA2dpEnabledLock;
            synchronized(eLock) {
                AudioSystem::SetForceUse(IAudioSystem::FOR_MEDIA,
                        mHost->mBluetoothA2dpEnabled ?
                                IAudioSystem::FORCE_NONE : IAudioSystem::FORCE_NO_BT_A2DP);
            }

            Object& sLock = mHost->mSettingsLock;
            synchronized(sLock) {
                AudioSystem::SetForceUse(IAudioSystem::FOR_DOCK,
                        mHost->mDockAudioMediaEnabled ?
                                IAudioSystem::FORCE_ANALOG_DOCK : IAudioSystem::FORCE_NONE);
            }
            if (mHost->mHdmiManager != NULL) {
                AutoPtr<IHdmiControlManager> lock = mHost->mHdmiManager;
                synchronized(lock) {
                    if (mHost->mHdmiTvClient != NULL) {
                        Int32 result;
                        mHost->SetHdmiSystemAudioSupported(mHost->mHdmiSystemAudioSupported, &result);
                    }
                }
            }
            // indicate the end of reconfiguration phase to audio HAL
            AudioSystem::SetParameters(String("restarting=false"));
            break;
        }
        case MSG_UNLOAD_SOUND_EFFECTS:
            OnUnloadSoundEffects();
            break;

        case MSG_LOAD_SOUND_EFFECTS: {
            //FIXME: onLoadSoundEffects() should be executed in a separate thread as it
            // can take several dozens of milliseconds to complete
            Boolean loaded = OnLoadSoundEffects();
            if (obj != NULL) {
                AutoPtr<LoadSoundEffectReply> reply = (LoadSoundEffectReply*)(IObject*)obj.Get();
                synchronized(reply) {
                    reply->mStatus = loaded ? 0 : -1;
                    reply->Notify();
                }
            }
            break;
        }
        case MSG_PLAY_SOUND_EFFECT:
            OnPlaySoundEffect(arg1, arg2);
            break;

        case MSG_BTA2DP_DOCK_TIMEOUT: {
            // msg.obj  == address of BTA2DP device
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            AutoPtr<IHashMap> lock = mHost->mConnectedDevices;
            synchronized(lock) {
                mHost->MakeA2dpDeviceUnavailableNow(str);
            }
            break;
        }
        case MSG_SET_FORCE_USE:
        case MSG_SET_FORCE_BT_A2DP_USE:
            SetForceUse(arg1, arg2);
            break;

        case MSG_BT_HEADSET_CNCT_FAILED:
            mHost->ResetBluetoothSco();
            break;

        case MSG_SET_WIRED_DEVICE_CONNECTION_STATE: {
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            mHost->OnSetWiredDeviceConnectionState(arg1, arg2, str);
            mHost->mAudioEventWakeLock->ReleaseLock();
            break;
        }
        case MSG_SET_A2DP_SRC_CONNECTION_STATE:
            mHost->OnSetA2dpSourceConnectionState(IBluetoothDevice::Probe(obj), arg1);
            mHost->mAudioEventWakeLock->ReleaseLock();
            break;

        case MSG_SET_A2DP_SINK_CONNECTION_STATE:
            mHost->OnSetA2dpSinkConnectionState(IBluetoothDevice::Probe(obj), arg1);
            mHost->mAudioEventWakeLock->ReleaseLock();
            break;

        case MSG_REPORT_NEW_ROUTES: {
            Int32 N;
            mHost->mRoutesObservers->BeginBroadcast(&N);
            if (N > 0) {
                AutoPtr<IAudioRoutesInfo> routes;
                AutoPtr<IAudioRoutesInfo> lock = mHost->mCurAudioRoutes;
                synchronized(lock) {
                    CAudioRoutesInfo::New(mHost->mCurAudioRoutes, (IAudioRoutesInfo**)&routes);
                }
                while (N > 0) {
                    N--;
                    AutoPtr<IInterface> obj;
                    mHost->mRoutesObservers->GetBroadcastItem(N, (IInterface**)&obj);
                    AutoPtr<IIAudioRoutesObserver> obs = IIAudioRoutesObserver::Probe(obj);
                    // try {
                        obs->DispatchAudioRoutesChanged(routes);
                    // } catch (RemoteException e) {
                    // }
                }
            }
            mHost->mRoutesObservers->FinishBroadcast();
            break;
        }

        case MSG_CHECK_MUSIC_ACTIVE:
            mHost->OnCheckMusicActive();
            break;

        case MSG_BROADCAST_AUDIO_BECOMING_NOISY:
            mHost->OnSendBecomingNoisyIntent();
            break;

        case MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED:
        case MSG_CONFIGURE_SAFE_MEDIA_VOLUME:
            mHost->OnConfigureSafeVolume(what == MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED);
            break;
        case MSG_PERSIST_SAFE_VOLUME_STATE:
            OnPersistSafeVolumeState(arg1);
            break;

        case MSG_BROADCAST_BT_CONNECTION_STATE:
            mHost->OnBroadcastScoConnectionState(arg1);
            break;

        case MSG_SYSTEM_READY:
            mHost->OnSystemReady();
            break;

        case MSG_PERSIST_MUSIC_ACTIVE_MS: {
            Int32 musicActiveMs = arg1;
            Boolean b;
            Settings::Secure::PutInt32ForUser(mHost->mContentResolver,
                    ISettingsSecure::UNSAFE_VOLUME_MUSIC_ACTIVE_MS, musicActiveMs,
                    IUserHandle::USER_CURRENT, &b);
            break;
        }
        case MSG_PERSIST_MICROPHONE_MUTE: {
            Boolean b;
            Settings::System::PutInt32ForUser(mHost->mContentResolver,
                                         ISettingsSystem::MICROPHONE_MUTE,
                                         arg1, arg2, &b);
            break;
        }
    }
    return NOERROR;
}

void CAudioService::AudioHandler::SetDeviceVolume(
    /* [in] */ VolumeStreamState* streamState,
    /* [in] */ Int32 device)
{
    // Apply volume
    streamState->ApplyDeviceVolume(device);

    // Apply change to all streams using this one as alias
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (streamType != streamState->mStreamType &&
                (*mHost->mStreamVolumeAlias)[streamType] == streamState->mStreamType) {
            // Make sure volume is also maxed out on A2DP device for aliased stream
            // that may have a different device selected
            Int32 streamDevice = mHost->GetDeviceForStream(streamType);
            if ((device != streamDevice) && mHost->mAvrcpAbsVolSupported &&
                    ((device & IAudioSystem::DEVICE_OUT_ALL_A2DP) != 0)) {
                (*mHost->mStreamStates)[streamType]->ApplyDeviceVolume(device);
            }
            (*mHost->mStreamStates)[streamType]->ApplyDeviceVolume(streamDevice);
        }
    }

    // Post a persist volume msg
    SendMsg(mHost->mAudioHandler,
            MSG_PERSIST_VOLUME,
            SENDMSG_QUEUE,
            device,
            0,
            (IInterface*)(IObject*)streamState,
            PERSIST_DELAY);
}

void CAudioService::AudioHandler::SetAllVolumes(
    /* [in] */ VolumeStreamState* streamState)
{
    // Apply volume
    streamState->ApplyAllVolumes();

    // Apply change to all streams using this one as alias
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (streamType != streamState->mStreamType &&
                (*mHost->mStreamVolumeAlias)[streamType] == streamState->mStreamType) {
            (*mHost->mStreamStates)[streamType]->ApplyAllVolumes();
        }
    }
}

void CAudioService::AudioHandler::PersistVolume(
    /* [in] */ VolumeStreamState* streamState,
    /* [in] */ Int32 device)
{
    if (mHost->mUseFixedVolume) {
        return;
    }
    if (mHost->IsPlatformTelevision() &&
            (streamState->mStreamType != IAudioSystem::STREAM_MUSIC)) {
        return;
    }
    String str;
    streamState->GetSettingNameForDevice(device, &str);
    Int32 value;
    streamState->GetIndex(device, &value);
    Boolean b;
    Settings::System::PutInt32ForUser(mHost->mContentResolver,
              str, (value + 5)/ 10,
              IUserHandle::USER_CURRENT, &b);
}

void CAudioService::AudioHandler::PersistRingerMode(
    /* [in] */ Int32 ringerMode)
{
    if (mHost->mUseFixedVolume) {
        return;
    }
    Boolean b;
    Settings::Global::PutInt32(mHost->mContentResolver, ISettingsGlobal::MODE_RINGER, ringerMode, &b);
}

Boolean CAudioService::AudioHandler::OnLoadSoundEffects()
{
    Int32 status = 0;

    synchronized(mHost->mSoundEffectsLock) {
        if (!mHost->mSystemReady) {
            Logger::W(TAG, "onLoadSoundEffects() called before boot complete");
            return FALSE;
        }

        if (mHost->mSoundPool != NULL) {
            return TRUE;
        }

        mHost->LoadTouchSoundAssets();

        AutoPtr<CSoundPool::Builder> sb = new CSoundPool::Builder();
        sb->SetMaxStreams(NUM_SOUNDPOOL_CHANNELS);
        AutoPtr<IAudioAttributesBuilder> ab;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&ab);
        ab->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
        ab->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
        AutoPtr<IAudioAttributes> aa;
        ab->Build((IAudioAttributes**)&aa);
        sb->SetAudioAttributes(aa);
        sb->Build((ISoundPool**)&mHost->mSoundPool);

        mHost->mSoundPoolCallBack = NULL;
        mHost->mSoundPoolListenerThread = new SoundPoolListenerThread(mHost);
        mHost->mSoundPoolListenerThread->Start();
        Int32 attempts = 3;
        while ((mHost->mSoundPoolCallBack == NULL) && (attempts-- > 0)) {
            // try {
            // Wait for mHost->mSoundPoolCallBack to be set by the other thread
            mHost->mSoundEffectsLock.Wait(SOUND_EFFECTS_LOAD_TIMEOUT_MS);
            // } catch (InterruptedException e) {
            //     Log.w(TAG, "Interrupted while waiting sound pool listener thread.");
            // }
        }

        if (mHost->mSoundPoolCallBack == NULL) {
            Logger::W(TAG, "onLoadSoundEffects() SoundPool listener or thread creation error");
            if (mHost->mSoundPoolLooper != NULL) {
                mHost->mSoundPoolLooper->Quit();
                mHost->mSoundPoolLooper = NULL;
            }
            mHost->mSoundPoolListenerThread = NULL;
            mHost->mSoundPool->ReleaseResources();
            mHost->mSoundPool = NULL;
            return FALSE;
        }
        /*
         * poolId table: The value -1 in this table indicates that corresponding
         * file (same index in SOUND_EFFECT_FILES[] has not been loaded.
         * Once loaded, the value in poolId is the sample ID and the same
         * sample can be reused for another effect using the same file.
         */
        Int32 size;
        SOUND_EFFECT_FILES->GetSize(&size);
        AutoPtr<ArrayOf<Int32> > poolId = ArrayOf<Int32>::Alloc(size);
        for (Int32 fileIdx = 0; fileIdx < size; fileIdx++) {
            poolId->Set(fileIdx, -1);
        }
        /*
         * Effects whose value in SOUND_EFFECT_FILES_MAP[effect][1] is -1 must be loaded.
         * If load succeeds, value in SOUND_EFFECT_FILES_MAP[effect][1] is > 0:
         * this indicates we have a valid sample loaded for this effect.
         */

        Int32 numSamples = 0;
        for (Int32 effect = 0; effect < IAudioManager::NUM_SOUND_EFFECTS; effect++) {
            // Do not load sample if this effect uses the MediaPlayer
            if (mHost->SOUND_EFFECT_FILES_MAP[effect][1] == 0) {
                continue;
            }
            if ((*poolId)[mHost->SOUND_EFFECT_FILES_MAP[effect][0]] == -1) {
                AutoPtr<IFile> file = Environment::GetRootDirectory();
                String str;
                file->ToString(&str);
                AutoPtr<IInterface> obj;
                SOUND_EFFECT_FILES->Get(mHost->SOUND_EFFECT_FILES_MAP[effect][0], (IInterface**)&obj);
                String s = Object::ToString(obj);
                String filePath = str + SOUND_EFFECTS_PATH + s;

                Int32 sampleId;
                mHost->mSoundPool->Load(filePath, 0, &sampleId);
                if (sampleId <= 0) {
                    Logger::W(TAG, "Soundpool could not load file: %s", filePath.string());
                }
                else {
                    mHost->SOUND_EFFECT_FILES_MAP[effect][1] = sampleId;
                    poolId->Set(mHost->SOUND_EFFECT_FILES_MAP[effect][0], sampleId);
                    numSamples++;
                }
            }
            else {
                mHost->SOUND_EFFECT_FILES_MAP[effect][1] =
                        (*poolId)[mHost->SOUND_EFFECT_FILES_MAP[effect][0]];
            }
        }
        // wait for all samples to be loaded
        if (numSamples > 0) {
            mHost->mSoundPoolCallBack->SetSamples(poolId);

            attempts = 3;
            status = 1;
            while ((status == 1) && (attempts-- > 0)) {
                // try {
                mHost->mSoundEffectsLock.Wait(SOUND_EFFECTS_LOAD_TIMEOUT_MS);
                mHost->mSoundPoolCallBack->Status(&status);
                // } catch (InterruptedException e) {
                //     Log.w(TAG, "Interrupted while waiting sound pool callback.");
                // }
            }
        }
        else {
            status = -1;
        }

        if (mHost->mSoundPoolLooper != NULL) {
            mHost->mSoundPoolLooper->Quit();
            mHost->mSoundPoolLooper = NULL;
        }
        mHost->mSoundPoolListenerThread = NULL;
        if (status != 0) {
            Logger::W(TAG,
                    "onLoadSoundEffects(), Error %d while loading samples", status);
            for (Int32 effect = 0; effect < IAudioManager::NUM_SOUND_EFFECTS; effect++) {
                if (mHost->SOUND_EFFECT_FILES_MAP[effect][1] > 0) {
                    mHost->SOUND_EFFECT_FILES_MAP[effect][1] = -1;
                }
            }

            mHost->mSoundPool->ReleaseResources();
            mHost->mSoundPool = NULL;
        }
    }
    return (status == 0);
}

void CAudioService::AudioHandler::OnUnloadSoundEffects()
{
    synchronized(mHost->mSoundEffectsLock) {
        if (mHost->mSoundPool == NULL) {
            return;
        }

        Int32 size;
        SOUND_EFFECT_FILES->GetSize(&size);
        AutoPtr<ArrayOf<Int32> > poolId = ArrayOf<Int32>::Alloc(size);

        for (Int32 fileIdx = 0; fileIdx < size; fileIdx++) {
            poolId->Set(fileIdx, 0);
        }

        for (Int32 effect = 0; effect < IAudioManager::NUM_SOUND_EFFECTS; effect++) {
            if (mHost->SOUND_EFFECT_FILES_MAP[effect][1] <= 0) {
                continue;
            }
            if ((*poolId)[mHost->SOUND_EFFECT_FILES_MAP[effect][0]] == 0) {
                Boolean b;
                mHost->mSoundPool->Unload(mHost->SOUND_EFFECT_FILES_MAP[effect][1], &b);
                mHost->SOUND_EFFECT_FILES_MAP[effect][1] = -1;
                poolId->Set(mHost->SOUND_EFFECT_FILES_MAP[effect][0], -1);
            }
        }
        mHost->mSoundPool->ReleaseResources();
        mHost->mSoundPool = NULL;
    }
}

void CAudioService::AudioHandler::OnPlaySoundEffect(
    /* [in] */ Int32 effectType,
    /* [in] */ Int32 volume)
{
    synchronized(mHost->mSoundEffectsLock) {

        OnLoadSoundEffects();

        if (mHost->mSoundPool == NULL) {
            return;
        }
        Float volFloat;
        // use default if volume is not specified by caller
        if (volume < 0) {
            volFloat = (Float)Elastos::Core::Math::Pow(10, (Float)sSoundEffectVolumeDb/20);
        }
        else {
            volFloat = volume / 1000.0f;
        }

        if (mHost->SOUND_EFFECT_FILES_MAP[effectType][1] > 0) {
            Int32 result;
            mHost->mSoundPool->Play(
                mHost->SOUND_EFFECT_FILES_MAP[effectType][1],
                volFloat, volFloat, 0, 0, 1.0f, &result);
        }
        else {
            AutoPtr<IMediaPlayer> mediaPlayer;
            CMediaPlayer::New((IMediaPlayer**)&mediaPlayer);
            // try {
            AutoPtr<IFile> file = Environment::GetRootDirectory();
            String str;
            file->ToString(&str);
            AutoPtr<IInterface> obj;
            SOUND_EFFECT_FILES->Get(
                mHost->SOUND_EFFECT_FILES_MAP[effectType][0], (IInterface**)&obj);
            String s = Object::ToString(obj);
            String filePath = str + SOUND_EFFECTS_PATH + s;
            mediaPlayer->SetDataSource(filePath);
            mediaPlayer->SetAudioStreamType(IAudioSystem::STREAM_SYSTEM);
            mediaPlayer->Prepare();
            mediaPlayer->SetVolume(volFloat);

            AutoPtr<AudioHandlerOnCompletionListener> l =
                    new AudioHandlerOnCompletionListener(this);
            mediaPlayer->SetOnCompletionListener(l);

            AutoPtr<AudioHandlerOnErrorListener> listener =
                    new AudioHandlerOnErrorListener(this);
            mediaPlayer->SetOnErrorListener(listener);

            mediaPlayer->Start();
            // } catch (IOException ex) {
            //     Log.w(TAG, "MediaPlayer IOException: "+ex);
            // } catch (IllegalArgumentException ex) {
            //     Log.w(TAG, "MediaPlayer IllegalArgumentException: "+ex);
            // } catch (IllegalStateException ex) {
            //     Log.w(TAG, "MediaPlayer IllegalStateException: "+ex);
            // }
        }
    }
}

void CAudioService::AudioHandler::CleanupPlayer(
    /* [in] */ IMediaPlayer* mp)
{
    if (mp != NULL) {
        // try {
        mp->Stop();
        mp->ReleaseResources();
        // } catch (IllegalStateException ex) {
        //     Log.w(TAG, "MediaPlayer IllegalStateException: "+ex);
        // }
    }
}

void CAudioService::AudioHandler::SetForceUse(
    /* [in] */ Int32 usage,
    /* [in] */ Int32 config)
{
    AudioSystem::SetForceUse(usage, config);
}

void CAudioService::AudioHandler::OnPersistSafeVolumeState(
    /* [in] */ Int32 state)
{
    Boolean b;
    Settings::Global::PutInt32(mHost->mContentResolver,
            ISettingsGlobal::AUDIO_SAFE_VOLUME_STATE,
            state, &b);
}

//==============================================================================
//  CAudioService::SettingsObserver
//==============================================================================

CAudioService::SettingsObserver::SettingsObserver(
    /* [in] */ CAudioService* host)
    : mHost(host)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    ContentObserver::constructor(handler);
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(
        ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED, (IUri**)&uri);
    mHost->mContentResolver->RegisterContentObserver(uri, FALSE, this);

    uri = NULL;
    Settings::Global::GetUriFor(
        ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED, (IUri**)&uri);
    mHost->mContentResolver->RegisterContentObserver(uri, FALSE, this);
}

ECode CAudioService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);
    // FIXME This synchronized is not necessary if mSettingsLock only protects mRingerMode.
    //       However there appear to be some missing locks around mRingerModeMutedStreams
    //       and mRingerModeAffectedStreams, so will leave this synchronized for now.
    //       mRingerModeMutedStreams and mMuteAffectedStreams are safe (only accessed once).
    Object& lock = mHost->mSettingsLock;
    synchronized(lock) {
        Boolean b;
        if (mHost->UpdateRingerModeAffectedStreams(&b), b) {
            /*
             * Ensure all stream types that should be affected by ringer mode
             * are in the proper state.
             */
            Int32 mode;
            mHost->GetRingerMode(&mode);
            mHost->SetRingerModeInt(mode, FALSE);
        }
        mHost->ReadDockAudioSettings(mHost->mContentResolver);

        Int32 resTmp = 0;
        Settings::Secure::GetInt32(mHost->mContentResolver, ISettingsSecure::VOLUME_LINK_NOTIFICATION, 1, &resTmp);
        mHost->mLinkNotificationWithVolume = (resTmp == 1);
        if (mHost->mLinkNotificationWithVolume) {
            (*mHost->mStreamVolumeAlias)[IAudioSystem::STREAM_NOTIFICATION] = IAudioSystem::STREAM_RING;
        }
        else {
            (*mHost->mStreamVolumeAlias)[IAudioSystem::STREAM_NOTIFICATION] = IAudioSystem::STREAM_NOTIFICATION;
        }
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::AudioServiceBroadcastReceiver
//==============================================================================

ECode CAudioService::AudioServiceBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 outDevice;
    Int32 inDevice;
    Int32 state;

    if (action.Equals(IIntent::ACTION_DOCK_EVENT)) {
        Int32 dockState;
        intent->GetInt32Extra(IIntent::EXTRA_DOCK_STATE,
                IIntent::EXTRA_DOCK_STATE_UNDOCKED, &dockState);
        Int32 config;
        switch (dockState) {
            case IIntent::EXTRA_DOCK_STATE_DESK:
                config = IAudioSystem::FORCE_BT_DESK_DOCK;
                break;
            case IIntent::EXTRA_DOCK_STATE_CAR:
                config = IAudioSystem::FORCE_BT_CAR_DOCK;
                break;
            case IIntent::EXTRA_DOCK_STATE_LE_DESK:
                config = IAudioSystem::FORCE_ANALOG_DOCK;
                break;
            case IIntent::EXTRA_DOCK_STATE_HE_DESK:
                config = IAudioSystem::FORCE_DIGITAL_DOCK;
                break;
            case IIntent::EXTRA_DOCK_STATE_UNDOCKED:
            default:
                config = IAudioSystem::FORCE_NONE;
        }
        // Low end docks have a menu to enable or disable audio
        // (see mDockAudioMediaEnabled)
        if (!((dockState == IIntent::EXTRA_DOCK_STATE_LE_DESK) ||
              ((dockState == IIntent::EXTRA_DOCK_STATE_UNDOCKED) &&
               (mHost->mDockState == IIntent::EXTRA_DOCK_STATE_LE_DESK)))) {
            AudioSystem::SetForceUse(IAudioSystem::FOR_DOCK, config);
        }
        mHost->mDockState = dockState;
    }
    else if (action.Equals(IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED)) {
        intent->GetInt32Extra(IBluetoothProfile::EXTRA_STATE,
                IBluetoothProfile::STATE_DISCONNECTED, &state);
        outDevice = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
        inDevice = IAudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET;
        String address;

        AutoPtr<IParcelable> p;
        intent->GetParcelableExtra(IBluetoothDevice::EXTRA_DEVICE, (IParcelable**)&p);
        AutoPtr<IBluetoothDevice> btDevice = IBluetoothDevice::Probe(p);
        if (btDevice == NULL) {
            return NOERROR;
        }

        btDevice->GetAddress(&address);
        AutoPtr<IBluetoothClass> btClass;
        btDevice->GetBluetoothClass((IBluetoothClass**)&btClass);
        if (btClass != NULL) {
            Int32 cls;
            btClass->GetDeviceClass(&cls);
            switch (cls) {
            case IBluetoothClassDevice::AUDIO_VIDEO_WEARABLE_HEADSET:
            case IBluetoothClassDevice::AUDIO_VIDEO_HANDSFREE:
                outDevice = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                break;
            case IBluetoothClassDevice::AUDIO_VIDEO_CAR_AUDIO:
                outDevice = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                break;
            }
        }

        Boolean b;
        b = CBluetoothAdapter::CheckBluetoothAddress(address);
        if (!b) {
            address = String("");
        }

        Boolean connected;
        connected = (state == IBluetoothProfile::STATE_CONNECTED);
        Boolean success = mHost->HandleDeviceConnection(connected, outDevice, address) &&
                              mHost->HandleDeviceConnection(connected, inDevice, address);
        if (success) {
            AutoPtr<IArrayList> lock = mHost->mScoClients;
            synchronized(lock) {
                if (connected) {
                    mHost->mBluetoothHeadsetDevice = btDevice;
                }
                else {
                    mHost->mBluetoothHeadsetDevice = NULL;
                    mHost->ResetBluetoothSco();
                }
            }
        }
    }
    else if (action.Equals(IAudioManager::ACTION_USB_AUDIO_ACCESSORY_PLUG)) {
        intent->GetInt32Extra(String("state"), 0, &state);

        Int32 alsaCard;
        intent->GetInt32Extra(String("card"), -1, &alsaCard);
        Int32 alsaDevice;
        intent->GetInt32Extra(String("device"), -1, &alsaDevice);

        String params = (alsaCard == -1 && alsaDevice == -1 ? String("")
                : String("card=") + StringUtils::ToString(alsaCard) + ";device=" + StringUtils::ToString(alsaDevice));

        // Playback Device
        outDevice = IAudioSystem::DEVICE_OUT_USB_ACCESSORY;
        mHost->SetWiredDeviceConnectionState(outDevice, state, params);
    }
    else if (action.Equals(IAudioManager::ACTION_USB_AUDIO_DEVICE_PLUG)) {
        // FIXME Does not yet handle the case where the setting is changed
        // after device connection.  Ideally we should handle the settings change
        // in SettingsObserver. Here we should log that a USB device is connected
        // and disconnected with its address (card , device) and force the
        // connection or disconnection when the setting changes.
        Int32 isDisabled;
        Settings::Secure::GetInt32(mHost->mContentResolver,
                ISettingsSecure::USB_AUDIO_AUTOMATIC_ROUTING_DISABLED, 0, &isDisabled);
        if (isDisabled != 0) {
            return NOERROR;
        }

        intent->GetInt32Extra(String("state"), 0, &state);

        Int32 alsaCard;
        intent->GetInt32Extra(String("card"), -1, &alsaCard);
        Int32 alsaDevice;
        intent->GetInt32Extra(String("device"), -1, &alsaDevice);
        Boolean hasPlayback;
        intent->GetBooleanExtra(String("hasPlayback"), FALSE, &hasPlayback);
        Boolean hasCapture;
        intent->GetBooleanExtra(String("hasCapture"), FALSE, &hasCapture);
        Boolean hasMIDI;
        intent->GetBooleanExtra(String("hasMIDI"), FALSE, &hasMIDI);

        String params = (alsaCard == -1 && alsaDevice == -1 ? String("")
                : String("card=") + StringUtils::ToString(alsaCard) + ";device=" + StringUtils::ToString(alsaDevice));

        // Playback Device
        if (hasPlayback) {
            outDevice = IAudioSystem::DEVICE_OUT_USB_DEVICE;
            mHost->SetWiredDeviceConnectionState(outDevice, state, params);
        }

        // Capture Device
        if (hasCapture) {
            inDevice = IAudioSystem::DEVICE_IN_USB_DEVICE;
            mHost->SetWiredDeviceConnectionState(inDevice, state, params);
        }
    }
    else if (action.Equals(IBluetoothHeadset::ACTION_AUDIO_STATE_CHANGED)) {
        Boolean broadcast = FALSE;
        Int32 scoAudioState = IAudioManager::SCO_AUDIO_STATE_ERROR;
        AutoPtr<IArrayList> lock = mHost->mScoClients;
        synchronized(lock) {
            Int32 btState;
            intent->GetInt32Extra(IBluetoothProfile::EXTRA_STATE, -1, &btState);
            // broadcast intent if the connection was initated by AudioService
            Boolean b;
            if ((mHost->mScoClients->IsEmpty(&b), !b) &&
                    (mHost->mScoAudioState == SCO_STATE_ACTIVE_INTERNAL ||
                     mHost->mScoAudioState == SCO_STATE_ACTIVATE_REQ ||
                     mHost->mScoAudioState == SCO_STATE_DEACTIVATE_REQ)) {
                broadcast = TRUE;
            }
            switch (btState) {
            case IBluetoothHeadset::STATE_AUDIO_CONNECTED:
                scoAudioState = IAudioManager::SCO_AUDIO_STATE_CONNECTED;
                if (mHost->mScoAudioState != SCO_STATE_ACTIVE_INTERNAL &&
                    mHost->mScoAudioState != SCO_STATE_DEACTIVATE_REQ &&
                    mHost->mScoAudioState != SCO_STATE_DEACTIVATE_EXT_REQ) {
                    mHost->mScoAudioState = SCO_STATE_ACTIVE_EXTERNAL;
                }
                break;
            case IBluetoothHeadset::STATE_AUDIO_DISCONNECTED:
                scoAudioState = IAudioManager::SCO_AUDIO_STATE_DISCONNECTED;
                mHost->mScoAudioState = SCO_STATE_INACTIVE;
                mHost->ClearAllScoClients(0, FALSE);
                break;
            case IBluetoothHeadset::STATE_AUDIO_CONNECTING:
                if (mHost->mScoAudioState != SCO_STATE_ACTIVE_INTERNAL &&
                    mHost->mScoAudioState != SCO_STATE_DEACTIVATE_REQ &&
                    mHost->mScoAudioState != SCO_STATE_DEACTIVATE_EXT_REQ) {
                    mHost->mScoAudioState = SCO_STATE_ACTIVE_EXTERNAL;
                }
            default:
                // do not broadcast CONNECTING or invalid state
                broadcast = FALSE;
                break;
            }
        }
        if (broadcast) {
            mHost->BroadcastScoConnectionState(scoAudioState);
            //FIXME: this is to maintain compatibility with deprecated intent
            // IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED. Remove when appropriate.
            AutoPtr<IIntent> newIntent;
            CIntent::New(IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED, (IIntent**)&newIntent);
            newIntent->PutExtra(IAudioManager::EXTRA_SCO_AUDIO_STATE, scoAudioState);
            mHost->SendStickyBroadcastToAll(newIntent);
        }
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        if (mHost->mMonitorRotation) {
            mHost->mOrientationListener->OnOrientationChanged(0); //argument is ignored anyway
            mHost->mOrientationListener->Enable();
        }
        AudioSystem::SetParameters(String("screen_state=on"));
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        if (mHost->mMonitorRotation) {
            //reduce wakeups (save current) by only listening when display is on
            mHost->mOrientationListener->Disable();
        }
        AudioSystem::SetParameters(String("screen_state=off"));
    }
    else if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        mHost->HandleConfigurationChanged(context);
    }
    else if (action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        // attempt to stop music playback for background user
        SendMsg(mHost->mAudioHandler,
                MSG_BROADCAST_AUDIO_BECOMING_NOISY,
                SENDMSG_REPLACE,
                0,
                0,
                NULL,
                0);
        // the current audio focus owner is no longer valid
        mHost->mMediaFocusControl->DiscardAudioFocusOwner();

        // load volume settings for new user
        mHost->ReadAudioSettings(TRUE /*userSwitch*/);
        // preserve STREAM_MUSIC volume from one user to the next.
        SendMsg(mHost->mAudioHandler,
                MSG_SET_ALL_VOLUMES,
                SENDMSG_QUEUE,
                0,
                0,
                (IInterface*)(IObject*)(*mHost->mStreamStates)[IAudioSystem::STREAM_MUSIC], 0);
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::MyDisplayStatusCallback
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::MyDisplayStatusCallback, \
        Object, IHdmiPlaybackClientDisplayStatusCallback)

ECode CAudioService::MyDisplayStatusCallback::OnComplete(
    /* [in] */ Int32 status)
{
    if (mHost->mHdmiManager != NULL) {
        AutoPtr<IHdmiControlManager> lock = mHost->mHdmiManager;
        synchronized(lock) {
            mHost->mHdmiCecSink = (status != IHdmiControlManager::POWER_STATUS_UNKNOWN);
            // Television devices without CEC service apply software volume on HDMI output
            if (mHost->IsPlatformTelevision() && !mHost->mHdmiCecSink) {
                mHost->mFixedVolumeDevices &= ~IAudioSystem::DEVICE_OUT_HDMI;
            }
            mHost->CheckAllFixedVolumeDevices();
        }
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::StreamOverride
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::StreamOverride, \
        Object, IAccessibilityManagerTouchExplorationStateChangeListener)

Int32 CAudioService::StreamOverride::sDelayMs = 0;
const Int32 CAudioService::StreamOverride::DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS = 5000;
const Int32 CAudioService::StreamOverride::TOUCH_EXPLORE_STREAM_TYPE_OVERRIDE_DELAY_MS = 1000;

ECode CAudioService::StreamOverride::OnTouchExplorationStateChanged(
    /* [in] */ Boolean enabled)
{
    UpdateDefaultStreamOverrideDelay(enabled);
    return NOERROR;
}

ECode CAudioService::StreamOverride::Init(
    /* [in] */ IContext* ctxt)
{
    AutoPtr<IInterface> service;
    ctxt->GetSystemService(IContext::ACCESSIBILITY_SERVICE, ((IInterface**)&service));
    IAccessibilityManager* accessibilityManager = IAccessibilityManager::Probe(service);
    if (accessibilityManager) {
        Boolean b;
        accessibilityManager->IsTouchExplorationEnabled(&b);

        UpdateDefaultStreamOverrideDelay(b);

        AutoPtr<StreamOverride> so = new StreamOverride();
        accessibilityManager->AddTouchExplorationStateChangeListener(so, &b);
    }

    return NOERROR;
}

void CAudioService::StreamOverride::UpdateDefaultStreamOverrideDelay(
    /* [in] */ Boolean touchExploreEnabled)
{
    if (touchExploreEnabled) {
        sDelayMs = TOUCH_EXPLORE_STREAM_TYPE_OVERRIDE_DELAY_MS;
    }
    else {
        sDelayMs = DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS;
    }
    if (DEBUG_VOL) Logger::D(TAG, "Touch exploration enabled=%d stream override delay is now %d ms",
            touchExploreEnabled, sDelayMs);
}

//==============================================================================
//  CAudioService::MyAudioSystemCallback
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::MyAudioSystemCallback,
        Object, IAudioSystemErrorCallback)

ECode CAudioService::MyAudioSystemCallback::OnError(
    /* [in] */ Int32 error)
{
    switch (error) {
    case IAudioSystem::AUDIO_STATUS_SERVER_DIED:
        SendMsg(mHost->mAudioHandler, MSG_MEDIA_SERVER_DIED,
                SENDMSG_NOOP, 0, 0, NULL, 0);
        break;
    default:
        break;
    }
    return NOERROR;
}

//==============================================================================
//  CAudioService::AudioHandlerOnCompletionListener
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::AudioHandlerOnCompletionListener,
        Object, IMediaPlayerOnCompletionListener)

ECode CAudioService::AudioHandlerOnCompletionListener::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    mHost->CleanupPlayer(mp);
    return NOERROR;
}

//==============================================================================
//  CAudioService::AudioHandlerOnErrorListener
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::AudioHandlerOnErrorListener,
        Object, IMediaPlayerOnErrorListener)

ECode CAudioService::AudioHandlerOnErrorListener::OnError(
    /* [in] */ IMediaPlayer* mp,
    /* [in] */ Int32 what,
    /* [in] */ Int32 extra,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->CleanupPlayer(mp);
    *result = TRUE;
    return NOERROR;
}

//==============================================================================
//  CAudioService::SetVolumeControllerDeathRecipient
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::SetVolumeControllerDeathRecipient,
        Object, IProxyDeathRecipient)

ECode CAudioService::SetVolumeControllerDeathRecipient::ProxyDied()
{
    Boolean b;
    if (mHost->mVolumeController->IsSameBinder(mController, &b), b) {
        Logger::W(TAG, "Current remote volume controller died, unregistering");
        mHost->SetVolumeController(NULL);
    }
    return NOERROR;
}

//==============================================================================
//                        CAudioService::MediaPlayerInfo
//==============================================================================
CAudioService::MediaPlayerInfo::MediaPlayerInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean isfocussed)
    : mPackageName(packageName)
    , mIsfocussed(isfocussed)
{
}

ECode CAudioService::MediaPlayerInfo::IsFocussed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsfocussed;
    return NOERROR;
}

ECode CAudioService::MediaPlayerInfo::SetFocus(
    /* [in] */ Boolean focus)
{
    mIsfocussed = focus;
    return NOERROR;
}

ECode CAudioService::MediaPlayerInfo::GetPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPackageName;
    return NOERROR;
}

//==============================================================================
//                        CAudioService::BluetoothProfileServiceListener
//==============================================================================

CAR_INTERFACE_IMPL(CAudioService::BluetoothProfileServiceListener,
        Object, IBluetoothProfileServiceListener)

ECode CAudioService::BluetoothProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    AutoPtr<IBluetoothDevice> btDevice;
    AutoPtr<IList> deviceList;
    switch(profile) {
    case IBluetoothProfile::A2DP:
        synchronized(mHost->mA2dpAvrcpLock) {
            mHost->mA2dp = IBluetoothA2dp::Probe(proxy);
            IBluetoothProfile::Probe(mHost->mA2dp)->GetConnectedDevices((IList**)&deviceList);
            Int32 size;
            deviceList->GetSize(&size);
            if (size > 0) {
                AutoPtr<IInterface> obj;
                deviceList->Get(0, (IInterface**)&obj);
                btDevice = IBluetoothDevice::Probe(obj);
                synchronized(mHost->mConnectedDevices) {
                    Int32 state;
                    IBluetoothProfile::Probe(mHost->mA2dp)->GetConnectionState(btDevice, &state);
                    Int32 delay = mHost->CheckSendBecomingNoisyIntent(
                            IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
                            (state == IBluetoothProfile::STATE_CONNECTED) ? 1 : 0);
                    mHost->QueueMsgUnderWakeLock(mHost->mAudioHandler,
                            MSG_SET_A2DP_SINK_CONNECTION_STATE,
                            state,
                            0,
                            btDevice,
                            delay);
                }
            }
        }
        break;

    case IBluetoothProfile::A2DP_SINK:
        proxy->GetConnectedDevices((IList**)&deviceList);
        Int32 size;
        deviceList->GetSize(&size);
        if (size > 0) {
            AutoPtr<IInterface> obj;
            deviceList->Get(0, (IInterface**)&obj);
            btDevice = IBluetoothDevice::Probe(obj);
            synchronized(mHost->mConnectedDevices) {
                Int32 state;
                proxy->GetConnectionState(btDevice, &state);
                mHost->QueueMsgUnderWakeLock(mHost->mAudioHandler,
                        MSG_SET_A2DP_SRC_CONNECTION_STATE,
                        state,
                        0,
                        btDevice,
                        0 /* delay */);
            }
        }
        break;

    case IBluetoothProfile::HEADSET:
        synchronized(mHost->mScoClients) {
            // Discard timeout message
            mHost->mAudioHandler->RemoveMessages(MSG_BT_HEADSET_CNCT_FAILED);
            mHost->mBluetoothHeadset = IBluetoothHeadset::Probe(proxy);
            IBluetoothProfile::Probe(mHost->mBluetoothHeadset)->GetConnectedDevices(
                    (IList**)&deviceList);
            Int32 size;
            deviceList->GetSize(&size);
            if (size > 0) {
                AutoPtr<IInterface> obj;
                deviceList->Get(0, (IInterface**)&obj);
                mHost->mBluetoothHeadsetDevice = IBluetoothDevice::Probe(obj);
            }
            else {
                mHost->mBluetoothHeadsetDevice = NULL;
            }
            // Refresh SCO audio state
            mHost->CheckScoAudioState();
            // Continue pending action if any
            if (mHost->mScoAudioState == SCO_STATE_ACTIVATE_REQ ||
                    mHost->mScoAudioState == SCO_STATE_DEACTIVATE_REQ ||
                    mHost->mScoAudioState == SCO_STATE_DEACTIVATE_EXT_REQ) {
                Boolean status = FALSE;
                if (mHost->mBluetoothHeadsetDevice != NULL) {
                    switch (mHost->mScoAudioState) {
                    case SCO_STATE_ACTIVATE_REQ:
                        mHost->mScoAudioState = SCO_STATE_ACTIVE_INTERNAL;
                        if (mHost->mScoAudioMode == SCO_MODE_VR) {
                            mHost->mBluetoothHeadset->StartVoiceRecognition(
                                    mHost->mBluetoothHeadsetDevice, &status);
                        }
                        else {
                            mHost->mBluetoothHeadset->StartScoUsingVirtualVoiceCall(
                                    mHost->mBluetoothHeadsetDevice, &status);
                        }
                        break;
                    case SCO_STATE_DEACTIVATE_REQ:
                        if (mHost->mScoAudioMode == SCO_MODE_VR) {
                            mHost->mBluetoothHeadset->StopVoiceRecognition(
                                mHost->mBluetoothHeadsetDevice, &status);
                        }
                        else {
                            mHost->mBluetoothHeadset->StopScoUsingVirtualVoiceCall(
                                    mHost->mBluetoothHeadsetDevice, &status);
                        }
                        break;
                    case SCO_STATE_DEACTIVATE_EXT_REQ:
                        mHost->mBluetoothHeadset->StopVoiceRecognition(
                                mHost->mBluetoothHeadsetDevice, &status);
                    }
                }
                if (!status) {
                    SendMsg(mHost->mAudioHandler, MSG_BT_HEADSET_CNCT_FAILED,
                            SENDMSG_REPLACE, 0, 0, NULL, 0);
                }
            }
        }
        break;

    default:
        break;
    }

    return NOERROR;
}

ECode CAudioService::BluetoothProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    Logger::D(TAG, "onServiceDisconnected: Bluetooth profile: %d", profile);
    switch(profile) {
    case IBluetoothProfile::A2DP:
        synchronized(mHost->mA2dpAvrcpLock) {
            mHost->mA2dp = NULL;
            synchronized(mHost->mConnectedDevices) {
                AutoPtr<IInteger32> i32;
                CInteger32::New(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, (IInteger32**)&i32);
                Boolean b;
                if (mHost->mConnectedDevices->ContainsKey(i32, &b), b) {
                    Logger::D(TAG, "A2dp service disconnects, pause music player");
                    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
                    AutoPtr<IInterface> obj;
                    mHost->mConnectedDevices->Get(i32, (IInterface**)&obj);
                    String str;
                    ICharSequence::Probe(obj)->ToString(&str);
                    AutoPtr<IBluetoothDevice> btDevice;
                    adapter->GetRemoteDevice(str, (IBluetoothDevice**)&btDevice);
                    Int32 delay = mHost->CheckSendBecomingNoisyIntent(
                            IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, 0);
                    mHost->QueueMsgUnderWakeLock(mHost->mAudioHandler,
                                        MSG_SET_A2DP_SRC_CONNECTION_STATE,
                                        IBluetoothProfile::STATE_DISCONNECTED,
                                        0,
                                        btDevice,
                                        delay);
                }
            }
        }
        break;

    case IBluetoothProfile::A2DP_SINK:
        synchronized(mHost->mConnectedDevices) {
            AutoPtr<IInteger32> i32;
            CInteger32::New(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP, (IInteger32**)&i32);
            Boolean b;
            if (mHost->mConnectedDevices->ContainsKey(i32, &b), b) {
                AutoPtr<IInterface> obj;
                mHost->mConnectedDevices->Get(i32, (IInterface**)&obj);
                String str;
                ICharSequence::Probe(obj)->ToString(&str);
                mHost->MakeA2dpSrcUnavailable(str);
            }
        }
        break;

    case IBluetoothProfile::HEADSET:
        synchronized(mHost->mScoClients) {
            mHost->mBluetoothHeadset = NULL;
            synchronized(mHost->mConnectedDevices) {
                if (mHost->mForcedUseForComm == IAudioSystem::FORCE_BT_SCO) {
                    Logger::D(TAG, "Hfp service disconnects, update device to NONE");
                    mHost->mForcedUseForComm = IAudioSystem::FORCE_NONE;
                    SendMsg(mHost->mAudioHandler, MSG_SET_FORCE_USE, SENDMSG_QUEUE,
                            IAudioSystem::FOR_COMMUNICATION, mHost->mForcedUseForComm, NULL, 0);
                    SendMsg(mHost->mAudioHandler, MSG_SET_FORCE_USE, SENDMSG_QUEUE,
                            IAudioSystem::FOR_RECORD, mHost->mForcedUseForComm, NULL, 0);
                }
            }
        }
        break;

    default:
        break;
    }

    return NOERROR;
}

//==============================================================================
//  CAudioService
//==============================================================================

CAR_INTERFACE_IMPL_2(CAudioService, Object, IAudioService, IIAudioService)

CAR_OBJECT_IMPL(CAudioService)

CAudioService::CAudioService()
    : mFullVolumeDevices(0)
    , mPlatformType(0)
    , mMode(IAudioSystem::MODE_NORMAL)
    , mUseFixedVolume(FALSE)
    , mRingerMode(0)
    , mRingerModeAffectedStreams(0)
    , mRingerModeMutedStreams(0)
    , mMuteAffectedStreams(0)
    , mVibrateSetting(0)
    , mHasVibrator(FALSE)
    , mForcedUseForComm(0)
    , mUseMasterVolume(FALSE)
    , mScoAudioState(0)
    , mScoAudioMode(0)
    , mScoConnectionState(0)
    , mSystemReady(FALSE)
    , mPrevVolDirection(IAudioManager::ADJUST_SAME)
    , mVolumeControlStream(-1)
    , mDeviceOrientation(IConfiguration::ORIENTATION_UNDEFINED)
    , mDeviceRotation(ISurface::ROTATION_0)
    , mBluetoothA2dpEnabled(FALSE)
    , mMonitorOrientation(FALSE)
    , mMonitorRotation(FALSE)
    , mDockAudioMediaEnabled(TRUE)
    , mDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mAvrcpAbsVolSupported(FALSE)
    , mRmtSbmxFullVolRefCount(0)
    , mMcc(0)
    , mSafeMediaVolumeIndex(0)
    , mMusicActiveMs(0)
    , mHdmiSystemAudioSupported(FALSE)
    , mHdmiCecSink(FALSE)
{
    mFixedVolumeDevices = IAudioSystem::DEVICE_OUT_HDMI |
            IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET |
            IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET |
            IAudioSystem::DEVICE_OUT_PROXY |
            IAudioSystem::DEVICE_OUT_HDMI_ARC |
            IAudioSystem::DEVICE_OUT_SPDIF |
            IAudioSystem::DEVICE_OUT_AUX_LINE;

    mBecomingNoisyIntentDevices =
            IAudioSystem::DEVICE_OUT_WIRED_HEADSET | IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE |
            IAudioSystem::DEVICE_OUT_ALL_A2DP | IAudioSystem::DEVICE_OUT_HDMI |
            IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET | IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET |
            IAudioSystem::DEVICE_OUT_ALL_USB | IAudioSystem::DEVICE_OUT_LINE;

    mVolumeController = new VolumeController(this);

    STREAM_VOLUME_ALIAS_VOICE = ArrayOf<Int32>::Alloc(10);
    STREAM_VOLUME_ALIAS_VOICE->Set(0, IAudioSystem::STREAM_VOICE_CALL);      // STREAM_VOICE_CALL
    STREAM_VOLUME_ALIAS_VOICE->Set(1, IAudioSystem::STREAM_RING);            // STREAM_SYSTEM
    STREAM_VOLUME_ALIAS_VOICE->Set(2, IAudioSystem::STREAM_RING);            // STREAM_RING
    STREAM_VOLUME_ALIAS_VOICE->Set(3, IAudioSystem::STREAM_MUSIC);           // STREAM_MUSIC
    STREAM_VOLUME_ALIAS_VOICE->Set(4, IAudioSystem::STREAM_ALARM);           // STREAM_ALARM
    STREAM_VOLUME_ALIAS_VOICE->Set(5, IAudioSystem::STREAM_RING);            // STREAM_NOTIFICATION
    STREAM_VOLUME_ALIAS_VOICE->Set(6, IAudioSystem::STREAM_BLUETOOTH_SCO);   // STREAM_BLUETOOTH_SCO
    STREAM_VOLUME_ALIAS_VOICE->Set(7, IAudioSystem::STREAM_RING);            // STREAM_SYSTEM_ENFORCED
    STREAM_VOLUME_ALIAS_VOICE->Set(8, IAudioSystem::STREAM_RING);            // STREAM_DTMF
    STREAM_VOLUME_ALIAS_VOICE->Set(9, IAudioSystem::STREAM_MUSIC);           // STREAM_TTS

    STREAM_VOLUME_ALIAS_TELEVISION = ArrayOf<Int32>::Alloc(10);
    STREAM_VOLUME_ALIAS_TELEVISION->Set(0, IAudioSystem::STREAM_MUSIC);       // STREAM_VOICE_CALL
    STREAM_VOLUME_ALIAS_TELEVISION->Set(1, IAudioSystem::STREAM_MUSIC);       // STREAM_SYSTEM
    STREAM_VOLUME_ALIAS_TELEVISION->Set(2, IAudioSystem::STREAM_MUSIC);       // STREAM_RING
    STREAM_VOLUME_ALIAS_TELEVISION->Set(3, IAudioSystem::STREAM_MUSIC);       // STREAM_MUSIC
    STREAM_VOLUME_ALIAS_TELEVISION->Set(4, IAudioSystem::STREAM_MUSIC);       // STREAM_ALARM
    STREAM_VOLUME_ALIAS_TELEVISION->Set(5, IAudioSystem::STREAM_MUSIC);       // STREAM_NOTIFICATION
    STREAM_VOLUME_ALIAS_TELEVISION->Set(6, IAudioSystem::STREAM_MUSIC);       // STREAM_BLUETOOTH_SCO
    STREAM_VOLUME_ALIAS_TELEVISION->Set(7, IAudioSystem::STREAM_MUSIC);       // STREAM_SYSTEM_ENFORCED
    STREAM_VOLUME_ALIAS_TELEVISION->Set(8, IAudioSystem::STREAM_MUSIC);       // STREAM_DTMF
    STREAM_VOLUME_ALIAS_TELEVISION->Set(9, IAudioSystem::STREAM_MUSIC);       // STREAM_TTS

    STREAM_VOLUME_ALIAS_DEFAULT = ArrayOf<Int32>::Alloc(10);
    STREAM_VOLUME_ALIAS_DEFAULT->Set(0, IAudioSystem::STREAM_VOICE_CALL);      // STREAM_VOICE_CALL
    STREAM_VOLUME_ALIAS_DEFAULT->Set(1, IAudioSystem::STREAM_RING);            // STREAM_SYSTEM
    STREAM_VOLUME_ALIAS_DEFAULT->Set(2, IAudioSystem::STREAM_RING);            // STREAM_RING
    STREAM_VOLUME_ALIAS_DEFAULT->Set(3, IAudioSystem::STREAM_MUSIC);           // STREAM_MUSIC
    STREAM_VOLUME_ALIAS_DEFAULT->Set(4, IAudioSystem::STREAM_ALARM);           // STREAM_ALARM
    STREAM_VOLUME_ALIAS_DEFAULT->Set(5, IAudioSystem::STREAM_RING);            // STREAM_NOTIFICATION
    STREAM_VOLUME_ALIAS_DEFAULT->Set(6, IAudioSystem::STREAM_BLUETOOTH_SCO);   // STREAM_BLUETOOTH_SCO
    STREAM_VOLUME_ALIAS_DEFAULT->Set(7, IAudioSystem::STREAM_RING);            // STREAM_SYSTEM_ENFORCED
    STREAM_VOLUME_ALIAS_DEFAULT->Set(8, IAudioSystem::STREAM_RING);            // STREAM_DTMF
    STREAM_VOLUME_ALIAS_DEFAULT->Set(9, IAudioSystem::STREAM_MUSIC);           // STREAM_TTS

    mAudioSystemCallback = new MyAudioSystemCallback(this);

    mReceiver = new AudioServiceBroadcastReceiver(this);

    CHashMap::New((IHashMap**)&mConnectedDevices);

    CArrayList::New((IArrayList**)&mSetModeDeathHandlers);

    CArrayList::New((IArrayList**)&mScoClients);

    CAudioRoutesInfo::New((IAudioRoutesInfo**)&mCurAudioRoutes);

    CRemoteCallbackList::New((IRemoteCallbackList**)&mRoutesObservers);

    CArrayList::New((IArrayList**)&mRmtSbmxFullVolDeathHandlers);

    mSafeMediaVolumeDevices = IAudioSystem::DEVICE_OUT_WIRED_HEADSET |
            IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE;

    CHashMap::New((IHashMap**)&mAudioPolicies);

    CBoolean::New(FALSE, (IBoolean**)&mCameraSoundForced);

    mBluetoothProfileServiceListener = new BluetoothProfileServiceListener(this);
}

CAudioService::~CAudioService()
{
}

ECode CAudioService::constructor()
{
    return NOERROR;
}

ECode CAudioService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mContentResolver);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::APP_OPS_SERVICE, ((IInterface**)&service));
    mAppOps = IAppOpsManager::Probe(service);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IPackageManager> pkgM;
    context->GetPackageManager((IPackageManager**)&pkgM);
    Boolean b;
    if (res->GetBoolean(R::bool_::config_voice_capable, &b), b) {
        mPlatformType = PLATFORM_VOICE;
    }
    else if (pkgM->HasSystemFeature(
            IPackageManager::FEATURE_TELEVISION, &b), b) {
        mPlatformType = PLATFORM_TELEVISION;
    }
    else {
        mPlatformType = PLATFORM_DEFAULT;
    }

    service = NULL;
    context->GetSystemService(IContext::POWER_SERVICE, ((IInterface**)&service));
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("handleAudioEvent"),
            (IPowerManagerWakeLock**)&mAudioEventWakeLock);

    service = NULL;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, ((IInterface**)&service));
    AutoPtr<IVibrator> vibrator = IVibrator::Probe(service);

    mHasVibrator = vibrator == NULL ? FALSE : (vibrator->HasVibrator(&b), b);

    // Intialized volume
    SystemProperties::GetInt32(
        String("ro.config.vc_call_vol_steps"),
       (*MAX_STREAM_VOLUME)[IAudioSystem::STREAM_VOICE_CALL],
       &(*MAX_STREAM_VOLUME)[IAudioSystem::STREAM_VOICE_CALL]);

    SystemProperties::GetInt32(
        String("ro.config.media_vol_steps"),
       (*MAX_STREAM_VOLUME)[IAudioSystem::STREAM_MUSIC],
       &(*MAX_STREAM_VOLUME)[IAudioSystem::STREAM_MUSIC]);

    res->GetInteger(
            R::integer::config_soundEffectVolumeDb,
            &sSoundEffectVolumeDb);

    mForcedUseForComm = IAudioSystem::FORCE_NONE;

    CreateAudioSystemThread();

    AutoPtr<ILooper> looper;
    mAudioHandler->GetLooper((ILooper**)&looper);
    mMediaFocusControl = new MediaFocusControl(looper,
            mContext, IAudioServiceVolumeController::Probe(mVolumeController),
            this);

    AudioSystem::SetErrorCallback(mAudioSystemCallback);

    Boolean cameraSoundForced;
    res->GetBoolean(R::bool_::config_camera_sound_forced, &cameraSoundForced);
    mCameraSoundForced = NULL;
    CBoolean::New(cameraSoundForced, (IBoolean**)&mCameraSoundForced);

    SendMsg(mAudioHandler,
        MSG_SET_FORCE_USE, SENDMSG_QUEUE, IAudioSystem::FOR_SYSTEM,
        cameraSoundForced ? IAudioSystem::FORCE_SYSTEM_ENFORCED : IAudioSystem::FORCE_NONE,
        NULL, 0);

    Int32 val;
    Settings::Global::GetInt32(mContentResolver,
            ISettingsGlobal::AUDIO_SAFE_VOLUME_STATE,
            SAFE_MEDIA_VOLUME_NOT_CONFIGURED, &val);
    CInteger32::New(val, (IInteger32**)&mSafeMediaVolumeState);
    // The default safe volume index read here will be replaced by the actual value when
    // the mcc is read by onConfigureSafeVolume()
    res->GetInteger(R::integer::config_safe_media_volume_index, &mSafeMediaVolumeIndex);
    mSafeMediaVolumeIndex *= 10;

    res->GetBoolean(R::bool_::config_useFixedVolume, &mUseFixedVolume);

    // must be called before readPersistedSettings() which needs a valid mStreamVolumeAlias[]
    // array initialized by updateStreamVolumeAlias()
    UpdateStreamVolumeAlias(FALSE /*updateVolumes*/);
    ReadPersistedSettings();
    mSettingsObserver = new SettingsObserver(this);
    CreateStreamStates();

    ReadAndSetLowRamDevice();

    // Call setRingerModeInt() to apply correct mute
    // state on streams affected by ringer mode.
    mRingerModeMutedStreams = 0;
    Int32 mode;
    GetRingerMode(&mode);
    SetRingerModeInt(mode, FALSE);

    // Register for device connection intent broadcasts.
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IBluetoothHeadset::ACTION_AUDIO_STATE_CHANGED,
            (IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED);
    intentFilter->AddAction(IIntent::ACTION_DOCK_EVENT);
    intentFilter->AddAction(IAudioManager::ACTION_USB_AUDIO_ACCESSORY_PLUG);
    intentFilter->AddAction(IAudioManager::ACTION_USB_AUDIO_DEVICE_PLUG);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    intentFilter->AddAction(IUsbManager::ACTION_USB_DEVICE_ATTACHED);

    intentFilter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    // TODO merge orientation and rotation
    SystemProperties::GetBoolean(String("ro.audio.monitorOrientation"),
            FALSE, &mMonitorOrientation);
    if (mMonitorOrientation) {
        Logger::V(TAG, "monitoring device orientation");
        // initialize orientation in AudioSystem
        SetOrientationForAudioSystem();
    }
    SystemProperties::GetBoolean(String("ro.audio.monitorRotation"),
            FALSE, &mMonitorRotation);

    if (mMonitorRotation) {
        service = NULL;
        context->GetSystemService(IContext::WINDOW_SERVICE, ((IInterface**)&service));
        AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
        AutoPtr<IDisplay> dp;
        wm->GetDefaultDisplay((IDisplay**)&dp);
        dp->GetRotation(&mDeviceRotation);
        Logger::V(TAG, "monitoring device rotation, initial=%d", mDeviceRotation);

        mOrientationListener = new AudioOrientationEventListener(this, mContext);
        mOrientationListener->Enable();

        // initialize rotation in AudioSystem
        SetRotationForAudioSystem();
    }

    AutoPtr<IIntent> retIntent;
    context->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&retIntent);

    res->GetBoolean(R::bool_::config_useMasterVolume, &mUseMasterVolume);
    RestoreMasterVolume();

    res->GetInt32Array(R::array::config_masterVolumeRamp,
            (ArrayOf<Int32>**)&mMasterVolumeRamp);

    AutoPtr<AudioServiceInternal> asi = new AudioServiceInternal(this);
    LocalServices::AddService(EIID_IAudioManagerInternal, (IInterface*)(IObject*)asi);
    return NOERROR;
}

ECode CAudioService::SystemReady()
{
    SendMsg(mAudioHandler, MSG_SYSTEM_READY, SENDMSG_QUEUE,
            0, 0, NULL, 0);
    return NOERROR;
}

ECode CAudioService::OnSystemReady()
{
    mSystemReady = TRUE;
    SendMsg(mAudioHandler, MSG_LOAD_SOUND_EFFECTS, SENDMSG_QUEUE,
            0, 0, NULL, 0);

    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::KEYGUARD_SERVICE, ((IInterface**)&service));
    mKeyguardManager = IKeyguardManager::Probe(service);

    mScoConnectionState = IAudioManager::SCO_AUDIO_STATE_ERROR;
    ResetBluetoothSco();
    GetBluetoothHeadset();
    //FIXME: this is to maintain compatibility with deprecated intent
    // AudioManager.ACTION_SCO_AUDIO_STATE_CHANGED. Remove when appropriate.
    AutoPtr<IIntent> newIntent;
    CIntent::New(IAudioManager::ACTION_SCO_AUDIO_STATE_CHANGED, (IIntent**)&newIntent);
    newIntent->PutExtra(IAudioManager::EXTRA_SCO_AUDIO_STATE,
            IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
    SendStickyBroadcastToAll(newIntent);

    AutoPtr<IBluetoothAdapter> adapter;
    adapter = CBluetoothAdapter::GetDefaultAdapter();
    if (adapter != NULL) {
        Boolean b;
        adapter->GetProfileProxy(
            mContext, mBluetoothProfileServiceListener, IBluetoothProfile::A2DP, &b);
    }

    service = NULL;
    mContext->GetSystemService(IContext::HDMI_CONTROL_SERVICE, ((IInterface**)&service));
    mHdmiManager = IHdmiControlManager::Probe(service);

    if (mHdmiManager != NULL) {
        synchronized(mHdmiManager) {
            mHdmiManager->GetTvClient((IHdmiTvClient**)&mHdmiTvClient);
            if (mHdmiTvClient != NULL) {
                mFixedVolumeDevices &= ~IAudioSystem::DEVICE_ALL_HDMI_SYSTEM_AUDIO_AND_SPEAKER;
            }
            mHdmiManager->GetPlaybackClient((IHdmiPlaybackClient**)&mHdmiPlaybackClient);
            mHdmiCecSink = FALSE;
        }
    }

    SendMsg(mAudioHandler, MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED,
        SENDMSG_REPLACE, 0, 0, NULL, SAFE_VOLUME_CONFIGURE_TIMEOUT_MS);

    return StreamOverride::Init(mContext);
}

ECode CAudioService::AddMediaPlayerAndUpdateRemoteController(
    /* [in] */ const String& packageName)
{
    Int32 mediaPlayerSize = 0;
    mMediaPlayers->GetSize(&mediaPlayerSize);
    String strMediaPalyerSize = StringUtils::ToString(mediaPlayerSize);
    Logger::V(TAG, String("addMediaPlayerAndUpdateRemoteController: size of existing list: ") + strMediaPalyerSize);
    Boolean playerToAdd = TRUE;
    if (mediaPlayerSize > 0) {
        AutoPtr<IIterator> rccIterator;
        mMediaPlayers->GetIterator((IIterator**)&rccIterator);
        Boolean hasNext = FALSE;
        while (rccIterator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> interfaceTmp;
            rccIterator->GetNext((IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            MediaPlayerInfo* player = (MediaPlayerInfo*)objTmp;
            String playerPackageName;
            player->GetPackageName(&playerPackageName);
            if (packageName.Equals(playerPackageName)) {
                Logger::E(TAG, "Player entry present, no need to add");
                playerToAdd = FALSE;
                player->SetFocus(TRUE);
            }
            else {
                Logger::E(TAG, String("Player: ") + playerPackageName+ String("Lost Focus"));
                player->SetFocus(FALSE);
            }
        }
    }
    if (playerToAdd) {
        Logger::E(TAG, String("Adding Player: ") + packageName + String(" to available player list"));
        AutoPtr<MediaPlayerInfo> playerInfo = new MediaPlayerInfo(packageName, TRUE);
        mMediaPlayers->Add(TO_IINTERFACE(playerInfo));
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::RCC_CHANGED_ACTION, (IIntent**)&intent);
    intent->PutExtra(IAudioManager::EXTRA_CALLING_PACKAGE_NAME, packageName);
    intent->PutBooleanExtra(IAudioManager::EXTRA_FOCUS_CHANGED_VALUE, TRUE);
    intent->PutBooleanExtra(IAudioManager::EXTRA_AVAILABLITY_CHANGED_VALUE, TRUE);
    SendBroadcastToAll(intent);
    Logger::V(TAG, String("updating focussed RCC change to RCD: CallingPackageName:")
            + packageName);
    return NOERROR;
}

ECode CAudioService::UpdateRemoteControllerOnExistingMediaPlayers()
{
    Int32 mediaPlayerSize = 0;
    mMediaPlayers->GetSize(&mediaPlayerSize);
    String strMediaPalyerSize = StringUtils::ToString(mediaPlayerSize);
    Logger::V(TAG, String("updateRemoteControllerOnExistingMediaPlayers: size of Player list: ") + strMediaPalyerSize);
    if (mediaPlayerSize > 0) {
        Logger::V(TAG, "Inform RemoteController regarding existing RCC entry");
        AutoPtr<IIterator> rccIterator;
        mMediaPlayers->GetIterator((IIterator**)&rccIterator);
        Boolean hasNext = FALSE;
        while (rccIterator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> interfaceTmp;
            rccIterator->GetNext((IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            MediaPlayerInfo* player = (MediaPlayerInfo*)objTmp;

            AutoPtr<IIntent> intent;
            CIntent::New(IAudioManager::RCC_CHANGED_ACTION, (IIntent**)&intent);

            String playerPackageName;
            player->GetPackageName(&playerPackageName);
            intent->PutExtra(IAudioManager::EXTRA_CALLING_PACKAGE_NAME, playerPackageName);

            Boolean isFocussed = FALSE;
            player->IsFocussed(&isFocussed);
            intent->PutBooleanExtra(IAudioManager::EXTRA_FOCUS_CHANGED_VALUE, isFocussed);
            intent->PutBooleanExtra(IAudioManager::EXTRA_AVAILABLITY_CHANGED_VALUE, TRUE);
            SendBroadcastToAll(intent);
            Logger::V(TAG, String("updating RCC change: CallingPackageName:") + playerPackageName);
        }
    }
    else {
        Logger::E(TAG, "No RCC entry present to update");
    }
    return NOERROR;
}

ECode CAudioService::RemoveMediaPlayerAndUpdateRemoteController(
    /* [in] */ const String& packageName)
{
    Int32 mediaPlayerSize = 0;
    mMediaPlayers->GetSize(&mediaPlayerSize);
    String strMediaPalyerSize = StringUtils::ToString(mediaPlayerSize);
    Logger::V(TAG, String("removeMediaPlayerAndUpdateRemoteController: size of existing list: ") + strMediaPalyerSize);
    Boolean playerToRemove = FALSE;
    Int32 index = -1;
    if (mediaPlayerSize > 0) {
        AutoPtr<IIterator> rccIterator;
        mMediaPlayers->GetIterator((IIterator**)&rccIterator);
        Boolean hasNext = FALSE;
        while (rccIterator->HasNext(&hasNext), hasNext) {
            ++index;
            AutoPtr<IInterface> interfaceTmp;
            rccIterator->GetNext((IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            MediaPlayerInfo* player = (MediaPlayerInfo*)objTmp;

            String playerPackageName;
            player->GetPackageName(&playerPackageName);

            if (packageName.Equals(playerPackageName)) {
                Logger::V(TAG, "Player entry present remove and update RemoteController");
                playerToRemove = TRUE;
                break;
            }
            else {
                Logger::V(TAG, String("Player entry for ") + playerPackageName + String(" is not present"));
            }
        }
    }
    if (playerToRemove) {
        Logger::E(TAG, String("Removing Player: ") + packageName + String(" from index") + StringUtils::ToString(index));
        mMediaPlayers->Remove(index);
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::RCC_CHANGED_ACTION, (IIntent**)&intent);

    intent->PutExtra(IAudioManager::EXTRA_CALLING_PACKAGE_NAME, packageName);
    intent->PutBooleanExtra(IAudioManager::EXTRA_FOCUS_CHANGED_VALUE, FALSE);
    intent->PutBooleanExtra(IAudioManager::EXTRA_AVAILABLITY_CHANGED_VALUE, FALSE);
    SendBroadcastToAll(intent);
    Logger::V(TAG, String("Updated List size: ") + strMediaPalyerSize);
    return NOERROR;
}

ECode CAudioService::StreamToString(
    /* [in] */ Int32 stream,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (stream >= 0 && stream < STREAM_NAMES->GetLength()) {
        *result = (*STREAM_NAMES)[stream];
        return NOERROR;
    }
    if (stream == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
        *result = String("USE_DEFAULT_STREAM_TYPE");
        return NOERROR;
    }
    *result = String("UNKNOWN_STREAM_") + StringUtils::ToString(stream);
    return NOERROR;
}

ECode CAudioService::AdjustSuggestedStreamVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 suggestedStreamType,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    AdjustSuggestedStreamVolume(direction, suggestedStreamType, flags,
            callingPackage, Binder::GetCallingUid());
    return NOERROR;
}

ECode CAudioService::AdjustStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    AdjustStreamVolume(streamType, direction, flags, callingPackage,
            Binder::GetCallingUid());
    return NOERROR;
}

ECode CAudioService::AdjustMasterVolume(
    /* [in] */ Int32 steps,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    if (mUseFixedVolume) {
        return NOERROR;
    }
    EnsureValidSteps(steps);
    Float vol;
    AudioSystem::GetMasterVolume(&vol);
    Int32 volume = Elastos::Core::Math::Round(vol * MAX_MASTER_VOLUME);
    Int32 delta = 0;
    Int32 numSteps = Elastos::Core::Math::Abs(steps);
    Int32 direction = steps > 0 ? IAudioManager::ADJUST_RAISE : IAudioManager::ADJUST_LOWER;
    for (Int32 i = 0; i < numSteps; ++i) {
        delta = FindVolumeDelta(direction, volume);
        volume += delta;
    }

    //Log.d(TAG, "adjustMasterVolume volume: " + volume + " steps: " + steps);
    return SetMasterVolume(volume, flags, callingPackage);
}

ECode CAudioService::SetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    SetStreamVolume(streamType, index, flags, callingPackage,
            Binder::GetCallingUid());
    return NOERROR;
}

ECode CAudioService::ForceVolumeControlStream(
    /* [in] */ Int32 streamType,
    /* [in] */ IBinder* cb)
{
    synchronized(mForceControlStreamLock) {
        mVolumeControlStream = streamType;
        if (mVolumeControlStream == -1) {
            if (mForceControlStreamClient != NULL) {
                mForceControlStreamClient->ReleaseResources();
                mForceControlStreamClient = NULL;
            }
        }
        else {
            mForceControlStreamClient = new ForceControlStreamClient(this, cb);
        }
    }
    return NOERROR;
}

ECode CAudioService::SetStreamSolo(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state,
    /* [in] */ IBinder* cb)
{
    if (mUseFixedVolume) {
        return NOERROR;
    }

    for (Int32 stream = 0; stream < mStreamStates->GetLength(); stream++) {
        Boolean b;
        if ((IsStreamAffectedByMute(stream, &b), !b) || stream == streamType)
            continue;
        (*mStreamStates)[stream]->Mute(cb, state);
     }
     return NOERROR;
}

ECode CAudioService::SetStreamMute(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state,
    /* [in] */ IBinder* cb)
{
    if (mUseFixedVolume) {
        return NOERROR;
    }

    Boolean b;
    if (IsStreamAffectedByMute(streamType, &b), b) {
        if (mHdmiManager != NULL) {
            synchronized(mHdmiManager) {
                if (streamType == IAudioSystem::STREAM_MUSIC && mHdmiTvClient != NULL) {
                    synchronized(mHdmiTvClient) {
                        if (mHdmiSystemAudioSupported) {
                            mHdmiTvClient->SetSystemAudioMute(state);
                        }
                    }
                }
            }
        }
        (*mStreamStates)[streamType]->Mute(cb, state);
    }
    return NOERROR;
}

ECode CAudioService::IsStreamMute(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (*mStreamStates)[streamType]->IsMuted();
    return NOERROR;
}

ECode CAudioService::ForceRemoteSubmixFullVolume(
    /* [in] */ Boolean startForcing,
    /* [in] */ IBinder* cb)
{
    if (cb == NULL) {
        return NOERROR;
    }

    Int32 perm;
    mContext->CheckCallingOrSelfPermission(
                    Manifest::permission::CAPTURE_AUDIO_OUTPUT, &perm);
    if (IPackageManager::PERMISSION_GRANTED != perm) {
        Logger::W(TAG, "Trying to call forceRemoteSubmixFullVolume() without CAPTURE_AUDIO_OUTPUT");
        return NOERROR;
    }
    synchronized(mRmtSbmxFullVolDeathHandlers) {
        Boolean applyRequired = FALSE;
        if (startForcing) {
            if (!HasRmtSbmxFullVolDeathHandlerFor(cb)) {
                AutoPtr<RmtSbmxFullVolDeathHandler> r =
                        new RmtSbmxFullVolDeathHandler(this, cb);
                mRmtSbmxFullVolDeathHandlers->Add(cb);
                if (mRmtSbmxFullVolRefCount == 0) {
                    mFullVolumeDevices |= IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX;
                    mFixedVolumeDevices |= IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX;
                    applyRequired = TRUE;
                }
                mRmtSbmxFullVolRefCount++;
            }
        }
        else {
            if (DiscardRmtSbmxFullVolDeathHandlerFor(cb) && (mRmtSbmxFullVolRefCount > 0)) {
                mRmtSbmxFullVolRefCount--;
                if (mRmtSbmxFullVolRefCount == 0) {
                    mFullVolumeDevices &= ~IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX;
                    mFixedVolumeDevices &= ~IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX;
                    applyRequired = TRUE;
                }
            }
        }
        if (applyRequired) {
            // Assumes only STREAM_MUSIC going through DEVICE_OUT_REMOTE_SUBMIX
            CheckAllFixedVolumeDevices(IAudioSystem::STREAM_MUSIC);
            (*mStreamStates)[IAudioSystem::STREAM_MUSIC]->ApplyAllVolumes();
        }
    }
    return NOERROR;
}

ECode CAudioService::SetMasterMute(
    /* [in] */ Boolean state,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ IBinder* cb)
{
    if (mUseFixedVolume) {
        return NOERROR;
    }

    Int32 val;
    mAppOps->NoteOp(IAppOpsManager::OP_AUDIO_MASTER_VOLUME,
            Binder::GetCallingUid(), callingPackage, &val);
    if (val != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    Boolean b;
    if (state != (AudioSystem::GetMasterMute(&b), b)) {
        AudioSystem::SetMasterMute(state);
        // Post a persist master volume msg
        SendMsg(mAudioHandler, MSG_PERSIST_MASTER_VOLUME_MUTE, SENDMSG_REPLACE, state ? 1
                : 0, UserHandle::GetCallingUserId(), NULL, PERSIST_DELAY);
        SendMasterMuteUpdate(state, flags);
    }
    return NOERROR;
}

ECode CAudioService::IsMasterMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return AudioSystem::GetMasterMute(result);
}

ECode CAudioService::GetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureValidStreamType(streamType);
    Int32 device = GetDeviceForStream(streamType);
    Int32 index;
    (*mStreamStates)[streamType]->GetIndex(device, &index);

    // by convention getStreamVolume() returns 0 when a stream is muted.
    if ((*mStreamStates)[streamType]->IsMuted()) {
        index = 0;
    }
    if (index != 0 && ((*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC) &&
            (device & mFixedVolumeDevices) != 0) {
        (*mStreamStates)[streamType]->GetMaxIndex(&index);
    }
    *result = (index + 5) / 10;
    return NOERROR;
}

ECode CAudioService::GetMasterVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    if (IsMasterMute(&b), b) {
        *result = 0;
        return NOERROR;
    }
    return GetLastAudibleMasterVolume(result);
}

ECode CAudioService::SetMasterVolume(
    /* [in] */ Int32 volume,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    if (mUseFixedVolume) {
        return NOERROR;
    }
    Int32 val;
    mAppOps->NoteOp(IAppOpsManager::OP_AUDIO_MASTER_VOLUME, Binder::GetCallingUid(),
            callingPackage, &val);
    if (val != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    if (volume < 0) {
        volume = 0;
    }
    else if (volume > MAX_MASTER_VOLUME) {
        volume = MAX_MASTER_VOLUME;
    }
    DoSetMasterVolume((Float)volume / MAX_MASTER_VOLUME, flags);
    return NOERROR;
}

ECode CAudioService::GetStreamMaxVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureValidStreamType(streamType);
    Int32 index;
    (*mStreamStates)[streamType]->GetMaxIndex(&index);
    *result = (index + 5) / 10;
    return NOERROR;
}

ECode CAudioService::GetMasterMaxVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MAX_MASTER_VOLUME;
    return NOERROR;
}

ECode CAudioService::GetLastAudibleStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureValidStreamType(streamType);
    Int32 device = GetDeviceForStream(streamType);
    Int32 index;
    (*mStreamStates)[streamType]->GetIndex(device, &index);
    *result = (index + 5) / 10;
    return NOERROR;
}

ECode CAudioService::GetLastAudibleMasterVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Float vol;
    AudioSystem::GetMasterVolume(&vol);
    *result = Elastos::Core::Math::Round(vol * MAX_MASTER_VOLUME);
    return NOERROR;
}

ECode CAudioService::GetMasterStreamType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (*mStreamVolumeAlias)[IAudioSystem::STREAM_SYSTEM];
    return NOERROR;
}

ECode CAudioService::SetMicrophoneMute(
    /* [in] */ Boolean on,
    /* [in] */ const String& callingPackage)
{
    Int32 val;
    mAppOps->NoteOp(IAppOpsManager::OP_MUTE_MICROPHONE, Binder::GetCallingUid(),
            callingPackage, &val);
    if (val != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    Boolean b;
    if (CheckAudioSettingsPermission(String("setMicrophoneMute()"), &b), !b) {
        return NOERROR;
    }

    AudioSystem::MuteMicrophone(on);
    // Post a persist microphone msg.
    SendMsg(mAudioHandler, MSG_PERSIST_MICROPHONE_MUTE, SENDMSG_REPLACE, on ? 1
            : 0, UserHandle::GetCallingUserId(), NULL, PERSIST_DELAY);
    return NOERROR;
}

ECode CAudioService::GetRingerMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mSettingsLock) {
        *result = mRingerMode;
    }
    return NOERROR;
}

ECode CAudioService::SetRingerMode(
    /* [in] */ Int32 ringerMode,
    /* [in] */ Boolean checkZen)
{
    if (mUseFixedVolume || IsPlatformTelevision()) {
        return NOERROR;
    }

    if ((ringerMode == IAudioManager::RINGER_MODE_VIBRATE) && !mHasVibrator) {
        ringerMode = IAudioManager::RINGER_MODE_SILENT;
    }
    if (checkZen) {
        CheckZen(ringerMode);
    }
    Int32 mode;
    GetRingerMode(&mode);
    if (ringerMode != mode) {
        SetRingerModeInt(ringerMode, TRUE);
        // Send sticky broadcast
        BroadcastRingerMode(ringerMode);
    }
    return NOERROR;
}

ECode CAudioService::ShouldVibrate(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasVibrator) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 mode;
    GetRingerMode(&mode);
    Int32 setting;
    switch (GetVibrateSetting(vibrateType, &setting), setting) {

        case IAudioManager::VIBRATE_SETTING_ON:
            *result = mode != IAudioManager::RINGER_MODE_SILENT;
            return NOERROR;

        case IAudioManager::VIBRATE_SETTING_ONLY_SILENT:
            *result = mode == IAudioManager::RINGER_MODE_VIBRATE;
            return NOERROR;

        case IAudioManager::VIBRATE_SETTING_OFF:
            // return FALSE, even for incoming calls
            *result = FALSE;
            return NOERROR;

        default:
            *result = FALSE;
            return NOERROR;
    }
    return NOERROR;
}

ECode CAudioService::GetVibrateSetting(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasVibrator) {
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
    if (!mHasVibrator) return NOERROR;

    GetValueForVibrateSetting(mVibrateSetting, vibrateType,
            vibrateSetting, &mVibrateSetting);

    // Broadcast change
    BroadcastVibrateSetting(vibrateType);
    return NOERROR;
}

ECode CAudioService::GetValueForVibrateSetting(
    /* [in] */ Int32 existingValue,
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

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
    if (DEBUG_MODE) { Logger::V(TAG, "setMode(mode=%d)", mode); }

    Boolean b;
    if (CheckAudioSettingsPermission(String("setMode()"), &b), !b) {
        return NOERROR;
    }

    Int32 val;
    mContext->CheckCallingOrSelfPermission(
                    Manifest::permission::MODIFY_PHONE_STATE, &val);
    if ( (mode == IAudioSystem::MODE_IN_CALL) &&
            (val != IPackageManager::PERMISSION_GRANTED)) {
        Logger::W(TAG, "MODIFY_PHONE_STATE Permission Denial: setMode(MODE_IN_CALL) from pid=%d, uid=%d",
                Binder::GetCallingPid(), Binder::GetCallingUid());
        return NOERROR;
    }

    if (mode < IAudioSystem::MODE_CURRENT || mode >= IAudioSystem::NUM_MODES) {
        return NOERROR;
    }

    Int32 newModeOwnerPid = 0;
    synchronized(mSetModeDeathHandlers) {
        if (mode == IAudioSystem::MODE_CURRENT) {
            mode = mMode;
        }
        if ((mode == IAudioSystem::MODE_IN_CALL) && IsInCommunication()) {
             AudioSystem::SetParameters(String("in_call=true"));
        }
        newModeOwnerPid = SetModeInt(mode, cb, Binder::GetCallingPid());
    }
    // when entering RINGTONE, IN_CALL or IN_COMMUNICATION mode, clear all
    // SCO connections not started by the application changing the mode
    if (newModeOwnerPid != 0) {
         DisconnectBluetoothSco(newModeOwnerPid);
    }
    return NOERROR;
}

ECode CAudioService::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMode;
    return NOERROR;
}

ECode CAudioService::PlaySoundEffect(
    /* [in] */ Int32 effectType)
{
    return PlaySoundEffectVolume(effectType, -1.0f);
}

ECode CAudioService::PlaySoundEffectVolume(
    /* [in] */ Int32 effectType,
    /* [in] */ Float volume)
{
    if (effectType >= IAudioManager::NUM_SOUND_EFFECTS || effectType < 0) {
        Logger::W(TAG, "AudioService effectType value %d out of range", effectType);
        return NOERROR;
    }

    SendMsg(mAudioHandler, MSG_PLAY_SOUND_EFFECT, SENDMSG_QUEUE,
            effectType, (Int32) (volume * 1000), NULL, 0);
    return NOERROR;
}

ECode CAudioService::LoadSoundEffects(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 attempts = 3;
    AutoPtr<LoadSoundEffectReply> reply = new LoadSoundEffectReply();

    synchronized(reply) {
        SendMsg(mAudioHandler, MSG_LOAD_SOUND_EFFECTS, SENDMSG_QUEUE, 0, 0,
                (IInterface*)(IObject*)reply, 0);
        while ((reply->mStatus == 1) && (attempts-- > 0)) {
            // try {
            reply->Wait(SOUND_EFFECTS_LOAD_TIMEOUT_MS);
            // } catch (InterruptedException e) {
            //     Log.w(TAG, "loadSoundEffects Interrupted while waiting sound pool loaded.");
            // }
        }
    }
    *result = (reply->mStatus == 0);
    return NOERROR;
}

ECode CAudioService::UnloadSoundEffects()
{
    SendMsg(mAudioHandler, MSG_UNLOAD_SOUND_EFFECTS, SENDMSG_QUEUE, 0, 0, NULL, 0);
    return NOERROR;
}

ECode CAudioService::ReloadAudioSettings()
{
    ReadAudioSettings(FALSE /*userSwitch*/);
    return NOERROR;
}

ECode CAudioService::SetSpeakerphoneOn(
    /* [in] */ Boolean on)
{
    Boolean b;
    if (CheckAudioSettingsPermission(String("setSpeakerphoneOn()"), &b), !b) {
        return NOERROR;
    }

    if (on) {
        if (mForcedUseForComm == IAudioSystem::FORCE_BT_SCO) {
                SendMsg(mAudioHandler, MSG_SET_FORCE_USE, SENDMSG_QUEUE,
                        IAudioSystem::FOR_RECORD, IAudioSystem::FORCE_NONE, NULL, 0);
        }
        mForcedUseForComm = IAudioSystem::FORCE_SPEAKER;
    }
    else if (mForcedUseForComm == IAudioSystem::FORCE_SPEAKER){
        mForcedUseForComm = IAudioSystem::FORCE_NONE;
    }

    SendMsg(mAudioHandler, MSG_SET_FORCE_USE, SENDMSG_QUEUE,
            IAudioSystem::FOR_COMMUNICATION, mForcedUseForComm, NULL, 0);
    return NOERROR;
}

ECode CAudioService::IsSpeakerphoneOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mForcedUseForComm == IAudioSystem::FORCE_SPEAKER);
    return NOERROR;
}

ECode CAudioService::SetBluetoothScoOn(
    /* [in] */ Boolean on)
{
    Boolean b;
    if (CheckAudioSettingsPermission(String("setBluetoothScoOn()"), &b), !b) {
        return NOERROR;
    }

    if (on) {
        mForcedUseForComm = IAudioSystem::FORCE_BT_SCO;
    }
    else if (mForcedUseForComm == IAudioSystem::FORCE_BT_SCO) {
        mForcedUseForComm = IAudioSystem::FORCE_NONE;
    }

    SendMsg(mAudioHandler, MSG_SET_FORCE_USE, SENDMSG_QUEUE,
            IAudioSystem::FOR_COMMUNICATION, mForcedUseForComm, NULL, 0);
    SendMsg(mAudioHandler, MSG_SET_FORCE_USE, SENDMSG_QUEUE,
            IAudioSystem::FOR_RECORD, mForcedUseForComm, NULL, 0);
    return NOERROR;
}

ECode CAudioService::IsBluetoothScoOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mForcedUseForComm == IAudioSystem::FORCE_BT_SCO);
    return NOERROR;
}

ECode CAudioService::SetBluetoothA2dpOn(
    /* [in] */ Boolean on)
{
    synchronized(mBluetoothA2dpEnabledLock) {
        mBluetoothA2dpEnabled = on;
        SendMsg(mAudioHandler, MSG_SET_FORCE_BT_A2DP_USE, SENDMSG_QUEUE,
                IAudioSystem::FOR_MEDIA,
                mBluetoothA2dpEnabled ? IAudioSystem::FORCE_NONE : IAudioSystem::FORCE_NO_BT_A2DP,
                NULL, 0);
    }
    return NOERROR;
}

ECode CAudioService::IsBluetoothA2dpOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mBluetoothA2dpEnabledLock) {
        *result = mBluetoothA2dpEnabled;
        return NOERROR;
    }
    return NOERROR;
}

ECode CAudioService::StartBluetoothSco(
    /* [in] */ IBinder* cb,
    /* [in] */ Int32 targetSdkVersion)
{
    Int32 scoAudioMode =
            (targetSdkVersion < Build::VERSION_CODES::JELLY_BEAN_MR2) ?
                    SCO_MODE_VIRTUAL_CALL : SCO_MODE_UNDEFINED;
    return StartBluetoothScoInt(cb, scoAudioMode);
}

ECode CAudioService::StartBluetoothScoVirtualCall(
    /* [in] */ IBinder* cb)
{
    return StartBluetoothScoInt(cb, SCO_MODE_VIRTUAL_CALL);
}

ECode CAudioService::StopBluetoothSco(
    /* [in] */ IBinder* cb)
{
    Boolean b;
    if ((CheckAudioSettingsPermission(String("stopBluetoothSco()"), &b), !b) ||
            !mSystemReady) {
        return NOERROR;
    }
    AutoPtr<ScoClient> client = GetScoClient(cb, FALSE);
    // The calling identity must be cleared before calling ScoClient.decCount().
    // decCount() calls requestScoState() which in turn can call BluetoothHeadset APIs
    // and this must be done on behalf of system server to make sure permissions are granted.
    Int64 ident = Binder::ClearCallingIdentity();
    if (client != NULL) {
        client->DecCount();
    }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CAudioService::ClearAllScoClients(
    /* [in] */ Int32 exceptPid,
    /* [in] */ Boolean stopSco)
{
    synchronized(mScoClients) {
        AutoPtr<ScoClient> savedClient;
        Int32 size;
        mScoClients->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mScoClients->Get(i, (IInterface**)&obj);
            AutoPtr<ScoClient> cl = (ScoClient*)(IObject*)obj.Get();
            Int32 pid;
            cl->GetPid(&pid);
            if (pid != exceptPid) {
                cl->ClearCount(stopSco);
            }
            else {
                savedClient = cl;
            }
        }
        mScoClients->Clear();
        if (savedClient != NULL) {
            mScoClients->Add((IInterface*)(IObject*)savedClient);
        }
    }
    return NOERROR;
}

ECode CAudioService::IsStreamAffectedByMute(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mMuteAffectedStreams & (1 << streamType)) != 0;
    return NOERROR;
}

ECode CAudioService::SetWiredDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    synchronized(mConnectedDevices) {
        Int32 delay = CheckSendBecomingNoisyIntent(device, state);
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        QueueMsgUnderWakeLock(mAudioHandler,
                MSG_SET_WIRED_DEVICE_CONNECTION_STATE,
                device, state, cs, delay);
    }
    return NOERROR;
}

ECode CAudioService::SetBluetoothA2dpDeviceConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 state,
    /* [in] */ Int32 profile,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 delay;
    if (profile != IBluetoothProfile::A2DP &&
            profile != IBluetoothProfile::A2DP_SINK) {
        // throw new IllegalArgumentException("invalid profile " + profile);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    synchronized(mConnectedDevices) {
        if (profile == IBluetoothProfile::A2DP) {
            delay = CheckSendBecomingNoisyIntent(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
                    (state == IBluetoothProfile::STATE_CONNECTED) ? 1 : 0);
        }
        else {
            delay = 0;
        }
        QueueMsgUnderWakeLock(mAudioHandler,
                (profile == IBluetoothProfile::A2DP ?
                MSG_SET_A2DP_SINK_CONNECTION_STATE : MSG_SET_A2DP_SRC_CONNECTION_STATE),
                state, 0, device, delay);
    }
    *result = delay;
    return NOERROR;
}

ECode CAudioService::AvrcpSupportsAbsoluteVolume(
    /* [in] */ const String& address,
    /* [in] */ Boolean support)
{
    // address is not used for now, but may be used when multiple a2dp devices are supported
    synchronized(mA2dpAvrcpLock) {
        mAvrcpAbsVolSupported = support;
        SendMsg(mAudioHandler, MSG_SET_DEVICE_VOLUME, SENDMSG_QUEUE,
                IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, 0,
                (IInterface*)(IObject*)(*mStreamStates)[IAudioSystem::STREAM_MUSIC], 0);
        SendMsg(mAudioHandler, MSG_SET_DEVICE_VOLUME, SENDMSG_QUEUE,
                IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, 0,
                (IInterface*)(IObject*)(*mStreamStates)[IAudioSystem::STREAM_RING], 0);
    }
    return NOERROR;
}

ECode CAudioService::RegisterRemoteController(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IComponentName* listenerComp,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMediaFocusControl->RegisterRemoteController(rcd, w, h, listenerComp);
    return NOERROR;
}

ECode CAudioService::RegisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMediaFocusControl->RegisterRemoteControlDisplay(rcd, w, h);
    return NOERROR;
}

ECode CAudioService::UnregisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    mMediaFocusControl->UnregisterRemoteControlDisplay(rcd);
    return NOERROR;
}

ECode CAudioService::RemoteControlDisplayUsesBitmapSize(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    mMediaFocusControl->RemoteControlDisplayUsesBitmapSize(rcd, w, h);
    return NOERROR;
}

ECode CAudioService::RemoteControlDisplayWantsPlaybackPositionSync(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean wantsSync)
{
    mMediaFocusControl->RemoteControlDisplayWantsPlaybackPositionSync(rcd, wantsSync);
    return NOERROR;
}

ECode CAudioService::SetRemoteControlClientPlayItem(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    mMediaFocusControl->SetRemoteControlClientPlayItem(uid, scope);
    return NOERROR;
}

ECode CAudioService::GetRemoteControlClientNowPlayingEntries()
{
    mMediaFocusControl->GetRemoteControlClientNowPlayingEntries();
    return NOERROR;
}

ECode CAudioService::SetRemoteControlClientBrowsedPlayer()
{
    mMediaFocusControl->SetRemoteControlClientBrowsedPlayer();
    return NOERROR;
}

ECode CAudioService::RequestAudioFocus(
    /* [in] */ Int32 mainStreamType,
    /* [in] */ Int32 durationHint,
    /* [in] */ IBinder* cb,
    /* [in] */ IIAudioFocusDispatcher* fd,
    /* [in] */ const String& clientId,
    /* [in] */ const String& callingPackageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMediaFocusControl->RequestAudioFocus(mainStreamType, durationHint, cb, fd,
            clientId, callingPackageName);
    return NOERROR;
}

ECode CAudioService::AbandonAudioFocus(
    /* [in] */ IIAudioFocusDispatcher* fd,
    /* [in] */ const String& clientId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMediaFocusControl->AbandonAudioFocus(fd, clientId);
    return NOERROR;
}

ECode CAudioService::UnregisterAudioFocusClient(
    /* [in] */ const String& clientId)
{
    mMediaFocusControl->UnregisterAudioFocusClient(clientId);
    return NOERROR;
}

ECode CAudioService::GetCurrentAudioFocus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMediaFocusControl->GetCurrentAudioFocus();
    return NOERROR;
}

ECode CAudioService::SetBluetoothA2dpOnInt(
    /* [in] */ Boolean on)
{
    synchronized(mBluetoothA2dpEnabledLock) {
        mBluetoothA2dpEnabled = on;
        mAudioHandler->RemoveMessages(MSG_SET_FORCE_BT_A2DP_USE);
        AudioSystem::SetForceUse(IAudioSystem::FOR_MEDIA,
                mBluetoothA2dpEnabled ? IAudioSystem::FORCE_NONE : IAudioSystem::FORCE_NO_BT_A2DP);
    }
    return NOERROR;
}

ECode CAudioService::IsCameraSoundForced(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mCameraSoundForced) {
        mCameraSoundForced->GetValue(result);
        return NOERROR;
    }
    return NOERROR;
}

ECode CAudioService::RegisterAudioPolicy(
    /* [in] */ IAudioPolicyConfig* policyConfig,
    /* [in] */ IBinder* cb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    //Log.v(TAG, "registerAudioPolicy for " + cb + " got policy:" + policyConfig);
    Int32 val;
    mContext->CheckCallingOrSelfPermission(
                    Manifest::permission::MODIFY_AUDIO_ROUTING, &val);
    Boolean hasPermissionForPolicy =
            (IPackageManager::PERMISSION_GRANTED == val);
    if (!hasPermissionForPolicy) {
        Logger::W(TAG, "Can't register audio policy for pid %d / uid %d, need MODIFY_AUDIO_ROUTING",
                Binder::GetCallingPid(), Binder::GetCallingUid());
        *result = FALSE;
        return NOERROR;
    }
    synchronized(mAudioPolicies) {
        AutoPtr<AudioPolicyProxy> app = new AudioPolicyProxy(this, policyConfig, cb);
        // try {
        AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
        if (proxy != NULL) proxy->LinkToDeath(app, 0);
        mAudioPolicies->Put(cb, (IInterface*)(IObject*)app);
        // } catch (RemoteException e) {
        //     // audio policy owner has already died!
        //     Slog.w(TAG, "Audio policy registration failed, could not link to " + cb +
        //             " binder death", e);
        //     return FALSE;
        // }
    }
    // TODO implement registration with native audio policy (including permission check)
    *result = TRUE;
    return NOERROR;
}

ECode CAudioService::UnregisterAudioPolicyAsync(
    /* [in] */ IBinder* cb)
{
    synchronized(mAudioPolicies) {
        AutoPtr<IInterface> obj;
        mAudioPolicies->Remove(cb, (IInterface**)&obj);
        AutoPtr<AudioPolicyProxy> app = (AudioPolicyProxy*)(IObject*)obj.Get();
        if (app == NULL) {
            Logger::W(TAG, "Trying to unregister unknown audio policy for pid %d / uid %d",
                    Binder::GetCallingPid(), Binder::GetCallingUid());
        }
        else {
            AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
            Boolean b;
            if (proxy != NULL) proxy->UnlinkToDeath(app, 0, &b);
        }
    }
    // TODO implement registration with native audio policy
    return NOERROR;
}

ECode CAudioService::StartBluetoothScoInt(
    /* [in] */ IBinder* cb,
    /* [in] */ Int32 scoAudioMode)
{
    Boolean b;
    if ((CheckAudioSettingsPermission(String("startBluetoothSco()"), &b), !b) ||
            !mSystemReady) {
        return NOERROR;
    }
    AutoPtr<ScoClient> client = GetScoClient(cb, TRUE);
    // The calling identity must be cleared before calling ScoClient.incCount().
    // inCount() calls requestScoState() which in turn can call BluetoothHeadset APIs
    // and this must be done on behalf of system server to make sure permissions are granted.
    // The caller identity must be cleared after getScoClient() because it is needed if a new
    // client is created.
    Int64 ident = Binder::ClearCallingIdentity();
    client->IncCount(scoAudioMode);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CAudioService::IsStreamAffectedByRingerMode(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mRingerModeAffectedStreams & (1 << streamType)) != 0;
    return NOERROR;
}

ECode CAudioService::UpdateRingerModeAffectedStreams(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 ringerModeAffectedStreams;
    // make sure settings for ringer mode are consistent with device type: non voice capable
    // devices (tablets) include media stream in silent mode whereas phones don't.
    Settings::System::GetInt32ForUser(mContentResolver,
            ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
            ((1 << IAudioSystem::STREAM_RING)|(1 << IAudioSystem::STREAM_NOTIFICATION)|
             (1 << IAudioSystem::STREAM_SYSTEM)|(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED)),
             IUserHandle::USER_CURRENT, &ringerModeAffectedStreams);

    // ringtone, notification, system and dtmf streams are always affected by ringer mode
    ringerModeAffectedStreams |= (1 << IAudioSystem::STREAM_RING)|
                                    (1 << IAudioSystem::STREAM_NOTIFICATION)|
                                    (1 << IAudioSystem::STREAM_SYSTEM)|
                                    (1 << IAudioSystem::STREAM_DTMF);

    switch (mPlatformType) {
        case PLATFORM_TELEVISION:
            ringerModeAffectedStreams = 0;
            break;
        default:
            ringerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_MUSIC);
            break;
    }

    synchronized(mCameraSoundForced) {
        if (mCameraSoundForced) {
            ringerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
        }
        else {
            ringerModeAffectedStreams |= (1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
        }
    }
    if ((*mStreamVolumeAlias)[IAudioSystem::STREAM_DTMF] == IAudioSystem::STREAM_RING) {
        ringerModeAffectedStreams |= (1 << IAudioSystem::STREAM_DTMF);
    }
    else {
        ringerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_DTMF);
    }

    if (ringerModeAffectedStreams != mRingerModeAffectedStreams) {
        Boolean b;
        Settings::System::PutInt32ForUser(mContentResolver,
                ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
                ringerModeAffectedStreams,
                IUserHandle::USER_CURRENT, &b);
        mRingerModeAffectedStreams = ringerModeAffectedStreams;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CAudioService::CheckAudioSettingsPermission(
    /* [in] */ const String& method,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 perm;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::MODIFY_AUDIO_SETTINGS, &perm);
    if (perm == IPackageManager::PERMISSION_GRANTED) {
        *result = TRUE;
        return NOERROR;
    }

    StringBuilder sb;
    sb += "Audio Settings Permission Denial: ";
    sb += method;
    sb += " from pid=";
    sb += Binder::GetCallingPid();
    sb += ", uid=";
    sb += Binder::GetCallingUid();
    Logger::W(TAG, sb.ToString().string());
    *result = FALSE;
    return NOERROR;
}

ECode CAudioService::SetRemoteStreamVolume(
    /* [in] */ Int32 index)
{
    EnforceSelfOrSystemUI(String("set the remote stream volume"));
    mMediaFocusControl->SetRemoteStreamVolume(index);
    return NOERROR;
}

ECode CAudioService::SetRingtonePlayer(
    /* [in] */ IIRingtonePlayer* player)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::REMOTE_AUDIO_PLAYBACK, String(NULL));
    mRingtonePlayer = player;
    return NOERROR;
}

ECode CAudioService::GetRingtonePlayer(
    /* [out] */ IIRingtonePlayer** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRingtonePlayer;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioService::StartWatchingRoutes(
    /* [in] */ IIAudioRoutesObserver* observer,
    /* [out] */ IAudioRoutesInfo** result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mCurAudioRoutes) {
        AutoPtr<IAudioRoutesInfo> routes;
        CAudioRoutesInfo::New(mCurAudioRoutes, (IAudioRoutesInfo**)&routes);
        Boolean b;
        mRoutesObservers->Register(observer, &b);
        *result = routes;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode CAudioService::DisableSafeMediaVolume()
{
    EnforceSelfOrSystemUI(String("disable the safe media volume"));
    synchronized(mSafeMediaVolumeStateLock) {
        SetSafeMediaVolumeEnabled(FALSE);
        if (mPendingVolumeCommand != NULL) {
            OnSetStreamVolume(mPendingVolumeCommand->mStreamType,
                              mPendingVolumeCommand->mIndex,
                              mPendingVolumeCommand->mFlags,
                              mPendingVolumeCommand->mDevice);
            mPendingVolumeCommand = NULL;
        }
    }
    return NOERROR;
}

ECode CAudioService::SetHdmiSystemAudioSupported(
    /* [in] */ Boolean on,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 device = IAudioSystem::DEVICE_NONE;
    if (mHdmiManager != NULL) {
        synchronized(mHdmiManager) {
            if (mHdmiTvClient == NULL) {
                Logger::W(TAG, "Only Hdmi-Cec enabled TV device supports system audio mode.");
                *result = device;
                return NOERROR;
            }

            synchronized(mHdmiTvClient) {
                if (mHdmiSystemAudioSupported != on) {
                    mHdmiSystemAudioSupported = on;
                    AudioSystem::SetForceUse(IAudioSystem::FOR_HDMI_SYSTEM_AUDIO,
                            on ? IAudioSystem::FORCE_HDMI_SYSTEM_AUDIO_ENFORCED :
                                 IAudioSystem::FORCE_NONE);
                }
                AudioSystem::GetDevicesForStream(IAudioSystem::STREAM_MUSIC, &device);
            }
        }
    }
    *result = device;
    return NOERROR;
}

ECode CAudioService::IsHdmiSystemAudioSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHdmiSystemAudioSupported;
    return NOERROR;
}

ECode CAudioService::SetVolumeController(
    /* [in] */ IIVolumeController* controller)
{
    EnforceSelfOrSystemUI(String("set the volume controller"));

    // return early if things are not actually changing
    Boolean b;
    if (mVolumeController->IsSameBinder(controller, &b), b) {
        return NOERROR;
    }

    // dismiss the old volume controller
    mVolumeController->PostDismiss();
    if (controller != NULL) {
        // we are about to register a new controller, listen for its death
        // try {
        AutoPtr<IProxy> proxy = (IProxy*)(IBinder::Probe(controller))->Probe(EIID_IProxy);
        AutoPtr<SetVolumeControllerDeathRecipient> dr =
                new SetVolumeControllerDeathRecipient(this, controller);
        if (proxy != NULL) proxy->LinkToDeath(dr, 0);
        // } catch (RemoteException e) {
        //     // noop
        // }
    }
    mVolumeController->SetController(controller);
    // if (DEBUG_VOL) Logger::D(TAG, "Volume controller: " + mVolumeController);
    return NOERROR;
}

ECode CAudioService::NotifyVolumeControllerVisible(
    /* [in] */ IIVolumeController* controller,
    /* [in] */ Boolean visible)
{
    EnforceSelfOrSystemUI(String("notify about volume controller visibility"));

    // return early if the controller is not current
    Boolean b;
    if (mVolumeController->IsSameBinder(controller, &b), !b) {
        return NOERROR;
    }

    mVolumeController->SetVisible(visible);
    if (DEBUG_VOL) Logger::D(TAG, "Volume controller visible: %d", visible);
    return NOERROR;
}

ECode CAudioService::GetMaxStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (*MAX_STREAM_VOLUME)[streamType];
    return NOERROR;
}

ECode CAudioService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP, TAG);

    mMediaFocusControl->Dump(pw);
    DumpStreamStates(pw);
    DumpRingerMode(pw);
    pw->Println(String("\nAudio routes:"));
    pw->Print(String("  mMainType=0x"));
    Int32 mainType;
    mCurAudioRoutes->GetMainType(&mainType);
    pw->Println(StringUtils::ToHexString(mainType));
    pw->Print(String("  mBluetoothName="));
    AutoPtr<ICharSequence> cs;
    mCurAudioRoutes->GetBluetoothName((ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    pw->Println(str);

    pw->Println(String("\nOther state:"));
    pw->Print(String("  mVolumeController="));
    pw->Println((IInterface*)(IObject*)mVolumeController);
    pw->Print(String("  mSafeMediaVolumeState="));
    pw->Println(SafeMediaVolumeStateToString(mSafeMediaVolumeState));
    pw->Print(String("  mSafeMediaVolumeIndex="));
    pw->Println(mSafeMediaVolumeIndex);
    pw->Print(String("  mPendingVolumeCommand="));
    pw->Println((IInterface*)(IObject*)mPendingVolumeCommand);
    pw->Print(String("  mMusicActiveMs="));
    pw->Println(mMusicActiveMs);
    pw->Print(String("  mMcc="));
    pw->Println(mMcc);
    return NOERROR;
}

Boolean CAudioService::IsPlatformVoice()
{
    return mPlatformType == PLATFORM_VOICE;
}

Boolean CAudioService::IsPlatformTelevision()
{
    return mPlatformType == PLATFORM_TELEVISION;
}

void CAudioService::CreateAudioSystemThread()
{
    mAudioSystemThread = new AudioSystemThread(this);
    mAudioSystemThread->Start();
    WaitForAudioHandlerCreation();
}

void CAudioService::WaitForAudioHandlerCreation()
{
    synchronized(this) {
        while (mAudioHandler == NULL) {
            // try {
                // Wait for mAudioHandler to be set by the other thread
            Wait();
            // } catch (InterruptedException e) {
            //     Log.e(TAG, "Interrupted while waiting on volume handler.");
            // }
        }
    }
}

void CAudioService::CheckAllAliasStreamVolumes()
{
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = 0; streamType < numStreamTypes; streamType++) {
        if (streamType != (*mStreamVolumeAlias)[streamType]) {
            (*mStreamStates)[streamType]->SetAllIndexes(
                    IAudioServiceVolumeStreamState::Probe((*mStreamStates)[(*mStreamVolumeAlias)[streamType]]));
        }
        // apply stream volume
        if (!(*mStreamStates)[streamType]->IsMuted()) {
            (*mStreamStates)[streamType]->ApplyAllVolumes();
        }
    }
}

void CAudioService::CheckAllFixedVolumeDevices()
{
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = 0; streamType < numStreamTypes; streamType++) {
        (*mStreamStates)[streamType]->CheckFixedVolumeDevices();
    }
}

void CAudioService::CheckAllFixedVolumeDevices(
    /* [in] */ Int32 streamType)
{
    (*mStreamStates)[streamType]->CheckFixedVolumeDevices();
}

void CAudioService::CreateStreamStates()
{
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    AutoPtr<ArrayOf<VolumeStreamState*> > streams = mStreamStates =
            ArrayOf<VolumeStreamState*>::Alloc(numStreamTypes);

    for (Int32 i = 0; i < numStreamTypes; i++) {
        AutoPtr<VolumeStreamState> vss =
                new VolumeStreamState(this, (*Settings::System::VOLUME_SETTINGS)[(*mStreamVolumeAlias)[i]], i);
        streams->Set(i, vss);
    }

    CheckAllFixedVolumeDevices();
    CheckAllAliasStreamVolumes();
}

void CAudioService::DumpStreamStates(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("\nStream volumes (device: index)"));
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 i = 0; i < numStreamTypes; i++) {
        pw->Println(String("- ") + (*STREAM_NAMES)[i]+":");
        (*mStreamStates)[i]->Dump(pw);
        pw->Println(String(""));
    }
    pw->Print(String("\n- mute affected streams = 0x"));
    pw->Println(StringUtils::ToHexString(mMuteAffectedStreams));
}

void CAudioService::UpdateStreamVolumeAlias(
    /* [in] */ Boolean updateVolumes)
{
    Int32 dtmfStreamAlias;

    switch (mPlatformType) {
    case PLATFORM_VOICE:
        mStreamVolumeAlias = STREAM_VOLUME_ALIAS_VOICE;
        dtmfStreamAlias = IAudioSystem::STREAM_RING;
        break;
    case PLATFORM_TELEVISION:
        mStreamVolumeAlias = STREAM_VOLUME_ALIAS_TELEVISION;
        dtmfStreamAlias = IAudioSystem::STREAM_MUSIC;
        break;
    default:
        mStreamVolumeAlias = STREAM_VOLUME_ALIAS_DEFAULT;
        dtmfStreamAlias = IAudioSystem::STREAM_MUSIC;
    }

    if (IsPlatformTelevision()) {
        mRingerModeAffectedStreams = 0;
    }
    else {
        if (IsInCommunication()) {
            dtmfStreamAlias = IAudioSystem::STREAM_VOICE_CALL;
            mRingerModeAffectedStreams &= ~(1 << IAudioSystem::STREAM_DTMF);
        }
        else {
            mRingerModeAffectedStreams |= (1 << IAudioSystem::STREAM_DTMF);
        }
    }

    (*mStreamVolumeAlias)[IAudioSystem::STREAM_DTMF] = dtmfStreamAlias;
    if (mLinkNotificationWithVolume) {
        (*mStreamVolumeAlias)[IAudioSystem::STREAM_NOTIFICATION] = IAudioSystem::STREAM_RING;
    }
    else {
        (*mStreamVolumeAlias)[IAudioSystem::STREAM_NOTIFICATION] = IAudioSystem::STREAM_NOTIFICATION;
    }

    if (updateVolumes) {
        (*mStreamStates)[IAudioSystem::STREAM_DTMF]->SetAllIndexes(
                IAudioServiceVolumeStreamState::Probe((*mStreamStates)[dtmfStreamAlias]));
        // apply stream mute states according to new value of mRingerModeAffectedStreams
        Int32 mode;
        GetRingerMode(&mode);
        SetRingerModeInt(mode, FALSE);
        SendMsg(mAudioHandler,
                MSG_SET_ALL_VOLUMES,
                SENDMSG_QUEUE,
                0,
                0,
                (IInterface*)(IObject*)(*mStreamStates)[IAudioSystem::STREAM_DTMF], 0);
    }
}

void CAudioService::ReadDockAudioSettings(
    /* [in] */ IContentResolver* cr)
{
    Int32 val;
    Settings::Global::GetInt32(
            cr, ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED, 0, &val);

    mDockAudioMediaEnabled = val == 1;

    if (mDockAudioMediaEnabled) {
        mBecomingNoisyIntentDevices |= IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET;
    }
    else {
        mBecomingNoisyIntentDevices &= ~IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET;
    }

    SendMsg(mAudioHandler,
            MSG_SET_FORCE_USE,
            SENDMSG_QUEUE,
            IAudioSystem::FOR_DOCK,
            mDockAudioMediaEnabled ?
                    IAudioSystem::FORCE_ANALOG_DOCK : IAudioSystem::FORCE_NONE,
            NULL,
            0);
}

void CAudioService::ReadPersistedSettings()
{
    AutoPtr<IContentResolver> cr = mContentResolver;

    Int32 ringerModeFromSettings;
    Settings::Global::GetInt32(
            cr, ISettingsGlobal::MODE_RINGER, IAudioManager::RINGER_MODE_NORMAL,
            &ringerModeFromSettings);
    Int32 ringerMode = ringerModeFromSettings;
    // sanity check in case the settings are restored from a device with incompatible
    // ringer modes
    Boolean b;
    if (CAudioManager::IsValidRingerMode(ringerMode, &b), !b) {
        ringerMode = IAudioManager::RINGER_MODE_NORMAL;
    }
    if ((ringerMode == IAudioManager::RINGER_MODE_VIBRATE) && !mHasVibrator) {
        ringerMode = IAudioManager::RINGER_MODE_SILENT;
    }
    if (ringerMode != ringerModeFromSettings) {
        Settings::Global::PutInt32(cr, ISettingsGlobal::MODE_RINGER, ringerMode, &b);
    }
    if (mUseFixedVolume || IsPlatformTelevision()) {
        ringerMode = IAudioManager::RINGER_MODE_NORMAL;
    }
    synchronized(mSettingsLock) {
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

        UpdateRingerModeAffectedStreams(&b);
        ReadDockAudioSettings(cr);
    }

    Int32 resTmp = 0;
    Settings::Secure::GetInt32(cr, ISettingsSecure::VOLUME_LINK_NOTIFICATION, 1, &resTmp);
    mLinkNotificationWithVolume = (resTmp == 1);

    Settings::System::GetInt32ForUser(cr,
            ISettingsSystem::MUTE_STREAMS_AFFECTED,
            ((1 << IAudioSystem::STREAM_MUSIC)|
             (1 << IAudioSystem::STREAM_RING)|
             (1 << IAudioSystem::STREAM_SYSTEM)),
             IUserHandle::USER_CURRENT,
             &mMuteAffectedStreams);

    Int32 val;
    Settings::System::GetInt32ForUser(cr, ISettingsSystem::VOLUME_MASTER_MUTE,
            0, IUserHandle::USER_CURRENT, &val);
    Boolean masterMute = val == 1;

    if (mUseFixedVolume) {
        masterMute = FALSE;
        AudioSystem::SetMasterVolume(1.0f);
    }
    AudioSystem::SetMasterMute(masterMute);
    BroadcastMasterMuteStatus(masterMute);

    Settings::System::GetInt32ForUser(cr, ISettingsSystem::MICROPHONE_MUTE, 0, IUserHandle::USER_CURRENT, &val);
    Boolean microphoneMute = val == 1;
    AudioSystem::MuteMicrophone(microphoneMute);

    // Each stream will read its own persisted settings

    // Broadcast the sticky intent
    BroadcastRingerMode(ringerMode);

    // Broadcast vibrate settings
    BroadcastVibrateSetting(IAudioManager::VIBRATE_TYPE_RINGER);
    BroadcastVibrateSetting(IAudioManager::VIBRATE_TYPE_NOTIFICATION);

    // Load settings for the volume controller
    mVolumeController->LoadSettings(cr);
}

Int32 CAudioService::RescaleIndex(
    /* [in] */ Int32 index,
    /* [in] */ Int32 srcStream,
    /* [in] */ Int32 dstStream)
{
    Int32 dmax, smax;
    (*mStreamStates)[dstStream]->GetMaxIndex(&dmax);
    (*mStreamStates)[srcStream]->GetMaxIndex(&smax);
    return (index * dmax + smax / 2) / smax;
}

void CAudioService::AdjustSuggestedStreamVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 suggestedStreamType,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 uid)
{
    if (DEBUG_VOL) Logger::D(TAG, "adjustSuggestedStreamVolume() stream=%d, flags=%d",
            suggestedStreamType, flags);
    Int32 streamType;
    if (mVolumeControlStream != -1) {
        streamType = mVolumeControlStream;
    }
    else {
        streamType = GetActiveStreamType(suggestedStreamType);
    }
    Int32 resolvedStream = (*mStreamVolumeAlias)[streamType];

    // Play sounds on STREAM_RING only.
    if ((flags & IAudioManager::FLAG_PLAY_SOUND) != 0 &&
            resolvedStream != IAudioSystem::STREAM_RING) {
        flags &= ~IAudioManager::FLAG_PLAY_SOUND;
    }

    // For notifications/ring, show the ui before making any adjustments
    Boolean b;
    if (mVolumeController->SuppressAdjustment(resolvedStream, flags, &b), b) {
        direction = 0;
        flags &= ~IAudioManager::FLAG_PLAY_SOUND;
        flags &= ~IAudioManager::FLAG_VIBRATE;
        if (DEBUG_VOL) Logger::D(TAG, "Volume controller suppressed adjustment");
    }

    AdjustStreamVolume(streamType, direction, flags, callingPackage, uid);
}

void CAudioService::AdjustStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 uid)
{
    if (mUseFixedVolume) {
        return;
    }
    if (DEBUG_VOL) Logger::D(TAG, "adjustStreamVolume() stream=%d, dir=%d, flags=%d",
            streamType, direction, flags);

    EnsureValidDirection(direction);
    EnsureValidStreamType(streamType);

    // use stream type alias here so that streams with same alias have the same behavior,
    // including with regard to silent mode control (e.g the use of STREAM_RING below and in
    // checkForRingerModeChange() in place of STREAM_RING or STREAM_NOTIFICATION)
    Int32 streamTypeAlias = (*mStreamVolumeAlias)[streamType];
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamTypeAlias];

    Int32 device = GetDeviceForStream(streamTypeAlias);

    Int32 aliasIndex;
    streamState->GetIndex(device, &aliasIndex);
    Boolean adjustVolume = TRUE;
    Int32 step;

    // skip a2dp absolute volume control request when the device
    // is not an a2dp device
    if ((device & IAudioSystem::DEVICE_OUT_ALL_A2DP) == 0 &&
        (flags & IAudioManager::FLAG_BLUETOOTH_ABS_VOLUME) != 0) {
        return;
    }

    Int32 val;
    mAppOps->NoteOp((*STEAM_VOLUME_OPS)[streamTypeAlias], uid, callingPackage, &val);
    if (val != IAppOpsManager::MODE_ALLOWED) {
        return;
    }

    // reset any pending volume command
    synchronized(mSafeMediaVolumeStateLock) {
        mPendingVolumeCommand = NULL;
    }

    flags &= ~IAudioManager::FLAG_FIXED_VOLUME;
    if ((streamTypeAlias == IAudioSystem::STREAM_MUSIC) &&
           ((device & mFixedVolumeDevices) != 0)) {
        flags |= IAudioManager::FLAG_FIXED_VOLUME;

        // Always toggle between max safe volume and 0 for fixed volume devices where safe
        // volume is enforced, and max and 0 for the others.
        // This is simulated by stepping by the full allowed volume range
        Int32 value;
        mSafeMediaVolumeState->GetValue(&value);
        if (value == SAFE_MEDIA_VOLUME_ACTIVE &&
                (device & mSafeMediaVolumeDevices) != 0) {
            step = mSafeMediaVolumeIndex;
        }
        else {
            streamState->GetMaxIndex(&step);
        }
        if (aliasIndex != 0) {
            aliasIndex = step;
        }
    }
    else {
        // convert one UI step (+/-1) into a number of internal units on the stream alias
        step = RescaleIndex(10, streamType, streamTypeAlias);
    }

    // If either the client forces allowing ringer modes for this adjustment,
    // or the stream type is one that is affected by ringer modes
    GetMasterStreamType(&val);
    if (((flags & IAudioManager::FLAG_ALLOW_RINGER_MODES) != 0) ||
            (streamTypeAlias == val)) {
        Int32 ringerMode;
        GetRingerMode(&ringerMode);
        // do not vibrate if already in vibrate mode
        if (ringerMode == IAudioManager::RINGER_MODE_VIBRATE) {
            flags &= ~IAudioManager::FLAG_VIBRATE;
        }
        // Check if the ringer mode changes with this volume adjustment. If
        // it does, it will handle adjusting the volume, so we won't below
        Int32 result = CheckForRingerModeChange(aliasIndex, direction, step);
        adjustVolume = (result & FLAG_ADJUST_VOLUME) != 0;
        // If suppressing a volume adjustment in silent mode, display the UI hint
        if ((result & IAudioManager::FLAG_SHOW_SILENT_HINT) != 0) {
            flags |= IAudioManager::FLAG_SHOW_SILENT_HINT;
        }
    }

    Int32 oldIndex;
    (*mStreamStates)[streamType]->GetIndex(device, &oldIndex);

    if (adjustVolume && (direction != IAudioManager::ADJUST_SAME)) {

        // Check if volume update should be send to AVRCP
        if (streamTypeAlias == IAudioSystem::STREAM_MUSIC &&
            (device & IAudioSystem::DEVICE_OUT_ALL_A2DP) != 0 &&
            (flags & IAudioManager::FLAG_BLUETOOTH_ABS_VOLUME) == 0) {
            synchronized(mA2dpAvrcpLock) {
                if (mA2dp != NULL && mAvrcpAbsVolSupported) {
                    mA2dp->AdjustAvrcpAbsoluteVolume(direction);
                }
            }
        }

        Boolean b;
        if ((direction == IAudioManager::ADJUST_RAISE) &&
                !CheckSafeMediaVolume(streamTypeAlias, aliasIndex + step, device)) {
            Logger::E(TAG, "adjustStreamVolume() safe volume index = %d", oldIndex);
            mVolumeController->PostDisplaySafeVolumeWarning(flags);
        }
        else if (streamState->AdjustIndex(direction * step, device, &b), b) {
            // Post message to set system volume (it in turn will post a message
            // to persist). Do not change volume if stream is muted.
            SendMsg(mAudioHandler,
                    MSG_SET_DEVICE_VOLUME,
                    SENDMSG_QUEUE,
                    device,
                    0,
                    (IInterface*)(IObject*)streamState,
                    0);
        }

        // Check if volume update should be send to Hdmi system audio.
        Int32 newIndex;
        (*mStreamStates)[streamType]->GetIndex(device, &newIndex);
        if (mHdmiManager != NULL) {
            synchronized(mHdmiManager) {
                if (mHdmiTvClient != NULL &&
                    streamTypeAlias == IAudioSystem::STREAM_MUSIC &&
                    (flags & IAudioManager::FLAG_HDMI_SYSTEM_AUDIO_VOLUME) == 0 &&
                    oldIndex != newIndex) {
                    Int32 maxIndex;
                    GetStreamMaxVolume(streamType, &maxIndex);
                    synchronized(mHdmiTvClient) {
                        if (mHdmiSystemAudioSupported) {
                            mHdmiTvClient->SetSystemAudioVolume(
                                    (oldIndex + 5) / 10, (newIndex + 5) / 10, maxIndex);
                        }
                    }
                }
                // mHdmiCecSink TRUE => mHdmiPlaybackClient != NULL
                if (mHdmiCecSink &&
                        streamTypeAlias == IAudioSystem::STREAM_MUSIC &&
                        oldIndex != newIndex) {
                    synchronized(mHdmiPlaybackClient) {
                        Int32 keyCode = (direction == -1) ? IKeyEvent::KEYCODE_VOLUME_DOWN :
                                IKeyEvent::KEYCODE_VOLUME_UP;
                        IHdmiClient::Probe(mHdmiPlaybackClient)->SendKeyEvent(keyCode, TRUE);
                        IHdmiClient::Probe(mHdmiPlaybackClient)->SendKeyEvent(keyCode, FALSE);
                    }
                }
            }
        }
    }
    Int32 index;
    (*mStreamStates)[streamType]->GetIndex(device, &index);
    SendVolumeUpdate(streamType, oldIndex, index, flags);
}

void CAudioService::OnSetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 device)
{
    SetStreamVolumeInt((*mStreamVolumeAlias)[streamType], index, device, FALSE);
    // setting volume on master stream type also controls silent mode
    Int32 type;
    GetMasterStreamType(&type);
    if (((flags & IAudioManager::FLAG_ALLOW_RINGER_MODES) != 0) ||
            ((*mStreamVolumeAlias)[streamType] == type)) {
        Int32 newRingerMode;
        if (index == 0) {
            newRingerMode = mHasVibrator ? IAudioManager::RINGER_MODE_VIBRATE
                    : VOLUME_SETS_RINGER_MODE_SILENT ? IAudioManager::RINGER_MODE_SILENT
                    : IAudioManager::RINGER_MODE_NORMAL;
        }
        else {
            newRingerMode = IAudioManager::RINGER_MODE_NORMAL;
        }
        SetRingerMode(newRingerMode, FALSE /*checkZen*/);
    }
}

void CAudioService::SetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 uid)
{
    if (mUseFixedVolume) {
        return;
    }

    EnsureValidStreamType(streamType);
    Int32 streamTypeAlias = (*mStreamVolumeAlias)[streamType];
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamTypeAlias];

    Int32 device = GetDeviceForStream(streamType);
    Int32 oldIndex;

    // skip a2dp absolute volume control request when the device
    // is not an a2dp device
    if ((device & IAudioSystem::DEVICE_OUT_ALL_A2DP) == 0 &&
        (flags & IAudioManager::FLAG_BLUETOOTH_ABS_VOLUME) != 0) {
        return;
    }

    Int32 val;
    mAppOps->NoteOp((*STEAM_VOLUME_OPS)[streamTypeAlias], uid, callingPackage, &val);
    if (val != IAppOpsManager::MODE_ALLOWED) {
        return;
    }

    synchronized(mSafeMediaVolumeStateLock) {
        // reset any pending volume command
        mPendingVolumeCommand = NULL;

        streamState->GetIndex(device, &oldIndex);

        index = RescaleIndex(index * 10, streamType, streamTypeAlias);

        if (streamTypeAlias == IAudioSystem::STREAM_MUSIC &&
            (device & IAudioSystem::DEVICE_OUT_ALL_A2DP) != 0 &&
            (flags & IAudioManager::FLAG_BLUETOOTH_ABS_VOLUME) == 0) {
            synchronized(mA2dpAvrcpLock) {
                if (mA2dp != NULL && mAvrcpAbsVolSupported) {
                    mA2dp->SetAvrcpAbsoluteVolume(index / 10);
                }
            }
        }

        if (mHdmiManager != NULL) {
            synchronized(mHdmiManager) {
                if (mHdmiTvClient != NULL &&
                    streamTypeAlias == IAudioSystem::STREAM_MUSIC &&
                    (flags & IAudioManager::FLAG_HDMI_SYSTEM_AUDIO_VOLUME) == 0 &&
                    oldIndex != index) {
                    Int32 maxIndex;
                    GetStreamMaxVolume(streamType, &maxIndex);
                    synchronized(mHdmiTvClient) {
                        if (mHdmiSystemAudioSupported) {
                            mHdmiTvClient->SetSystemAudioVolume(
                                    (oldIndex + 5) / 10, (index + 5) / 10, maxIndex);
                        }
                    }
                }
            }
        }

        flags &= ~IAudioManager::FLAG_FIXED_VOLUME;
        if ((streamTypeAlias == IAudioSystem::STREAM_MUSIC) &&
                ((device & mFixedVolumeDevices) != 0)) {
            flags |= IAudioManager::FLAG_FIXED_VOLUME;

            // volume is either 0 or max allowed for fixed volume devices
            if (index != 0) {
                Int32 value;
                mSafeMediaVolumeState->GetValue(&value);
                if (value == SAFE_MEDIA_VOLUME_ACTIVE &&
                        (device & mSafeMediaVolumeDevices) != 0) {
                    index = mSafeMediaVolumeIndex;
                }
                else {
                    streamState->GetMaxIndex(&index);
                }
            }
        }

        if (!CheckSafeMediaVolume(streamTypeAlias, index, device)) {
            mVolumeController->PostDisplaySafeVolumeWarning(flags);
            mPendingVolumeCommand = new StreamVolumeCommand(
                    streamType, index, flags, device);
        }
        else {
            OnSetStreamVolume(streamType, index, flags, device);
            (*mStreamStates)[streamType]->GetIndex(device, &index);
        }
    }
    SendVolumeUpdate(streamType, oldIndex, index, flags);
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
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
}

void CAudioService::SendStickyBroadcastToAll(
    /* [in] */ IIntent* intent)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
}

void CAudioService::SendVolumeUpdate(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    if (!IsPlatformVoice() && (streamType == IAudioSystem::STREAM_RING)) {
        streamType = IAudioSystem::STREAM_NOTIFICATION;
    }

    // If Hdmi-CEC system audio mode is on, show volume bar
    // only when TV receives volume notification from Audio Receiver.
    if (mHdmiTvClient != NULL && streamType == IAudioSystem::STREAM_MUSIC) {
        synchronized(mHdmiTvClient) {
            if (mHdmiSystemAudioSupported &&
                    ((flags & IAudioManager::FLAG_HDMI_SYSTEM_AUDIO_VOLUME) == 0)) {
                flags &= ~IAudioManager::FLAG_SHOW_UI;
            }
        }
    }
    mVolumeController->PostVolumeChanged(streamType, flags);

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
    mVolumeController->PostMasterVolumeChanged(flags);

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
    mVolumeController->PostMasterMuteChanged(flags);
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
    /* [in] */ Boolean force)
{
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamType];

    Boolean b;
    streamState->SetIndex(index, device, &b);
    if (b || force) {
        // Post message to set system volume (it in turn will post a message
        // to persist).
        SendMsg(mAudioHandler,
                MSG_SET_DEVICE_VOLUME,
                SENDMSG_QUEUE,
                device,
                0,
                (IInterface*)(IObject*)streamState,
                0);
    }
}

Boolean CAudioService::DiscardRmtSbmxFullVolDeathHandlerFor(
    /* [in] */ IBinder* cb)
{
    AutoPtr<IIterator> it;
    mRmtSbmxFullVolDeathHandlers->GetIterator((IIterator**)&it);
    Boolean b;
    while (it->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<RmtSbmxFullVolDeathHandler> handler =
                (RmtSbmxFullVolDeathHandler*)(IObject*)obj.Get();

        if (handler->IsHandlerFor(cb, &b), b) {
            handler->Forget();
            mRmtSbmxFullVolDeathHandlers->Remove((IInterface*)(IObject*)handler);
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CAudioService::HasRmtSbmxFullVolDeathHandlerFor(
    /* [in] */ IBinder* cb)
{
    AutoPtr<IIterator> it;
    mRmtSbmxFullVolDeathHandlers->GetIterator((IIterator**)&it);
    Boolean b;
    while (it->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<RmtSbmxFullVolDeathHandler> handler =
                (RmtSbmxFullVolDeathHandler*)(IObject*)obj.Get();

        if (handler->IsHandlerFor(cb, &b), b) {
            return TRUE;
        }
    }
    return FALSE;
}

void CAudioService::DoSetMasterVolume(
    /* [in] */ Float volume,
    /* [in] */ Int32 flags)
{
    // don't allow changing master volume when muted
    Boolean b;
    if (AudioSystem::GetMasterMute(&b), !b) {
        Int32 oldVolume;
        GetMasterVolume(&oldVolume);
        AudioSystem::SetMasterVolume(volume);

        Int32 newVolume;
        GetMasterVolume(&newVolume);
        if (newVolume != oldVolume) {
            // Post a persist master volume msg
            SendMsg(mAudioHandler, MSG_PERSIST_MASTER_VOLUME, SENDMSG_REPLACE,
                    Elastos::Core::Math::Round(volume * (Float)1000.0), 0, NULL, PERSIST_DELAY);
        }
        // Send the volume update regardless whether there was a change.
        SendMasterVolumeUpdate(flags, oldVolume, newVolume);
    }
}

ECode CAudioService::EnsureValidRingerMode(
    /* [in] */ Int32 ringerMode)
{
    Boolean b;
    if (CAudioManager::IsValidRingerMode(ringerMode, &b), !b) {
        // throw new IllegalArgumentException("Bad ringer mode " + ringerMode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

void CAudioService::CheckZen(
    /* [in] */ Int32 ringerMode)
{
    // leave zen when callers set ringer-mode = normal or vibrate
    Int32 zen;
    Settings::Global::GetInt32(mContentResolver,
            ISettingsGlobal::ZEN_MODE, ISettingsGlobal::ZEN_MODE_OFF, &zen);
    if (ringerMode != IAudioManager::RINGER_MODE_SILENT &&
            zen != ISettingsGlobal::ZEN_MODE_OFF) {
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        Boolean b;
        Settings::Global::PutInt32(mContentResolver,
                ISettingsGlobal::ZEN_MODE, ISettingsGlobal::ZEN_MODE_OFF, &b);
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
    }
}

void CAudioService::SetRingerModeInt(
    /* [in] */ Int32 ringerMode,
    /* [in] */ Boolean persist)
{
    synchronized(mSettingsLock) {
        mRingerMode = ringerMode;
    }

    // Mute stream if not previously muted by ringer mode and ringer mode
    // is not IAudioManager::RINGER_MODE_NORMAL and stream is affected by ringer mode.
    // Unmute stream if previously muted by ringer mode and ringer mode
    // is IAudioManager::RINGER_MODE_NORMAL or stream is not affected by ringer mode.
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = numStreamTypes - 1; streamType >= 0; streamType--) {
        if (IsStreamMutedByRingerMode(streamType)) {
            Boolean b;
            if ((IsStreamAffectedByRingerMode(streamType, &b), !b) ||
                ringerMode == IAudioManager::RINGER_MODE_NORMAL) {
                // ring and notifications volume should never be 0 when not silenced
                // on voice capable devices
                if (IsPlatformVoice() &&
                        (*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_RING) {
                    AutoPtr<VolumeStreamState> vss = (*mStreamStates)[streamType];
                    synchronized(vss) {
                        //TODO: delete this lock when mIndex is IConcurrentHashMap
                        AutoLock lock((*mStreamStates)[streamType]->mIndex);
                        AutoPtr<ISet> set;
                        IMap::Probe((*mStreamStates)[streamType]->mIndex)->GetEntrySet((ISet**)&set);
                        AutoPtr<IIterator> i;
                        set->GetIterator((IIterator**)&i);
                        while (i->HasNext(&b), b) {
                            AutoPtr<IInterface> obj;
                            i->GetNext((IInterface**)&obj);
                            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
                            obj = NULL;
                            entry->GetValue((IInterface**)&obj);
                            Int32 value;
                            IInteger32::Probe(obj)->GetValue(&value);
                            if (value == 0) {
                                AutoPtr<IInteger32> i32;
                                CInteger32::New(10, (IInteger32**)&i32);
                                obj = NULL;
                                entry->SetValue(i32, (IInterface**)&obj);
                            }
                        }
                    }
                }
                (*mStreamStates)[streamType]->Mute(NULL, FALSE);
                mRingerModeMutedStreams &= ~(1 << streamType);
            }
        }
        else {
            Boolean b;
            if ((IsStreamAffectedByRingerMode(streamType, &b), b) &&
                ringerMode != IAudioManager::RINGER_MODE_NORMAL) {
               (*mStreamStates)[streamType]->Mute(NULL, TRUE);
               mRingerModeMutedStreams |= (1 << streamType);
           }
        }
    }

    // Post a persist ringer mode msg
    if (persist) {
        SendMsg(mAudioHandler, MSG_PERSIST_RINGER_MODE,
                SENDMSG_REPLACE, 0, 0, NULL, PERSIST_DELAY);
    }
}

void CAudioService::RestoreMasterVolume()
{
    if (mUseFixedVolume) {
        AudioSystem::SetMasterVolume(1.0f);
        return;
    }
    if (mUseMasterVolume) {
        Float volume;
        Settings::System::GetFloatForUser(mContentResolver,
                ISettingsSystem::VOLUME_MASTER, -1.0f, IUserHandle::USER_CURRENT, &volume);
        if (volume >= 0.0f) {
            AudioSystem::SetMasterVolume(volume);
        }
    }
}

Int32 CAudioService::SetModeInt(
    /* [in] */ Int32 mode,
    /* [in] */ IBinder* _cb,
    /* [in] */ Int32 pid)
{
    if (DEBUG_MODE) { Logger::V(TAG, "setModeInt(mode=%d, pid=%d)", mode, pid); }
    AutoPtr<IBinder> cb = _cb;
    Int32 newModeOwnerPid = 0;
    if (cb == NULL) {
        Logger::E(TAG, "setModeInt() called with NULL binder");
        return newModeOwnerPid;
    }

    AutoPtr<SetModeDeathHandler> hdlr;
    AutoPtr<IIterator> iter;
    mSetModeDeathHandlers->GetIterator((IIterator**)&iter);
    Boolean b;
    while (iter->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<SetModeDeathHandler> h = (SetModeDeathHandler*)(IObject*)obj.Get();
        Int32 p;
        h->GetPid(&p);
        if (p == pid) {
            hdlr = h;
            // Remove from client list so that it is re-inserted at top of list
            iter->Remove();
            AutoPtr<IBinder> binder;
            hdlr->GetBinder((IBinder**)&binder);
            AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
            if (proxy != NULL) proxy->UnlinkToDeath(hdlr, 0, &b);
            break;
        }
    }
    Int32 status = IAudioSystem::AUDIO_STATUS_OK;
    do {
        if (mode == IAudioSystem::MODE_NORMAL) {
            // get new mode from client at top the list if any
            if (mSetModeDeathHandlers->IsEmpty(&b), !b) {
                AutoPtr<IInterface> obj;
                mSetModeDeathHandlers->Get(0, (IInterface**)&obj);
                hdlr = (SetModeDeathHandler*)(IObject*)obj.Get();
                hdlr->GetBinder((IBinder**)&cb);
                hdlr->GetMode(&mode);
                if (DEBUG_MODE) {
                    Logger::W(TAG, " using mode=%d instead due to death hdlr at pid=%d",
                            mode, hdlr->mPid);
                }
            }
        }
        else {
            if (hdlr == NULL) {
                hdlr = new SetModeDeathHandler(this, cb, pid);
            }
            // Register for client death notification
            // try {
            AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
            if (proxy != NULL) proxy->LinkToDeath(hdlr, 0);
            // } catch (RemoteException e) {
            //     // Client has died!
            //     Log.w(TAG, "setMode() could not link to "+cb+" binder death");
            // }

            // Last client to call setMode() is always at top of client list
            // as required by SetModeDeathHandler.binderDied()
            mSetModeDeathHandlers->Add(0, (IInterface*)(IObject*)hdlr);
            hdlr->SetMode(mode);
        }

        if (mode != mMode) {
            AudioSystem::SetPhoneState(mode, &status);
            if (status == IAudioSystem::AUDIO_STATUS_OK) {
                if (DEBUG_MODE) { Logger::V(TAG, " mode successfully set to %d", mode); }
                mMode = mode;
            }
            else {
                if (hdlr != NULL) {
                    mSetModeDeathHandlers->Remove((IInterface*)(IObject*)hdlr);
                    AutoPtr<IProxy> proxy = (IProxy*)cb->Probe(EIID_IProxy);
                    if (proxy != NULL) proxy->UnlinkToDeath(hdlr, 0, &b);
                }
                // force reading new top of mSetModeDeathHandlers stack
                if (DEBUG_MODE) { Logger::W(TAG, " mode set to MODE_NORMAL after phoneState pb"); }
                mode = IAudioSystem::MODE_NORMAL;
            }
        }
        else {
            status = IAudioSystem::AUDIO_STATUS_OK;
        }
    } while (status != IAudioSystem::AUDIO_STATUS_OK && (mSetModeDeathHandlers->IsEmpty(&b), !b));

    if (status == IAudioSystem::AUDIO_STATUS_OK) {
        if (mode != IAudioSystem::MODE_NORMAL) {
            if (mSetModeDeathHandlers->IsEmpty(&b), b) {
                Logger::E(TAG, "setMode() different from MODE_NORMAL with empty mode client stack");
            }
            else {
                AutoPtr<IInterface> obj;
                mSetModeDeathHandlers->Get(0, (IInterface**)&obj);
                AutoPtr<SetModeDeathHandler> h = (SetModeDeathHandler*)(IObject*)obj.Get();
                h->GetPid(&newModeOwnerPid);
            }
        }
        Int32 streamType = GetActiveStreamType(IAudioManager::USE_DEFAULT_STREAM_TYPE);
        Int32 device = GetDeviceForStream(streamType);
        Int32 index;
        (*mStreamStates)[(*mStreamVolumeAlias)[streamType]]->GetIndex(device, &index);
        SetStreamVolumeInt((*mStreamVolumeAlias)[streamType], index, device, TRUE);

        UpdateStreamVolumeAlias(TRUE /*updateVolumes*/);
    }
    return newModeOwnerPid;
}

void CAudioService::LoadTouchSoundAssetDefaults()
{
    AutoPtr<ICharSequence> cs;
    CString::New(String("Effect_Tick.ogg"), (ICharSequence**)&cs);
    SOUND_EFFECT_FILES->Add(cs);
    for (Int32 i = 0; i < IAudioManager::NUM_SOUND_EFFECTS; i++) {
        SOUND_EFFECT_FILES_MAP[i][0] = 0;
        SOUND_EFFECT_FILES_MAP[i][1] = -1;
    }
}

void CAudioService::LoadTouchSoundAssets()
{
    AutoPtr<IXmlResourceParser> parser;

    // only load assets once.
    Boolean b;
    if (SOUND_EFFECT_FILES->IsEmpty(&b), !b) {
        return;
    }

    LoadTouchSoundAssetDefaults();

    // try {
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetXml(R::xml::audio_assets, (IXmlResourceParser**)&parser);

    IXmlPullParser* xmlPullParser = IXmlPullParser::Probe(parser);
    XmlUtils::BeginDocument(xmlPullParser, TAG_AUDIO_ASSETS);
    String version;
    xmlPullParser->GetAttributeValue(String(NULL), ATTR_VERSION, &version);
    Boolean inTouchSoundsGroup = FALSE;

    if (ASSET_FILE_VERSION.Equals(version)) {
        while (TRUE) {
            XmlUtils::NextElement(xmlPullParser);
            String element;
            xmlPullParser->GetName(&element);
            if (element == NULL) {
                break;
            }
            if (element.Equals(TAG_GROUP)) {
                String name;
                xmlPullParser->GetAttributeValue(String(NULL), ATTR_GROUP_NAME, &name);
                if (name.Equals(GROUP_TOUCH_SOUNDS)) {
                    inTouchSoundsGroup = TRUE;
                    break;
                }
            }
        }
        while (inTouchSoundsGroup) {
            XmlUtils::NextElement(xmlPullParser);
            String element;
            xmlPullParser->GetName(&element);
            if (element == NULL) {
                break;
            }
            if (element.Equals(TAG_ASSET)) {
                String id;
                xmlPullParser->GetAttributeValue(String(NULL), ATTR_ASSET_ID, &id);
                String file;
                xmlPullParser->GetAttributeValue(String(NULL), ATTR_ASSET_FILE, &file);
                Int32 fx = CAudioManager::GetInt32FieldValue(id);
                if (fx < 0) {
                    Logger::W(TAG, "Invalid touch sound ID:%d", id.string());
                    continue;
                }

                AutoPtr<ICharSequence> cs;
                CString::New(file, (ICharSequence**)&cs);
                Int32 i;
                SOUND_EFFECT_FILES->IndexOf(cs, &i);
                if (i == -1) {
                    SOUND_EFFECT_FILES->GetSize(&i);
                    AutoPtr<ICharSequence> cs;
                    CString::New(file, (ICharSequence**)&cs);
                    SOUND_EFFECT_FILES->Add(cs);
                }
                SOUND_EFFECT_FILES_MAP[fx][0] = i;
            }
            else {
                break;
            }
        }
    }
    // } catch (Resources.NotFoundException e) {
    //     Log.w(TAG, "audio assets file not found", e);
    // } catch (XmlPullParserException e) {
    //     Log.w(TAG, "XML parser exception reading touch sound assets", e);
    // } catch (IOException e) {
    //     Log.w(TAG, "I/O exception reading touch sound assets", e);
    // } finally {
    if (parser != NULL) {
        parser->Close();
    }
    // }
}

void CAudioService::ReadAudioSettings(
    /* [in] */ Boolean userSwitch)
{
    // restore ringer mode, ringer mode affected streams, mute affected streams and vibrate settings
    ReadPersistedSettings();

    // restore volume settings
    Int32 numStreamTypes;
    AudioSystem::GetNumStreamTypes(&numStreamTypes);
    for (Int32 streamType = 0; streamType < numStreamTypes; streamType++) {
        AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[streamType];

        if (userSwitch && (*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC) {
            continue;
        }

        synchronized(streamState) {
            streamState->ReadSettings();

            // unmute stream that was muted but is not affect by mute anymore
            Boolean b;
            if (streamState->IsMuted() && (((IsStreamAffectedByMute(streamType, &b), !b) &&
                    !IsStreamMutedByRingerMode(streamType)) || mUseFixedVolume)) {
                Int32 size;
                streamState->mDeathHandlers->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    streamState->mDeathHandlers->Get(i, (IInterface**)&obj);
                    AutoPtr<VolumeStreamState::VolumeDeathHandler> h =
                            (VolumeStreamState::VolumeDeathHandler*)(IObject*)obj.Get();
                    h->mMuteCount = 1;
                    h->Mute(FALSE);
                }
            }
        }
    }

    // apply new ringer mode before checking volume for alias streams so that streams
    // muted by ringer mode have the correct volume
    Int32 mode;
    GetRingerMode(&mode);
    SetRingerModeInt(mode, FALSE);

    CheckAllFixedVolumeDevices();
    CheckAllAliasStreamVolumes();

    synchronized(mSafeMediaVolumeStateLock) {
        Int32 val;
        Settings::System::GetInt32ForUser(mContentResolver,
                ISettingsSecure::UNSAFE_VOLUME_MUSIC_ACTIVE_MS, 0, IUserHandle::USER_CURRENT, &val);
        mMusicActiveMs = MathUtils::Constrain(val,
                0, UNSAFE_VOLUME_MUSIC_ACTIVE_MS_MAX);
        Int32 value;
        mSafeMediaVolumeState->GetValue(&value);
        if (value == SAFE_MEDIA_VOLUME_ACTIVE) {
            EnforceSafeMediaVolume();
        }
    }
}

void CAudioService::CheckScoAudioState()
{
    Int32 state;
    if (mBluetoothHeadset != NULL && mBluetoothHeadsetDevice != NULL &&
            mScoAudioState == SCO_STATE_INACTIVE
            && (mBluetoothHeadset->GetAudioState(mBluetoothHeadsetDevice, &state), state)
            != IBluetoothHeadset::STATE_AUDIO_DISCONNECTED) {
        mScoAudioState = SCO_STATE_ACTIVE_EXTERNAL;
    }
}

AutoPtr<CAudioService::ScoClient> CAudioService::GetScoClient(
    /* [in] */ IBinder* cb,
    /* [in] */ Boolean create)
{
    synchronized(mScoClients) {
        AutoPtr<ScoClient> client;
        Int32 size;
        mScoClients->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mScoClients->Get(i, (IInterface**)&obj);
            client = (ScoClient*)(IObject*)obj.Get();
            AutoPtr<IBinder> binder;
            client->GetBinder((IBinder**)&binder);
            if (binder.Get() == cb) {
                return client;
            }
        }
        if (create) {
            client = new ScoClient(this, cb);
            mScoClients->Add((IInterface*)(IObject*)client);
        }
        return client;
    }
    return NULL;
}

Boolean CAudioService::GetBluetoothHeadset()
{
    Boolean result = FALSE;
    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
    if (adapter != NULL) {
        adapter->GetProfileProxy(mContext, mBluetoothProfileServiceListener,
                IBluetoothProfile::HEADSET, &result);
    }

    // If we could not get a bluetooth headset proxy, send a failure message
    // without delay to reset the SCO audio state and clear SCO clients.
    // If we could get a proxy, send a delayed failure message that will reset our state
    // in case we don't receive onServiceConnected().
    SendMsg(mAudioHandler, MSG_BT_HEADSET_CNCT_FAILED,
            SENDMSG_REPLACE, 0, 0, NULL, result ? BT_HEADSET_CNCT_TIMEOUT_MS : 0);
    return result;
}

void CAudioService::DisconnectBluetoothSco(
    /* [in] */ Int32 exceptPid)
{
    synchronized(mScoClients) {
        CheckScoAudioState();
        if (mScoAudioState == SCO_STATE_ACTIVE_EXTERNAL ||
                mScoAudioState == SCO_STATE_DEACTIVATE_EXT_REQ) {
            if (mBluetoothHeadsetDevice != NULL) {
                if (mBluetoothHeadset != NULL) {
                    Boolean b;
                    if (mBluetoothHeadset->StopVoiceRecognition(
                            mBluetoothHeadsetDevice, &b), !b) {
                        SendMsg(mAudioHandler, MSG_BT_HEADSET_CNCT_FAILED,
                                SENDMSG_REPLACE, 0, 0, NULL, 0);
                    }
                }
                else if (mScoAudioState == SCO_STATE_ACTIVE_EXTERNAL &&
                        GetBluetoothHeadset()) {
                    mScoAudioState = SCO_STATE_DEACTIVATE_EXT_REQ;
                }
            }
        }
        else {
            ClearAllScoClients(exceptPid, TRUE);
        }
    }
}

void CAudioService::ResetBluetoothSco()
{
    synchronized(mScoClients) {
        ClearAllScoClients(0, FALSE);
        mScoAudioState = SCO_STATE_INACTIVE;
        BroadcastScoConnectionState(IAudioManager::SCO_AUDIO_STATE_DISCONNECTED);
    }
}

void CAudioService::BroadcastScoConnectionState(
    /* [in] */ Int32 state)
{
    SendMsg(mAudioHandler, MSG_BROADCAST_BT_CONNECTION_STATE,
            SENDMSG_QUEUE, state, 0, NULL, 0);
}

void CAudioService::OnBroadcastScoConnectionState(
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

void CAudioService::OnCheckMusicActive()
{
    synchronized(mSafeMediaVolumeStateLock) {
        Int32 value;
        mSafeMediaVolumeState->GetValue(&value);
        if (value == SAFE_MEDIA_VOLUME_INACTIVE) {
            Int32 device = GetDeviceForStream(IAudioSystem::STREAM_MUSIC);

            if ((device & mSafeMediaVolumeDevices) != 0) {
                SendMsg(mAudioHandler,
                        MSG_CHECK_MUSIC_ACTIVE,
                        SENDMSG_REPLACE,
                        0,
                        0,
                        NULL,
                        MUSIC_ACTIVE_POLL_PERIOD_MS);
                Int32 index;
                (*mStreamStates)[IAudioSystem::STREAM_MUSIC]->GetIndex(device, &index);
                Boolean b;
                if ((AudioSystem::IsStreamActive(IAudioSystem::STREAM_MUSIC, 0, &b), b) &&
                        (index > mSafeMediaVolumeIndex)) {
                    // Approximate cumulative active music time
                    mMusicActiveMs += MUSIC_ACTIVE_POLL_PERIOD_MS;
                    if (mMusicActiveMs > UNSAFE_VOLUME_MUSIC_ACTIVE_MS_MAX) {
                        SetSafeMediaVolumeEnabled(TRUE);
                        mMusicActiveMs = 0;
                    }
                    SaveMusicActiveMs();
                }
            }
        }
    }
}

void CAudioService::SaveMusicActiveMs()
{
    AutoPtr<IMessage> msg;
    mAudioHandler->ObtainMessage(MSG_PERSIST_MUSIC_ACTIVE_MS, mMusicActiveMs, 0, (IMessage**)&msg);
    msg->SendToTarget();
}

void CAudioService::OnConfigureSafeVolume(
    /* [in] */ Boolean force)
{
    synchronized(mSafeMediaVolumeStateLock) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> conf;
        res->GetConfiguration((IConfiguration**)&conf);
        Int32 mcc;
        conf->GetMcc(&mcc);
        if ((mMcc != mcc) || ((mMcc == 0) && force)) {
            res->GetInteger(R::integer::config_safe_media_volume_index, &mSafeMediaVolumeIndex);
            mSafeMediaVolumeIndex *= 10;
            Boolean b1, b2;
            SystemProperties::GetBoolean(String("audio.safemedia.force"), FALSE, &b1);
            res->GetBoolean(R::bool_::config_safe_media_volume_enabled, &b2);
            Boolean safeMediaVolumeEnabled = b1 || b2;

            // The persisted state is either "disabled" or "active": this is the state applied
            // next time we boot and cannot be "inactive"
            Int32 persistedState;
            if (safeMediaVolumeEnabled) {
                persistedState = SAFE_MEDIA_VOLUME_ACTIVE;
                // The state can already be "inactive" here if the user has forced it before
                // the 30 seconds timeout for forced configuration. In this case we don't reset
                // it to "active".
                Int32 value;
                mSafeMediaVolumeState->GetValue(&value);
                if (value != SAFE_MEDIA_VOLUME_INACTIVE) {
                    if (mMusicActiveMs == 0) {
                        mSafeMediaVolumeState = NULL;
                        CInteger32::New(SAFE_MEDIA_VOLUME_ACTIVE, (IInteger32**)&mSafeMediaVolumeState);
                        EnforceSafeMediaVolume();
                    }
                    else {
                        // We have existing playback time recorded, already confirmed.
                        mSafeMediaVolumeState = NULL;
                        CInteger32::New(SAFE_MEDIA_VOLUME_INACTIVE, (IInteger32**)&mSafeMediaVolumeState);
                    }
                }
            }
            else {
                persistedState = SAFE_MEDIA_VOLUME_DISABLED;
                mSafeMediaVolumeState = NULL;
                CInteger32::New(SAFE_MEDIA_VOLUME_DISABLED, (IInteger32**)&mSafeMediaVolumeState);
            }
            mMcc = mcc;
            SendMsg(mAudioHandler,
                    MSG_PERSIST_SAFE_VOLUME_STATE,
                    SENDMSG_QUEUE,
                    persistedState,
                    0,
                    NULL,
                    0);
        }
    }
}

Int32 CAudioService::CheckForRingerModeChange(
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 step)
{
    Int32 result = FLAG_ADJUST_VOLUME;
    Int32 ringerMode;
    GetRingerMode(&ringerMode);

    switch (ringerMode) {
    case IAudioManager::RINGER_MODE_NORMAL:
        if (direction == IAudioManager::ADJUST_LOWER) {
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
                if ((oldIndex < step)
                        && VOLUME_SETS_RINGER_MODE_SILENT
                        && mPrevVolDirection != IAudioManager::ADJUST_LOWER) {
                    ringerMode = IAudioManager::RINGER_MODE_SILENT;
                }
            }
        }
        break;
    case IAudioManager::RINGER_MODE_VIBRATE:
        if (!mHasVibrator) {
            Logger::E(TAG, String("checkForRingerModeChange() current ringer mode is vibrate") +
                    "but no vibrator is present");
            break;
        }
        if ((direction == IAudioManager::ADJUST_LOWER)) {
            if (VOLUME_SETS_RINGER_MODE_SILENT
                    && mPrevVolDirection != IAudioManager::ADJUST_LOWER) {
                ringerMode = IAudioManager::RINGER_MODE_SILENT;
            }
        }
        else if (direction == IAudioManager::ADJUST_RAISE) {
            ringerMode = IAudioManager::RINGER_MODE_NORMAL;
        }
        result &= ~FLAG_ADJUST_VOLUME;
        break;
    case IAudioManager::RINGER_MODE_SILENT:
        if (direction == IAudioManager::ADJUST_RAISE) {
            if (PREVENT_VOLUME_ADJUSTMENT_IF_SILENT) {
                result |= IAudioManager::FLAG_SHOW_SILENT_HINT;
            }
            else {
              if (mHasVibrator) {
                  ringerMode = IAudioManager::RINGER_MODE_VIBRATE;
              }
              else {
                  ringerMode = IAudioManager::RINGER_MODE_NORMAL;
              }
            }
        }
        result &= ~FLAG_ADJUST_VOLUME;
        break;
    default:
        Logger::E(TAG, "checkForRingerModeChange() wrong ringer mode: %d", ringerMode);
        break;
    }

    SetRingerMode(ringerMode, FALSE /*checkZen*/);

    mPrevVolDirection = direction;

    return result;
}

Boolean CAudioService::IsStreamMutedByRingerMode(
    /* [in] */ Int32 streamType)
{
    return (mRingerModeMutedStreams & (1 << streamType)) != 0;
}

ECode CAudioService::EnsureValidDirection(
    /* [in] */ Int32 direction)
{
    if (direction < IAudioManager::ADJUST_LOWER || direction > IAudioManager::ADJUST_RAISE) {
        // throw new IllegalArgumentException("Bad direction " + direction);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioService::EnsureValidSteps(
    /* [in] */ Int32 steps)
{
    if (Elastos::Core::Math::Abs(steps) > MAX_BATCH_VOLUME_ADJUST_STEPS) {
        // throw new IllegalArgumentException("Bad volume adjust steps " + steps);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioService::EnsureValidStreamType(
    /* [in] */ Int32 streamType)
{
    if (streamType < 0 || streamType >= mStreamStates->GetLength()) {
        // throw new IllegalArgumentException("Bad stream type " + streamType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Boolean CAudioService::IsInCommunication()
{
    Boolean IsInCall = FALSE;

    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::TELECOM_SERVICE, ((IInterface**)&service));
    AutoPtr<ITelecomManager> telecomManager = ITelecomManager::Probe(service);
    if (telecomManager) {
        telecomManager->IsInCall(&IsInCall);
    }

    Int32 mode;
    GetMode(&mode);
    return (IsInCall || mode == IAudioManager::MODE_IN_COMMUNICATION);
}

Boolean CAudioService::IsAfMusicActiveRecently(
    /* [in] */ Int32 delay_ms)
{
    Boolean b1, b2;
    AudioSystem::IsStreamActive(IAudioSystem::STREAM_MUSIC, delay_ms, &b1);
    AudioSystem::IsStreamActiveRemotely(IAudioSystem::STREAM_MUSIC, delay_ms, &b2);
    return b1 || b2;
}

Int32 CAudioService::GetActiveStreamType(
    /* [in] */ Int32 suggestedStreamType)
{
    switch (mPlatformType) {
        case PLATFORM_VOICE:
            if (IsInCommunication()) {
                Int32 val;
                AudioSystem::GetForceUse(IAudioSystem::FOR_COMMUNICATION, &val);
                if (val == IAudioSystem::FORCE_BT_SCO) {
                    // Log.v(TAG, "getActiveStreamType: Forcing STREAM_BLUETOOTH_SCO...");
                    return IAudioSystem::STREAM_BLUETOOTH_SCO;
                }
                else {
                    // Log.v(TAG, "getActiveStreamType: Forcing STREAM_VOICE_CALL...");
                    return IAudioSystem::STREAM_VOICE_CALL;
                }
            }
            else if (suggestedStreamType == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
                if (IsAfMusicActiveRecently(StreamOverride::sDelayMs)) {
                    if (DEBUG_VOL)
                        Logger::V(TAG, "getActiveStreamType: Forcing STREAM_MUSIC stream active");
                    return IAudioSystem::STREAM_MUSIC;
                    }
                    else {
                        if (DEBUG_VOL)
                            Logger::V(TAG, "getActiveStreamType: Forcing STREAM_RING b/c default");
                        return IAudioSystem::STREAM_RING;
                }
            }
            else if (IsAfMusicActiveRecently(0)) {
                if (DEBUG_VOL)
                    Logger::V(TAG, "getActiveStreamType: Forcing STREAM_MUSIC stream active");
                return IAudioSystem::STREAM_MUSIC;
            }
            break;
        case PLATFORM_TELEVISION:
            if (suggestedStreamType == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
                    // TV always defaults to STREAM_MUSIC
                    return IAudioSystem::STREAM_MUSIC;
            }
            break;
        default: {
            Boolean b1, b2;
            AudioSystem::IsStreamActive(IAudioSystem::STREAM_NOTIFICATION,
                    StreamOverride::sDelayMs, &b1);
            AudioSystem::IsStreamActive(IAudioSystem::STREAM_RING,
                            StreamOverride::sDelayMs, &b2);

            if (IsInCommunication()) {
                Int32 val;
                AudioSystem::GetForceUse(IAudioSystem::FOR_COMMUNICATION, &val);
                if (val == IAudioSystem::FORCE_BT_SCO) {
                    if (DEBUG_VOL) Logger::V(TAG, "getActiveStreamType: Forcing STREAM_BLUETOOTH_SCO");
                    return IAudioSystem::STREAM_BLUETOOTH_SCO;
                }
                else {
                    if (DEBUG_VOL)  Logger::V(TAG, "getActiveStreamType: Forcing STREAM_VOICE_CALL");
                    return IAudioSystem::STREAM_VOICE_CALL;
                }
            }
            else if (b1 || b2) {
                if (DEBUG_VOL) Logger::V(TAG, "getActiveStreamType: Forcing STREAM_NOTIFICATION");
                return IAudioSystem::STREAM_NOTIFICATION;
            }
            else if (suggestedStreamType == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
                if (IsAfMusicActiveRecently(StreamOverride::sDelayMs)) {
                    if (DEBUG_VOL) Logger::V(TAG, "getActiveStreamType: forcing STREAM_MUSIC");
                    return IAudioSystem::STREAM_MUSIC;
                }
                else {
                    if (DEBUG_VOL) Logger::V(TAG,
                            "getActiveStreamType: using STREAM_NOTIFICATION as default");
                    return IAudioSystem::STREAM_NOTIFICATION;
                }
            }
            break;
        }
    }
    if (DEBUG_VOL) Logger::V(TAG, "getActiveStreamType: Returning suggested type "
            + suggestedStreamType);
    return suggestedStreamType;
}

void CAudioService::BroadcastRingerMode(
    /* [in] */ Int32 ringerMode)
{
    // Send sticky broadcast
    AutoPtr<IIntent> broadcast;
    CIntent::New(IAudioManager::RINGER_MODE_CHANGED_ACTION, (IIntent**)&broadcast);
    broadcast->PutExtra(IAudioManager::EXTRA_RINGER_MODE, ringerMode);
    broadcast->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    SendStickyBroadcastToAll(broadcast);
}

void CAudioService::BroadcastVibrateSetting(
    /* [in] */ Int32 vibrateType)
{
    // Send broadcast
    if (ActivityManagerNative::IsSystemReady()) {
        AutoPtr<IIntent> broadcast;
        CIntent::New(IAudioManager::VIBRATE_SETTING_CHANGED_ACTION, (IIntent**)&broadcast);
        broadcast->PutExtra(IAudioManager::EXTRA_VIBRATE_TYPE, vibrateType);
        Int32 setting;
        GetVibrateSetting(vibrateType, &setting);
        broadcast->PutExtra(IAudioManager::EXTRA_VIBRATE_SETTING, setting);
        SendBroadcastToAll(broadcast);
    }
}

void CAudioService::QueueMsgUnderWakeLock(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 delay)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // Always acquire the wake lock as AudioService because it is released by the
    // message handler.
    mAudioEventWakeLock->AcquireLock();
    Binder::RestoreCallingIdentity(ident);
    SendMsg(handler, msg, SENDMSG_QUEUE, arg1, arg2, obj, delay);
}

void CAudioService::SendMsg(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg,
    /* [in] */ Int32 existingMsgPolicy,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 delay)
{
    Boolean b;
    if (existingMsgPolicy == SENDMSG_REPLACE) {
        handler->RemoveMessages(msg);
    }
    else if (existingMsgPolicy == SENDMSG_NOOP && (handler->HasMessages(msg, &b), b)) {
        return;
    }

    AutoPtr<IMessage> message;
    handler->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&message);
    handler->SendMessageDelayed(message, delay, &b);
}

Int32 CAudioService::GetDeviceForStream(
    /* [in] */ Int32 stream)
{
    Int32 device;
    AudioSystem::GetDevicesForStream(stream, &device);
    if ((device & (device - 1)) != 0) {
        // Multiple device selection is either:
        //  - speaker + one other device: give priority to speaker in this case.
        //  - one A2DP device + another device: happens with duplicated output. In this case
        // retain the device on the A2DP output as the other must not correspond to an active
        // selection if not the speaker.
        //  - HDMI-CEC system audio mode only output: give priority to available item in order.
        if ((device & IAudioSystem::DEVICE_OUT_SPEAKER) != 0) {
            device = IAudioSystem::DEVICE_OUT_SPEAKER;
        }
        else if ((device & IAudioSystem::DEVICE_OUT_HDMI_ARC) != 0) {
            device = IAudioSystem::DEVICE_OUT_HDMI_ARC;
        }
        else if ((device & IAudioSystem::DEVICE_OUT_SPDIF) != 0) {
            device = IAudioSystem::DEVICE_OUT_SPDIF;
        }
        else if ((device & IAudioSystem::DEVICE_OUT_AUX_LINE) != 0) {
            device = IAudioSystem::DEVICE_OUT_AUX_LINE;
        }
        else {
            device &= IAudioSystem::DEVICE_OUT_ALL_A2DP;
        }
    }
    return device;
}

void CAudioService::MakeA2dpDeviceAvailable(
    /* [in] */ const String& address)
{
    // enable A2DP before notifying A2DP connection to avoid unecessary processing in
    // audio policy manager
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[IAudioSystem::STREAM_MUSIC];
    SendMsg(mAudioHandler, MSG_SET_DEVICE_VOLUME, SENDMSG_QUEUE,
            IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, 0, (IInterface*)(IObject*)streamState, 0);
    SetBluetoothA2dpOnInt(TRUE);
    AudioSystem::SetDeviceConnectionState(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
            IAudioSystem::DEVICE_STATE_AVAILABLE,
            address);
    // Reset A2DP suspend state each time a new sink is connected
    AudioSystem::SetParameters(String("A2dpSuspended=FALSE"));
    AutoPtr<IInteger32> i32;
    CInteger32::New(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, (IInteger32**)&i32);
    AutoPtr<ICharSequence> cs;
    CString::New(address, (ICharSequence**)&cs);
    mConnectedDevices->Put(i32, cs);
}

void CAudioService::OnSendBecomingNoisyIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAudioManager::ACTION_AUDIO_BECOMING_NOISY, (IIntent**)&intent);
    SendBroadcastToAll(intent);
}

void CAudioService::MakeA2dpDeviceUnavailableNow(
    /* [in] */ const String& address)
{
    synchronized(mA2dpAvrcpLock) {
        mAvrcpAbsVolSupported = FALSE;
    }
    AudioSystem::SetDeviceConnectionState(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
            IAudioSystem::DEVICE_STATE_UNAVAILABLE,
            address);
    AutoPtr<IInteger32> i32;
    CInteger32::New(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, (IInteger32**)&i32);
    mConnectedDevices->Remove(i32);
    synchronized(mCurAudioRoutes) {
        // Remove A2DP routes as well
        AutoPtr<ICharSequence> cs;
        mCurAudioRoutes->GetBluetoothName((ICharSequence**)&cs);
        if (cs != NULL) {
            mCurAudioRoutes->SetBluetoothName(NULL);
            SendMsg(mAudioHandler, MSG_REPORT_NEW_ROUTES,
                    SENDMSG_NOOP, 0, 0, NULL, 0);
        }
    }
}

void CAudioService::MakeA2dpDeviceUnavailableLater(
    /* [in] */ const String& address)
{
    // prevent any activity on the A2DP audio output to avoid unwanted
    // reconnection of the sink.
    AudioSystem::SetParameters(String("A2dpSuspended=TRUE"));
    // the device will be made unavailable later, so consider it disconnected right away
    AutoPtr<IInteger32> i32;
    CInteger32::New(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, (IInteger32**)&i32);
    mConnectedDevices->Remove(i32);
    // send the delayed message to make the device unavailable later
    AutoPtr<ICharSequence> cs;
    CString::New(address, (ICharSequence**)&cs);
    AutoPtr<IMessage> msg;
    mAudioHandler->ObtainMessage(MSG_BTA2DP_DOCK_TIMEOUT, cs, (IMessage**)&msg);
    Boolean b;
    mAudioHandler->SendMessageDelayed(msg, BTA2DP_DOCK_TIMEOUT_MILLIS, &b);
}

void CAudioService::MakeA2dpSrcAvailable(
    /* [in] */ const String& address)
{
    AudioSystem::SetDeviceConnectionState(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP,
            IAudioSystem::DEVICE_STATE_AVAILABLE,
            address);
    AutoPtr<IInteger32> i32;
    CInteger32::New(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP, (IInteger32**)&i32);
    AutoPtr<ICharSequence> cs;
    CString::New(address, (ICharSequence**)&cs);
    mConnectedDevices->Put(i32, cs);
}

void CAudioService::MakeA2dpSrcUnavailable(
    /* [in] */ const String& address)
{
    AudioSystem::SetDeviceConnectionState(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP,
            IAudioSystem::DEVICE_STATE_UNAVAILABLE,
            address);
    AutoPtr<IInteger32> i32;
    CInteger32::New(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP, (IInteger32**)&i32);
    mConnectedDevices->Remove(i32);
}

void CAudioService::CancelA2dpDeviceTimeout()
{
    mAudioHandler->RemoveMessages(MSG_BTA2DP_DOCK_TIMEOUT);
}

Boolean CAudioService::HasScheduledA2dpDockTimeout()
{
    Boolean b;
    mAudioHandler->HasMessages(MSG_BTA2DP_DOCK_TIMEOUT, &b);
    return b;
}

void CAudioService::OnSetA2dpSinkConnectionState(
    /* [in] */ IBluetoothDevice* btDevice,
    /* [in] */ Int32 state)
{
    if (DEBUG_VOL) {
        // Logger::D(TAG, "onSetA2dpSinkConnectionState btDevice="+btDevice+"state="+state);
    }
    if (btDevice == NULL) {
        return;
    }
    String address;
    btDevice->GetAddress(&address);
    if (!CBluetoothAdapter::CheckBluetoothAddress(address)) {
        address = String("");
    }

    synchronized(mConnectedDevices) {
        AutoPtr<IInteger32> i32;
        CInteger32::New(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, (IInteger32**)&i32);
        Boolean b;
        mConnectedDevices->ContainsKey(i32, &b);
        AutoPtr<IInterface> obj;
        mConnectedDevices->Get(i32, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        Boolean isConnected = (b && str.Equals(address));

        if (isConnected && state != IBluetoothProfile::STATE_CONNECTED) {
            if (btDevice->IsBluetoothDock(&b), b) {
                if (state == IBluetoothProfile::STATE_DISCONNECTED) {
                    // introduction of a delay for transient disconnections of docks when
                    // power is rapidly turned off/on, this message will be canceled if
                    // we reconnect the dock under a preset delay
                    MakeA2dpDeviceUnavailableLater(address);
                    // the next time isConnected is evaluated, it will be FALSE for the dock
                }
            }
            else {
                MakeA2dpDeviceUnavailableNow(address);
            }
            synchronized(mCurAudioRoutes) {
                AutoPtr<ICharSequence> cs;
                mCurAudioRoutes->GetBluetoothName((ICharSequence**)&cs);
                if (cs != NULL) {
                    mCurAudioRoutes->SetBluetoothName(NULL);
                    SendMsg(mAudioHandler, MSG_REPORT_NEW_ROUTES,
                            SENDMSG_NOOP, 0, 0, NULL, 0);
                }
            }
        }
        else if (!isConnected && state == IBluetoothProfile::STATE_CONNECTED) {
            if (btDevice->IsBluetoothDock(&b), b) {
                // this could be a reconnection after a transient disconnection
                CancelA2dpDeviceTimeout();
                mDockAddress = address;
            }
            else {
                // this could be a connection of another A2DP device before the timeout of
                // a dock: cancel the dock timeout, and make the dock unavailable now
                if(HasScheduledA2dpDockTimeout()) {
                    CancelA2dpDeviceTimeout();
                    MakeA2dpDeviceUnavailableNow(mDockAddress);
                }
            }
            MakeA2dpDeviceAvailable(address);
            synchronized(mCurAudioRoutes) {
                String name;
                btDevice->GetAliasName(&name);
                AutoPtr<ICharSequence> cs;
                mCurAudioRoutes->GetBluetoothName((ICharSequence**)&cs);
                String str;
                cs->ToString(&str);
                if (!name.Equals(str)) {
                    AutoPtr<ICharSequence> cs;
                    CString::New(name, (ICharSequence**)&cs);
                    mCurAudioRoutes->SetBluetoothName(cs);
                    SendMsg(mAudioHandler, MSG_REPORT_NEW_ROUTES,
                            SENDMSG_NOOP, 0, 0, NULL, 0);
                }
            }
        }
    }
}

void CAudioService::OnSetA2dpSourceConnectionState(
    /* [in] */ IBluetoothDevice* btDevice,
    /* [in] */ Int32 state)
{
    if (DEBUG_VOL) {
        // Logger::D(TAG, "onSetA2dpSourceConnectionState btDevice="+btDevice+" state="+state);
    }
    if (btDevice == NULL) {
        return;
    }
    String address;
    btDevice->GetAddress(&address);
    if (!CBluetoothAdapter::CheckBluetoothAddress(address)) {
        address = String("");
    }

    synchronized(mConnectedDevices) {
        AutoPtr<IInteger32> i32;
        CInteger32::New(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP, (IInteger32**)&i32);
        Boolean b;
        mConnectedDevices->ContainsKey(i32, &b);
        AutoPtr<IInterface> obj;
        mConnectedDevices->Get(i32, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        Boolean isConnected = (b && str.Equals(address));

        if (isConnected && state != IBluetoothProfile::STATE_CONNECTED) {
            MakeA2dpSrcUnavailable(address);
        }
        else if (!isConnected && state == IBluetoothProfile::STATE_CONNECTED) {
            MakeA2dpSrcAvailable(address);
        }
    }
}

Boolean CAudioService::HandleDeviceConnection(
    /* [in] */ Boolean connected,
    /* [in] */ Int32 device,
    /* [in] */ const String& params)
{
    synchronized(mConnectedDevices) {
        AutoPtr<IInteger32> i32;
        CInteger32::New(device, (IInteger32**)&i32);
        Boolean b;
        mConnectedDevices->ContainsKey(i32, &b);
        AutoPtr<IInterface> obj;
        mConnectedDevices->Get(i32, (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        Boolean isConnected = (b &&
                (params.IsEmpty() || str.Equals(params)));

        if (isConnected && !connected) {
            AutoPtr<IInteger32> i32;
            CInteger32::New(device, (IInteger32**)&i32);
            AutoPtr<IInterface> obj;
            mConnectedDevices->Get(i32, (IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            AudioSystem::SetDeviceConnectionState(device,
                    IAudioSystem::DEVICE_STATE_UNAVAILABLE, str);
            mConnectedDevices->Remove(i32);
            return TRUE;
        }
        else if (!isConnected && connected) {
             AudioSystem::SetDeviceConnectionState(device,
                    IAudioSystem::DEVICE_STATE_AVAILABLE, params);
             AutoPtr<IInteger32> i32;
             CInteger32::New(device, (IInteger32**)&i32);
             AutoPtr<ICharSequence> cs;
             CString::New(params, (ICharSequence**)&cs);
             mConnectedDevices->Put(i32, cs);
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
        AutoPtr<ISet> set;
        mConnectedDevices->GetKeySet((ISet**)&set);

        AutoPtr<ArrayOf<IInterface*> > array;
        set->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            Int32 dev;
            IInteger32::Probe((*array)[i])->GetValue(&dev);

            if (((dev & IAudioSystem::DEVICE_BIT_IN) == 0) &&
                    ((dev & mBecomingNoisyIntentDevices) != 0)) {
               devices |= dev;
            }
        }
        if (devices == device) {
            SendMsg(mAudioHandler,
                    MSG_BROADCAST_AUDIO_BECOMING_NOISY,
                    SENDMSG_REPLACE,
                    0,
                    0,
                    NULL,
                    0);
            delay = 1000;
        }
    }

    Boolean b1, b2, b3;
    mAudioHandler->HasMessages(MSG_SET_A2DP_SRC_CONNECTION_STATE, &b1);
    mAudioHandler->HasMessages(MSG_SET_A2DP_SINK_CONNECTION_STATE, &b2);
    mAudioHandler->HasMessages(MSG_SET_WIRED_DEVICE_CONNECTION_STATE, &b3);
    if (b1 || b2 || b3) {
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
        if (state == 1) {
            StartMusicPlayer();
        }
    }
    else if (device == IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE ||
               device == IAudioSystem::DEVICE_OUT_LINE) {
        /*do apps care about line-out vs headphones?*/
        connType = CAudioRoutesInfo::MAIN_HEADPHONES;
        intent->SetAction(IIntent::ACTION_HEADSET_PLUG);
        intent->PutExtra(String("microphone"), 0);
        if (state == 1) {
            StartMusicPlayer();
        }
    }
    else if (device == IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET) {
        connType = CAudioRoutesInfo::MAIN_DOCK_SPEAKERS;
        intent->SetAction(IAudioManager::ACTION_ANALOG_AUDIO_DOCK_PLUG);
    }
    else if (device == IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET) {
        connType = CAudioRoutesInfo::MAIN_DOCK_SPEAKERS;
        intent->SetAction(IAudioManager::ACTION_DIGITAL_AUDIO_DOCK_PLUG);
    }
    else if (device == IAudioSystem::DEVICE_OUT_HDMI) {
        connType = CAudioRoutesInfo::MAIN_HDMI;
        ConfigureHdmiPlugIntent(intent, state);
    }

    synchronized(mCurAudioRoutes) {
        if (connType != 0) {
            Int32 type;
            mCurAudioRoutes->GetMainType(&type);
            Int32 newConn = type;
            if (state != 0) {
                newConn |= connType;
            }
            else {
                newConn &= ~connType;
            }
            if (newConn != type) {
                mCurAudioRoutes->SetMainType(newConn);
                SendMsg(mAudioHandler, MSG_REPORT_NEW_ROUTES,
                        SENDMSG_NOOP, 0, 0, NULL, 0);
            }
        }
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
}

void CAudioService::StartMusicPlayer()
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 resTmp = 0;
    Settings::System::GetInt32ForUser(contentResolver,
            ISettingsSystem::HEADSET_CONNECT_PLAYER, 0, IUserHandle::USER_CURRENT, &resTmp);
    Boolean launchPlayer = (resTmp != 0);
    if (launchPlayer) {
        AutoPtr<IIntent> playerIntent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&playerIntent);
        playerIntent->AddCategory(IIntent::CATEGORY_APP_MUSIC);
        playerIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        // try {
            mContext->StartActivity(playerIntent);
        // } catch(ActivityNotFoundException e) {
            // Log.e(TAG, "error launching music player", e);
        // }
    }
}

void CAudioService::OnSetWiredDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    synchronized(mConnectedDevices) {
        if ((state == 0) && ((device == IAudioSystem::DEVICE_OUT_WIRED_HEADSET) ||
                (device == IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE) ||
                (device == IAudioSystem::DEVICE_OUT_LINE))) {
            SetBluetoothA2dpOnInt(TRUE);
        }
        Boolean isUsb = ((device & ~IAudioSystem::DEVICE_OUT_ALL_USB) == 0) ||
                        (((device & IAudioSystem::DEVICE_BIT_IN) != 0) &&
                         ((device & ~IAudioSystem::DEVICE_IN_ALL_USB) == 0));
        HandleDeviceConnection((state == 1), device, (isUsb ? name : String("")));
        if (state != 0) {
            if ((device == IAudioSystem::DEVICE_OUT_WIRED_HEADSET) ||
                (device == IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE) ||
                (device == IAudioSystem::DEVICE_OUT_LINE)) {
                SetBluetoothA2dpOnInt(FALSE);
            }
            if ((device & mSafeMediaVolumeDevices) != 0) {
                SendMsg(mAudioHandler,
                        MSG_CHECK_MUSIC_ACTIVE,
                        SENDMSG_REPLACE,
                        0,
                        0,
                        NULL,
                        MUSIC_ACTIVE_POLL_PERIOD_MS);
            }
            // Television devices without CEC service apply software volume on HDMI output
            if (IsPlatformTelevision() && ((device & IAudioSystem::DEVICE_OUT_HDMI) != 0)) {
                mFixedVolumeDevices |= IAudioSystem::DEVICE_OUT_HDMI;
                CheckAllFixedVolumeDevices();
                if (mHdmiManager != NULL) {
                    synchronized(mHdmiManager) {
                        if (mHdmiPlaybackClient != NULL) {
                            mHdmiCecSink = FALSE;
                            mHdmiPlaybackClient->QueryDisplayStatus(mHdmiDisplayStatusCallback);
                        }
                    }
                }
            }
        }
        else {
            if (IsPlatformTelevision() && ((device & IAudioSystem::DEVICE_OUT_HDMI) != 0)) {
                if (mHdmiManager != NULL) {
                    synchronized(mHdmiManager) {
                        mHdmiCecSink = FALSE;
                    }
                }
            }
        }
        if (!isUsb && (device != IAudioSystem::DEVICE_IN_WIRED_HEADSET)) {
            SendDeviceConnectionIntent(device, state, name);
        }
    }
}

void CAudioService::ConfigureHdmiPlugIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 state)
{
    intent->SetAction(IAudioManager::ACTION_HDMI_AUDIO_PLUG);
    intent->PutExtra(IAudioManager::EXTRA_AUDIO_PLUG_STATE, state);
    if (state == 1) {
        AutoPtr<IArrayList> ports;
        CArrayList::New((IArrayList**)&ports);
        AutoPtr<ArrayOf<Int32> > portGeneration = ArrayOf<Int32>::Alloc(1);
        Int32 status;
        AudioSystem::ListAudioPorts(ports, portGeneration, &status);
        if (status == IAudioManager::SUCCESS) {
            Int32 size;
            ports->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                ports->Get(i, (IInterface**)&obj);
                AutoPtr<IAudioPort> port = IAudioPort::Probe(obj);

                AutoPtr<IAudioDevicePort> devicePort = IAudioDevicePort::Probe(port);
                if (devicePort != NULL) {
                    Int32 type;
                    devicePort->Type(&type);
                    if (type == IAudioManager::DEVICE_OUT_HDMI) {
                        // format the list of supported encodings
                        AutoPtr<ArrayOf<Int32> > formats;
                        IAudioPort::Probe(devicePort)->Formats((ArrayOf<Int32>**)&formats);
                        if (formats->GetLength() > 0) {
                            AutoPtr<IArrayList> encodingList;
                            CArrayList::New((IArrayList**)&encodingList);

                            for (Int32 j = 0; j < formats->GetLength(); j++) {
                                Int32 format = (*formats)[i];
                                // a format in the list can be 0, skip it
                                if (format != IAudioFormat::ENCODING_INVALID) {
                                    AutoPtr<IInteger32> i32;
                                    CInteger32::New(format, (IInteger32**)&i32);
                                    encodingList->Add(i32);
                                }
                            }
                            Int32 len;
                            encodingList->GetSize(&len);
                            AutoPtr<ArrayOf<Int32> > encodingArray = ArrayOf<Int32>::Alloc(len);
                            for (Int32 i = 0 ; i < encodingArray->GetLength() ; i++) {
                                AutoPtr<IInterface> obj;
                                encodingList->Get(i, (IInterface**)&obj);
                                Int32 value;
                                IInteger32::Probe(obj)->GetValue(&value);
                                encodingArray->Set(i, value);
                            }
                            intent->PutExtra(IAudioManager::EXTRA_ENCODINGS, encodingArray);
                        }
                        // find the maximum supported number of channels
                        Int32 maxChannels = 0;
                        AutoPtr<ArrayOf<Int32> > array;
                        IAudioPort::Probe(devicePort)->ChannelMasks((ArrayOf<Int32>**)&array);
                        for (Int32 k = 0; k < array->GetLength(); k++) {
                            Int32 mask = (*array)[k];
                            Int32 channelCount;
                            CAudioFormat::ChannelCountFromOutChannelMask(mask, &channelCount);
                            if (channelCount > maxChannels) {
                                maxChannels = channelCount;
                            }
                        }
                        intent->PutExtra(IAudioManager::EXTRA_MAX_CHANNEL_COUNT, maxChannels);
                    }
                }
            }
        }
    }
}

void CAudioService::HandleConfigurationChanged(
    /* [in] */ IContext* context)
{
    // try {
    // reading new orientation "safely" (i.e. under try catch) in case anything
    // goes wrong when obtaining resources and configuration
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    // TODO merge rotation and orientation
    if (mMonitorOrientation) {
        Int32 newOrientation;
        config->GetOrientation(&newOrientation);
        if (newOrientation != mDeviceOrientation) {
            mDeviceOrientation = newOrientation;
            SetOrientationForAudioSystem();
        }
    }
    SendMsg(mAudioHandler,
            MSG_CONFIGURE_SAFE_MEDIA_VOLUME,
            SENDMSG_REPLACE,
            0,
            0,
            NULL,
            0);

    Boolean cameraSoundForced;
    res->GetBoolean(R::bool_::config_camera_sound_forced, &cameraSoundForced);
    synchronized(mSettingsLock) {
        synchronized(mCameraSoundForced) {
            Boolean b;
            mCameraSoundForced->GetValue(&b);
            if (cameraSoundForced != b) {
                mCameraSoundForced = NULL;
                CBoolean::New(cameraSoundForced, (IBoolean**)&mCameraSoundForced);

                if (!IsPlatformTelevision()) {
                    AutoPtr<VolumeStreamState> s = (*mStreamStates)[IAudioSystem::STREAM_SYSTEM_ENFORCED];
                    if (cameraSoundForced) {
                        s->SetAllIndexesToMax();
                        mRingerModeAffectedStreams &=
                                ~(1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
                    }
                    else {
                        s->SetAllIndexes(
                            IAudioServiceVolumeStreamState::Probe((*mStreamStates)[IAudioSystem::STREAM_SYSTEM]));
                        mRingerModeAffectedStreams |=
                                (1 << IAudioSystem::STREAM_SYSTEM_ENFORCED);
                    }
                    // take new state into account for streams muted by ringer mode
                    Int32 mode;
                    GetRingerMode(&mode);
                    SetRingerModeInt(mode, FALSE);
                }

                SendMsg(mAudioHandler,
                        MSG_SET_FORCE_USE,
                        SENDMSG_QUEUE,
                        IAudioSystem::FOR_SYSTEM,
                        cameraSoundForced ?
                                IAudioSystem::FORCE_SYSTEM_ENFORCED : IAudioSystem::FORCE_NONE,
                        NULL,
                        0);

                SendMsg(mAudioHandler,
                        MSG_SET_ALL_VOLUMES,
                        SENDMSG_QUEUE, 0, 0,
                        (IInterface*)(IObject*)(*mStreamStates)[IAudioSystem::STREAM_SYSTEM_ENFORCED], 0);
            }
        }
    }

    Int32 direction;
    config->GetLayoutDirection(&direction);
    mVolumeController->SetLayoutDirection(direction);
    // } catch (Exception e) {
    //     Log.e(TAG, "Error handling configuration change: ", e);
    // }
}

void CAudioService::SetOrientationForAudioSystem()
{
    switch (mDeviceOrientation) {
        case IConfiguration::ORIENTATION_LANDSCAPE:
            //Log.i(TAG, "orientation is landscape");
            AudioSystem::SetParameters(String("orientation=landscape"));
            break;
        case IConfiguration::ORIENTATION_PORTRAIT:
            //Log.i(TAG, "orientation is portrait");
            AudioSystem::SetParameters(String("orientation=portrait"));
            break;
        case IConfiguration::ORIENTATION_SQUARE:
            //Log.i(TAG, "orientation is square");
            AudioSystem::SetParameters(String("orientation=square"));
            break;
        case IConfiguration::ORIENTATION_UNDEFINED:
            //Log.i(TAG, "orientation is undefined");
            AudioSystem::SetParameters(String("orientation=undefined"));
            break;
        default:
            Logger::E(TAG, "Unknown orientation");
    }
}

void CAudioService::SetRotationForAudioSystem()
{
    switch (mDeviceRotation) {
        case ISurface::ROTATION_0:
            AudioSystem::SetParameters(String("rotation=0"));
            break;
        case ISurface::ROTATION_90:
            AudioSystem::SetParameters(String("rotation=90"));
            break;
        case ISurface::ROTATION_180:
            AudioSystem::SetParameters(String("rotation=180"));
            break;
        case ISurface::ROTATION_270:
            AudioSystem::SetParameters(String("rotation=270"));
            break;
        default:
            Logger::E(TAG, "Unknown device rotation");
    }
}

void CAudioService::SetSafeMediaVolumeEnabled(
    /* [in] */ Boolean on)
{
    synchronized(mSafeMediaVolumeStateLock) {
        Int32 value;
        mSafeMediaVolumeState->GetValue(&value);
        if ((value != SAFE_MEDIA_VOLUME_NOT_CONFIGURED) &&
                (value != SAFE_MEDIA_VOLUME_DISABLED)) {
            if (on && (value == SAFE_MEDIA_VOLUME_INACTIVE)) {
                mSafeMediaVolumeState = NULL;
                CInteger32::New(SAFE_MEDIA_VOLUME_ACTIVE, (IInteger32**)&mSafeMediaVolumeState);
                EnforceSafeMediaVolume();
            }
            else if (!on && (value == SAFE_MEDIA_VOLUME_ACTIVE)) {
                mSafeMediaVolumeState = NULL;
                CInteger32::New(SAFE_MEDIA_VOLUME_INACTIVE, (IInteger32**)&mSafeMediaVolumeState);
                mMusicActiveMs = 1;  // nonzero = confirmed
                SaveMusicActiveMs();
                SendMsg(mAudioHandler,
                        MSG_CHECK_MUSIC_ACTIVE,
                        SENDMSG_REPLACE,
                        0,
                        0,
                        NULL,
                        MUSIC_ACTIVE_POLL_PERIOD_MS);
            }
        }
    }
}

void CAudioService::EnforceSafeMediaVolume()
{
    AutoPtr<VolumeStreamState> streamState = (*mStreamStates)[IAudioSystem::STREAM_MUSIC];
    Int32 devices = mSafeMediaVolumeDevices;
    Int32 i = 0;

    while (devices != 0) {
        Int32 device = 1 << i++;
        if ((device & devices) == 0) {
            continue;
        }
        Int32 index;
        streamState->GetIndex(device, &index);
        if (index > mSafeMediaVolumeIndex) {
            Boolean b;
            streamState->SetIndex(mSafeMediaVolumeIndex, device, &b);
            SendMsg(mAudioHandler,
                    MSG_SET_DEVICE_VOLUME,
                    SENDMSG_QUEUE,
                    device,
                    0,
                    (IInterface*)(IObject*)streamState,
                    0);
        }
        devices &= ~device;
    }
}

Boolean CAudioService::CheckSafeMediaVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 device)
{
    synchronized(mSafeMediaVolumeStateLock) {
        Int32 value;
        mSafeMediaVolumeState->GetValue(&value);
        if ((value == SAFE_MEDIA_VOLUME_ACTIVE) &&
                ((*mStreamVolumeAlias)[streamType] == IAudioSystem::STREAM_MUSIC) &&
                ((device & mSafeMediaVolumeDevices) != 0) &&
                (index > mSafeMediaVolumeIndex)) {
            return FALSE;
        }
        return TRUE;
    }
    return TRUE;
}

void CAudioService::DumpRingerMode(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("\nRinger mode: "));
    pw->Println(String("- mode: ")+(*RINGER_MODE_NAMES)[mRingerMode]);
    pw->Print(String("- ringer mode affected streams = 0x"));
    pw->Println(StringUtils::ToHexString(mRingerModeAffectedStreams));
    pw->Print(String("- ringer mode muted streams = 0x"));
    pw->Println(StringUtils::ToHexString(mRingerModeMutedStreams));
}

String CAudioService::SafeMediaVolumeStateToString(
    /* [in] */ IInteger32* state)
{
    Int32 val;
    state->GetValue(&val);
    switch(val) {
        case SAFE_MEDIA_VOLUME_NOT_CONFIGURED: return String("SAFE_MEDIA_VOLUME_NOT_CONFIGURED");
        case SAFE_MEDIA_VOLUME_DISABLED: return String("SAFE_MEDIA_VOLUME_DISABLED");
        case SAFE_MEDIA_VOLUME_INACTIVE: return String("SAFE_MEDIA_VOLUME_INACTIVE");
        case SAFE_MEDIA_VOLUME_ACTIVE: return String("SAFE_MEDIA_VOLUME_ACTIVE");
    }
    return String(NULL);
}

void CAudioService::ReadAndSetLowRamDevice()
{
    Int32 status;
    AudioSystem::SetLowRamDevice(CActivityManager::IsLowRamDeviceStatic(), &status);
    if (status != 0) {
        Logger::W(TAG, "AudioFlinger informed of device's low RAM attribute; status %d", status);
    }
}

void CAudioService::EnforceSelfOrSystemUI(
    /* [in] */ const String& action)
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::STATUS_BAR_SERVICE,
            String("Only SystemUI can ") + action);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
