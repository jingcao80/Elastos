
#include "org/apache/http/conn/ssl/CStrictHostnameVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

CAR_OBJECT_IMPL(CStrictHostnameVerifier)

ECode CStrictHostnameVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<String>* cns,
    /* [in] */ ArrayOf<String>* subjectAlts)
{
    return Verify(host, cns, subjectAlts, TRUE);
}

ECode CStrictHostnameVerifier::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("STRICT");
    return NOERROR;
}

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org