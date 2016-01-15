#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContacts.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContacts)

CAR_INTERFACE_IMPL(CContacts, Singleton, IContacts)

const String CContacts::TAG("Contacts");

ECode CContacts::constructor()
{
    return NOERROR;
}

ECode CContacts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://") + IContacts::AUTHORITY, uri);
}

} // Provider
} // Droid
} // Elastos