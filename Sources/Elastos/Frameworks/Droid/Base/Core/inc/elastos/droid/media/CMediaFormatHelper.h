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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAFORMATHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAFORMATHELPER_H__

#include "_Elastos_Droid_Media_CMediaFormatHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaFormatHelper)
    , public Singleton
    , public IMediaFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateAudioFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelCount,
        /* [out] */ IMediaFormat** result);

    /**
     * Creates a minimal subtitle format.
     * @param mime The mime type of the content.
     * @param language The language of the content, using either ISO 639-1 or 639-2/T
     *        codes.  Specify null or "und" if language information is only included
     *        in the content.  (This will also work if there are multiple language
     *        tracks in the content.)
     */
    CARAPI CreateSubtitleFormat(
        /* [in] */ const String& mime,
        /* [in] */ const String& language,
        /* [out] */ IMediaFormat** result);

    /**
     * Creates a minimal video format.
     * @param mime The mime type of the content.
     * @param width The width of the content (in pixels)
     * @param height The height of the content (in pixels)
     */
    CARAPI CreateVideoFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IMediaFormat** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAFORMATHELPER_H__
