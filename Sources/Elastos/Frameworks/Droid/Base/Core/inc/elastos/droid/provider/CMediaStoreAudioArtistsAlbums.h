#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOARTISTSALBUMS_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOARTISTSALBUMS_H__

#include "_Elastos_Droid_Provider_CMediaStoreAudioArtistsAlbums.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreAudioArtistsAlbums)
    , public Singleton
    , public IMediaStoreAudioArtistsAlbums
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 artistId,
        /* [out] */ IUri** uri);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOARTISTSALBUMS_H__
