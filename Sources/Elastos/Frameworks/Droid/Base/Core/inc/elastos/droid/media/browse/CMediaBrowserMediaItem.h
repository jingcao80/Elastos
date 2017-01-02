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

#ifndef __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERMEDIAITEM_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERMEDIAITEM_H__

#include "_Elastos_Droid_Media_Browse_CMediaBrowserMediaItem.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

CarClass(CMediaBrowserMediaItem)
    , public Object
    , public IMediaBrowserMediaItem
    , public IParcelable
{
public:
    CMediaBrowserMediaItem();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaBrowserMediaItem();

    CARAPI constructor();

    /**
     * Create a new MediaItem for use in browsing media.
     * @param description The description of the media, which must include a
     *            media id.
     * @param flags The flags for this item.
     */
    CARAPI constructor(
        /* [in] */ IMediaDescription * description,
        /* [in] */ Int32 flags);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Gets the flags of the item.
     */
    CARAPI GetFlags(
        /* [out] */ Int32 * result);

    /**
     * Returns whether this item is browsable.
     * @see #FLAG_BROWSABLE
     */
    CARAPI IsBrowsable(
        /* [out] */ Boolean * result);

    /**
     * Returns whether this item is playable.
     * @see #FLAG_PLAYABLE
     */
    CARAPI IsPlayable(
        /* [out] */ Boolean * result);

    /**
     * Returns the description of the media.
     */
    CARAPI GetDescription(
        /* [out] */ IMediaDescription ** result);

    /**
     * Returns the media id for this item.
     */
    CARAPI GetMediaId(
        /* [out] */ String * result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mFlags;
    AutoPtr<IMediaDescription> mDescription;

};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERMEDIAITEM_H__
