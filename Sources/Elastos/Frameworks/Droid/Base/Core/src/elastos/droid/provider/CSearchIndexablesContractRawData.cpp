#include "elastos/droid/provider/CSearchIndexablesContractRawData.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSearchIndexablesContractRawData)

CAR_INTERFACE_IMPL(CSearchIndexablesContractRawData, Singleton, ISearchIndexablesContractRawData)

ECode CSearchIndexablesContractRawData::GetRawDataMIME_TYPE(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::RawData::MIME_TYPE;
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
