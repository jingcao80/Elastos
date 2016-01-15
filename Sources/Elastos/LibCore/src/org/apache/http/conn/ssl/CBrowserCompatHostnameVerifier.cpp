
#include "org/apache/http/conn/ssl/CBrowserCompatHostnameVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

CAR_OBJECT_IMPL(CBrowserCompatHostnameVerifier)

ECode CBrowserCompatHostnameVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<String>* cns,
    /* [in] */ ArrayOf<String>* subjectAlts)
{
    return Verify(host, cns, subjectAlts, FALSE);
}

ECode CBrowserCompatHostnameVerifier::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("BROWSER_COMPATIBLE");
    return NOERROR;
}

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org