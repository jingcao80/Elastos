#ifndef __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTNONINDEXABLEKEY_H
#define __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTNONINDEXABLEKEY_H

#include "_Elastos_Droid_Provider_CSearchIndexablesContractNonIndexableKey.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSearchIndexablesContractNonIndexableKey)
    , public Singleton
    , public ISearchIndexablesContractNonIndexableKey
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNonIndexableKeyMIME_TYPE(
        /* [out] */ String* result);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTNONINDEXABLEKEY_H

