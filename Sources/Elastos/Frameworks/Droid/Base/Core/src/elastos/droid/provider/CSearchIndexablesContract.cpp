#include "elastos/droid/provider/CSearchIndexablesContract.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSearchIndexablesContract)

CAR_INTERFACE_IMPL(CSearchIndexablesContract, Singleton, ISearchIndexablesContract)

ECode CSearchIndexablesContract::GetINDEXABLES_XML_RES_PATH(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::INDEXABLES_XML_RES_PATH;
    return NOERROR;
}

ECode CSearchIndexablesContract::GetINDEXABLES_RAW_PATH(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::INDEXABLES_RAW_PATH;
    return NOERROR;
}

ECode CSearchIndexablesContract::GetNON_INDEXABLES_KEYS_PATH(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::NON_INDEXABLES_KEYS_PATH;
    return NOERROR;
}

ECode CSearchIndexablesContract::GetINDEXABLES_XML_RES_COLUMNS(
    /* [out, callee] */ ArrayOf<String>** resColumns)
{
    VALIDATE_NOT_NULL(resColumns);
    *resColumns = SearchIndexablesContract::INDEXABLES_XML_RES_COLUMNS;
    REFCOUNT_ADD(*resColumns);
    return NOERROR;
}

ECode CSearchIndexablesContract::GetINDEXABLES_RAW_COLUMNS(
    /* [out, callee] */ ArrayOf<String>** rawColumns)
{
    VALIDATE_NOT_NULL(rawColumns);
    *rawColumns = SearchIndexablesContract::INDEXABLES_RAW_COLUMNS;
    REFCOUNT_ADD(*rawColumns);
    return NOERROR;
}

ECode CSearchIndexablesContract::GetNON_INDEXABLES_KEYS_COLUMNS(
    /* [out, callee] */ ArrayOf<String>** nonColumns)
{
    VALIDATE_NOT_NULL(nonColumns);
    *nonColumns = SearchIndexablesContract::NON_INDEXABLES_KEYS_COLUMNS;
    REFCOUNT_ADD(*nonColumns);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos