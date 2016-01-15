#include "elastos/droid/provider/CContactsContractData.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractData)

CAR_INTERFACE_IMPL_2(CContactsContractData, Singleton
    , IContactsContractData
    , IContactsContractDataColumnsWithJoins)

ECode CContactsContractData::constructor()
{
    return NOERROR;
}

ECode CContactsContractData::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractData::GetCONTENT_URI(uri);
}

ECode CContactsContractData::GetContactLookupUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* dataUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractData::GetContactLookupUri(resolver, dataUri, uri);
}

} //Provider
} //Droid
} //Elastos