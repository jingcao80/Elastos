
#include "org/apache/harmony/security/x509/CAuthorityKeyIdentifierHelper.h"
#include "org/apache/harmony/security/x509/CAuthorityKeyIdentifier.h"

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
    VALIDATE_NOT_NULL(ppKeyIdentifier);

    return CAuthorityKeyIdentifier::Decode(pEncoding, ppKeyIdentifier);
}

ECode CAuthorityKeyIdentifierHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CAuthorityKeyIdentifier::GetASN1(ppAsn1);
}

ECode CAuthorityKeyIdentifierHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CAuthorityKeyIdentifier::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org