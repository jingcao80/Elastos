#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractDataUsageFeedback.h"
#include "elastos/droid/provider/ContactsContractContacts.h"
#include "elastos/droid/provider/ContactsContractData.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractDataUsageFeedback)

CAR_INTERFACE_IMPL(CContactsContractDataUsageFeedback, Singleton, IContactsContractDataUsageFeedback)

ECode CContactsContractDataUsageFeedback::GetFEEDBACKURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> conUri;
    FAIL_RETURN(ContactsContractData::GetCONTENT_URI((IUri**)&conUri))
    return Uri::WithAppendedPath(conUri, String("usagefeedback"), uri);
}

ECode CContactsContractDataUsageFeedback::GetDELETEUSAGEURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("delete_usage"), uri);
}

} //Provider
} //Droid
} //Elastos