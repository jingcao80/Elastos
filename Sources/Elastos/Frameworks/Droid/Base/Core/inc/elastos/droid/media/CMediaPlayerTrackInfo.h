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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAPLAYERTRACKINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAPLAYERTRACKINFO_H__

#include "_Elastos_Droid_Media_CMediaPlayerTrackInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Class for MediaPlayer to return each audio/video/subtitle track's metadata.
 *
 * @see android.media.MediaPlayer#getTrackInfo
 */
CarClass(CMediaPlayerTrackInfo)
    , public Object
    , public IMediaPlayerTrackInfo
    , public IParcelable
{
public:
    CMediaPlayerTrackInfo();

    virtual ~CMediaPlayerTrackInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ IMediaFormat* format);

    /**
     * Gets the track type.
     * @return TrackType which indicates if the track is video, audio, timed text.
     */
    CARAPI GetTrackType(
        /* [out] */ Int32* type);

    CARAPI SetTrackType(
        /* [in] */ Int32 type);

    /**
     * Gets the language code of the track.
     * @return a language code in either way of ISO-639-1 or ISO-639-2.
     * When the language is unknown or could not be determined,
     * ISO-639-2 language code, "und", is returned.
     */
    CARAPI GetLanguage(
        /* [out] */ String* language);

    /**
     * Gets the {@link MediaFormat} of the track.  If the format is
     * unknown or could not be determined, null is returned.
     */
    CARAPI GetFormat(
        /* [out] */ IMediaFormat** result);

    CARAPI SetFormat(
        /* [in] */ IMediaFormat* format);

    /**
     * {@inheritDoc}
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * {@inheritDoc}
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* src);

    CARAPI ToString(
        /* [out] */ String* s);

public:
    Int32 mTrackType;
    AutoPtr<IMediaFormat> mFormat;
};

} // namespace media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAPLAYERTRACKINFO_H__
