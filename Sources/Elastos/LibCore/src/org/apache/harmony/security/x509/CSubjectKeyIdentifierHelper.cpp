
#include "org/apache/harmony/security/x509/CSubjectKeyIdentifierHelper.h"
#include "org/apache/harmony/security/x509/CSubjectKeyIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CSubjectKeyIdentifierHelper)
CAR_INTERFACE_IMPL(CSubjectKeyIdentifierHelper, Singleton, ISubjectKeyIdentifierHelper)

ECode CSubjectKeyIdentifierHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ ISubjectKeyIdentifier** result)
{
    VALIDATE_NOT_NULL(result);
    return CSubjectKeyIdentifier::Decode(pEncoding, result);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org