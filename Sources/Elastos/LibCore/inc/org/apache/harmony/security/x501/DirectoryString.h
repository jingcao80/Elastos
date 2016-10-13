
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_DIRECTORYSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_DIRECTORYSTRING_H__

#include "ASN1Choice.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Choice;
using Org::Apache::Harmony::Security::Asn1::IASN1Choice;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

/**
 * The class encapsulates the ASN.1 DER encoding/decoding work
 * with the DirectoryString structure
 * (as specified in RFC 3280 -
 *  Internet X.509 Public Key Infrastructure.
 *  Certificate and Certificate Revocation List (CRL) Profile.
 *  http://www.ietf.org/rfc/rfc3280.txt):
 *
 * <pre>
 *   DirectoryString ::= CHOICE {
 *        teletexString             TeletexString   (SIZE (1..MAX)),
 *        printableString           PrintableString (SIZE (1..MAX)),
 *        universalString           UniversalString (SIZE (1..MAX)),
 *        utf8String              UTF8String      (SIZE (1..MAX)),
 *        bmpString               BMPString       (SIZE (1..MAX))
 *   }
 * </pre>
 */
class DirectoryString
{
private:
    class ASN1ChoiceCls: public ASN1Choice
    {
    public:
        ASN1ChoiceCls();

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type*>* asn1Type);

        CARAPI GetIndex(
            /* [in] */ IInterface* object,
            /* [out] */ Int32* index);

        CARAPI GetObjectToEncode(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);
    };

private:
    static CARAPI_(AutoPtr<IASN1Choice>) Init();

public:
    static AutoPtr<IASN1Choice> ASN1;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_DIRECTORYSTRING_H__
