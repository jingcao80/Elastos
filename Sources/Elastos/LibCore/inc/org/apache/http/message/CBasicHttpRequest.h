
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPREQUEST_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPREQUEST_H_

#include "_Org_Apache_Http_Message_CBasicHttpRequest.h"
#include "org/apache/http/message/BasicHttpRequest.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of an HTTP request that can be modified.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 573864 $
 *
 * @since 4.0
 */
CarClass(CBasicHttpRequest) , public BasicHttpRequest
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& method,
        /* [in] */ const String& uri);

    CARAPI constructor(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* ver);

    CARAPI constructor(
        /* [in] */ IRequestLine* requestline);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPREQUEST_H_
