
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1NAMEDBITLIST_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1NAMEDBITLIST_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1NamedBitList.h"
#include "ASN1NamedBitList.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1NamedBitList), public ASN1NamedBitList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1NAMEDBITLIST_H__
