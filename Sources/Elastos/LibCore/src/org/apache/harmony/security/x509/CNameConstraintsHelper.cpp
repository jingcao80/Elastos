
#include "org/apache/harmony/security/x509/CNameConstraintsHelper.h"
#include "org/apache/harmony/security/x509/CNameConstraints.h"

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
    VALIDATE_NOT_NULL(ppObject);

    return CNameConstraints::Decode(pEncoding, ppObject);
}

ECode CNameConstraintsHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CNameConstraints::GetASN1(ppAsn1);
}

ECode CNameConstraintsHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CNameConstraints::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org