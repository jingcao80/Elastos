
#include "CASN1OidHelper.h"
#include "ASN1Oid.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CASN1OidHelper::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    return ASN1Oid::GetInstance(instance);
}

ECode CASN1OidHelper::GetInstanceForString(
    /* [out] */ IASN1Type** instance)
{
    return ASN1Oid::GetInstanceForString(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

