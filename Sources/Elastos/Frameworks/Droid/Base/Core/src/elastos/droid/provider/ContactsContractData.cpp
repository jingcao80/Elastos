
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/ContactsContractContacts.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::Uri;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Provider {

ECode ContactsContractData::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("data"), uri);
}

ECode ContactsContractData::GetContactLookupUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* dataUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = IContactsContractStreamItemsColumns::CONTACT_ID;
    (*args)[1] = IContactsContractContactsColumns::LOOKUP_KEY;
    FAIL_RETURN(resolver->Query(dataUri, args, String(NULL), NULL, String(NULL), (ICursor**)&cursor))

    //try {
    Boolean result;
    if (cursor != NULL && (cursor->MoveToFirst(&result), result)) {
        Int64 contactId;
        FAIL_GOTO(cursor->GetInt64(0, &contactId), EXIT)
        String lookupKey;
        FAIL_GOTO(cursor->GetString(1, &lookupKey), EXIT)
        FAIL_GOTO(ContactsContractContacts::GetLookupUri(contactId, lookupKey, uri), EXIT)
        return ICloseable::Probe(cursor)->Close();
    }
    //} finally {
EXIT:
    if (cursor != NULL) {
        FAIL_RETURN(ICloseable::Probe(cursor)->Close())
    }
    //}
    *uri = NULL;
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos