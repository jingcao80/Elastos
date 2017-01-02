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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/SubtitleData.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(SubtitleData, Object, ISubtitleData)

SubtitleData::SubtitleData()
    : mTrackIndex(0)
    , mStartTimeUs(0)
    , mDurationUs(0)
{}

SubtitleData::~SubtitleData()
{}

const String SubtitleData::TAG("SubtitleData");

ECode SubtitleData::constructor(
    /* [in] */ IParcel* parcel)
{
    if (!ParseParcel(parcel)) {
        // throw new IllegalArgumentException("parseParcel() fails");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SubtitleData::GetTrackIndex(
    /* [out] */ Int32* trackIndex)
{
    VALIDATE_NOT_NULL(trackIndex);
    *trackIndex = mTrackIndex;
    return NOERROR;
}

ECode SubtitleData::GetStartTimeUs(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTrackIndex;
    return NOERROR;
}

ECode SubtitleData::GetDurationUs(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDurationUs;
    return NOERROR;
}

ECode SubtitleData::GetData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean SubtitleData::ParseParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mTrackIndex);
    in->ReadInt64(&mStartTimeUs);
    in->ReadInt64(&mDurationUs);
    in->Marshall((ArrayOf<Byte>**)&mData);
    return TRUE;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos