#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSSTRUCTUREDPOSTAL_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSSTRUCTUREDPOSTAL_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsStructuredPostal.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsStructuredPostal)
    , public Singleton
    , public IContactsContractCommonDataKindsStructuredPostal
    , public IContactsContractDataColumnsWithJoins
    , public IContactsContractCommonDataKindsCommonColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for all data records of the
     * {@link StructuredPostal#CONTENT_ITEM_TYPE} MIME type.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

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

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSSTRUCTUREDPOSTAL_H__
