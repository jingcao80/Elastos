#include "elastos/droid/provider/CSearchIndexablesContractXmlResource.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSearchIndexablesContractXmlResource)

CAR_INTERFACE_IMPL(CSearchIndexablesContractXmlResource, Singleton, ISearchIndexablesContractXmlResource)

ECode CSearchIndexablesContractXmlResource::GetXmlResourceMIME_TYPE(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchIndexablesContract::XmlResource::MIME_TYPE;
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
