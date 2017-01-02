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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIADESCRIPTION_H__
#define __ELASTOS_DROID_MEDIA_CMEDIADESCRIPTION_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Media_CMediaDescription.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * A simple set of metadata for a media item suitable for display. This can be
 * created using the Builder or retrieved from existing metadata using
 * {@link MediaMetadata#getDescription()}.
 */
CarClass(CMediaDescription)
    , public Object
    , public IMediaDescription
    , public IParcelable
{
public:
    CMediaDescription();

    virtual ~CMediaDescription();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& mediaId,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* subtitle,
        /* [in] */ ICharSequence* description,
        /* [in] */ IBitmap* icon,
        /* [in] */ IUri* iconUri,
        /* [in] */ IBundle* extras);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the media id or null. See
     * {@link MediaMetadata#METADATA_KEY_MEDIA_ID}.
     */
    CARAPI GetMediaId(
        /* [out] */ String* result);

    /**
     * Returns a title suitable for display or null.
     *
     * @return A title or null.
     */
    CARAPI GetTitle(
        /* [out] */ ICharSequence** result);

    /**
     * Returns a subtitle suitable for display or null.
     *
     * @return A subtitle or null.
     */
    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** result);

    /**
     * Returns a description suitable for display or null.
     *
     * @return A description or null.
     */
    CARAPI GetDescription(
        /* [out] */ ICharSequence** result);

    /**
     * Returns a bitmap icon suitable for display or null.
     *
     * @return An icon or null.
     */
    CARAPI GetIconBitmap(
        /* [out] */ IBitmap** result);

    /**
     * Returns a Uri for an icon suitable for display or null.
     *
     * @return An icon uri or null.
     */
    CARAPI GetIconUri(
        /* [out] */ IUri** result);

    /**
     * Returns any extras that were added to the description.
     *
     * @return A bundle of extras or null.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    /**
     * A unique persistent id for the content or null.
     */
    String mMediaId;
    /**
     * A primary title suitable for display or null.
     */
    AutoPtr<ICharSequence> mTitle;
    /**
     * A subtitle suitable for display or null.
     */
    AutoPtr<ICharSequence> mSubtitle;
    /**
     * A description suitable for display or null.
     */
    AutoPtr<ICharSequence> mDescription;
    /**
     * A bitmap icon suitable for display or null.
     */
    AutoPtr<IBitmap> mIcon;
    /**
     * A Uri for an icon suitable for display or null.
     */
    AutoPtr<IUri> mIconUri;
    /**
     * Extras for opaque use by apps/system.
     */
    AutoPtr<IBundle> mExtras;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIADESCRIPTION_H__
