#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/media/CAudioManager.h"
#include "elastos/droid/media/CAudioManagerAudioFocusDispatcher.h"
#include "elastos/droid/media/CAudioHandle.h"
#include "elastos/droid/media/CAudioPort.h"
#include "elastos/droid/media/CAudioPortConfig.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelper.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::IProfileGroup;
using Elastos::Droid::App::ProfileGroupMode;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::AudioPolicy::IAudioPolicyConfig;
using Elastos::Droid::Media::CAudioHandle;
using Elastos::Droid::Media::CAudioPort;
using Elastos::Droid::Media::CAudioPortConfig;
using Elastos::Droid::Media::Session::CMediaSessionLegacyHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelper;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CAudioManager::DEFAULT_STREAM_VOLUME[] = {
    4,  // STREAM_VOICE_CALL
    7,  // STREAM_SYSTEM
    5,  // STREAM_RING
    11, // STREAM_MUSIC
    6,  // STREAM_ALARM
    5,  // STREAM_NOTIFICATION
    7,  // STREAM_BLUETOOTH_SCO
    7,  // STREAM_SYSTEM_ENFORCED
    11, // STREAM_DTMF
    11, // STREAM_TTS
    4   // STREAM_INCALL_MUSIC
};

const Int32 CAudioManager::AUDIOPORT_GENERATION_INIT = 0;
const String CAudioManager::TAG("CAudioManager");
const Int32 CAudioManager::RINGER_MODE_MAX = RINGER_MODE_NORMAL;
AutoPtr<IIAudioService> CAudioManager::sService;

//================================================================================
//                      CAudioManager::FocusEventHandlerDelegate::MyHandler
//================================================================================
ECode CAudioManager::FocusEventHandlerDelegate::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    ICharSequence* seq = ICharSequence::Probe(obj);
    String info;
    seq->ToString(&info);

    AutoPtr<IAudioManagerOnAudioFocusChangeListener> listener;
    Object& lock = mHost->mOwner->mFocusListenerLock;
    synchronized(lock) {
        listener = mHost->mOwner->FindFocusListener(info);
    }
    if (listener != NULL) {
        listener->OnAudioFocusChange(what);
    }
    return NOERROR;
}

//================================================================================
//                      CAudioManager::FocusEventHandlerDelegate
//================================================================================
CAudioManager::FocusEventHandlerDelegate::FocusEventHandlerDelegate(
    /* [in] */ CAudioManager* owner)
    : mOwner(owner)
{
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    if (looper == NULL) {
        looper = Looper::GetMainLooper();
    }

    if (looper != NULL) {
        // implement the event handler delegate to receive audio focus events
        mHandler = new MyHandler(looper, this);
    }
    else {
        mHandler = NULL;
    }
}

AutoPtr<IHandler> CAudioManager::FocusEventHandlerDelegate::GetHandler()
{
    return mHandler;
}

//================================================================================
//                      CAudioManager::CAudioManager
//================================================================================

CAR_INTERFACE_IMPL(CAudioManager, Object, IAudioManager)

CAR_OBJECT_IMPL(CAudioManager)

CAudioManager::CAudioManager()
    : mVolumeKeyUpTime(0)
    , mUseMasterVolume(FALSE)
    , mUseVolumeKeySounds(FALSE)
    , mUseFixedVolume(FALSE)
{
    CBinder::New((IBinder**)&mToken);
    CBinder::New((IBinder**)&mICallBack);
    mAudioFocusEventHandlerDelegate = new FocusEventHandlerDelegate(this);
    CAudioManagerAudioFocusDispatcher::New(this, (IIAudioFocusDispatcher**)&mAudioFocusDispatcher);

    CInteger32::New(AUDIOPORT_GENERATION_INIT, (IInteger32**)&mAudioPortGeneration);
    CArrayList::New((IArrayList**)&mAudioPortsCached);
    CArrayList::New((IArrayList**)&mAudioPatchesCached);
}

CAudioManager::~CAudioManager()
{
}

ECode CAudioManager::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::config_useMasterVolume, &mUseMasterVolume);
    resources->GetBoolean(R::bool_::config_useVolumeKeySounds, &mUseVolumeKeySounds);

    mAudioPortEventHandler = new AudioPortEventHandler(this);
    resources->GetBoolean(R::bool_::config_useFixedVolume, &mUseFixedVolume);

    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&interfaceTmp);
    mProfileManager = IProfileManager::Probe(interfaceTmp);
    return NOERROR;
}

AutoPtr<IIAudioService> CAudioManager::GetService()
{
    if (sService != NULL) {
        return sService;
    }

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->GetService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    sService = IIAudioService::Probe(obj.Get());

    if (sService == NULL) {
        Logger::E(TAG, "AudioService is not ready!");
    }
    return sService;
}

ECode CAudioManager::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent)
{
    AutoPtr<IMediaSessionLegacyHelper> helper;
    CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
    return helper->SendMediaButtonEvent(keyEvent, FALSE);
}

ECode CAudioManager::PreDispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 stream)
{
    /*
     * If the user hits another key within the play sound delay, then
     * cancel the sound
     */
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    if (keyCode != IKeyEvent::KEYCODE_VOLUME_DOWN
        && keyCode != IKeyEvent::KEYCODE_VOLUME_UP
        && keyCode != IKeyEvent::KEYCODE_VOLUME_MUTE
        && mVolumeKeyUpTime + IAudioService::PLAY_SOUND_DELAY
            > SystemClock::GetUptimeMillis()) {
        /*
         * The user has hit another key during the delay (e.g., 300ms)
         * since the last volume key up, so cancel any sounds.
         */
        if (mUseMasterVolume) {
            AdjustMasterVolume(ADJUST_SAME, FLAG_REMOVE_SOUND_AND_VIBRATE);
        }
        else {
            AdjustSuggestedStreamVolume(ADJUST_SAME, stream, FLAG_REMOVE_SOUND_AND_VIBRATE);
        }
    }
    return NOERROR;
}

