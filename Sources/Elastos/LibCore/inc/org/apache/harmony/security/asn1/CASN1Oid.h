
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OID_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OID_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Oid.h"
#include <ASN1Oid.h>
#include <Asn1TypeMacro.h>

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Oid), public ASN1Oid
{
public:
    CAR_OBJECT_DECL()

    ASN1TYPE_METHODS_DECL()

    CARAPI constructor();
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OID_H__
