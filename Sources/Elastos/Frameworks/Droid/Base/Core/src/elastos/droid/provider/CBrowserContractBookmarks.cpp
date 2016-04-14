
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractBookmarks.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractBookmarks)

CAR_INTERFACE_IMPL(CBrowserContractBookmarks, Object, IBrowserContractBookmarks)

ECode CBrowserContractBookmarks::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Bookmarks::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowserContractBookmarks::GetCONTENT_URI_DEFAULT_FOLDER(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Bookmarks::CONTENT_URI_DEFAULT_FOLDER;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CBrowserContractBookmarks::BuildFolderUri(
    /* [in] */ Int64 folderId,
    /* [out] */ IUri** uri)
{
    return BrowserContract::Bookmarks::BuildFolderUri(folderId, uri);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
