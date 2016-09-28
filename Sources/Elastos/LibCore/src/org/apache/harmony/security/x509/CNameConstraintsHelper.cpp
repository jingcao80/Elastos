
#include "org/apache/harmony/security/x509/CNameConstraintsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CNameConstraintsHelper)

CAR_INTERFACE_IMPL(CNameConstraintsHelper, Singleton, INameConstraintsHelper)

ECode CNameConstraintsHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ INameConstraints** ppObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraintsHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org