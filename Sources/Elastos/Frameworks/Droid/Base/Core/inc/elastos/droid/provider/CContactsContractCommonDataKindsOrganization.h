#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSORGANIZATION_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSORGANIZATION_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsOrganization.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsOrganization)
    , public Singleton
    , public IContactsContractCommonDataKindsOrganization
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
    CARAPI GetTypeLabelResource(
        /* [in] */ Int32 type,
        /* [out] */ Int32* resource);

    /**
     * Return a {@link CharSequence} that best describes the given type,
     * possibly substituting the given {@link #LABEL} value
     * for {@link #TYPE_CUSTOM}.
     */
    CARAPI GetTypeLabel(
        /* [in] */ IResources* res,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [out] */ ICharSequence** lb);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSORGANIZATION_H__
