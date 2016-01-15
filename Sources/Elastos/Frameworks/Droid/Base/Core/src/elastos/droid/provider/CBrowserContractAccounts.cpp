
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractAccounts.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractAccounts)

CAR_INTERFACE_IMPL(CBrowserContractAccounts, Singleton, IBrowserContractAccounts)

CARAPI CBrowserContractAccounts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Accounts::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
