#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractAggregationExceptions.h"
#include "elastos/droid/provider/ContactsContract.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractAggregationExceptions)

CAR_INTERFACE_IMPL_2(CContactsContractAggregationExceptions, Singleton
    , IContactsContractAggregationExceptions
    , IBaseColumns)

ECode CContactsContractAggregationExceptions::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("aggregation_exceptions"), uri);
}

} //Provider
} //Droid
} //Elastos