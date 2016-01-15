
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CONTENTTYPE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CONTENTTYPE_H__

#include <elastos.h>

_ELASTOS_NAMESPACE_USING

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

/**
 * This class incapsulates the constants determining the
 * types of SSL/TLS record's content data.
 * Constant values are taken according to the TLS v1 specification
 * (http://www.ietf.org/rfc/rfc2246.txt).
 */

class ContentType {
    /**
     * Identifies change cipher spec message
     */
public:
    static const Byte CHANGE_CIPHER_SPEC;

    /**
     * Identifies alert message
     */
    static const Byte ALERT;

    /**
     * Identifies handshake message
     */
    static const Byte HANDSHAKE;

    /**
     * Identifies application data message
     */
    static const Byte APPLICATION_DATA;
};

} // namespace Jsse
} // namespace Provider
} // namespace Xnet
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CONTENTTYPE_H__
