
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CBEROUTPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CBEROUTPUTSTREAM_H__

#include "_Org_Apache_Harmony_Security_Asn1_CBerOutputStream.h"
#include "BerOutputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CBerOutputStream), public BerOutputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CBEROUTPUTSTREAM_H__
