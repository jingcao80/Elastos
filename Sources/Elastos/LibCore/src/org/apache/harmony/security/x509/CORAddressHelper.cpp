
#include "org/apache/harmony/security/x509/CORAddressHelper.h"
#include "org/apache/harmony/security/x509/CORAddress.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CORAddressHelper)

CAR_INTERFACE_IMPL(CORAddressHelper, Singleton, IORAddressHelper)

ECode CORAddressHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return CORAddress::GetASN1(ppAsn1);
}

ECode CORAddressHelper::SetASN1(
    /* [in] */ IASN1Sequence* ppAsn1)
{
    return CORAddress::SetASN1(ppAsn1);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org