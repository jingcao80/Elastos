#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTBOOKMARKS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTBOOKMARKS_H__

#include "_Elastos_Droid_Provider_CBrowserContractBookmarks.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractBookmarks)
    , public Singleton
    , public IBrowserContractBookmarks
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for the default folder
     * @hide
     */
    CARAPI GetCONTENT_URI_DEFAULT_FOLDER(
        /* [out] */ IUri** uri);

    /**
     * Builds a URI that points to a specific folder.
     * @param folderId the ID of the folder to point to
     * @hide
     */
    CARAPI BuildFolderUri(
        /* [in] */ Int64 folderId,
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTBOOKMARKS_H__
