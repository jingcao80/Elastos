
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHEADER_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHEADER_H_

#include "_Org_Apache_Http_Message_CBasicHeader.h"
#include "org/apache/http/message/BasicHeader.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Represents an HTTP header field.
 *
 * <p>The HTTP header fields follow the same generic format as
 * that given in Section 3.1 of RFC 822. Each header field consists
 * of a name followed by a colon (":") and the field value. Field names
 * are case-insensitive. The field value MAY be preceded by any amount
 * of LWS, though a single SP is preferred.
 *
 *<pre>
 *     message-header = field-name ":" [ field-value ]
 *     field-name     = token
 *     field-value    = *( field-content | LWS )
 *     field-content  = &lt;the OCTETs making up the field-value
 *                      and consisting of either *TEXT or combinations
 *                      of token, separators, and quoted-string&gt;
 *</pre>
 *
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 652956 $ $Date: 2008-05-02 17:13:05 -0700 (Fri, 02 May 2008) $
 *
 * @since 4.0
 */
CarClass(CBasicHeader) , public BasicHeader
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHEADER_H_
