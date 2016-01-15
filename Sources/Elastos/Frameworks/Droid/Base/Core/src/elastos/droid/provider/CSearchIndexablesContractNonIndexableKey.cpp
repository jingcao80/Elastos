#include "elastos/droid/provider/CSearchIndexablesContractNonIndexableKey.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSearchIndexablesContractNonIndexableKey)

CAR_INTERFACE_IMPL(CSearchIndexablesContractNonIndexableKey, Singleton, ISearchIndexablesContractNonIndexableKey)

ECode CSearchIndexablesContractNonIndexableKey::GetNonIndexableKeyMIME_TYPE(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::NonIndexableKey::MIME_TYPE;
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos

