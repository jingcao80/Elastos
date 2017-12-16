
#include "elastos/droid/media/IAudioService.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Media {

const String IAudioServiceProxy::DESCRIPTOR("android.media.IAudioService");
const Int32 IAudioServiceProxy::TRANSACTION_playSoundEffect = android::IBinder::FIRST_CALL_TRANSACTION + 22;

CAR_INTERFACE_IMPL(IAudioServiceProxy, Object, IIAudioService);

IAudioServiceProxy::IAudioServiceProxy(
    /* [in] */ android::sp<android::IBinder> remote)
    : mRemote(remote)
{}

ECode IAudioServiceProxy::AdjustSuggestedStreamVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 suggestedStreamType,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::AdjustStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::AdjustMasterVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetRemoteStreamVolume(
    /* [in] */ Int32 index)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetMasterVolume(
    /* [in] */ Int32 index,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetStreamSolo(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state,
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetStreamMute(
    /* [in] */ Int32 streamType,
    /* [in] */ Boolean state,
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsStreamMute(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::ForceRemoteSubmixFullVolume(
    /* [in] */ Boolean startForcing,
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetMasterMute(
    /* [in] */ Boolean state,
    /* [in] */ Int32 flags,
    /* [in] */ const String& callingPackage,
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsMasterMute(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetMasterVolume(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetStreamMaxVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetMasterMaxVolume(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetLastAudibleStreamVolume(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetLastAudibleMasterVolume(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetMicrophoneMute(
    /* [in] */ Boolean on,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetRingerMode(
    /* [in] */ Int32 ringerMode,
    /* [in] */ Boolean checkZen)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetRingerMode(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetVibrateSetting(
    /* [in] */ Int32 vibrateType,
    /* [in] */ Int32 vibrateSetting)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetVibrateSetting(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::ShouldVibrate(
    /* [in] */ Int32 vibrateType,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetMode(
    /* [in] */ Int32 mode,
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetMode(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::PlaySoundEffect(
    /* [in] */ Int32 effectType)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, effectType);
    mRemote->transact(TRANSACTION_playSoundEffect, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IAudioServiceProxy::PlaySoundEffectVolume(
    /* [in] */ Int32 effectType,
    /* [in] */ Float volume)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::LoadSoundEffects(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::UnloadSoundEffects()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::ReloadAudioSettings()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::AvrcpSupportsAbsoluteVolume(
    /* [in] */ const String& address,
    /* [in] */ Boolean support)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetSpeakerphoneOn(
    /* [in] */ Boolean on)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsSpeakerphoneOn(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetBluetoothScoOn(
    /* [in] */ Boolean on)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsBluetoothScoOn(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetBluetoothA2dpOn(
    /* [in] */ Boolean on)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsBluetoothA2dpOn(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RequestAudioFocus(
    /* [in] */ Int32 mainStreamType,
    /* [in] */ Int32 durationHint,
    /* [in] */ IBinder* cb,
    /* [in] */ IIAudioFocusDispatcher* fd,
    /* [in] */ const String& clientId,
    /* [in] */ const String& callingPackageName,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::AbandonAudioFocus(
    /* [in] */ IIAudioFocusDispatcher* fd,
    /* [in] */ const String& clientId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::UnregisterAudioFocusClient(
    /* [in] */ const String& clientId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetCurrentAudioFocus(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RegisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RegisterRemoteController(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IComponentName* listenerComp,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::UnregisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RemoteControlDisplayUsesBitmapSize(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RemoteControlDisplayWantsPlaybackPositionSync(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean wantsSync)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::StartBluetoothSco(
    /* [in] */ IBinder* cb,
    /* [in] */ Int32 targetSdkVersion)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::StartBluetoothScoVirtualCall(
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::StopBluetoothSco(
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::ForceVolumeControlStream(
    /* [in] */ Int32 streamType,
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetRingtonePlayer(
    /* [in] */ IIRingtonePlayer* player)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetRingtonePlayer(
    /* [out] */ IIRingtonePlayer** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetMasterStreamType(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetWiredDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& name)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetBluetoothA2dpDeviceConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 state,
    /* [in] */ Int32 profile,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::StartWatchingRoutes(
    /* [in] */ IIAudioRoutesObserver* observer,
    /* [out] */ IAudioRoutesInfo** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsCameraSoundForced(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetVolumeController(
    /* [in] */ IIVolumeController* controller)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::NotifyVolumeControllerVisible(
    /* [in] */ IIVolumeController* controller,
    /* [in] */ Boolean visible)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsStreamAffectedByRingerMode(
    /* [in] */ Int32 streamType,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::DisableSafeMediaVolume()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetHdmiSystemAudioSupported(
    /* [in] */ Boolean on,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::IsHdmiSystemAudioSupported(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RegisterAudioPolicy(
    /* [in] */ IAudioPolicyConfig* policyConfig,
    /* [in] */ IBinder* cb,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::UnregisterAudioPolicyAsync(
    /* [in] */ IBinder* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetRemoteControlClientBrowsedPlayer()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::GetRemoteControlClientNowPlayingEntries()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SetRemoteControlClientPlayItem(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::UpdateRemoteControllerOnExistingMediaPlayers()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::AddMediaPlayerAndUpdateRemoteController(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::RemoveMediaPlayerAndUpdateRemoteController(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAudioServiceProxy::SystemReady()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Media
} // namespace Droid
} // namespace Elastos
