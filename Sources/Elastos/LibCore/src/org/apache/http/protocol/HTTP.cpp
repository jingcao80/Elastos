
#include "org/apache/http/protocol/HTTP.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(HTTP, Object, IHTTP)

Boolean HTTP::IsWhitespace(
    /* [in] */ Char32 ch)
{
    return (Int32)ch == SP || (Int32)ch == HT || (Int32)ch == CR || (Int32)ch == LF;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
