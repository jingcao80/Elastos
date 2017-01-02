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

#include "elastos/droid/media/VolumeProvider.h"

namespace Elastos {
namespace Droid {
namespace Media {

VolumeProvider::VolumeProvider()
{}

VolumeProvider::~VolumeProvider()
{}

CAR_INTERFACE_IMPL(VolumeProvider, Object, IVolumeProvider)

ECode VolumeProvider::constructor(
    /* [in] */ Int32 volumeControl,
    /* [in] */ Int32 maxVolume,
    /* [in] */ Int32 currentVolume)
{
    mControlType = volumeControl;
    mMaxVolume = maxVolume;
    mCurrentVolume = currentVolume;
    return NOERROR;
}

ECode VolumeProvider::GetVolumeControl(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControlType;
    return NOERROR;
}

ECode VolumeProvider::GetMaxVolume(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMaxVolume;
    return NOERROR;
}

ECode VolumeProvider::GetCurrentVolume(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrentVolume;
    return NOERROR;
}

ECode VolumeProvider::SetCurrentVolume(
    /* [in] */ Int32 currentVolume)
{
    mCurrentVolume = currentVolume;
    if (mCallback != NULL) {
        mCallback->OnVolumeChanged(this);
    }
    return NOERROR;
}

ECode VolumeProvider::OnSetVolumeTo(
    /* [in] */ Int32 volume)
{
    return NOERROR;
}

ECode VolumeProvider::OnAdjustVolume(
    /* [in] */ Int32 direction)
{
    return NOERROR;
}

ECode VolumeProvider::SetCallback(
    /* [in] */ IVolumeProviderCallback * cb)
{
    mCallback = cb;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos