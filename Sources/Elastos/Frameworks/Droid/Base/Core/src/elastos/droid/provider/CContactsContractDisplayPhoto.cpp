#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/CContactsContractDisplayPhoto.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractDisplayPhoto)

CAR_INTERFACE_IMPL(CContactsContractDisplayPhoto, Singleton, IContactsContractDisplayPhoto)

ECode CContactsContractDisplayPhoto::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("display_photo"), uri);
}

ECode CContactsContractDisplayPhoto::GetCONTENTMAXDIMENSIONSURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("photo_dimensions"), uri);
}

} //Provider
} //Droid
} //Elastos