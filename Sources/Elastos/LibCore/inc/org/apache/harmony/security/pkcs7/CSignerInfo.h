
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNERINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNERINFO_H__

#include "_Org_Apache_Harmony_Security_Pkcs7_CSignerInfo.h"
#include <ASN1SequenceMacro.h>

using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CarClass(CSignerInfo)
{
private:
    friend class ASN1SequenceDerived1;
    friend class ASN1SequenceDerived2;
    class ASN1SequenceDerived1
        : public ASN1Sequence
        , public IASN1Sequence
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        ASN1SEQUENCE_METHODS_DECL()

        ASN1SequenceDerived1(
            /* [in] */ ArrayOf<IASN1Type *>* type);
    };

    class ASN1SequenceDerived2
        : public ASN1Sequence
        , public IASN1Sequence
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        ASN1SEQUENCE_METHODS_DECL()

        ASN1SequenceDerived2(
            /* [in] */ ArrayOf<IASN1Type *>* type);
    };

public:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitStatic();

    CSignerInfo();

    CARAPI GetIssuer(
        /* [out] */ IX500Principal** issuer);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** serialNumber);

    CARAPI GetDigestAlgorithm(
        /* [out] */ String* algorithm);

    CARAPI GetDigestEncryptionAlgorithm(
        /* [out] */ String* digestEncryptionAlgorithm);

    CARAPI GetAuthenticatedAttributes(
        /* [out] */ IList** authenticatedAttributes);

    CARAPI GetEncodedAuthenticatedAttributes(
        /* [out, callee] */ ArrayOf<Byte>** encodedAuthenticatedAttributes);

    CARAPI GetEncryptedDigest(
        /* [out, callee] */ ArrayOf<Byte>** encryptedDigest);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CSignerInfo(
        /* [in] */ Int32 version,
        /* [in] */ ArrayOf<IInterface*>* issuerAndSerialNumber,
        /* [in] */ IAlgorithmIdentifier* digestAlgorithm,
        /* [in] */ IAuthenticatedAttributes* authenticatedAttributes,
        /* [in] */ IAlgorithmIdentifier* digestEncryptionAlgorithm,
        /* [in] */ ArrayOf<Byte>* encryptedDigest,
        /* [in] */ IList* unauthenticatedAttributes);

public:
    static AutoPtr<IASN1Sequence> ISSUER_AND_SERIAL_NUMBER;
    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;
    AutoPtr<IX500Principal> mIssuer;
    AutoPtr<IBigInteger> mSerialNumber;
    AutoPtr<IAlgorithmIdentifier> mDigestAlgorithm;
    AutoPtr<IAuthenticatedAttributes> mAuthenticatedAttributes;
    AutoPtr<IAlgorithmIdentifier> mDigestEncryptionAlgorithm;
    AutoPtr<ArrayOf<Byte> > mEncryptedDigest;
    AutoPtr<IList> mUnauthenticatedAttributes;
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNERINFO_H__
