
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERELEMENTITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERELEMENTITERATOR_H_

#include "_Org_Apache_Http_Message_CBasicHeaderElementIterator.h"
#include "org/apache/http/message/BasicHeaderElementIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link HeaderElementIterator}.
 *
 * @version $Revision: 592088 $
 *
 * @author Andrea Selva <selva.andre at gmail.com>
 * @author Oleg Kalnichevski <oleg at ural.ru>
 */
CarClass(CBasicHeaderElementIterator) , public BasicHeaderElementIterator
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHeaderIterator* headerIterator,
        /* [in] */ IHeaderValueParser* parser);

    CARAPI constructor(
        /* [in] */ IHeaderIterator* headerIterator);
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERELEMENTITERATOR_H_
