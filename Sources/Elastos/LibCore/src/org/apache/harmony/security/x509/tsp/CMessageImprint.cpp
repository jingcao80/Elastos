
#include "org/apache/harmony/security/x509/tsp/CMessageImprint.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CMessageImprint)

CAR_INTERFACE_IMPL(CMessageImprint, Object, IMessageImprint)

ECode CMessageImprint::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CMessageImprint::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CMessageImprint::constructor(
    /* [in] */ IAlgorithmIdentifier* pAlgId,
    /* [in] */ ArrayOf<Byte>* pHashedMessage)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org