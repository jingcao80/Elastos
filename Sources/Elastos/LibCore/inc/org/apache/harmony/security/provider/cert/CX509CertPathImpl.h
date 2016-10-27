
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTPATHIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTPATHIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CX509CertPathImpl.h"

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CX509CertPathImpl), public CertPath
{
private:
    class ASN1SequenceOfDerived
        : public ASN1SequenceOf
        , public IASN1SequenceOf
        , public ElRefBase
    {
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

        ASN1SequenceOfDerived(
            /* [in] */ IASN1Type* type);
    };

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

    private:
        // precalculated ASN.1 encodings for
        // version ,digestAlgorithms, content info field of SignedData
        AutoPtr<ArrayOf<Byte> > PRECALCULATED_HEAD;

        // precalculated empty set of SignerInfos
        AutoPtr<ArrayOf<Byte> > SIGNERS_INFO;
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

    private:
        // precalculated ASN.1 encoding for SignedData object oid
        AutoPtr<ArrayOf<Byte> > SIGNED_DATA_OID;
    };
public:
    CAR_OBJECT_DECL()

    static AutoPtr<ArrayOf<String> > InitStatic();

    static CARAPI GetInstance(
        /* [in] */ IInputStream* is,
        /* [out] */ IX509CertPathImpl** instance);

    static CARAPI GetInstance(
        /* [in] */ IInputStream* is,
        /* [in] */ const String& encoding,
        /* [out] */ IX509CertPathImpl** instance);

    static CARAPI GetInstance(
        /* [in] */ ArrayOf<Byte>* inp,
        /* [out] */ IX509CertPathImpl** instance);

    static CARAPI GetInstance(
        /* [in] */ ArrayOf<Byte>* inp,
        /* [in] */ const String& encoding,
        /* [out] */ IX509CertPathImpl** instance);

    CARAPI GetType(
        /* [out] */ String* type);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetCertificates(
        /* [out] */ IList** certs);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI GetEncodedEx(
        /* [in] */ const String& encoding,
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI GetEncodings(
        /* [out] */ IIterator** encodings);

    CARAPI constructor(
        /* [in] */ IList* certs);

private:
    CX509CertPathImpl(
        /* [in] */ IList* certs,
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<Byte>* encoding);

public:
    // supported encoding types:
    static Int32 PKI_PATH = 0;
    static Int32 PKCS7 = 1;
    static AutoPtr<IList> sEncodings;// = Collections.unmodifiableList(Arrays.asList(encodingsArr));

    static AutoPtr<IASN1SequenceOf> ASN1;

private:
    static Int64 sSerialVersionUID = 7989755106209515436L;

    // supported encoding names
    static AutoPtr<ArrayOf<String> > sEncodingsArr;// = new String[] {"PkiPath", "PKCS7"};

    static AutoPtr<IASN1Sequence> ASN1_SIGNED_DATA;

    static AutoPtr<IASN1Sequence> PKCS7_SIGNED_DATA_OBJECT;


    // the list of certificates representing this certification path
    AutoPtr<IList> mCertificates;
    // PkiPath encoding of the certification path
    AutoPtr<ArrayOf<Byte> > mPkiPathEncoding;
    // PKCS7 encoding of the certification path
    AutoPtr<ArrayOf<Byte> > mPkcs7Encoding;
};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTPATHIMPL_H__
