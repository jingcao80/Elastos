
#include "elastos/droid/provider/CThemesContract.h"
#include "elastos/droid/provider/ThemesContract.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CThemesContract)

CAR_INTERFACE_IMPL(CThemesContract, Singleton, IThemesContract)

ECode CThemesContract::GetAUTHORITY_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = ThemesContract::AUTHORITY_URI;
    (*uri)->Release();

    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
