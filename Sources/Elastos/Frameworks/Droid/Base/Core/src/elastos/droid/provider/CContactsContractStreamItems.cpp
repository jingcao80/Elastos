#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractStreamItems.h"
#include "elastos/droid/provider/ContactsContract.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractStreamItems)

CAR_INTERFACE_IMPL_3(CContactsContractStreamItems, Singleton
    , IContactsContractStreamItems
    , IBaseColumns
    , IContactsContractStreamItemsColumns)

ECode CContactsContractStreamItems::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("stream_items"), uri);
}

ECode CContactsContractStreamItems::GetCONTENTPHOTOURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    return Uri::WithAppendedPath(_uri, String("photo"), uri);
}

ECode CContactsContractStreamItems::GetCONTENTLIMITURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("stream_items_limit"), uri);
}

} //Provider
} //Droid
} //Elastos