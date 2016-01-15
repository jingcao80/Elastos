
#ifndef __ORG_APACHE_HTTP_CONN_SSL_CBROWSERCOMPATHOSTNAMEVERIFIER_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CBROWSERCOMPATHOSTNAMEVERIFIER_H_

#include "_Org_Apache_Http_Conn_SSL_CBrowserCompatHostnameVerifier.h"
#include "org/apache/http/conn/ssl/AbstractVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * The HostnameVerifier that works the same way as Curl and Firefox.
 * <p/>
 * The hostname must match either the first CN, or any of the subject-alts.
 * A wildcard can occur in the CN, and in any of the subject-alts.
 * <p/>
 * The only difference between BROWSER_COMPATIBLE and STRICT is that a wildcard
 * (such as "*.foo.com") with BROWSER_COMPATIBLE matches all subdomains,
 * including "a.b.foo.com".
 *
 * @author Julius Davies
 */
CarClass(CBrowserCompatHostnameVerifier)
    , public AbstractVerifier
{
public:
    CAR_OBJECT_DECL()

    using AbstractVerifier::Verify;

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<String>* cns,
        /* [in] */ ArrayOf<String>* subjectAlts);

    CARAPI ToString(
        /* [out] */ String* str);

};

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SSL_CBROWSERCOMPATHOSTNAMEVERIFIER_H_
