#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractPhoneLookup.h"
#include "elastos/droid/provider/ContactsContract.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractPhoneLookup)

CAR_INTERFACE_IMPL_5(CContactsContractPhoneLookup, Singleton
    , IContactsContractPhoneLookup
    , IBaseColumns
    , IContactsContractPhoneLookupColumns
    , IContactsContractContactsColumns
    , IContactsContractContactOptionsColumns)

ECode CContactsContractPhoneLookup::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("phone_lookup"), uri);
}

} //Provider
} //Droid
} //Elastos