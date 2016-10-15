
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TIME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TIME_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "ASN1Choice.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Choice;
using Org::Apache::Harmony::Security::Asn1::IASN1Choice;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

/**
 * Class represents the work with the following X.509 structure:
 * (as specified in RFC 3280 -
 *  Internet X.509 Public Key Infrastructure.
 *  Certificate and Certificate Revocation List (CRL) Profile.
 *  http://www.ietf.org/rfc/rfc3280.txt):
 *
 * <pre>
 * Time ::= CHOICE {
 *       utcTime        UTCTime,
 *       generalTime    GeneralizedTime
 * }
 * </pre>
 */
class Time
{
private:
    class ASN1ChoiceWrapper: public ASN1Choice
    {
    public:
        ASN1ChoiceWrapper();

        CARAPI GetIndex(
            /* [in] */ IInterface* object,
            /* [out] */ Int32* result);

        CARAPI GetObjectToEncode(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);
    };

private:
    static CARAPI_(AutoPtr<IASN1Choice>) InitASN1();

public:
    static AutoPtr<IASN1Choice> ASN1;

private:
    static const Int64 JAN_01_2050;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TIME_H__
