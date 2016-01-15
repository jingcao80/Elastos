
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERELEMENT_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERELEMENT_H_

#include "_Org_Apache_Http_Message_CBasicHeaderElement.h"
#include "org/apache/http/message/BasicHeaderElement.h"

using Elastos::Core::IArrayOf;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::INameValuePair;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * One element of an HTTP header's value.
 * <p>
 * Some HTTP headers (such as the set-cookie header) have values that
 * can be decomposed into multiple elements.  Such headers must be in the
 * following form:
 * </p>
 * <pre>
 * header  = [ element ] *( "," [ element ] )
 * element = name [ "=" [ value ] ] *( ";" [ param ] )
 * param   = name [ "=" [ value ] ]
 *
 * name    = token
 * value   = ( token | quoted-string )
 *
 * token         = 1*&lt;any char except "=", ",", ";", &lt;"&gt; and
 *                       white space&gt;
 * quoted-string = &lt;"&gt; *( text | quoted-char ) &lt;"&gt;
 * text          = any char except &lt;"&gt;
 * quoted-char   = "\" char
 * </pre>
 * <p>
 * Any amount of white space is allowed between any part of the
 * header, element or param and is ignored. A missing value in any
 * element or param will be stored as the empty {@link String};
 * if the "=" is also missing <var>null</var> will be stored instead.
 * </p>
 * <p>
 * This class represents an individual header element, containing
 * both a name/value pair (value may be <tt>null</tt>) and optionally
 * a set of additional parameters.
 * </p>
 *
 * @author <a href="mailto:bcholmes@interlog.com">B.C. Holmes</a>
 * @author <a href="mailto:jericho@thinkfree.com">Park, Sung-Gu</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.com">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $ $Date: 2007-12-16 06:11:11 -0800 (Sun, 16 Dec 2007) $
 *
 * @since 4.0
 */
CarClass(CBasicHeaderElement) , public BasicHeaderElement
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERELEMENT_H_
