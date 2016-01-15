
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICREQUESTLINE_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICREQUESTLINE_H_

#include "_Org_Apache_Http_Message_CBasicRequestLine.h"
#include "org/apache/http/message/BasicRequestLine.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * The first line of an {@link org.apache.http.HttpRequest HttpRequest}.
 * It contains the method, URI, and HTTP version of the request.
 * For details, see RFC 2616.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $
 *
 * @since 4.0
 */
CarClass(CBasicRequestLine) , public BasicRequestLine
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* version);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICREQUESTLINE_H_
