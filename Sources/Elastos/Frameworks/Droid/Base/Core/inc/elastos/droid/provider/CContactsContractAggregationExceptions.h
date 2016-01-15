#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTAGGREGATIONEXCEPTIONS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTAGGREGATIONEXCEPTIONS_H__

#include "_Elastos_Droid_Provider_CContactsContractAggregationExceptions.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

using Elastos::Droid::Net::IUri;

CarClass(CContactsContractAggregationExceptions)
    , public Singleton
    , public IContactsContractAggregationExceptions
    , public IBaseColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTAGGREGATIONEXCEPTIONS_H__