ECode CAudioManager::HandleKeyDown(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 stream)
{
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        {
            /*
             * Adjust the volume in on key down since it is more
             * responsive to the user.
             */
            Int32 flags = FLAG_SHOW_UI | FLAG_VIBRATE;
            if (mUseMasterVolume) {
                AdjustMasterVolume(keyCode == IKeyEvent::KEYCODE_VOLUME_UP
                    ? ADJUST_RAISE : ADJUST_LOWER, flags);
            }
            else {
                AdjustSuggestedStreamVolume(keyCode == IKeyEvent::KEYCODE_VOLUME_UP
                    ? ADJUST_RAISE : ADJUST_LOWER, stream, flags);
            }
            break;
        }
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
        {
            Int32 value;
            event->GetRepeatCount(&value);
            if (value == 0) {
                if (mUseMasterVolume) {
                    Boolean result;
                    IsMasterMute(&result);
                    SetMasterMute(!result);
                } else {
                    // TODO: Actually handle MUTE.
                }
            }
            break;
        }
    }
    return NOERROR;
}

ECode CAudioManager::HandleKeyUp(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 stream)
{
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        {
            /*
             * Play a sound. This is done on key up since we don't want the
             * sound to play when a user holds down volume down to mute.
             */
            if (mUseVolumeKeySounds) {
                if (mUseMasterVolume) {
                    AdjustMasterVolume(ADJUST_SAME, FLAG_PLAY_SOUND);
                }
                else {
                    Int32 flags;
                    flags = FLAG_PLAY_SOUND;
                    AdjustSuggestedStreamVolume(ADJUST_SAME, stream, flags);
                }
            }
            mVolumeKeyUpTime = SystemClock::GetUptimeMillis();
            break;
        }
    }
    return NOERROR;
}

ECode CAudioManager::IsVolumeFixed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUseFixedVolume;
    return NOERROR;
}

ECode CAudioManager::AdjustStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->AdjustMasterVolume(direction, flags, pkgName);
    }
    else {
        ec = service->AdjustStreamVolume(streamType, direction, flags, pkgName);
    }
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in AdjustStreamVolume");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::AdjustVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->AdjustMasterVolume(direction, flags, pkgName);
    }
    else {
        AutoPtr<IMediaSessionLegacyHelper> helper;
        CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
        helper->SendAdjustVolumeBy(IAudioManager::USE_DEFAULT_STREAM_TYPE, direction, flags);
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in AdjustVolume");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::AdjustSuggestedStreamVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 suggestedStreamType,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->AdjustMasterVolume(direction, flags, pkgName);
    }
    else {
        AutoPtr<IMediaSessionLegacyHelper> helper;
        CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
        ec = helper->SendAdjustVolumeBy(suggestedStreamType, direction, flags);
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in AdjustSuggestedStreamVolume");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::AdjustMasterVolume(
    /* [in] */ Int32 steps,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = service->AdjustMasterVolume(steps, flags, pkgName);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in AdjustMasterVolume");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::GetRingerMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RINGER_MODE_NORMAL;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetRingerMode(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetRingerMode");
        *result = RINGER_MODE_NORMAL;
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::IsValidRingerMode(
    /* [in] */ Int32 ringerMode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (ringerMode < 0 || ringerMode > RINGER_MODE_MAX) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CAudioManager::GetStreamMaxVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->GetMasterMaxVolume(result);
    }
    else {
        ec = service->GetStreamMaxVolume(streamType, result);
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetStreamMaxVolume");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::GetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->GetMasterVolume(result);
    }
    else {
        ec = service->GetStreamVolume(streamType, result);
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetStreamVolume");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::GetLastAudibleStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->GetLastAudibleMasterVolume(result);
    }
    else {
        ec = service->GetLastAudibleStreamVolume(streamType, result);
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetLastAudibleStreamVolume");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::GetMasterStreamType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = STREAM_RING;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetMasterStreamType(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetMasterStreamType");
        *result = STREAM_RING;
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::SetRingerMode(
    /* [in] */ Int32 ringerMode)
{
    return SetRingerMode(ringerMode, TRUE /*checkZen*/);
}

ECode CAudioManager::SetRingerMode(
    /* [in] */ Int32 ringerMode,
    /* [in] */ Boolean checkZen)
{
    Boolean result;
    IsValidRingerMode(ringerMode, &result);
    if (!result) return NOERROR;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetRingerMode(ringerMode, checkZen);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetRingerMode");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = NOERROR;
    if (mUseMasterVolume) {
        ec = service->SetMasterVolume(index, flags, pkgName);
    }
    else {
        ec = service->SetStreamVolume(streamType, index, flags, pkgName);
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetStreamVolume");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::GetMasterMaxVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetMasterMaxVolume(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetMasterMaxVolume");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::GetMasterVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetMasterVolume(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetMasterVolume");
        *result = 0;
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::GetLastAudibleMasterVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    ECode ec = NOERROR;
    ec = service->GetLastAudibleMasterVolume(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetLastAudibleMasterVolume");
        *result = 0;
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode CAudioManager::SetMasterVolume(
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = service->SetMasterVolume(index, flags, pkgName);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetMasterVolume");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetStreamSolo(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetStreamSolo(streamType, state, mICallBack);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetStreamSolo");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetStreamMute(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetStreamMute(streamType, state, mICallBack);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetStreamMute");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::IsStreamMute(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->IsStreamMute(streamType, result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in IsStreamMute");
        *result = FALSE;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetMasterMute(
    /* [in] */ Boolean state)
{
    return SetMasterMute(state, FLAG_SHOW_UI);
}

ECode CAudioManager::SetMasterMute(
    /* [in] */ Boolean state,
    /* [in] */ Int32 flags)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = service->SetMasterMute(state, flags, pkgName, mICallBack);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetMasterMuteEx");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::IsMasterMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->IsMasterMute(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in IsMasterMute");
        *result = FALSE;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::ForceVolumeControlStream(
    /* [in] */ Int32 streamType)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->ForceVolumeControlStream(streamType, mICallBack);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in ForceVolumeControlStream");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::ShouldVibrate(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    String packageName;
    mContext->GetPackageName(&packageName);
    // Don't apply profiles for "android" context, as these could
    // come from the NotificationManager, and originate from a real package.
    if (!packageName.Equals("android")) {
        AutoPtr<IProfileGroup> profileGroup;
        mProfileManager->GetActiveProfileGroup(packageName, (IProfileGroup**)&profileGroup);
        if (profileGroup != NULL) {
            // Logger::V(TAG, "shouldVibrate, group: " + profileGroup->GetUuid() + " mode: " + profileGroup->GetVibrateMode());
            ProfileGroupMode mode;
            profileGroup->GetVibrateMode(&mode);
            switch (mode) {
                case Elastos::Droid::App::ProfileGroupMode_OVERRIDE:
                    *result = TRUE;
                    return NOERROR;
                case Elastos::Droid::App::ProfileGroupMode_SUPPRESS:
                    *result = FALSE;
                    return NOERROR;
                case Elastos::Droid::App::ProfileGroupMode_DEFAULT:
                // Drop through
                    break;
            }
        }
    }
    else {
        Logger::V(TAG, "Not applying override for 'android' package");
    }

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->ShouldVibrate(vibrateType, result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in ShouldVibrate");
        *result = FALSE;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::GetVibrateSetting(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = VIBRATE_SETTING_OFF;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetVibrateSetting(vibrateType, result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetVibrateSetting");
        *result = VIBRATE_SETTING_OFF;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetVibrateSetting(
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetVibrateSetting(vibrateType, vibrateSetting);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetVibrateSetting");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetSpeakerphoneOn(
    /* [in] */ Boolean on)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetSpeakerphoneOn(on);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in setSpeakerphoneOn");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::IsSpeakerphoneOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->IsSpeakerphoneOn(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in IsSpeakerphoneOn");
        *result = FALSE;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::IsBluetoothScoAvailableOffCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    return resources->GetBoolean(R::bool_::config_bluetooth_sco_off_call, result);
}

ECode CAudioManager::StartBluetoothSco()
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    AutoPtr<IApplicationInfo> ai;
    mContext->GetApplicationInfo((IApplicationInfo**)&ai);
    Int32 ver;
    ai->GetTargetSdkVersion(&ver);
    ECode ec = service->StartBluetoothSco(mICallBack, ver);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in StartBluetoothSco");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::StartBluetoothScoVirtualCall()
{
    AutoPtr<IIAudioService> service = GetService();

    // try {
    return service->StartBluetoothScoVirtualCall(mICallBack);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in startBluetoothScoVirtualCall", e);
    // }
}

ECode CAudioManager::StopBluetoothSco()
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->StopBluetoothSco(mICallBack);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in StopBluetoothSco");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetBluetoothScoOn(
    /* [in] */ Boolean on)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetBluetoothScoOn(on);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetBluetoothScoOn");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::IsBluetoothScoOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->IsBluetoothScoOn(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in IsBluetoothScoOn");
        *result = FALSE;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetBluetoothA2dpOn(
    /* [in] */ Boolean on)
{
    return NOERROR;
}

ECode CAudioManager::IsBluetoothA2dpOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 state;
    String deviceAddress("");
    AudioSystem::GetDeviceConnectionState(DEVICE_OUT_BLUETOOTH_A2DP, deviceAddress, &state);
    if (state == IAudioSystem::DEVICE_STATE_UNAVAILABLE) {
        *result = FALSE;
    }
    else {
        *result = TRUE;
    }
    return NOERROR;
}

ECode CAudioManager::SetWiredHeadsetOn(
    /* [in] */ Boolean on)
{
    return NOERROR;
}

ECode CAudioManager::IsWiredHeadsetOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 state1;
    Int32 state2;
    String deviceAddress("");
    AudioSystem::GetDeviceConnectionState(DEVICE_OUT_BLUETOOTH_A2DP, deviceAddress, &state1);
    AudioSystem::GetDeviceConnectionState(DEVICE_OUT_BLUETOOTH_A2DP, deviceAddress, &state2);
    if (state1 == IAudioSystem::DEVICE_STATE_UNAVAILABLE &&
        state2 == IAudioSystem::DEVICE_STATE_UNAVAILABLE) {
        *result = FALSE;
    }
    else {
        *result = TRUE;
    }
    return NOERROR;
}

ECode CAudioManager::SetMicrophoneMute(
    /* [in] */ Boolean on)
{
    AutoPtr<IIAudioService> service = GetService();
    // try {
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    return service->SetMicrophoneMute(on, pkgName);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in setMicrophoneMute", e);
    // }
}

ECode CAudioManager::IsMicrophoneMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return AudioSystem::IsMicrophoneMuted(result);
}

ECode CAudioManager::SetMode(
    /* [in] */ Int32 mode)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetMode(mode, mICallBack);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetMode");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetMode(result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in GetMode");
        *result = MODE_INVALID;
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetRouting(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 routes,
    /* [in] */ Int32 mask)
{
    return NOERROR;
}

ECode CAudioManager::GetRouting(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    return NOERROR;
}

ECode CAudioManager::IsMusicActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return AudioSystem::IsStreamActive(STREAM_MUSIC, 0, result);
}

ECode CAudioManager::IsMusicActiveRemotely(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return AudioSystem::IsStreamActiveRemotely(STREAM_MUSIC, 0, result);
}

ECode CAudioManager::IsAudioFocusExclusive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIAudioService> service = GetService();

    // try {
    Int32 val;
    service->GetCurrentAudioFocus(&val);
    *result = val == IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in isAudioFocusExclusive()", e);
    //     return false;
    // }
}

ECode CAudioManager::GenerateAudioSessionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 session;
    AudioSystem::NewAudioSessionId(&session);
    if (session > 0) {
        *result = session;
        return NOERROR;
    } else {
        Logger::E(TAG, "Failure to generate a new audio session ID");
        *result = IAudioManager::ERROR;
        return NOERROR;
    }
}

ECode CAudioManager::SetParameter(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    SetParameters(key + "=" + value);
    return NOERROR;
}

ECode CAudioManager::SetParameters(
    /* [in] */ const String& keyValuePairs)
{
    return AudioSystem::SetParameters(keyValuePairs);
}

ECode CAudioManager::GetParameters(
    /* [in] */ const String& keys,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    return AudioSystem::GetParameters(keys, result);
}

