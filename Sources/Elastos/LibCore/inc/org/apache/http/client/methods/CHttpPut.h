
#ifndef __ORG_APACHE_HTTP_CLIENT_METHODS_CHTTPPUT_H_
#define __ORG_APACHE_HTTP_CLIENT_METHODS_CHTTPPUT_H_

#include "_Org_Apache_Http_Client_Methods_CHttpPut.h"
#include "org/apache/http/client/methods/HttpEntityEnclosingRequestBase.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

/**
 * HTTP PUT method.
 * <p>
 * The HTTP PUT method is defined in section 9.6 of
 * <a href="http://www.ietf.org/rfc/rfc2616.txt">RFC2616</a>:
 * <blockquote>
 * The PUT method requests that the enclosed entity be stored under the
 * supplied Request-URI. If the Request-URI refers to an already
 * existing resource, the enclosed entity SHOULD be considered as a
 * modified version of the one residing on the origin server.
 * </blockquote>
 * </p>
 *
 * @version $Revision: 664505 $
 *
 * @since 4.0
 */
CarClass(CHttpPut)
    , public HttpEntityEnclosingRequestBase
    , public IHttpPut
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

#endif // __ORG_APACHE_HTTP_CLIENT_METHODS_CHTTPPUT_H_
