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
