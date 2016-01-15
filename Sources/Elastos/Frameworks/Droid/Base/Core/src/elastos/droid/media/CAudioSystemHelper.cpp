
#include "elastos/droid/media/CAudioSystemHelper.h"
#include "elastos/droid/media/CAudioSystem.h"

namespace Elastos {
namespace Droid {
namespace Media {

ECode CAudioSystemHelper::GetNumStreamTypes(
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetNumStreamTypes(result);
}

ECode CAudioSystemHelper::MuteMicrophone(
    /* [in] */ Boolean on)
{
    return CAudioSystem::MuteMicrophone(on);
}

ECode CAudioSystemHelper::IsMicrophoneMuted(
    /* [out] */ Boolean* result)
{
    return CAudioSystem::IsMicrophoneMuted(result);
}

ECode CAudioSystemHelper::IsStreamActive(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 inPastMs,
    /* [out] */ Boolean* result)
{
    return CAudioSystem::IsStreamActive(stream, inPastMs, result);
}

ECode CAudioSystemHelper::IsSourceActive(
    /* [in] */ Int32 source,
    /* [out] */ Boolean* result)
{
    return CAudioSystem::IsSourceActive(source, result);
}

ECode CAudioSystemHelper::SetParameters(
    /* [in] */ const String& keyValuePairs)
{
    return CAudioSystem::SetParameters(keyValuePairs);
}

ECode CAudioSystemHelper::GetParameters(
    /* [in] */ const String& keys,
    /* [out] */ String* result)
{
    return CAudioSystem::GetParameters(keys, result);
}

ECode CAudioSystemHelper::SetErrorCallback(
    /* [in] */ IAudioSystemErrorCallback* cb)
{
    return CAudioSystem::SetErrorCallback(cb);
}

ECode CAudioSystemHelper::GetDeviceName(
    /* [in] */ Int32 device,
    /* [out] */ String* result)
{
    return CAudioSystem::GetDeviceName(device, result);
}

ECode CAudioSystemHelper::SetDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& device_address)
{
    return CAudioSystem::SetDeviceConnectionState(device, state, device_address);
}

ECode CAudioSystemHelper::GetDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ const String& device_address,
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetDeviceConnectionState(device, device_address, result);
}

ECode CAudioSystemHelper::SetPhoneState(
    /* [in] */ Int32 state)
{
    return CAudioSystem::SetPhoneState(state);
}

ECode CAudioSystemHelper::SetForceUse(
    /* [in] */ Int32 usage,
    /* [in] */ Int32 config)
{
    return CAudioSystem::SetForceUse(usage, config);
}

ECode CAudioSystemHelper::GetForceUse(
    /* [in] */ Int32 usage,
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetForceUse(usage, result);
}

ECode CAudioSystemHelper::InitStreamVolume(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 indexMin,
    /* [in] */ Int32 indexMax)
{
    return CAudioSystem::InitStreamVolume(stream, indexMin, indexMax);
}

ECode CAudioSystemHelper::SetStreamVolumeIndex(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 index,
    /* [in] */ Int32 device)
{
    return CAudioSystem::SetStreamVolumeIndex(stream, index, device);
}

ECode CAudioSystemHelper::GetStreamVolumeIndex(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 device,
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetStreamVolumeIndex(stream, device, result);
}

ECode CAudioSystemHelper::SetMasterVolume(
    /* [in] */ Float value)
{
    return CAudioSystem::SetMasterVolume(value);
}

ECode CAudioSystemHelper::GetMasterVolume(
    /* [out] */ Float* result)
{
    return CAudioSystem::GetMasterVolume(result);
}

ECode CAudioSystemHelper::SetMasterMute(
    /* [in] */ Boolean mute)
{
    return CAudioSystem::SetMasterMute(mute);
}

ECode CAudioSystemHelper::GetMasterMute(
    /* [out] */ Boolean* result)
{
    return CAudioSystem::GetMasterMute(result);
}

ECode CAudioSystemHelper::GetDevicesForStream(
    /* [in] */ Int32 stream,
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetDevicesForStream(stream, result);
}

ECode CAudioSystemHelper::GetPrimaryOutputSamplingRate(
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetPrimaryOutputSamplingRate(result);
}

ECode CAudioSystemHelper::GetPrimaryOutputFrameCount(
    /* [out] */ Int32* result)
{
    return CAudioSystem::GetPrimaryOutputFrameCount(result);
}


} // namespace Media
} // namespace Droid
} // namespace Elastos
