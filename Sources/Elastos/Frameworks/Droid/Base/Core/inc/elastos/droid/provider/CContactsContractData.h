#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDATA_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDATA_H__

#include "_Elastos_Droid_Provider_CContactsContractData.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractData)
    , public Singleton
    , public IContactsContractData
    , public IContactsContractDataColumnsWithJoins
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * This utility class cannot be instantiated
     */
    CARAPI constructor();

    /**
     * The content:// style URI for this table, which requests a directory
     * of data rows matching the selection criteria.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * <p>
     * Build a {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}
     * style {@link Uri} for the parent {@link android.provider.ContactsContract.Contacts}
     * entry of the given {@link ContactsContract.Data} entry.
     * </p>
     * <p>
     * Returns the Uri for the contact in the first entry returned by
     * {@link ContentResolver#query(Uri, String[], String, String[], String)}
     * for the provided {@code dataUri}.  If the query returns null or empty
     * results, silently returns null.
     * </p>
     */
    CARAPI GetContactLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* dataUri,
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDATA_H__
