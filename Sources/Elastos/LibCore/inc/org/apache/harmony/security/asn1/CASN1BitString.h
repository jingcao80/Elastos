
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1BITSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1BITSTRING_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1BitString.h"
#include "ASN1BitString.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1BitString), public ASN1BitString
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1BITSTRING_H__
