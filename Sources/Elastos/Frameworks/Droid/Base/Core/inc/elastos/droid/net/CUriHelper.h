
#ifndef __ELASTOS_DROID_NET_CURIHELPER_H__
#define __ELASTOS_DROID_NET_CURIHELPER_H__

#include "_Elastos_Droid_Net_CUriHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Immutable URI reference. A URI reference includes a URI and a fragment, the
 * component of the URI following a '#'. Builds and parses URI references
 * which conform to
 * <a href="http://www.faqs.org/rfcs/rfc2396.html">RFC 2396</a>.
 *
 * <p>In the interest of performance, this class performs little to no
 * validation. Behavior is undefined for invalid input. This class is very
 * forgiving--in the face of invalid input, it will return garbage
 * rather than throw an exception unless otherwise specified.
 */
CarClass(CUriHelper)
    , public Singleton
    , public IUriHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Parse(
        /* [in] */ const String& uriString,
        /* [out] */ IUri** result);

    CARAPI FromFile(
        /* [in] */ IFile* file,
        /* [out] */ IUri** result);

    CARAPI FromParts(
        /* [in] */ const String& scheme,
        /* [in] */ const String& ssp,
        /* [in] */ const String& fragment,
        /* [out] */ IUri** result);

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [in] */ const String& allow,
        /* [out] */ String* result);

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI WithAppendedPath(
        /* [in] */ IUri* baseUri,
        /* [in] */ const String& pathSegment,
        /* [out] */ IUri** result);

    CARAPI GetEMPTY(
            /* [out] */ IUri** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_CURIHELPER_H__
