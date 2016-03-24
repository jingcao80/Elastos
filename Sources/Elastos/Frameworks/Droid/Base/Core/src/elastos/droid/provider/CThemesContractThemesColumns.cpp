
#include "elastos/droid/provider/CThemesContractThemesColumns.h"
#include "elastos/droid/provider/ThemesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CThemesContractThemesColumns)

CAR_INTERFACE_IMPL(CThemesContractThemesColumns
    , Singleton, IThemesContractThemesColumns)

ECode CThemesContractThemesColumns::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = ThemesContract::ThemesColumns::CONTENT_URI;

    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
