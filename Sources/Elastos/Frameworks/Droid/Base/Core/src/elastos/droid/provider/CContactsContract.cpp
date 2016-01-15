
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CContactsContract.h"
#include "elastos/droid/provider/ContactsContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContract)

CAR_INTERFACE_IMPL(CContactsContract, Singleton, IContactsContract)

ECode CContactsContract::GetAUTHORITY_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContract::AUTHORITY_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContract::IsProfileId(
    /* [in] */ Int64 id,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    return ContactsContract::IsProfileId(id, res);
}

} //Provider
} //Droid
} //Elastos