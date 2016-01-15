
#include "org/apache/http/conn/ssl/CAllowAllHostnameVerifier.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

CAR_OBJECT_IMPL(CAllowAllHostnameVerifier)

ECode CAllowAllHostnameVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<String>* cns,
    /* [in] */ ArrayOf<String>* subjectAlts)
{
    // Allow everything - so never blowup.
    return NOERROR;
}

ECode CAllowAllHostnameVerifier::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ALLOW_ALL");
    return NOERROR;
}

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org