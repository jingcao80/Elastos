
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICLINEPARSER_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICLINEPARSER_H_

#include "_Org_Apache_Http_Message_CBasicLineParser.h"
#include "org/apache/http/message/BasicLineParser.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic parser for lines in the head section of an HTTP message.
 * There are individual methods for parsing a request line, a
 * status line, or a header line.
 * The lines to parse are passed in memory, the parser does not depend
 * on any specific IO mechanism.
 * Instances of this class are stateless and thread-safe.
 * Derived classes MUST maintain these properties.
 *
 * <p>
 * Note: This class was created by refactoring parsing code located in
 * various other classes. The author tags from those other classes have
 * been replicated here, although the association with the parsing code
 * taken from there has not been traced.
 * </p>
 *
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 * @author and others
 */
CarClass(CBasicLineParser) , public BasicLineParser
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICLINEPARSER_H_
