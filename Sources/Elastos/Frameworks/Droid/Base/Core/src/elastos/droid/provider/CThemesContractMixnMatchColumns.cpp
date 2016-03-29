
#include "elastos/droid/provider/CThemesContractMixnMatchColumns.h"
#include "elastos/droid/provider/ThemesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CThemesContractMixnMatchColumns)

CAR_INTERFACE_IMPL(CThemesContractMixnMatchColumns
    , Singleton, IThemesContractMixnMatchColumns)

ECode CThemesContractMixnMatchColumns::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = ThemesContract::MixnMatchColumns::CONTENT_URI;

    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CThemesContractMixnMatchColumns::GetROWS(
    /* [out, callee] */ ArrayOf<String>** rows)
{
    VALIDATE_NOT_NULL(rows);

    *rows = ThemesContract::MixnMatchColumns::ROWS;

    REFCOUNT_ADD(*rows);
    return NOERROR;
}

ECode CThemesContractMixnMatchColumns::ComponentToImageColName(
    /* [in] */ const String& component,
    /* [out] */ String* imageColName)
{
    return ThemesContract::MixnMatchColumns::ComponentToImageColName(
            component, imageColName);
}

ECode CThemesContractMixnMatchColumns::ComponentToMixNMatchKey(
    /* [in] */ const String& component,
    /* [out] */ String* mixNMatchKey)
{
    return ThemesContract::MixnMatchColumns::ComponentToMixNMatchKey(
            component, mixNMatchKey);
}

ECode CThemesContractMixnMatchColumns::MixNMatchKeyToComponent(
    /* [in] */ const String& mixnmatchKey,
    /* [out] */ String* component)
{
    return ThemesContract::MixnMatchColumns::MixNMatchKeyToComponent(
            mixnmatchKey, component);
}

} //Provider
} //Droid
} //Elastos
