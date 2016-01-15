
#include "elastos/droid/net/CParseException.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Thrown when parsing a URL fails.
*/
// See non-public class {@link WebAddress}.
CParseException::ParseException(
    /* [in] */ const String& in_response)
{
    this->response = in_response;
}


} //namespace Net
} //namespace Droid
} //namespace Elastos