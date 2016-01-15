#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractChromeSyncColumns.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractChromeSyncColumns)

CAR_INTERFACE_IMPL(CBrowserContractChromeSyncColumns, Singleton, IBrowserContractChromeSyncColumns)

ECode CBrowserContractChromeSyncColumns::GetSERVER_UNIQUE(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BrowserContract::ChromeSyncColumns::SERVER_UNIQUE;
    return NOERROR;
}

ECode CBrowserContractChromeSyncColumns::GetCLIENT_UNIQUE(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BrowserContract::ChromeSyncColumns::CLIENT_UNIQUE;
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos