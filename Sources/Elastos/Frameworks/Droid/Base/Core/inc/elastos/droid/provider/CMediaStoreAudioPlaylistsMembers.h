#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOPLAYLISTSMEMBERS_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOPLAYLISTSMEMBERS_H__

#include "_Elastos_Droid_Provider_CMediaStoreAudioPlaylistsMembers.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreAudioPlaylistsMembers)
    , public Singleton
    , public IMediaStoreAudioPlaylistsMembers
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 playlistId,
        /* [out] */ IUri** uri);

    /**
     * Convenience method to move a playlist item to a new location
     * @param res The content resolver to use
     * @param playlistId The numeric id of the playlist
     * @param from The position of the item to move
     * @param to The position to move the item to
     * @return true on success
     */
    CARAPI MoveItem(
        /* [in] */ IContentResolver* res,
        /* [in] */ Int64 playlistId,
        /* [in] */ Int32 from,
        /* [in] */ Int32 to,
        /* [out] */ Boolean* result);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOPLAYLISTSMEMBERS_H__
