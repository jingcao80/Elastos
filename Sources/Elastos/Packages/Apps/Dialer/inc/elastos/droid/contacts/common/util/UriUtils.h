
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_URIUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_URIUTILS_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Utility methods for dealing with URIs.
 */
class UriUtils
{
public:
    /** Checks whether two URI are equal, taking care of the case where either is null. */
    static CARAPI_(Boolean) AreEqual(
        /* [in] */ IUri* uri1,
        /* [in] */ IUri* uri2);

    /** Parses a string into a URI and returns null if the given string is null. */
    static CARAPI_(AutoPtr<IUri>) ParseUriOrNull(
        /* [in] */ const String& uriString);

    /** Converts a URI into a string, returns null if the given URI is null. */
    static CARAPI_(String) UriToString(
        /* [in] */ IUri* uri);

    static CARAPI_(Boolean) IsEncodedContactUri(
        /* [in] */ IUri* uri);

private:
    /** Static helper, not instantiable. */
    UriUtils() {}
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_URIUTILS_H__
