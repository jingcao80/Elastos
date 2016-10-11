
#include "CASN1OctetStringHelper.h"
#include "ASN1OctetString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_INTERFACE_IMPL(CASN1OctetStringHelper, Singleton, IASN1OctetStringHelper)

CAR_SINGLETON_IMPL(CASN1OctetStringHelper)

ECode CASN1OctetStringHelper::GetInstance(
    /* [out] */ IASN1OctetString** instance)
{
    return ASN1OctetString::GetInstance(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
