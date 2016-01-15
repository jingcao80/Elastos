#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContractProfile.h"
#include "elastos/droid/provider/ContactsContract.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Net::Uri;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Provider {

ECode ContactsContractProfile::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("profile"), uri);
}

ECode ContactsContractProfile::GetCONTENT_VCARD_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    return Uri::WithAppendedPath(_uri, String("as_vcard"), uri);

}

ECode ContactsContractProfile::GetCONTENTRAWCONTACTSURI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    return Uri::WithAppendedPath(_uri, String("raw_contacts"), uri);
}

ECode ContactsContractProfile::GetMINID(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    *id = Elastos::Core::Math::INT64_MAX_VALUE - (Int64)Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos