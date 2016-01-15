#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCONTACTSAGGREGATIONSUGGESTIONS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCONTACTSAGGREGATIONSUGGESTIONS_H__

#include "_Elastos_Droid_Provider_CContactsContractContactsAggregationSuggestions.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractContactsAggregationSuggestions)
    , public Singleton
    , public IContactsContractContactsAggregationSuggestions
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * @hide
     */
    CARAPI Builder(
        /* [out] */ IContactsContractContactsAggregationSuggestionsBuilder** build);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCONTACTSAGGREGATIONSUGGESTIONS_H__
