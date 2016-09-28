
#include "org/apache/harmony/security/x509/CAuthorityKeyIdentifierHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CAuthorityKeyIdentifierHelper)

CAR_INTERFACE_IMPL(CAuthorityKeyIdentifierHelper, Singleton, IAuthorityKeyIdentifierHelper)

ECode CAuthorityKeyIdentifierHelper::Decode(
    /* [in] */ ArrayOf<Byte> * pEncoding,
    /* [out] */ IAuthorityKeyIdentifier** ppKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifierHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifierHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org