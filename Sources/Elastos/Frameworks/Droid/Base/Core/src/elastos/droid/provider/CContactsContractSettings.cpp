#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/CContactsContractSettings.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractSettings)

CAR_INTERFACE_IMPL_2(CContactsContractSettings, Singleton
    , IContactsContractSettings
    , IContactsContractSettingsColumns)

ECode CContactsContractSettings::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("settings"), uri);
}

} //Provider
} //Droid
} //Elastos