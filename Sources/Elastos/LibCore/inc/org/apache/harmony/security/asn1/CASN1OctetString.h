
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRING_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1OctetString.h"
#include "ASN1StringType.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1OctetString), public ASN1StringType
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    static CARAPI GetInstance(
        /* [out] */ IASN1OctetString** instance);

    CARAPI Decode(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI GetDecodedObject(
        /* [in] */ IBerInputStream* bis,
        /* [out] */ IInterface** object);

    CARAPI EncodeContent(
        /* [in] */ IBerOutputStream* bos);

    CARAPI SetEncodingContent(
        /* [in] */ IBerOutputStream* bos);

private:
    static CARAPI_(AutoPtr<IASN1OctetString>) InitASN1();

private:
    /** default implementation */
    static const AutoPtr<IASN1OctetString> ASN1;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1OCTETSTRING_H__
