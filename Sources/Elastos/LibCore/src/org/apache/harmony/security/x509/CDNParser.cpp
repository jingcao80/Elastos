
#include "org/apache/harmony/security/x509/CDNParser.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CDNParser)

CAR_INTERFACE_IMPL(CDNParser, Object, IDNParser)

ECode CDNParser::Parse(
    /* [out] */ IList** ppList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDNParser::constructor(
    /* [in] */ const String& dn)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org