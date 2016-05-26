
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1UTCTIME_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1UTCTIME_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1UTCTime.h"
#include <ASN1Time.h>
#include <Asn1TypeMacro.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1UTCTime), public ASN1Time
{
public:
    CAR_OBJECT_DECL()

    ASN1TYPE_METHODS_DECL()

    static AutoPtr<IASN1UTCTime> InitStatic();

    static CARAPI GetInstance(
        /* [out] */ IASN1UTCTime** instance);

    CARAPI constructor();

private:
/** default implementation */
    static AutoPtr<IASN1UTCTime> ASN1;// = new ASN1UTCTime();
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1UTCTIME_H__
