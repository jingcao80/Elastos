#include "elastos/droid/provider/CContactsContractDeletedContacts.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractDeletedContacts)

CAR_INTERFACE_IMPL(CContactsContractDeletedContacts, Singleton, IContactsContractDeletedContacts)

const Int32 CContactsContractDeletedContacts::DAYS_KEPT = 30;

ECode CContactsContractDeletedContacts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(),
        String("deleted_contacts"), uri);
}

ECode CContactsContractDeletedContacts::GetDAYS_KEPT_MILLISECONDS(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 1000L * 60L * 60L * 24L * (Int64)DAYS_KEPT;
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos