
#include "org/apache/harmony/security/x509/CSubjectKeyIdentifierHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CSubjectKeyIdentifierHelper)

CAR_INTERFACE_IMPL(CSubjectKeyIdentifierHelper, Singleton, ISubjectKeyIdentifierHelper)

ECode CSubjectKeyIdentifierHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ ISubjectKeyIdentifier** ppIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org