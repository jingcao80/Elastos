
#include "org/apache/http/message/CBasicHeaderIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicHeaderIterator)

ECode CBasicHeaderIterator::constructor(
    /* [in] */ ArrayOf<IHeader*>* headers,
    /* [in] */ const String& name)
{
    return Init(headers, name);
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
