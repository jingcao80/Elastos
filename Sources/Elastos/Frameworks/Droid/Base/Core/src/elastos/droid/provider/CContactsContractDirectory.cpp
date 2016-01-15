#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractDirectory.h"
#include "elastos/droid/provider/ContactsContract.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractDirectory)

CAR_INTERFACE_IMPL(CContactsContractDirectory, Singleton, IContactsContractDirectory)

ECode CContactsContractDirectory::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("directories"), uri);
}

ECode CContactsContractDirectory::NotifyDirectoryChange(
    /* [in] */ IContentResolver* resolver)
{
    // This is done to trigger a query by Contacts Provider back to the directory provider.
    // No data needs to be sent back, because the provider can infer the calling
    // package from binder.
    AutoPtr<IContentValues> contentValues;
    CContentValues::New((IContentValues**)&contentValues);

    AutoPtr<IUri> auUri;
    GetCONTENT_URI((IUri**)&auUri);
    Int32 result;
    return resolver->Update(auUri, contentValues, String(NULL), NULL, &result);
}

} //Provider
} //Droid
} //Elastos