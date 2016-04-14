#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractHistory.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractHistory)

CAR_INTERFACE_IMPL(CBrowserContractHistory, Singleton, IBrowserContractHistory)

ECode CBrowserContractHistory::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::History::CONTENT_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos