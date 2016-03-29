#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSLOCALGROUP_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSLOCALGROUP_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsLocalGroup.h"
#include <elastos/core/Singleton.h>

//using Elastos::Droid::Content::IContext;
//using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
//using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsLocalGroup)
    , public Singleton
    , public IContactsContractCommonDataKindsLocalGroup
    , public IContactsContractDataColumnsWithJoins
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /** @hide */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /** @hide */
    CARAPI GetCONTENT_LOOKUP_URI(
        /* [out] */ IUri** uri);

    /** @hide */
    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSLOCALGROUP_H__
