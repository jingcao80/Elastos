#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSPHONE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSPHONE_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsPhone.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsPhone)
    , public Singleton
    , public IContactsContractCommonDataKindsPhone
    , public IContactsContractDataColumnsWithJoins
    , public IContactsContractCommonDataKindsCommonColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for all data records of the
     * {@link #CONTENT_ITEM_TYPE} MIME type, combined with the
     * associated raw contact and aggregate contact data.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for phone lookup using a filter. The filter returns
     * records of MIME type {@link #CONTENT_ITEM_TYPE}. The filter is applied
     * to display names as well as phone numbers. The filter argument should be passed
     * as an additional path segment after this URI.
     */
    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);

    /**
     * @deprecated use {@link #getTypeLabel(Resources, int, CharSequence)} instead.
     * @hide
     */
    // @Deprecated
    CARAPI GetDisplayLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [in] */ const ArrayOf<ICharSequence*>& labelArray,
        /* [out] */ ICharSequence** lb);

    /**
     * @deprecated use {@link #getTypeLabel(Resources, int, CharSequence)} instead.
     * @hide
     */
    // @Deprecated
    CARAPI GetDisplayLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [out] */ ICharSequence** lb);

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

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSPHONE_H__
