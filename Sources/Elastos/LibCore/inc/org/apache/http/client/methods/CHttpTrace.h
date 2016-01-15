
#ifndef __ORG_APACHE_HTTP_CLIENT_METHODS_CHTTPTRACE_H_
#define __ORG_APACHE_HTTP_CLIENT_METHODS_CHTTPTRACE_H_

#include "_Org_Apache_Http_Client_Methods_CHttpTrace.h"
#include "org/apache/http/client/methods/HttpRequestBase.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

/**
 * HTTP TRACE method.
 * <p>
 * The HTTP TRACE method is defined in section 9.6 of
 * <a href="http://www.ietf.org/rfc/rfc2616.txt">RFC2616</a>:
 * <blockquote>
 *  The TRACE method is used to invoke a remote, application-layer loop-
 *  back of the request message. The final recipient of the request
 *  SHOULD reflect the message received back to the client as the
 *  entity-body of a 200 (OK) response. The final recipient is either the
 *  origin server or the first proxy or gateway to receive a Max-Forwards
 *  value of zero (0) in the request (see section 14.31). A TRACE request
 *  MUST NOT include an entity.
 * </blockquote>
 * </p>
 *
 * @version $Revision: 664505 $
 *
 * @since 4.0
 */
CarClass(CHttpTrace)
    , public HttpRequestBase
    , public IHttpTrace
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetMethod(
        /* [out] */ String* method);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IURI* uri);

    CARAPI constructor(
        /* [in] */ const String& uri);
};

} // Methods
} // Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_METHODS_CHTTPTRACE_H_
