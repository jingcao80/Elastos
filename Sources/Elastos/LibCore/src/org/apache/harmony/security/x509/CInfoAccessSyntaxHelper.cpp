
#include "org/apache/harmony/security/x509/CInfoAccessSyntaxHelper.h"
#include "org/apache/harmony/security/x509/CInfoAccessSyntax.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CInfoAccessSyntaxHelper)

CAR_INTERFACE_IMPL(CInfoAccessSyntaxHelper, Singleton, IInfoAccessSyntaxHelper)

ECode CInfoAccessSyntaxHelper::Decode(
    /* [in] */ ArrayOf<Byte>* pEncoding,
    /* [out] */ IInfoAccessSyntax** ppSyntax)
{
    VALIDATE_NOT_NULL(ppSyntax);

    return CInfoAccessSyntax::Decode(pEncoding, ppSyntax);
}

ECode CInfoAccessSyntaxHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CInfoAccessSyntax::GetASN1(ppAsn1);
}

ECode CInfoAccessSyntaxHelper::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    return CInfoAccessSyntax::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org