
#include "UriUtils.h"
#include <elstos.droid.net.Uri.h>

using Elastos::Droid::Net::Uri;

namespace Elastos{
namespace Apps{
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
    return IObject::Probe(uri1)->Equals(uri2);
}

AutoPtr<IUri> UriUtils::ParseUriOrNull(
    /* [in] */ const String& uriString)
{
    if (uriString.IsNUll()) {
        return NULL;
    }
    AutoPtr<IUri> uri;
    Uri::Parse(uriString, &uri);
    return uri;
}

String UriUtils::UriToString(
    /* [in] */ IUri* uri)
{
    String str;
    return uri == NULL ? String(NULL) : uri->ToString(&str), str;
}

Boolean UriUtils::IsEncodedContactUri(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        return FALSE;
    }
    String lastPathSegment;
    uri->GetLastPathSegment(&lastPathSegment);
    if (lastPathSegment.IsNUll()) {
        return FALSE;
    }
    return lastPathSegment.Equals(IConstants::LOOKUP_URI_ENCODED);
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
