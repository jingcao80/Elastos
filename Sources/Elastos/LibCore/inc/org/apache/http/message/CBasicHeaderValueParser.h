
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERVALUEPARSER_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERVALUEPARSER_H_

#include "_Org_Apache_Http_Message_CBasicHeaderValueParser.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation for parsing header values into elements.
 * Instances of this class are stateless and thread-safe.
 * Derived classes are expected to maintain these properties.
 *
 * @author <a href="mailto:bcholmes@interlog.com">B.C. Holmes</a>
 * @author <a href="mailto:jericho@thinkfree.com">Park, Sung-Gu</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.com">Oleg Kalnichevski</a>
 * @author and others
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 595670 $
 *
 * @since 4.0
 */
CarClass(CBasicHeaderValueParser) , public BasicHeaderValueParser
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERVALUEPARSER_H_
