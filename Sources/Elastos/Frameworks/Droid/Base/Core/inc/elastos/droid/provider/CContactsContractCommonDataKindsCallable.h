#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSCALLABLE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSCALLABLE_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsCallable.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsCallable)
    , public Singleton
    , public IContactsContractCommonDataKindsCallable
    , public IContactsContractDataColumnsWithJoins
    , public IContactsContractCommonDataKindsCommonColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Similar to {@link Phone#CONTENT_URI}, but returns callable data instead of only
     * phone numbers.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
    /**
     * Similar to {@link Phone#CONTENT_FILTER_URI}, but allows users to filter callable
     * data.
     */
    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSCALLABLE_H__
