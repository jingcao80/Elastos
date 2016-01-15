
#ifndef __ORG_APACHE_HTTP_CONN_SSL_CSTRICTHOSTNAMEVERIFIER_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CSTRICTHOSTNAMEVERIFIER_H_

#include "_Org_Apache_Http_Conn_SSL_CStrictHostnameVerifier.h"
#include "org/apache/http/conn/ssl/AbstractVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * The Strict HostnameVerifier works the same way as Sun Java 1.4, Sun
 * Java 5, Sun Java 6-rc.  It's also pretty close to IE6.  This
 * implementation appears to be compliant with RFC 2818 for dealing with
 * wildcards.
 * <p/>
 * The hostname must match either the first CN, or any of the subject-alts.
 * A wildcard can occur in the CN, and in any of the subject-alts.  The
 * one divergence from IE6 is how we only check the first CN.  IE6 allows
 * a match against any of the CNs present.  We decided to follow in
 * Sun Java 1.4's footsteps and only check the first CN.  (If you need
 * to check all the CN's, feel free to write your own implementation!).
 * <p/>
 * A wildcard such as "*.foo.com" matches only subdomains in the same
 * level, for example "a.foo.com".  It does not match deeper subdomains
 * such as "a.b.foo.com".
 *
 * @author Julius Davies
 */
CarClass(CStrictHostnameVerifier)
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

#endif // __ORG_APACHE_HTTP_CONN_SSL_CSTRICTHOSTNAMEVERIFIER_H_
