
#include "CSignerInfoHelper.h"
#include "CSignerInfo.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

ECode CSignerInfoHelper::GetIssuerAndSerialNumber(
    /* [out] */ IASN1Sequence** issuerAndSerialNumber)
{
    VALIDATE_NOT_NULL(issuerAndSerialNumber)
    *issuerAndSerialNumber = CSignerInfo::ISSUER_AND_SERIAL_NUMBER;
    REFCOUNT_ADD(*issuerAndSerialNumber)
    return NOERROR;
}

ECode CSignerInfoHelper::GetAsn1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = CSignerInfo::ASN1;
    REFCOUNT_ADD(*asn1)
    return NOERROR;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

