
#include "org/apache/harmony/security/x509/CEDIPartyNameHelper.h"
#include "org/apache/harmony/security/x509/CEDIPartyName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CEDIPartyNameHelper)

CAR_INTERFACE_IMPL(CEDIPartyNameHelper, Singleton, IEDIPartyNameHelper)

ECode CEDIPartyNameHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CEDIPartyName::GetASN1(ppAsn1);
}

ECode CEDIPartyNameHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return CEDIPartyName::SetASN1(pAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org