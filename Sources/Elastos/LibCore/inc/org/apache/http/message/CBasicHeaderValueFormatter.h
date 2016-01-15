
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERVALUEFORMATTER_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERVALUEFORMATTER_H_

#include "_Org_Apache_Http_Message_CBasicHeaderValueFormatter.h"
#include "org/apache/http/message/BasicHeaderValueFormatter.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link TokenIterator}.
 * This implementation parses <tt>#token<tt> sequences as
 * defined by RFC 2616, section 2.
 * It extends that definition somewhat beyond US-ASCII.
 *
 * @version $Revision: 602520 $
 */
CarClass(CBasicHeaderValueFormatter) , public BasicHeaderValueFormatter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERVALUEFORMATTER_H_
