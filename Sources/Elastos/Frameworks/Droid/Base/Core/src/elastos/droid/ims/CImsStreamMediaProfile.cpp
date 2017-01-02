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

#include "elastos/droid/ims/CImsStreamMediaProfile.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

const String CImsStreamMediaProfile::TAG("CImsStreamMediaProfile");

CAR_INTERFACE_IMPL_2(CImsStreamMediaProfile, Object, IImsStreamMediaProfile, IParcelable)

CAR_OBJECT_IMPL(CImsStreamMediaProfile)

CImsStreamMediaProfile::CImsStreamMediaProfile()
    : mAudioQuality(AUDIO_QUALITY_AMR_WB)
    , mAudioDirection(DIRECTION_SEND_RECEIVE)
    , mVideoQuality(VIDEO_QUALITY_NONE)
    , mVideoDirection(DIRECTION_INVALID)
{
}

CImsStreamMediaProfile::~CImsStreamMediaProfile()
{
}

ECode CImsStreamMediaProfile::constructor()
{
    return NOERROR;
}

ECode CImsStreamMediaProfile::constructor(
    /* [in] */ Int32 audioQuality,
    /* [in] */ Int32 audioDirection,
    /* [in] */ Int32 videoQuality,
    /* [in] */ Int32 videoDirection)
{
    mAudioQuality = audioQuality;
    mAudioDirection = audioDirection;
    mVideoQuality = videoQuality;
    mVideoDirection = videoDirection;
    return NOERROR;
}

ECode CImsStreamMediaProfile::CopyFrom(
    /* [in] */ IImsStreamMediaProfile* profile)
{
    AutoPtr<CImsStreamMediaProfile> p = (CImsStreamMediaProfile*)profile;
    mAudioQuality = p->mAudioQuality;
    mAudioDirection = p->mAudioDirection;
    mVideoQuality = p->mVideoQuality;
    mVideoDirection = p->mVideoDirection;
    return NOERROR;
}

ECode CImsStreamMediaProfile::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mAudioQuality);
    source->ReadInt32(&mAudioDirection);
    source->ReadInt32(&mVideoQuality);
    source->ReadInt32(&mVideoDirection);
    return NOERROR;
}

ECode CImsStreamMediaProfile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mAudioQuality);
    dest->WriteInt32(mAudioDirection);
    dest->WriteInt32(mVideoQuality);
    dest->WriteInt32(mVideoDirection);
    return NOERROR;
}

ECode CImsStreamMediaProfile::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ audioQuality=");
    sb.Append(mAudioQuality);
    sb.Append(", audioDirection=");
    sb.Append(mAudioDirection);
    sb.Append(", videoQuality=");
    sb.Append(mVideoQuality);
    sb.Append(", videoDirection=");
    sb.Append(mVideoDirection);
    sb.Append(" }");

    return sb.ToString(result);
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos