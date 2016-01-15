
#include "org/apache/http/message/CBasicHeaderElementIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicHeaderElementIterator)

ECode CBasicHeaderElementIterator::constructor(
    /* [in] */ IHeaderIterator* headerIterator,
    /* [in] */ IHeaderValueParser* parser)
{
    return Init(headerIterator, parser);
}

ECode CBasicHeaderElementIterator::constructor(
    /* [in] */ IHeaderIterator* headerIterator)
{
    return Init(headerIterator);
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
