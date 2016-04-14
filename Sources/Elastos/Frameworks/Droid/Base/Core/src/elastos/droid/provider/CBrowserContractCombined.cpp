#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractCombined.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractCombined)

CAR_INTERFACE_IMPL(CBrowserContractCombined, Singleton, IBrowserContractCombined)

ECode CBrowserContractCombined::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Combined::CONTENT_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos