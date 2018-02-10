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

#include "elastos/droid/media/CMediaPlayerTrackInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL_2(CMediaPlayerTrackInfo, Object, IMediaPlayerTrackInfo, IParcelable)

CAR_OBJECT_IMPL(CMediaPlayerTrackInfo)

CMediaPlayerTrackInfo::CMediaPlayerTrackInfo()
    : mTrackType(IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_UNKNOWN)
{}

CMediaPlayerTrackInfo::~CMediaPlayerTrackInfo()
{
}

ECode CMediaPlayerTrackInfo::constructor()
{
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::constructor(
    /* [in] */ Int32 type,
    /* [in] */ IMediaFormat* format)
{
    mTrackType = type;
    mFormat = format;
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::GetTrackType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mTrackType;
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::SetTrackType(
    /* [in] */ Int32 type)
{
    mTrackType = type;
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::GetLanguage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String language;
    mFormat->GetString(IMediaFormat::KEY_LANGUAGE, &language);
    *result = language == NULL ? String("und") : language;
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::GetFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    if (mTrackType == MEDIA_TRACK_TYPE_TIMEDTEXT
            || mTrackType == MEDIA_TRACK_TYPE_SUBTITLE) {
        *result = mFormat;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::SetFormat(
    /* [in] */ IMediaFormat* format)
{
    mFormat = format;
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mTrackType);
    dest->WriteInterfacePtr(mFormat);
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    VALIDATE_NOT_NULL(src);
    src->ReadInt32(&mTrackType);
    AutoPtr<IInterface> obj;
    src->ReadInterfacePtr((HANDLE*)&obj);
    mFormat = IMediaFormat::Probe(obj);
    return NOERROR;
}

ECode CMediaPlayerTrackInfo::ToString(
    /* [out] */ String* s)
{
    StringBuilder out;
    out.Append("CMediaPlayerTrackInfo");
    out.AppendChar('{');
    switch (mTrackType) {
    case MEDIA_TRACK_TYPE_VIDEO:
        out.Append("VIDEO");
        break;
    case MEDIA_TRACK_TYPE_AUDIO:
        out.Append("AUDIO");
        break;
    case MEDIA_TRACK_TYPE_TIMEDTEXT:
        out.Append("TIMEDTEXT");
        break;
    case MEDIA_TRACK_TYPE_SUBTITLE:
        out.Append("SUBTITLE");
        break;
    default:
        out.Append("UNKNOWN");
        break;
    }
    out.Append(", ");
    String str;
    mFormat->ToString(&str);
    out.Append(str);
    out.Append("}");
    return out.ToString(s);
}

} // namespace media
} // namepsace Droid
} // namespace Elastos
