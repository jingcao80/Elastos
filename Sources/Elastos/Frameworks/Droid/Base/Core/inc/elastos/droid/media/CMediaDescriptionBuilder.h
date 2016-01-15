#ifndef __ELASTOS_DROID_MEDIA_CMEDIADESCRIPTIONBUILDER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIADESCRIPTIONBUILDER_H__

#include "_Elastos_Droid_Media_CMediaDescriptionBuilder.h"
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
 * Builder for {@link MediaDescription} objects.
 */
CarClass(CMediaDescriptionBuilder)
    , public Object
    , public IMediaDescriptionBuilder
{
public:
    CMediaDescriptionBuilder();

    virtual ~CMediaDescriptionBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates an initially empty builder.
     */
    CARAPI constructor();

    /**
     * Sets the media id.
     *
     * @param mediaId The unique id for the item or null.
     * @return this
     */
    CARAPI SetMediaId(
        /* [in] */ const String& mediaId);

    /**
     * Sets the title.
     *
     * @param title A title suitable for display to the user or null.
     * @return this
     */
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    /**
     * Sets the subtitle.
     *
     * @param subtitle A subtitle suitable for display to the user or null.
     * @return this
     */
    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle);

    /**
     * Sets the description.
     *
     * @param description A description suitable for display to the user or
     *            null.
     * @return this
     */
    CARAPI SetDescription(
        /* [in] */ ICharSequence* description);

    /**
     * Sets the icon.
     *
     * @param icon A {@link Bitmap} icon suitable for display to the user or
     *            null.
     * @return this
     */
    CARAPI SetIconBitmap(
        /* [in] */ IBitmap* icon);

    /**
     * Sets the icon uri.
     *
     * @param iconUri A {@link Uri} for an icon suitable for display to the
     *            user or null.
     * @return this
     */
    CARAPI SetIconUri(
        /* [in] */ IUri* iconUri);

    /**
     * Sets a bundle of extras.
     *
     * @param extras The extras to include with this description or null.
     * @return this
     */
    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    CARAPI Build(
        /* [out] */ IMediaDescription** result);

private:
    String mMediaId;
    AutoPtr<ICharSequence> mTitle;
    AutoPtr<ICharSequence> mSubtitle;
    AutoPtr<ICharSequence> mDescription;
    AutoPtr<IBitmap> mIcon;
    AutoPtr<IUri> mIconUri;
    AutoPtr<IBundle> mExtras;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIADESCRIPTIONBUILDER_H__
