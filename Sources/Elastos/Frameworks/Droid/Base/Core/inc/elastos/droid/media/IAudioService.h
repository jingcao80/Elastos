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

#ifndef __ELASTOS_DROID_MEDIA_IAUDIOSERVICE_H__
#define __ELASTOS_DROID_MEDIA_IAUDIOSERVICE_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Media::AudioPolicy::IAudioPolicyConfig;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Media {

class IAudioServiceProxy
    : public Object
    , public IIAudioService
{
public:
    IAudioServiceProxy(
        /* [in] */ android::sp<android::IBinder> remote);

    CAR_INTERFACE_DECL();

    CARAPI AdjustSuggestedStreamVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 suggestedStreamType,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    CARAPI AdjustStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    CARAPI AdjustMasterVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    CARAPI SetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    CARAPI SetRemoteStreamVolume(
        /* [in] */ Int32 index);

    CARAPI SetMasterVolume(
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    CARAPI SetStreamSolo(
        /* [in] */ Int32 streamType,
        /* [in] */ Boolean state,
        /* [in] */ IBinder* cb);

    CARAPI SetStreamMute(
        /* [in] */ Int32 streamType,
        /* [in] */ Boolean state,
        /* [in] */ IBinder* cb);

    CARAPI IsStreamMute(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    CARAPI ForceRemoteSubmixFullVolume(
        /* [in] */ Boolean startForcing,
        /* [in] */ IBinder* cb);

    CARAPI SetMasterMute(
        /* [in] */ Boolean state,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage,
        /* [in] */ IBinder* cb);

    CARAPI IsMasterMute(
        /* [out] */ Boolean* result);

    CARAPI GetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    CARAPI GetMasterVolume(
        /* [out] */ Int32* result);

    CARAPI GetStreamMaxVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    CARAPI GetMasterMaxVolume(
        /* [out] */ Int32* result);

    CARAPI GetLastAudibleStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    CARAPI GetLastAudibleMasterVolume(
        /* [out] */ Int32* result);

    CARAPI SetMicrophoneMute(
        /* [in] */ Boolean on,
        /* [in] */ const String& callingPackage);

    CARAPI SetRingerMode(
        /* [in] */ Int32 ringerMode,
        /* [in] */ Boolean checkZen);

    CARAPI GetRingerMode(
        /* [out] */ Int32* result);

    CARAPI SetVibrateSetting(
        /* [in] */ Int32 vibrateType,
        /* [in] */ Int32 vibrateSetting);

    CARAPI GetVibrateSetting(
        /* [in] */ Int32 vibrateType,
        /* [out] */ Int32* result);

    CARAPI ShouldVibrate(
        /* [in] */ Int32 vibrateType,
        /* [out] */ Boolean* result);

    CARAPI SetMode(
        /* [in] */ Int32 mode,
        /* [in] */ IBinder* cb);

    CARAPI GetMode(
        /* [out] */ Int32* result);

    CARAPI PlaySoundEffect(
        /* [in] */ Int32 effectType);

    CARAPI PlaySoundEffectVolume(
        /* [in] */ Int32 effectType,
        /* [in] */ Float volume);

    CARAPI LoadSoundEffects(
        /* [out] */ Boolean* result);

    CARAPI UnloadSoundEffects();

    CARAPI ReloadAudioSettings();

    CARAPI AvrcpSupportsAbsoluteVolume(
        /* [in] */ const String& address,
        /* [in] */ Boolean support);

    CARAPI SetSpeakerphoneOn(
        /* [in] */ Boolean on);

    CARAPI IsSpeakerphoneOn(
        /* [out] */ Boolean* result);

    CARAPI SetBluetoothScoOn(
        /* [in] */ Boolean on);

    CARAPI IsBluetoothScoOn(
        /* [out] */ Boolean* result);

    CARAPI SetBluetoothA2dpOn(
        /* [in] */ Boolean on);

    CARAPI IsBluetoothA2dpOn(
        /* [out] */ Boolean* result);

    CARAPI RequestAudioFocus(
        /* [in] */ Int32 mainStreamType,
        /* [in] */ Int32 durationHint,
        /* [in] */ IBinder* cb,
        /* [in] */ IIAudioFocusDispatcher* fd,
        /* [in] */ const String& clientId,
        /* [in] */ const String& callingPackageName,
        /* [out] */ Int32* result);

    CARAPI AbandonAudioFocus(
        /* [in] */ IIAudioFocusDispatcher* fd,
        /* [in] */ const String& clientId,
        /* [out] */ Int32* result);

    CARAPI UnregisterAudioFocusClient(
        /* [in] */ const String& clientId);

    CARAPI GetCurrentAudioFocus(
        /* [out] */ Int32* result);

    CARAPI RegisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [out] */ Boolean* result);

    CARAPI RegisterRemoteController(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IComponentName* listenerComp,
        /* [out] */ Boolean* result);

    CARAPI UnregisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    CARAPI RemoteControlDisplayUsesBitmapSize(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI RemoteControlDisplayWantsPlaybackPositionSync(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean wantsSync);

    CARAPI StartBluetoothSco(
        /* [in] */ IBinder* cb,
        /* [in] */ Int32 targetSdkVersion);

    CARAPI StartBluetoothScoVirtualCall(
        /* [in] */ IBinder* cb);

    CARAPI StopBluetoothSco(
        /* [in] */ IBinder* cb);

    CARAPI ForceVolumeControlStream(
        /* [in] */ Int32 streamType,
        /* [in] */ IBinder* cb);

    CARAPI SetRingtonePlayer(
        /* [in] */ IIRingtonePlayer* player);

    CARAPI GetRingtonePlayer(
        /* [out] */ IIRingtonePlayer** result);

    CARAPI GetMasterStreamType(
        /* [out] */ Int32* result);

    CARAPI SetWiredDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& name);

    CARAPI SetBluetoothA2dpDeviceConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 state,
        /* [in] */ Int32 profile,
        /* [out] */ Int32* result);

    CARAPI StartWatchingRoutes(
        /* [in] */ IIAudioRoutesObserver* observer,
        /* [out] */ IAudioRoutesInfo** result);

    CARAPI IsCameraSoundForced(
        /* [out] */ Boolean* result);

    CARAPI SetVolumeController(
        /* [in] */ IIVolumeController* controller);

    CARAPI NotifyVolumeControllerVisible(
        /* [in] */ IIVolumeController* controller,
        /* [in] */ Boolean visible);

    CARAPI IsStreamAffectedByRingerMode(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    CARAPI DisableSafeMediaVolume();

    CARAPI SetHdmiSystemAudioSupported(
        /* [in] */ Boolean on,
        /* [out] */ Int32* result);

    CARAPI IsHdmiSystemAudioSupported(
        /* [out] */ Boolean* result);

    CARAPI RegisterAudioPolicy(
        /* [in] */ IAudioPolicyConfig* policyConfig,
        /* [in] */ IBinder* cb,
        /* [out] */ Boolean* result);

    CARAPI UnregisterAudioPolicyAsync(
        /* [in] */ IBinder* cb);

    CARAPI SetRemoteControlClientBrowsedPlayer();

    CARAPI GetRemoteControlClientNowPlayingEntries();

    CARAPI SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    CARAPI UpdateRemoteControllerOnExistingMediaPlayers();

    CARAPI AddMediaPlayerAndUpdateRemoteController(
        /* [in] */ const String& packageName);

    CARAPI RemoveMediaPlayerAndUpdateRemoteController(
        /* [in] */ const String& packageName);

    CARAPI SystemReady();

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_playSoundEffect;

    android::sp<android::IBinder> mRemote;
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_IAUDIOSERVICE_H__
