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

#include "elastos/droid/media/CAudioTrackHelper.h"
#include "elastos/droid/media/CAudioTrack.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioTrackHelper, Singleton, IAudioTrackHelper)

CAR_SINGLETON_IMPL(CAudioTrackHelper)

ECode CAudioTrackHelper::GetMinVolume(
    /* [out] */ Float* minVolume)
{
    return CAudioTrack::GetMinVolume(minVolume);
}
ECode CAudioTrackHelper::GetMaxVolume(
    /* [out] */ Float* maxVolume)
{
    return CAudioTrack::GetMaxVolume(maxVolume);
}
ECode CAudioTrackHelper::GetNativeOutputSampleRate(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* rate)
{
    VALIDATE_NOT_NULL(rate);
    *rate = CAudioTrack::NativeGetOutputSampleRate(streamType);
    return NOERROR;
}
ECode CAudioTrackHelper::GetMinBufferSize(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* size)
{
    return CAudioTrack::GetMinBufferSize(sampleRateInHz,
            channelConfig, audioFormat, size);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
