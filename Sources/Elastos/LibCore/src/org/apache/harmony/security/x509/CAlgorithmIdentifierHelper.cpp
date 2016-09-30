
#include "org/apache/harmony/security/x509/CAlgorithmIdentifierHelper.h"
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CAlgorithmIdentifierHelper)

CAR_INTERFACE_IMPL(CAlgorithmIdentifierHelper, Singleton, IAlgorithmIdentifierHelper)

ECode CAlgorithmIdentifierHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return AlgorithmIdentifier::GetASN1(ppAsn1);
}

ECode CAlgorithmIdentifierHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return AlgorithmIdentifier::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org