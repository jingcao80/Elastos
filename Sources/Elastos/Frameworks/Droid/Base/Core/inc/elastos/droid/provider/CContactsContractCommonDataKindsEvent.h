#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSEVENT_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSEVENT_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsEvent.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsEvent)
    , public Singleton
    , public IContactsContractCommonDataKindsEvent
    , public IContactsContractDataColumnsWithJoins
    , public IContactsContractCommonDataKindsCommonColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Return the string resource that best describes the given
     * {@link #TYPE}. Will always return a valid resource.
     */
    CARAPI GetTypeResource(
        /* [in] */ IInteger32* type,
        /* [out] */ Int32* resource);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSEVENT_H__
