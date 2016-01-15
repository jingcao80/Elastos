
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractSettings.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractSettings)

CAR_INTERFACE_IMPL(CBrowserContractSettings, Singleton, IBrowserContractSettings)

ECode CBrowserContractSettings::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Settings::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowserContractSettings::IsSyncEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{
    return BrowserContract::Settings::IsSyncEnabled(context, res);
}

ECode CBrowserContractSettings::SetSyncEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    return BrowserContract::Settings::SetSyncEnabled(context, enabled);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