ECode CAudioManager::PlaySoundEffect(
    /* [in] */ Int32 effectType)
{
    if (effectType < 0 || effectType >= NUM_SOUND_EFFECTS) {
        return NOERROR;
    }

    AutoPtr<IUserHandle> userH;
    Process::MyUserHandle((IUserHandle**)&userH);
    Int32 id;
    userH->GetIdentifier(&id);
    if (!QuerySoundEffectsEnabled(id)) {
        return NOERROR;
    }

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->PlaySoundEffect(effectType);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in PlaySoundEffect");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::PlaySoundEffect(
    /* [in] */ Int32 effectType,
    /* [in] */ Int32 userId)
{
    if (effectType < 0 || effectType >= NUM_SOUND_EFFECTS) {
        return NOERROR;
    }

    if (!QuerySoundEffectsEnabled(userId)) {
        return NOERROR;
    }

    AutoPtr<IIAudioService> service = GetService();
    // try {
    return service->PlaySoundEffect(effectType);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in playSoundEffect"+e);
    // }
}

ECode CAudioManager::PlaySoundEffect(
    /* [in] */ Int32 effectType,
    /* [in] */ Float volume)
{
    if (effectType < 0 || effectType >= NUM_SOUND_EFFECTS) {
        return NOERROR;
    }

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->PlaySoundEffectVolume(effectType, volume);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in PlaySoundEffectEx");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

Boolean CAudioManager::QuerySoundEffectsEnabled(
    /* [in] */ Int32 user)
{
    // todo: Eric audio service ready?
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 value;
    Settings::System::GetInt32ForUser(contentResolver,
            ISettingsSystem::SOUND_EFFECTS_ENABLED, 0, user, &value);
    return  value != 0;
}

ECode CAudioManager::LoadSoundEffects()
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    Boolean result;
    ECode ec = service->LoadSoundEffects(&result);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in LoadSoundEffects");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::UnloadSoundEffects()
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->UnloadSoundEffects();
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in UnloadSoundEffects");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IAudioManagerOnAudioFocusChangeListener> CAudioManager::FindFocusListener(
    /* [in] */ const String& id)
{
    HashMap<String, AutoPtr<IAudioManagerOnAudioFocusChangeListener> >::Iterator it =
            mAudioFocusIdListenerMap.Find(id);
    if (it != mAudioFocusIdListenerMap.End()) {
        return it->mSecond;
    }
    return NULL;
}

String CAudioManager::GetIdForAudioFocusListener(
    /* [in] */ IAudioManagerOnAudioFocusChangeListener* l)
{
    String msg("AudioManager:0x");
    msg += StringUtils::ToHexString((Int32)this);
    if (l != NULL) {
        msg += ", OnAudioFocusChangeListener:0x";
        msg += StringUtils::ToHexString((Int32)l);
    }
    return msg;
}

ECode CAudioManager::RegisterAudioFocusListener(
    /* [in] */ IAudioManagerOnAudioFocusChangeListener* l)
{
    String key = GetIdForAudioFocusListener(l);
    synchronized(mFocusListenerLock) {
        HashMap<String, AutoPtr<IAudioManagerOnAudioFocusChangeListener> >::Iterator it =
            mAudioFocusIdListenerMap.Find(key);
        if (it != mAudioFocusIdListenerMap.End()) {
            return NOERROR;
        }
    }
    mAudioFocusIdListenerMap[key] = l;
    return NOERROR;
}

ECode CAudioManager::UnregisterAudioFocusListener(
    /* [in] */ IAudioManagerOnAudioFocusChangeListener* l)
{
    String key = GetIdForAudioFocusListener(l);
    synchronized(mFocusListenerLock) {
        HashMap<String, AutoPtr<IAudioManagerOnAudioFocusChangeListener> >::Iterator it =
        mAudioFocusIdListenerMap.Find(key);
        if (it != mAudioFocusIdListenerMap.End())
            mAudioFocusIdListenerMap.Erase(it);
    }
    return NOERROR;
}

ECode CAudioManager::RequestAudioFocus(
    /* [in] */ IAudioManagerOnAudioFocusChangeListener* l,
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 durationHint,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = AUDIOFOCUS_REQUEST_FAILED;

    Int32 status = AUDIOFOCUS_REQUEST_FAILED;
    if ((durationHint < AUDIOFOCUS_GAIN) || (durationHint > AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE)) {
        Logger::E(TAG, "Invalid duration hint, audio focus request denied");
        *result = status;
        return NOERROR;
    }

    RegisterAudioFocusListener(l);
    //TODO protect request by permission check?
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = NOERROR;
    String packageName;
    do {
        ec = mContext->GetOpPackageName(&packageName);
        if (FAILED(ec)) break;

        ec = service->RequestAudioFocus(streamType, durationHint, mICallBack,
             mAudioFocusDispatcher, GetIdForAudioFocusListener(l),
             packageName /* package name */, &status);
        if (FAILED(ec)) break;
    } while(0);

    if (FAILED(ec)) {
        Logger::E(TAG, "Can't call RequestAudioFocus() from AudioService due to ");
        return E_REMOTE_EXCEPTION;
    }

    *result = status;
    return NOERROR;
}

