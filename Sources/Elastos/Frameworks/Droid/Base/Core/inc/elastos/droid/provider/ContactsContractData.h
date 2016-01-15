
#ifndef __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTDATA_H__
#define __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTDATA_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

class ContactsContractData
{
public:
    /**
     * The content:// style URI for this table, which requests a directory
     * of data rows matching the selection criteria.
     */
    static CARAPI GetCONTENT_URI(
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
    static CARAPI GetContactLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* dataUri,
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTDATA_H__
