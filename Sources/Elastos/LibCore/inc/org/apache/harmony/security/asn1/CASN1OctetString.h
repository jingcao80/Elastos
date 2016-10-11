
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRING_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1OctetString.h"
#include "ASN1OctetString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1OctetString)
    , public ASN1OctetString
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRING_H__
