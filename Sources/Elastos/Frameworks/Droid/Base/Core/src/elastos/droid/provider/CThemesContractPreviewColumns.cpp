
#include "elastos/droid/provider/CThemesContractPreviewColumns.h"
#include "elastos/droid/provider/ThemesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CThemesContractPreviewColumns)

CAR_INTERFACE_IMPL(CThemesContractPreviewColumns
    , Singleton, IThemesContractPreviewColumns)

ECode CThemesContractPreviewColumns::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = ThemesContract::PreviewColumns::CONTENT_URI;

    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CThemesContractPreviewColumns::GetAPPLIED_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = ThemesContract::PreviewColumns::APPLIED_URI;

    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
