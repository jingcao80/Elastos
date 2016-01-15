#include "elastos/droid/provider/CContactsContractProfile.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/ContactsContractProfile.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractProfile)

CAR_INTERFACE_IMPL_6(CContactsContractProfile, Singleton
    , IContactsContractProfile
    , IBaseColumns
    , IContactsContractContactsColumns
    , IContactsContractContactOptionsColumns
    , IContactsContractContactNameColumns
    , IContactsContractContactStatusColumns)

ECode CContactsContractProfile::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractProfile::GetCONTENT_URI(uri);
}

ECode CContactsContractProfile::GetCONTENT_VCARD_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractProfile::GetCONTENT_VCARD_URI(uri);

}

ECode CContactsContractProfile::GetCONTENT_RAW_CONTACTS_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractProfile::GetCONTENTRAWCONTACTSURI(uri);
}

ECode CContactsContractProfile::GetMINID(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    return ContactsContractProfile::GetMINID(id);
}

} //Provider
} //Droid
} //Elastos