#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsExtensions.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsExtensions)

CAR_INTERFACE_IMPL_3(CContactsExtensions, Singleton, IContactsExtensions, IBaseColumns, IContactsExtensionsColumns)

ECode CContactsExtensions::constructor()
{
    return NOERROR;
}

ECode CContactsExtensions::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/extensions"), uri);
}

} // Provider
} // Droid
} // Elastos