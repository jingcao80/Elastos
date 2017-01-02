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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/media/tv/CTvTrackInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL_2(CTvTrackInfo, Object, ITvTrackInfo, IParcelable)

CAR_OBJECT_IMPL(CTvTrackInfo)

CTvTrackInfo::CTvTrackInfo()
    : mType(0)
    , mAudioChannelCount(0)
    , mAudioSampleRate(0)
    , mVideoWidth(0)
    , mVideoHeight(0)
    , mVideoFrameRate(0)
{
}

CTvTrackInfo::~CTvTrackInfo()
{
}

ECode CTvTrackInfo::constructor()
{
    return NOERROR;
}

ECode CTvTrackInfo::constructor(
    /* [in] */ Int32 type,
    /* [in] */ const String& id,
    /* [in] */ const String& language,
    /* [in] */ Int32 audioChannelCount,
    /* [in] */ Int32 audioSampleRate,
    /* [in] */ Int32 videoWidth,
    /* [in] */ Int32 videoHeight,
    /* [in] */ Float videoFrameRate,
    /* [in] */ IBundle* extra)
{
    mType = type;
    mId = id;
    mLanguage = language;
    mAudioChannelCount = audioChannelCount;
    mAudioSampleRate = audioSampleRate;
    mVideoWidth = videoWidth;
    mVideoHeight = videoHeight;
    mVideoFrameRate = videoFrameRate;
    mExtra = extra;
    return NOERROR;
}

ECode CTvTrackInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mType);
    source->ReadInt32(&mAudioChannelCount);
    source->ReadInt32(&mAudioSampleRate);
    source->ReadInt32(&mVideoWidth);
    source->ReadInt32(&mVideoHeight);
    source->ReadFloat(&mVideoFrameRate);
    source->ReadString(&mId);
    source->ReadString(&mLanguage);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mExtra = IBundle::Probe(obj);
    return NOERROR;
}

ECode CTvTrackInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mType);
    dest->WriteInt32(mAudioChannelCount);
    dest->WriteInt32(mAudioSampleRate);
    dest->WriteInt32(mVideoWidth);
    dest->WriteInt32(mVideoHeight);
    dest->WriteFloat(mVideoFrameRate);
    dest->WriteString(mId);
    dest->WriteString(mLanguage);
    dest->WriteInterfacePtr(mExtra);
    return NOERROR;
}

ECode CTvTrackInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode CTvTrackInfo::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CTvTrackInfo::GetLanguage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLanguage;
    return NOERROR;
}

ECode CTvTrackInfo::GetAudioChannelCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mType != ITvTrackInfo::TYPE_AUDIO) {
        // throw new IllegalStateException("Not an audio track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mAudioChannelCount;
    return NOERROR;
}

ECode CTvTrackInfo::GetAudioSampleRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mType != ITvTrackInfo::TYPE_AUDIO) {
        // throw new IllegalStateException("Not an audio track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mAudioSampleRate;
    return NOERROR;
}

ECode CTvTrackInfo::GetVideoWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mType != ITvTrackInfo::TYPE_VIDEO) {
        // throw new IllegalStateException("Not a video track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mVideoWidth;
    return NOERROR;
}

ECode CTvTrackInfo::GetVideoHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mType != ITvTrackInfo::TYPE_VIDEO) {
        // throw new IllegalStateException("Not a video track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mVideoHeight;
    return NOERROR;
}

ECode CTvTrackInfo::GetVideoFrameRate(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mType != ITvTrackInfo::TYPE_VIDEO) {
        // throw new IllegalStateException("Not a video track");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mVideoFrameRate;
    return NOERROR;
}

ECode CTvTrackInfo::GetExtra(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtra;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
