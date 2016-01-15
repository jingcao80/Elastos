#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOGENRES_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOGENRES_H__

#include "_Elastos_Droid_Provider_CMediaStoreAudioGenres.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreAudioGenres)
    , public Singleton
    , public IMediaStoreAudioGenres
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

    /**
     * Get the content:// style URI for the audio genres table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @return the URI to the audio genres table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

    /**
     * Get the content:// style URI for querying the genres of an audio file.
     *
     * @param volumeName the name of the volume to get the URI for
     * @param audioId the ID of the audio file for which to retrieve the genres
     * @return the URI to for querying the genres for the audio file
     * with the given the volume and audioID
     */
    CARAPI GetContentUriForAudioId(
        /* [in] */ const String& volumeName,
        /* [in] */ Int32 audioId,
        /* [out] */ IUri** uri);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOGENRES_H__
