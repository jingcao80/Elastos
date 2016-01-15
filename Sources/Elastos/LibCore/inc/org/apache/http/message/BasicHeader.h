
#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHEADER_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHEADER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Org::Apache::Http::IHeader;

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
class BasicHeader
    : public Object
    , public IHeader
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHeader() {}

    /**
     * Returns the header name.
     *
     * @return String name The name
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the header value.
     *
     * @return String value The current value.
     */
    CARAPI GetValue(
        /* [out] */ String* value);

    /**
     * Returns a {@link String} representation of the header.
     *
     * @return a string
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns an array of {@link HeaderElement}s constructed from my value.
     *
     * @see BasicHeaderValueParser#parseElements(String, HeaderValueParser)
     *
     * @return an array of header elements
     *
     * @throws ParseException   in case of a parse error
     */
    CARAPI GetElements(
        /* [out, callee] */ ArrayOf<IHeaderElement*>** elements);

protected:
    /**
     * Constructor with name and value
     *
     * @param name the header name
     * @param value the header value
     */
    CARAPI Init(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI_(void) CloneImpl(
        /* [in] */ BasicHeader* obj);

private:
    /**
     * Header name.
     */
    String mName;

    /**
     * Header value.
     */
    String mValue;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHEADER_H_
