
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_AUTHENTICATEDATTRIBUTES_H__

#include <cmdef.h>

using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

class AuthenticatedAttributes
 : public ElLightRefBase
 , public IInterface
{
private:
    class ASN1SetOfDerived
        : public ASN1SetOf
        , public IASN1SetOf
        , public ElLightRefBase {
    public:
        CAR_INTERFACE_DECL()

        ASN1TYPE_METHODS_DECL()

        CARAPI SetType(
            /* [in] */ IASN1Type* type);

        CARAPI GetType(
            /* [out] */ IASN1Type** type);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [out] */ ICollection** values);

        ASN1SetOfDerived(
            /* [in] */ IASN1Type* type);
    };
public:
    static AutoPtr<IASN1SetOf> InitStatic();

    CAR_INTERFACE_DECL()

    CARAPI GetAttributes(
        /* [out] */ IList** attributes);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

private:
    AuthenticatedAttributes(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [in] */ IList* authenticatedAttributes);

public:
    static AutoPtr<IASN1SetOf> ASN1;

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
