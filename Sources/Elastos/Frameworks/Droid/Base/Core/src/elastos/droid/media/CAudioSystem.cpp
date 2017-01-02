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

#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/media/CAudioSystem.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CAudioSystem)
CAR_INTERFACE_IMPL(CAudioSystem, Singleton, IAudioSystem);

ECode CAudioSystem::GetNumStreamTypes(
    /* [out] */ Int32* result)
{
    return AudioSystem::GetNumStreamTypes(result);
}

ECode CAudioSystem::MuteMicrophone(
    /* [in] */ Boolean on)
{
    return AudioSystem::MuteMicrophone(on);
}

ECode CAudioSystem::IsMicrophoneMuted(
    /* [out] */ Boolean* result)
{
    return AudioSystem::IsMicrophoneMuted(result);
}

ECode CAudioSystem::IsStreamActive(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 inPastMs,
    /* [out] */ Boolean* result)
{
    return AudioSystem::IsStreamActive(stream, inPastMs, result);
}

ECode CAudioSystem::IsStreamActiveRemotely(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 inPastMs,
    /* [out] */ Boolean * result)
{
    return AudioSystem::IsStreamActiveRemotely(stream, inPastMs, result);
}

ECode CAudioSystem::IsSourceActive(
    /* [in] */ Int32 source,
    /* [out] */ Boolean* result)
{
    return AudioSystem::IsSourceActive(source, result);
}

ECode CAudioSystem::NewAudioSessionId(
    /* [out] */ Int32* result)
{
    return AudioSystem::NewAudioSessionId(result);
}

ECode CAudioSystem::SetParameters(
    /* [in] */ const String& keyValuePairs)
{
    return AudioSystem::SetParameters(keyValuePairs);
}

ECode CAudioSystem::GetParameters(
    /* [in] */ const String& keys,
    /* [out] */ String* result)
{
    return AudioSystem::GetParameters(keys, result);
}

ECode CAudioSystem::SetErrorCallback(
    /* [in] */ IAudioSystemErrorCallback* cb)
{
    return AudioSystem::SetErrorCallback(cb);
}

ECode CAudioSystem::GetOutputDeviceName(
    /* [in] */ Int32 device,
    /* [out] */ String* result)
{
    return AudioSystem::GetOutputDeviceName(device, result);
}

ECode CAudioSystem::SetDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& device_address)
{
    return AudioSystem::SetDeviceConnectionState(device, state, device_address);
}

ECode CAudioSystem::GetDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ const String& device_address,
    /* [out] */ Int32* result)
{
    return AudioSystem::GetDeviceConnectionState(device, device_address, result);
}

ECode CAudioSystem::SetPhoneState(
    /* [in] */ Int32 state,
    /* [out] */ Int32* result)
{
    return AudioSystem::SetPhoneState(state, result);
}

ECode CAudioSystem::SetForceUse(
    /* [in] */ Int32 usage,
    /* [in] */ Int32 config)
{
    return AudioSystem::SetForceUse(usage, config);
}

ECode CAudioSystem::GetForceUse(
    /* [in] */ Int32 usage,
    /* [out] */ Int32* result)
{
    return AudioSystem::GetForceUse(usage, result);
}

ECode CAudioSystem::InitStreamVolume(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 indexMin,
    /* [in] */ Int32 indexMax)
{
    return AudioSystem::InitStreamVolume(stream, indexMin, indexMax);
}

ECode CAudioSystem::SetStreamVolumeIndex(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 index,
    /* [in] */ Int32 device)
{
    return AudioSystem::SetStreamVolumeIndex(stream, index, device);
}

ECode CAudioSystem::GetStreamVolumeIndex(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 device,
    /* [out] */ Int32* result)
{
    return AudioSystem::GetStreamVolumeIndex(stream, device, result);
}

ECode CAudioSystem::SetMasterVolume(
    /* [in] */ Float value)
{
    return AudioSystem::SetMasterVolume(value);
}

ECode CAudioSystem::GetMasterVolume(
    /* [out] */ Float* result)
{
    return AudioSystem::GetMasterVolume(result);
}

ECode CAudioSystem::SetMasterMute(
    /* [in] */ Boolean mute)
{
    return AudioSystem::SetMasterMute(mute);
}

ECode CAudioSystem::GetMasterMute(
    /* [out] */ Boolean* result)
{
    return AudioSystem::GetMasterMute(result);
}

ECode CAudioSystem::GetDevicesForStream(
    /* [in] */ Int32 stream,
    /* [out] */ Int32* result)
{
    return AudioSystem::GetDevicesForStream(stream, result);
}

ECode CAudioSystem::GetPrimaryOutputSamplingRate(
    /* [out] */ Int32* result)
{
    return AudioSystem::GetPrimaryOutputSamplingRate(result);
}

ECode CAudioSystem::GetPrimaryOutputFrameCount(
    /* [out] */ Int32* result)
{
    return AudioSystem::GetPrimaryOutputFrameCount(result);
}

ECode CAudioSystem::GetOutputLatency(
    /* [in] */ Int32 stream,
    /* [out] */ Int32* result)
{
    return AudioSystem::GetOutputLatency(stream, result);
}

ECode CAudioSystem::SetLowRamDevice(
    /* [in] */ Boolean isLowRamDevice,
    /* [out] */ Int32* result)
{
    return AudioSystem::SetLowRamDevice(isLowRamDevice, result);
}

ECode CAudioSystem::CheckAudioFlinger(
    /* [out] */ Int32* result)
{
    return AudioSystem::CheckAudioFlinger(result);
}

ECode CAudioSystem::ListAudioPorts(
    /* [in] */ IArrayList* ports,
    /* [in] */ ArrayOf<Int32>* generation,
    /* [out] */ Int32* result)
{
    return AudioSystem::ListAudioPorts(ports, generation, result);
}

ECode CAudioSystem::CreateAudioPatch(
    /* [in] */ ArrayOf<IAudioPatch*>* patch,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sinks,
    /* [out] */ Int32* result)
{
    return AudioSystem::CreateAudioPatch(patch, sources, sinks, result);
}

ECode CAudioSystem::ReleaseAudioPatch(
    /* [in] */ IAudioPatch* patch,
    /* [out] */ Int32* result)
{
    return AudioSystem::ReleaseAudioPatch(patch, result);
}

ECode CAudioSystem::ListAudioPatches(
    /* [in] */ IArrayList* patches,
    /* [in] */ ArrayOf<Int32>* generation,
    /* [out] */ Int32* result)
{
    return AudioSystem::ListAudioPatches(patches, generation, result);
}

ECode CAudioSystem::SetAudioPortConfig(
    /* [in] */ IAudioPortConfig* config,
    /* [out] */ Int32* result)
{
    return AudioSystem::SetAudioPortConfig(config, result);
}

ECode CAudioSystem::GetAudioHwSyncForSession(
    /* [in] */ Int32 sessionId,
    /* [out] */ Int32* result)
{
    return AudioSystem::GetAudioHwSyncForSession(sessionId, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
