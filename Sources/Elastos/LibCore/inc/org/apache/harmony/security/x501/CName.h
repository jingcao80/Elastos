
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CNAME_H__

#include "_Org_Apache_Harmony_Security_X501_CName.h"
#include "ASN1SequenceOf.h"

using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CName)
    , public Object
    , public IName
{
private:
    class ASN1SequenceOfDerived
        : public ASN1SequenceOf
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [out] */ ICollection** values);

        CARAPI constructor(
            /* [in] */ IASN1SetOf* set);

        ASN1SequenceOfDerived();
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetX500Principal(
        /* [out] */ Elastosx::Security::Auth::X500::IX500Principal ** ppX500Principal);

    CARAPI GetName(
        /* [in] */ const String& format,
        /* [out] */ String * pName);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pEncoding);

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ Elastos::Utility::IList * pRdn);

public:
    /**
     * According to RFC 3280 (http://www.ietf.org/rfc/rfc3280.txt)
     * X.501 Name structure is defined as follows:
     *
     * Name ::= CHOICE {
     *     RDNSequence }
     *
     * RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
     *
     * RelativeDistinguishedName ::=
     *     SET OF AttributeTypeAndValue
     *
     */
    static AutoPtr<IASN1SetOf> ASN1_RDN;

    static AutoPtr<IASN1SequenceOf> ASN1;

private:
    static CARAPI_(AutoPtr<IASN1SetOf>) InitASN1_RDN();

    static CARAPI_(AutoPtr<IASN1SequenceOf>) InitASN1();
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CNAME_H__
