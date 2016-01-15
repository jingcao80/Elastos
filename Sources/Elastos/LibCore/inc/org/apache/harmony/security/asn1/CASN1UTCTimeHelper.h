
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1UTCTIMEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1UTCTIMEHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1UTCTimeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1UTCTimeHelper)
{
public:
    CARAPI GetInstance(
        /* [out] */ IASN1UTCTime** instance);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1UTCTIMEHELPER_H__
