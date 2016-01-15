#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREIMAGESMEDIA_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREIMAGESMEDIA_H__

#include "_Elastos_Droid_Provider_CMediaStoreImagesMedia.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreImagesMedia)
    , public Singleton
    , public IMediaStoreImagesMedia
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for the internal storage.
     */
    CARAPI GetINTERNAL_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for the "primary" external storage
     * volume.
     */
    CARAPI GetEXTERNAL_CONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& where,
        /* [in] */ const String& orderBy,
        /* [out] */ ICursor** cursor);

    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& orderBy,
        /* [out] */ ICursor** cursor);

    /**
     * Retrieves an image for the given url as a {@link Bitmap}.
     *
     * @param cr The content resolver to use
     * @param url The url of the image
     * @throws FileNotFoundException
     * @throws IOException
     */
    CARAPI GetBitmap(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* url,
        /* [out] */ IBitmap** bitmap);

    /**
     * Insert an image and create a thumbnail for it.
     *
     * @param cr The content resolver to use
     * @param imagePath The path to the image to insert
     * @param name The name of the image
     * @param description The description of the image
     * @return The URL to the newly created image
     * @throws FileNotFoundException
     */
    CARAPI InsertImage(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& imagePath,
        /* [in] */ const String& name,
        /* [in] */ const String& description,
        /* [out] */ String* url);

    /**
     * Insert an image and create a thumbnail for it.
     *
     * @param cr The content resolver to use
     * @param source The stream to use for the image
     * @param title The name of the image
     * @param description The description of the image
     * @return The URL to the newly created image, or <code>null</code> if the image failed to be stored
     *              for any reason.
     */
    CARAPI InsertImage(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IBitmap* source,
        /* [in] */ const String& title,
        /* [in] */ const String& description,
        /* [out] */ String* urlvalue);

    /**
     * Get the content:// style URI for the image media table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @return the URI to the image media table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

private:
    CARAPI_(AutoPtr<IBitmap>) StoreThumbnail(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IBitmap* source,
        /* [in] */ Int64 id,
        /* [in] */ Float width,
        /* [in] */ Float height,
        /* [in] */ Int32 kind);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREIMAGESMEDIA_H__
