
#include "elastos/droid/contacts/common/util/UriUtils.h"
#include "elastos/droid/contacts/common/util/Constants.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

Boolean UriUtils::AreEqual(
    /* [in] */ IUri* uri1,
    /* [in] */ IUri* uri2)
{
    if (uri1 == NULL && uri2 == NULL) {
        return TRUE;
    }
    if (uri1 == NULL || uri2 == NULL) {
        return FALSE;
    }
    Boolean equals;
    IObject::Probe(uri1)->Equals(uri2, &equals);
    return equals;
}

AutoPtr<IUri> UriUtils::ParseUriOrNull(
    /* [in] */ const String& uriString)
{
    if (uriString.IsNull()) {
        return NULL;
    }
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(uriString, (IUri**)&uri);
    return uri;
}

String UriUtils::UriToString(
    /* [in] */ IUri* uri)
{
    String str;
    return uri == NULL ? String(NULL) : (IObject::Probe(uri)->ToString(&str), str);
}

Boolean UriUtils::IsEncodedContactUri(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        return FALSE;
    }
    String lastPathSegment;
    uri->GetLastPathSegment(&lastPathSegment);
    if (lastPathSegment.IsNull()) {
        return FALSE;
    }
    return lastPathSegment.Equals(Constants::LOOKUP_URI_ENCODED);
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
