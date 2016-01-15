
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICSTATUSLINE_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICSTATUSLINE_H_

#include "_Org_Apache_Http_Message_CBasicStatusLine.h"
#include "org/apache/http/message/BasicStatusLine.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Represents a status line as returned from a HTTP server.
 * See <a href="http://www.ietf.org/rfc/rfc2616.txt">RFC2616</a> section 6.1.
 * This class is immutable and therefore inherently thread safe.
 *
 * @see HttpStatus
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 * @version $Id: BasicStatusLine.java 604625 2007-12-16 14:11:11Z olegk $
 *
 * @since 4.0
 */
CarClass(CBasicStatusLine) , public BasicStatusLine
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ IProtocolVersion* version,
        /* [in] */ Int32 statusCode,
        /* [in] */ const String& reasonPhrase);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICSTATUSLINE_H_
