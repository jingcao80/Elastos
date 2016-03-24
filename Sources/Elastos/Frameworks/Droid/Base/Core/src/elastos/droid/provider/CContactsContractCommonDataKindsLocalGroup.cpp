
//#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsLocalGroup.h"
#include "elastos/droid/R.h"
//#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Net::Uri;
//using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsLocalGroup)

CAR_INTERFACE_IMPL_2(CContactsContractCommonDataKindsLocalGroup, Singleton
    , IContactsContractCommonDataKindsLocalGroup
    , IContactsContractDataColumnsWithJoins)

ECode CContactsContractCommonDataKindsLocalGroup::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(ContactsContractData::GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, String("local-groups"), uri);
}

ECode CContactsContractCommonDataKindsLocalGroup::GetCONTENT_LOOKUP_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, String("lookup"), uri);
}

ECode CContactsContractCommonDataKindsLocalGroup::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, String("filter"), uri);
}

} //Provider
} //Droid
} //Elastos
