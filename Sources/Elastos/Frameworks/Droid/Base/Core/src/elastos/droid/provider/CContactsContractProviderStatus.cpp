#include "elastos/droid/provider/CContactsContractProviderStatus.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractProviderStatus)

CAR_INTERFACE_IMPL(CContactsContractProviderStatus, Singleton, IContactsContractProviderStatus)

ECode CContactsContractProviderStatus::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("provider_status"), uri);
}

} //Provider
} //Droid
} //Elastos