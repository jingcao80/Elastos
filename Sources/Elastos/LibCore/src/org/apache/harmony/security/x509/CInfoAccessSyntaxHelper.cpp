
#include "org/apache/harmony/security/x509/CInfoAccessSyntaxHelper.h"

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
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInfoAccessSyntaxHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org