ECode CAudioManager::RequestAudioFocusForCall(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 durationHint)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    Int32 focus;
    String pkgName;
    mContext->GetOpPackageName(&pkgName);
    ECode ec = service->RequestAudioFocus(streamType, durationHint, mICallBack, NULL,
            String("AudioFocus_For_Phone_Ring_And_Calls"), pkgName, &focus);
    if (FAILED(ec)) {
        Logger::E(TAG, "Can't call RequestAudioFocusForCall() on AudioService due to ");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::AbandonAudioFocusForCall()
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    Int32 focus;
    ECode ec = service->AbandonAudioFocus(NULL, String("AudioFocus_For_Phone_Ring_And_Calls"), &focus);
    if (FAILED(ec)) {
        Logger::E(TAG, "Can't call AbandonAudioFocusForCall() on AudioService due to ");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::AbandonAudioFocus(
    /* [in] */ IAudioManagerOnAudioFocusChangeListener* l,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = AUDIOFOCUS_REQUEST_FAILED;

    UnregisterAudioFocusListener(l);
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    Int32 status = AUDIOFOCUS_REQUEST_FAILED;
    ECode ec = service->AbandonAudioFocus(mAudioFocusDispatcher,
        GetIdForAudioFocusListener(l),&status);
    if (FAILED(ec)) {
        Logger::E(TAG, "Can't call AbandonAudioFocus() on AudioService due to ");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

//====================================================================
// Remote Control

ECode CAudioManager::RegisterMediaButtonEventReceiver(
    /* [in] */ IComponentName* eventReceiver)
{
    if (eventReceiver == NULL) {
        return NOERROR;
    }
    String cpnName, contextName;
    eventReceiver->GetPackageName(&cpnName);
    mContext->GetPackageName(&contextName);
    if (!cpnName.Equals(contextName)) {
        Logger::E(TAG, "RegisterMediaButtonEventReceiver() error: "
            "receiver and context package names don't match");
        return NOERROR;
    }

    // construct a PendingIntent for the media button and register it
    AutoPtr<IIntent> mediaButtonIntent ;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&mediaButtonIntent);
    mediaButtonIntent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    //     the associated intent will be handled by the component being registered
    mediaButtonIntent->SetComponent(eventReceiver);
    AutoPtr<IPendingIntentHelper> pi;
    AutoPtr<IPendingIntent> pendingIntent;
    pi->GetBroadcast(mContext,
        0/*requestCode, ignored*/, mediaButtonIntent,
        0/*flags*/, (IPendingIntent**)&pendingIntent);
    return RegisterMediaButtonIntent(pendingIntent, eventReceiver);
}

ECode CAudioManager::RegisterMediaButtonEventReceiver(
    /* [in] */ IPendingIntent* eventReceiver)
{
    if (eventReceiver == NULL) {
        return NOERROR;
    }
    return RegisterMediaButtonIntent(eventReceiver, NULL);
}

ECode CAudioManager::RegisterMediaButtonIntent(
    /* [in] */ IPendingIntent* pi,
    /* [in] */ IComponentName* eventReceiver)
{
    if (pi == NULL) {
        Logger::E(TAG, "Cannot call registerMediaButtonIntent() with a null parameter");
        return NOERROR;
    }
    AutoPtr<IMediaSessionLegacyHelper> helper;
    CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
    return helper->AddMediaButtonListener(pi, eventReceiver, mContext);
}

ECode CAudioManager::UnregisterMediaButtonEventReceiver(
    /* [in] */ IComponentName* eventReceiver)
{
    if (eventReceiver == NULL) {
        return NOERROR;
    }
    // construct a PendingIntent for the media button and unregister it
    AutoPtr<IIntent> mediaButtonIntent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&mediaButtonIntent);
    //     the associated intent will be handled by the component being registered
    mediaButtonIntent->SetComponent(eventReceiver);
    AutoPtr<IPendingIntent> pendingIntent;
    AutoPtr<IPendingIntentHelper> pi;
    pi->GetBroadcast(mContext,
        0/*requestCode, ignored*/, mediaButtonIntent,
        0/*flags*/, (IPendingIntent**)&pendingIntent);
    return UnregisterMediaButtonIntent(pendingIntent);
}

ECode CAudioManager::UnregisterMediaButtonEventReceiver(
        /* [in] */ IPendingIntent* eventReceiver)
{
    if (eventReceiver == NULL) {
        return NOERROR;
    }
    return UnregisterMediaButtonIntent(eventReceiver);
}

ECode CAudioManager::UnregisterMediaButtonIntent(
    /* [in] */ IPendingIntent* pi)
{
    AutoPtr<IMediaSessionLegacyHelper> helper;
    CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
    return helper->RemoveMediaButtonListener(pi);
}

ECode CAudioManager::RegisterRemoteControlClient(
    /* [in] */ IRemoteControlClient* rcClient)
{
    AutoPtr<IPendingIntent> pendIntent;
    rcClient->GetRcMediaIntent((IPendingIntent**)&pendIntent);
    if (rcClient == NULL || pendIntent == NULL) {
        return NOERROR;
    }

    AutoPtr<IMediaSessionLegacyHelper> helper;
    CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
    return rcClient->RegisterWithSession(helper);
}

ECode CAudioManager::UnregisterRemoteControlClient(
    /* [in] */ IRemoteControlClient* rcClient)
{
    AutoPtr<IPendingIntent> pendIntent;
    rcClient->GetRcMediaIntent((IPendingIntent**)&pendIntent);
    if (rcClient == NULL || pendIntent == NULL) {
        return NOERROR;
    }

    AutoPtr<IMediaSessionLegacyHelper> helper;
    CMediaSessionLegacyHelper::GetHelper(mContext, (IMediaSessionLegacyHelper**)&helper);
    return rcClient->UnregisterWithSession(helper);
}

ECode CAudioManager::RegisterRemoteController(
    /* [in] */ IRemoteController* rctlr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (rctlr == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    rctlr->StartListeningToSessions();
    AutoPtr<IIAudioService> service = GetService();
    // try {
        service->UpdateRemoteControllerOnExistingMediaPlayers();
    // } catch (RemoteException e) {
        // Log.e(TAG, "Error in calling Audio service interface" +
        //     "updateRemoteControllerOnExistingMediaPlayers() due to " + e);
    // }
    *result = TRUE;
    return NOERROR;
}

ECode CAudioManager::UnregisterRemoteController(
    /* [in] */ IRemoteController* rctlr)
{
    if (rctlr == NULL) {
        return NOERROR;
    }
    return rctlr->StopListeningToSessions();
}

ECode CAudioManager::UpdateMediaPlayerList(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean toAdd)
{
    AutoPtr<IIAudioService> service = GetService();
    // try {
        if (toAdd) {
            Logger::D(TAG, String("updateMediaPlayerList: Add RCC ") + packageName + String(" to List"));
            service->AddMediaPlayerAndUpdateRemoteController(packageName);
        }
        else {
            Logger::D(TAG, String("updateMediaPlayerList: Remove RCC ") + packageName + String(" from List"));
            service->RemoveMediaPlayerAndUpdateRemoteController(packageName);
        }
    // } catch (RemoteException e) {
        // Log.e(TAG, "Exception while executing updateMediaPlayerList: " + e);
    // }
    return NOERROR;
}

ECode CAudioManager::RegisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    // passing a negative value for art work width and height as they are unknown at this stage
    return RegisterRemoteControlDisplay(rcd, /*w*/-1, /*h*/ -1);
}

ECode CAudioManager::RegisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    if (rcd == NULL) {
        return NOERROR;
    }

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    Boolean b;
    ECode ec = service->RegisterRemoteControlDisplay(rcd, w, h, &b);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in RegisterRemoteControlDisplay");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::UnregisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    if (rcd == NULL) {
        return NOERROR;
    }

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->UnregisterRemoteControlDisplay(rcd);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in UnregisterRemoteControlDisplay ");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::RemoteControlDisplayUsesBitmapSize(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    if (rcd == NULL) {
        return NOERROR;
    }

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->RemoteControlDisplayUsesBitmapSize(rcd, w, h);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in RemoteControlDisplayUsesBitmapSize ");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::RemoteControlDisplayWantsPlaybackPositionSync(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean wantsSync)
{
    if (rcd == NULL) {
        return NOERROR;
    }
    AutoPtr<IIAudioService> service = GetService();
    // try {
    return service->RemoteControlDisplayWantsPlaybackPositionSync(rcd, wantsSync);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in remoteControlDisplayWantsPlaybackPositionSync " + e);
    // }
}

ECode CAudioManager::RegisterAudioPolicy(
    /* [in] */ IAudioPolicy* policy,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (policy == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioPolicy argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IIAudioService> service = GetService();
    // try {
    AutoPtr<IAudioPolicyConfig> config;
    policy->GetConfig((IAudioPolicyConfig**)&config);
    AutoPtr<IBinder> token;
    policy->Token((IBinder**)&token);
    Boolean b;
    service->RegisterAudioPolicy(config, token, &b);
    if (!b) {
        *result = IAudioManager::ERROR;
        return NOERROR;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in registerAudioPolicyAsync()", e);
    //     return ERROR;
    // }
    *result = IAudioManager::SUCCESS;
    return NOERROR;
}

ECode CAudioManager::UnregisterAudioPolicyAsync(
    /* [in] */ IAudioPolicy* policy)
{
    if (policy == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioPolicy argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IIAudioService> service = GetService();
    // try {
    AutoPtr<IBinder> token;
    policy->Token((IBinder**)&token);
    return service->UnregisterAudioPolicyAsync(token);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in unregisterAudioPolicyAsync()", e);
    // }
}

ECode CAudioManager::SetRemoteControlClientPlayItem(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    AutoPtr<IIAudioService> service = GetService();
    // try {
        service->SetRemoteControlClientPlayItem(uid, scope);
    // } catch (RemoteException e) {
        // Log.e(TAG, "Dead object in setRemoteControlClientPlayItem(" + uid + ", " + scope + ")", e);
    // }
    return NOERROR;
}

/**
 * @hide
 * Request the user of a RemoteControlClient to provide with the now playing list entries.
 * @param generationId the RemoteControlClient generation counter for which this request is
 *     issued.
 */
ECode CAudioManager::GetRemoteControlClientNowPlayingEntries()
{
    AutoPtr<IIAudioService> service = GetService();
    // try {
        service->GetRemoteControlClientNowPlayingEntries();
    // } catch (RemoteException e) {
        // Log.e(TAG, "Dead object in getRemoteControlClientNowPlayingEntries(" + ")", e);
    // }
    return NOERROR;
}

/**
 * @hide
 * Request the user of a RemoteControlClient to set the music player as current browsed player.
 */
ECode CAudioManager::SetRemoteControlClientBrowsedPlayer()
{
    Logger::D(TAG, "setRemoteControlClientBrowsedPlayer: ");
    AutoPtr<IIAudioService> service = GetService();
    // try {
        service->SetRemoteControlClientBrowsedPlayer();
    // } catch (RemoteException e) {
        // Log.e(TAG, "Dead object in setRemoteControlClientBrowsedPlayer(" + ")", e);
    // }
    return NOERROR;
}

ECode CAudioManager::ReloadAudioSettings()
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->ReloadAudioSettings();
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in ReloadAudioSettings");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::AvrcpSupportsAbsoluteVolume(
    /* [in] */ const String& address,
    /* [in] */ Boolean support)
{
    AutoPtr<IIAudioService> service = GetService();
    // try {
    return service->AvrcpSupportsAbsoluteVolume(address, support);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Dead object in avrcpSupportsAbsoluteVolume", e);
    // }
}

ECode CAudioManager::IsSilentMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 ringerMode;
    GetRingerMode(&ringerMode);
    *result = (ringerMode == RINGER_MODE_SILENT)
        || (ringerMode == RINGER_MODE_VIBRATE);
    return NOERROR;
}

ECode CAudioManager::IsOutputDevice(
    /* [in] */ Int32 device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (device & IAudioSystem::DEVICE_BIT_IN) == 0;
    return NOERROR;
}

ECode CAudioManager::IsInputDevice(
    /* [in] */ Int32 device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (device & IAudioSystem::DEVICE_BIT_IN) == IAudioSystem::DEVICE_BIT_IN;
    return NOERROR;
}

ECode CAudioManager::GetDevicesForStream(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    switch (streamType) {
        case STREAM_VOICE_CALL:
        case STREAM_SYSTEM:
        case STREAM_RING:
        case STREAM_MUSIC:
        case STREAM_ALARM:
        case STREAM_NOTIFICATION:
        case STREAM_DTMF:
        {
            return AudioSystem::GetDevicesForStream(streamType, result);
        }

        default:
        *result = 0;
        return NOERROR;
    }
}

ECode CAudioManager::SetWiredDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->SetWiredDeviceConnectionState(device, state, name);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetWiredDeviceConnectionState ");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAudioManager::SetBluetoothA2dpDeviceConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 state,
    /* [in] */ Int32 profile,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    Int32 delay;
    ECode ec = service->SetBluetoothA2dpDeviceConnectionState(device, state, profile, &delay);
    if (FAILED(ec)) {
        Logger::E(TAG, "Dead object in SetBluetoothA2dpDeviceConnectionState ");
        *result = 0;
        return E_REMOTE_EXCEPTION;
    }

    *result = delay;
    return NOERROR;
}

