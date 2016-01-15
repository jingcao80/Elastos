#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOMEDIA_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOMEDIA_H__

#include "_Elastos_Droid_Provider_CMediaStoreAudioMedia.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreAudioMedia)
    , public Singleton
    , public IMediaStoreAudioMedia
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
     * Get the content:// style URI for the audio media table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @return the URI to the audio media table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

    CARAPI GetContentUriForPath(
        /* [in] */ const String& path,
        /* [out] */ IUri** uri);

private:
    static AutoPtr<ArrayOf<String> > initEXTERNAL_PATHS();
private:
    static const AutoPtr<ArrayOf<String> > EXTERNAL_PATHS;
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOMEDIA_H__
