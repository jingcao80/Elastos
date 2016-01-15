
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICLINEFORMATTER_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICLINEFORMATTER_H_

#include "_Org_Apache_Http_Message_CBasicLineFormatter.h"
#include "org/apache/http/message/BasicLineFormatter.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Interface for formatting elements of the HEAD section of an HTTP message.
 * This is the complement to {@link LineParser}.
 * There are individual methods for formatting a request line, a
 * status line, or a header line. The formatting does <i>not</i> include the
 * trailing line break sequence CR-LF.
 * The formatted lines are returned in memory, the formatter does not depend
 * on any specific IO mechanism.
 * Instances of this interface are expected to be stateless and thread-safe.
 *
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author and others
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 574185 $
 *
 * @since 4.0
 */
CarClass(CBasicLineFormatter) , public BasicLineFormatter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICLINEFORMATTER_H_
