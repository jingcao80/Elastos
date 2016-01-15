#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSORGANIZATIONS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSORGANIZATIONS_H__

#include "_Elastos_Droid_Provider_CContactsOrganizations.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsOrganizations)
    , public Singleton
    , public IContactsOrganizations
    , public IBaseColumns
    , public IContactsOrganizationColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * no public constructor since this is a utility class
     */
    CARAPI constructor();

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI GetDisplayLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [out] */ ICharSequence** lb);
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSORGANIZATIONS_H__
