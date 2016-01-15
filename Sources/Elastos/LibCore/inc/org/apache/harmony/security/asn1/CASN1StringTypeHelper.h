
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1STRINGTYPEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1STRINGTYPEHELPER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1StringTypeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1StringTypeHelper)
{
public:
    CARAPI GetBMPSTRING(
        /* [out] */ IASN1Type** BMPSTRING);

    CARAPI GetIA5STRING(
        /* [out] */ IASN1Type** IA5STRING);

    CARAPI GetGENERALSTRING(
        /* [out] */ IASN1Type** GENERALSTRING);

    CARAPI GetPRINTABLESTRING(
        /* [out] */ IASN1Type** PRINTABLESTRING);

    CARAPI GetTELETEXSTRING(
        /* [out] */ IASN1Type** TELETEXSTRING);

    CARAPI GetUNIVERSALSTRING(
        /* [out] */ IASN1Type** UNIVERSALSTRING);

    CARAPI GetUTF8STRING(
        /* [out] */ IASN1Type** UTF8STRING);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1STRINGTYPEHELPER_H__
