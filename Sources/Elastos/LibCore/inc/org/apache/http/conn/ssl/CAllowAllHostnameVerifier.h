
#ifndef __ORG_APACHE_HTTP_CONN_SSL_CALLOWALLHOSTNAMEVERIFIER_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CALLOWALLHOSTNAMEVERIFIER_H_

#include "_Org_Apache_Http_Conn_SSL_CAllowAllHostnameVerifier.h"
#include "org/apache/http/conn/ssl/AbstractVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * The ALLOW_ALL HostnameVerifier essentially turns hostname verification
 * off. This implementation is a no-op, and never throws the SSLException.
 *
 * @author Julius Davies
 */
CarClass(CAllowAllHostnameVerifier)
    , public AbstractVerifier
{
public:
    CAR_OBJECT_DECL()

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

#endif // __ORG_APACHE_HTTP_CONN_SSL_CALLOWALLHOSTNAMEVERIFIER_H_
