
#include "org/apache/http/message/CBasicListHeaderIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicListHeaderIterator)

ECode CBasicListHeaderIterator::constructor(
    /* [in] */ IList* headers,
    /* [in] */ const String& name)
{
    return Init(headers, name);
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
