
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractSearches.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractSearches)

CAR_INTERFACE_IMPL(CBrowserContractSearches, Singleton, IBrowserContractSearches)

/**
 * The content:// style URI for this table
 */
ECode CBrowserContractSearches::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Searches::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
