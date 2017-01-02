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

#include "elastos/droid/media/CMediaFormatHelper.h"
#include "elastos/droid/media/CMediaFormat.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaFormatHelper, Singleton, IMediaFormatHelper)

CAR_SINGLETON_IMPL(CMediaFormatHelper)

ECode CMediaFormatHelper::CreateAudioFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channelCount,
    /* [out] */ IMediaFormat** result)
{
    return CMediaFormat::CreateAudioFormat(mime, sampleRate, channelCount, result);
}

ECode CMediaFormatHelper::CreateSubtitleFormat(
    /* [in] */ const String& mime,
    /* [in] */ const String& language,
    /* [out] */ IMediaFormat** result)
{
    return CMediaFormat::CreateSubtitleFormat(mime, language, result);
}

ECode CMediaFormatHelper::CreateVideoFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IMediaFormat** result)
{
    return CMediaFormat::CreateAudioFormat(mime, width, height, result);
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
