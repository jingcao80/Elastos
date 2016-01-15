
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERITERATOR_H_

#include "_Org_Apache_Http_Message_CBasicHeaderIterator.h"
#include "org/apache/http/message/BasicHeaderIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link HeaderIterator}.
 *
 * @version $Revision: 581981 $
 */
CarClass(CBasicHeaderIterator) , public BasicHeaderIterator
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<IHeader*>* headers,
        /* [in] */ const String& name);
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHEADERITERATOR_H_
