#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractImageMappings.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractImageMappings)

CAR_INTERFACE_IMPL(CBrowserContractImageMappings, Singleton, IBrowserContractImageMappings)

ECode CBrowserContractImageMappings::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::ImageMappings::CONTENT_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos