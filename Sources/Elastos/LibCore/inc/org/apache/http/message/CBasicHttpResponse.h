
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPRESPONSE_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPRESPONSE_H_

#include "_Org_Apache_Http_Message_CBasicHttpResponse.h"
#include "org/apache/http/message/BasicHttpResponse.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of an HTTP message that can be modified.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 620287 $
 *
 * @since 4.0
 */
CarClass(CBasicHttpResponse) , public BasicHttpResponse
{
public:
    CAR_OBJECT_DECL()

    /**
     * Creates a new response.
     * This is the constructor to which all others map.
     *
     * @param statusline        the status line
     * @param catalog           the reason phrase catalog, or
     *                          <code>null</code> to disable automatic
     *                          reason phrase lookup
     * @param locale            the locale for looking up reason phrases, or
     *                          <code>null</code> for the system locale
     */
    CARAPI constructor(
        /* [in] */ IStatusLine* statusline,
        /* [in] */ IReasonPhraseCatalog* catalog,
        /* [in] */ ILocale* locale);

    /**
     * Creates a response from a status line.
     * The response will not have a reason phrase catalog and
     * use the system default locale.
     *
     * @param statusline        the status line
     */
    CARAPI constructor(
        /* [in] */ IStatusLine* statusline);

    /**
     * Creates a response from elements of a status line.
     * The response will not have a reason phrase catalog and
     * use the system default locale.
     *
     * @param ver       the protocol version of the response
     * @param code      the status code of the response
     * @param reason    the reason phrase to the status code, or
     *                  <code>null</code>
     */
    CARAPI constructor(
        /* [in] */ IProtocolVersion* ver,
        /* [in] */ Int32 code,
        /* [in] */ const String& reason);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPRESPONSE_H_
