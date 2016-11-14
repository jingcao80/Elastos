
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__

#include "asn1/ASN1SetOf.h"

using Org::Apache::Harmony::Security::Asn1::ASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

class AuthenticatedAttributes : public Object
{
private:
    class ASN1SetOfDerived : public ASN1SetOf
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);
    };

public:
    static AutoPtr<ASN1SetOf> InitStatic();

    CARAPI GetAttributes(
        /* [out] */ IList** attributes);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    TO_STRING_IMPL("AuthenticatedAttributes")

private:
    AuthenticatedAttributes(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [in] */ IList* authenticatedAttributes);

public:
    static AutoPtr<ASN1SetOf> ASN1;

private:
    AutoPtr<ArrayOf<Byte> > mEncoding;
    AutoPtr<IList> mAuthenticatedAttributes;
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__
