#ifndef __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTXMLRESOURCE_H
#define __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTXMLRESOURCE_H

#include "_Elastos_Droid_Provider_CSearchIndexablesContractXmlResource.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSearchIndexablesContractXmlResource)
    , public Singleton
    , public ISearchIndexablesContractXmlResource
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetXmlResourceMIME_TYPE(
        /* [out] */ String* result);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTXMLRESOURCE_H