ECode CAudioManager::GetRingtonePlayer(
    /* [out] */ IIRingtonePlayer** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IIAudioService> service = GetService();
    if (!service) return E_REMOTE_EXCEPTION;

    ECode ec = service->GetRingtonePlayer(result);
    if (FAILED(ec)) {
        *result = NULL;
        return E_REMOTE_EXCEPTION;
    }

    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAudioManager::GetProperty(
    /* [in] */ const String& key,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    if (PROPERTY_OUTPUT_SAMPLE_RATE.Equals(key)) {
        Int32 outputSampleRate;
        AudioSystem::GetPrimaryOutputSamplingRate(&outputSampleRate);
        if (outputSampleRate > 0) {
            *result = StringUtils::ToString(outputSampleRate);
        }
        return NOERROR;
    }
    else if (PROPERTY_OUTPUT_FRAMES_PER_BUFFER.Equals(key)) {
        Int32 outputFramesPerBuffer;
        AudioSystem::GetPrimaryOutputFrameCount(&outputFramesPerBuffer);
        if (outputFramesPerBuffer > 0) {
            *result = StringUtils::ToString(outputFramesPerBuffer);
        }
        return NOERROR;
    }
    else {
        // NULL or unknown key
        return NOERROR;
    }
}

ECode CAudioManager::GetOutputLatency(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return AudioSystem::GetOutputLatency(streamType, result);
}

ECode CAudioManager::SetVolumeController(
    /* [in] */ IIVolumeController* controller)
{
    // try {
    return GetService()->SetVolumeController(controller);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error setting volume controller", e);
    // }
}

ECode CAudioManager::NotifyVolumeControllerVisible(
    /* [in] */ IIVolumeController* controller,
    /* [in] */ Boolean visible)
{
    // try {
    return GetService()->NotifyVolumeControllerVisible(controller, visible);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error notifying about volume controller visibility", e);
    // }
}

ECode CAudioManager::IsStreamAffectedByRingerMode(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return GetService()->IsStreamAffectedByRingerMode(streamType, result);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error calling isStreamAffectedByRingerMode", e);
    //     return false;
    // }
}

ECode CAudioManager::DisableSafeMediaVolume()
{
    // try {
    return GetService()->DisableSafeMediaVolume();
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error disabling safe media volume", e);
    // }
}

ECode CAudioManager::SetHdmiSystemAudioSupported(
    /* [in] */ Boolean on,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return GetService()->SetHdmiSystemAudioSupported(on, result);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error setting system audio mode", e);
    //     return AudioSystem.DEVICE_NONE;
    // }
}

ECode CAudioManager::IsHdmiSystemAudioSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return GetService()->IsHdmiSystemAudioSupported(result);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Error querying system audio mode", e);
    //     return false;
    // }
}

ECode CAudioManager::ListAudioPorts(
    /* [in] */ IArrayList* ports,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return UpdateAudioPortCache(ports, NULL, result);
}

ECode CAudioManager::ListAudioDevicePorts(
    /* [in] */ IArrayList* devices,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> ports;
    CArrayList::New((IArrayList**)&ports);
    Int32 status;
    UpdateAudioPortCache(ports, NULL, &status);
    if (status == IAudioManager::SUCCESS) {
        devices->Clear();
        Int32 size;
        ports->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            ports->Get(i, (IInterface**)&obj);
            if (IAudioDevicePort::Probe(obj) != NULL) {
                devices->Add(obj);
            }
        }
    }
    *result = status;
    return NOERROR;
}

ECode CAudioManager::CreateAudioPatch(
    /* [in] */ ArrayOf<IAudioPatch*>* patch,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sinks,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return AudioSystem::CreateAudioPatch(patch, sources, sinks, result);
}

ECode CAudioManager::ReleaseAudioPatch(
    /* [in] */ IAudioPatch* patch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return AudioSystem::ReleaseAudioPatch(patch, result);
}

ECode CAudioManager::ListAudioPatches(
    /* [in] */ IArrayList* patches,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return UpdateAudioPortCache(NULL, patches, result);
}

ECode CAudioManager::SetAudioPortGain(
    /* [in] */ IAudioPort* port,
    /* [in] */ IAudioGainConfig* gain,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (port == NULL || gain == NULL) {
        *result = IAudioManager::ERROR_BAD_VALUE;
    }
    AutoPtr<IAudioPortConfig> activeConfig;
    port->ActiveConfig((IAudioPortConfig**)&activeConfig);
    Int32 samplingRate;
    activeConfig->SamplingRate(&samplingRate);
    Int32 channelMask;
    activeConfig->ChannelMask(&channelMask);
    Int32 format;
    activeConfig->Format(&format);
    AutoPtr<CAudioPortConfig> config;
    CAudioPortConfig::NewByFriend(port, samplingRate,
            channelMask, format, gain, (CAudioPortConfig**)&config);
    config->mConfigMask = CAudioPortConfig::GAIN;
    return AudioSystem::SetAudioPortConfig(config, result);
}

ECode CAudioManager::RegisterAudioPortUpdateListener(
    /* [in] */ IAudioManagerOnAudioPortUpdateListener* l)
{
    return mAudioPortEventHandler->RegisterListener(l);
}

ECode CAudioManager::UnregisterAudioPortUpdateListener(
    /* [in] */ IAudioManagerOnAudioPortUpdateListener* l)
{
    return mAudioPortEventHandler->UnregisterListener(l);
}

ECode CAudioManager::ResetAudioPortGeneration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 generation;
    synchronized(mAudioPortGeneration) {
        mAudioPortGeneration->GetValue(&generation);
        mAudioPortGeneration = NULL;
        CInteger32::New(AUDIOPORT_GENERATION_INIT, (IInteger32**)&mAudioPortGeneration);
    }
    *result = generation;
    return NOERROR;
}

