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

#include "elastos/droid/telecom/CVideoProfileVideoStateHelper.h"
#include "elastos/droid/telecom/VideoProfile.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CAR_INTERFACE_IMPL(CVideoProfileVideoStateHelper, Singleton, IVideoProfileVideoStateHelper)
CAR_SINGLETON_IMPL(CVideoProfileVideoStateHelper)

ECode CVideoProfileVideoStateHelper::IsAudioOnly(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsAudioOnly(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsVideo(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsVideo(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsTransmissionEnabled(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsTransmissionEnabled(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsReceptionEnabled(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsReceptionEnabled(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsBidirectional(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsBidirectional(videoState, result);
}

ECode CVideoProfileVideoStateHelper::IsPaused(
    /* [in] */ Int32 videoState,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::IsPaused(videoState, result);
}

ECode CVideoProfileVideoStateHelper::HasState(
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 state,
    /* [out] */ Boolean* result)
{
    return VideoProfile::VideoState::HasState(videoState, state, result);
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
