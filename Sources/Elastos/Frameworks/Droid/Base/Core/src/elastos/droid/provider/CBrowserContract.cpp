
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContract.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContract)

CAR_INTERFACE_IMPL(CBrowserContract, Singleton, IBrowserContract)

ECode CBrowserContract::GetAUTHORITY_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::AUTHORITY_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
