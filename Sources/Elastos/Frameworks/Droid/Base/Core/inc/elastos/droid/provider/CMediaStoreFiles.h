#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREFILES_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREFILES_H__

#include "_Elastos_Droid_Provider_CMediaStoreFiles.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreFiles)
    , public Singleton
    , public IMediaStoreFiles
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Get the content:// style URI for the files table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @return the URI to the files table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

    /**
     * Get the content:// style URI for a single row in the files table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @param rowId the file to get the URI for
     * @return the URI to the files table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 rowId,
        /* [out] */ IUri** uri);

    /**
     * For use only by the MTP implementation.
     * @hide
     */
    CARAPI GetMtpObjectsUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

    /**
     * For use only by the MTP implementation.
     * @hide
     */
    CARAPI GetMtpObjectsUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 fileId,
        /* [out] */ IUri** uri);

    /**
     * Used to implement the MTP GetObjectReferences and SetObjectReferences commands.
     * @hide
     */
    CARAPI GetMtpReferencesUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 fileId,
        /* [out] */ IUri** uri);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREFILES_H__
