
#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICLISTHEADERITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICLISTHEADERITERATOR_H_

#include "_Org_Apache_Http_Message_CBasicListHeaderIterator.h"
#include "org/apache/http/message/BasicListHeaderIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Implementation of a {@link HeaderIterator} based on a {@link List}.
 * For use by {@link HeaderGroup}.
 *
 * @version $Revision: 584542 $
 */
CarClass(CBasicListHeaderIterator) , public BasicListHeaderIterator
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IList* headers,
        /* [in] */ const String& value);
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICLISTHEADERITERATOR_H_
