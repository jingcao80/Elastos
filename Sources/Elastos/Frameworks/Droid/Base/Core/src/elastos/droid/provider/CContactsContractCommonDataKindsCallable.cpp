#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsCallable.h"
#include "elastos/droid/provider/ContactsContractData.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsCallable)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsCallable, Singleton
    , IContactsContractCommonDataKindsCallable
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsCallable::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(ContactsContractData::GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, String("callables"), uri);
}

ECode CContactsContractCommonDataKindsCallable::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    return Uri::WithAppendedPath(_uri, String("filter"), uri);
}

} //Provider
} //Droid
} //Elastos