ECode CAudioManager::UpdateAudioPortCache(
    /* [in] */ IArrayList* ports,
    /* [in] */ IArrayList* patches,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mAudioPortGeneration) {
        Int32 val;
        mAudioPortGeneration->GetValue(&val);
        if (val == AUDIOPORT_GENERATION_INIT) {
            AutoPtr<ArrayOf<Int32> > patchGeneration = ArrayOf<Int32>::Alloc(1);
            AutoPtr<ArrayOf<Int32> > portGeneration = ArrayOf<Int32>::Alloc(1);
            Int32 status;
            AutoPtr<IArrayList> newPorts;
            CArrayList::New((IArrayList**)&newPorts);
            AutoPtr<IArrayList> newPatches;
            CArrayList::New((IArrayList**)&newPatches);

            do {
                newPorts->Clear();
                AudioSystem::ListAudioPorts(newPorts, portGeneration, &status);
                if (status != SUCCESS) {
                    *result = status;
                    return NOERROR;
                }
                newPatches->Clear();
                AudioSystem::ListAudioPatches(newPatches, patchGeneration, &status);
                if (status != SUCCESS) {
                    *result = status;
                    return NOERROR;
                }
            } while ((*patchGeneration)[0] != (*portGeneration)[0]);

            Int32 size;
            newPatches->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                newPatches->Get(i, (IInterface**)&obj);
                AutoPtr<IAudioPatch> p = IAudioPatch::Probe(obj);
                AutoPtr<ArrayOf<IAudioPortConfig*> > sources;
                p->Sources((ArrayOf<IAudioPortConfig*>**)&sources);
                for (Int32 j = 0; j < sources->GetLength(); j++) {
                    AutoPtr<IInterface> o;
                    newPatches->Get(i, (IInterface**)&o);
                    AutoPtr<IAudioPatch> ap = IAudioPatch::Probe(o);
                    AutoPtr<ArrayOf<IAudioPortConfig*> > array;
                    ap->Sources((ArrayOf<IAudioPortConfig*>**)&array);
                    AutoPtr<IAudioPortConfig> portCfg;
                    UpdatePortConfig((*array)[j], newPorts, (IAudioPortConfig**)&portCfg);
                    if (portCfg == NULL) {
                        *result = IAudioManager::ERROR;
                        return NOERROR;
                    }
                    array->Set(j , portCfg);
                }

                AutoPtr<ArrayOf<IAudioPortConfig*> > sinks;
                p->Sinks((ArrayOf<IAudioPortConfig*>**)&sinks);
                for (Int32 j = 0; j < sinks->GetLength(); j++) {
                    AutoPtr<IInterface> o;
                    newPatches->Get(i, (IInterface**)&o);
                    AutoPtr<IAudioPatch> ap = IAudioPatch::Probe(o);
                    AutoPtr<ArrayOf<IAudioPortConfig*> > array;
                    ap->Sinks((ArrayOf<IAudioPortConfig*>**)&array);
                    AutoPtr<IAudioPortConfig> portCfg;
                    UpdatePortConfig((*array)[j], newPorts, (IAudioPortConfig**)&portCfg);
                    if (portCfg == NULL) {
                        *result = IAudioManager::ERROR;
                        return NOERROR;
                    }
                    array->Set(j , portCfg);
                }
            }

            mAudioPortsCached = newPorts;
            mAudioPatchesCached = newPatches;
            mAudioPortGeneration = NULL;
            CInteger32::New((*portGeneration)[0], (IInteger32**)&mAudioPortGeneration);
        }
        if (ports != NULL) {
            ports->Clear();
            ports->AddAll(ICollection::Probe(mAudioPortsCached));
        }
        if (patches != NULL) {
            patches->Clear();
            patches->AddAll(ICollection::Probe(mAudioPatchesCached));
        }
    }
    *result = IAudioManager::SUCCESS;
    return NOERROR;
}

ECode CAudioManager::UpdatePortConfig(
    /* [in] */ IAudioPortConfig* portCfg,
    /* [in] */ IArrayList* ports,
    /* [out] */ IAudioPortConfig** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IAudioPort> port;
    portCfg->Port((IAudioPort**)&port);
    Int32 size;
    ports->GetSize(&size);
    Int32 k;
    for (k = 0; k < size; k++) {
        // compare handles because the port returned by JNI is not of the correct
        // subclass
        AutoPtr<IInterface> obj;
        ports->Get(k, (IInterface**)&obj);
        AutoPtr<IAudioPort> p = IAudioPort::Probe(obj);
        AutoPtr<IAudioHandle> handle;
        p->Handle((IAudioHandle**)&handle);
        AutoPtr<IAudioHandle> pHandle;
        port->Handle((IAudioHandle**)&pHandle);
        Boolean b;
        ((CAudioHandle*)handle.Get())->Equals(pHandle, &b);
        if (b) {
            port = p;
            break;
        }
    }
    if (k == size) {
        // this hould never happen
        AutoPtr<IAudioHandle> handle;
        port->Handle((IAudioHandle**)&handle);
        Int32 id;
        ((CAudioHandle*)handle.Get())->Id(&id);
        Logger::E(TAG, "updatePortConfig port not found for handle: %d", id);
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IAudioGainConfig> gainCfg;
    portCfg->Gain((IAudioGainConfig**)&gainCfg);
    if (gainCfg != NULL) {
        Int32 index;
        gainCfg->Index(&index);
        AutoPtr<IAudioGain> gain;
        ((CAudioPort*)port.Get())->Gain(index, (IAudioGain**)&gain);
        Int32 mode, channelMask, rampDurationMs;
        gainCfg->Mode(&mode);
        gainCfg->ChannelMask(&channelMask);
        gainCfg->RampDurationMs(&rampDurationMs);
        AutoPtr<ArrayOf<Int32> > values;
        gainCfg->Values((ArrayOf<Int32>**)&values);
        gain->BuildConfig(mode, channelMask, values, rampDurationMs,
               (IAudioGainConfig**)&gainCfg);
    }
    Int32 samplingRate, channelMask, format;
    portCfg->SamplingRate(&samplingRate);
    portCfg->ChannelMask(&channelMask);
    portCfg->Format(&format);
    return port->BuildConfig(samplingRate,
            channelMask, format, gainCfg, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
