#ifndef __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTRAWDATA_H__
#define __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTRAWDATA_H__

#include "_Elastos_Droid_Provider_CSearchIndexablesContractRawData.h"
#include "elastos/droid/provider/SearchIndexablesContract.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSearchIndexablesContractRawData)
    , public Singleton
    , public ISearchIndexablesContractRawData
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetRawDataMIME_TYPE(
        /* [out] */ String* result);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CSEARCHINDEXABLESCONTRACTRAWDATA_H__