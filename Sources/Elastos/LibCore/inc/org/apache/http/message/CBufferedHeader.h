
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBUFFEREDHEADER_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBUFFEREDHEADER_H_

#include "_Org_Apache_Http_Message_CBufferedHeader.h"
#include "org/apache/http/message/BufferedHeader.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * This class represents a raw HTTP header whose content is parsed 'on demand'
 * only when the header value needs to be consumed.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $ $Date: 2007-12-16 06:11:11 -0800 (Sun, 16 Dec 2007) $
 */
CarClass(CBufferedHeader) , public BufferedHeader
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ ICharArrayBuffer* buffer);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBUFFEREDHEADER_H_
