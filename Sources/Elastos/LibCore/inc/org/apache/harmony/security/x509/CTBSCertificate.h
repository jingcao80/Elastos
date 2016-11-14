
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__

#include "_Org_Apache_Harmony_Security_X509_CTBSCertificate.h"
#include "ASN1Sequence.h"

using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X501::IName;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CTBSCertificate)
    , public Object
    , public ITBSCertificate
{
private:
    class ASN1SequenceWrapper: public ASN1Sequence
    {
    public:
        ASN1SequenceWrapper();

        CARAPI constructor(
            /* [in] */ /* [in] */ ArrayOf<IASN1Type*>* types);

    protected:
        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("CTBSCertificate::ASN1SequenceWrapper")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CTBSCertificate();

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** ppSerialNumber);

    CARAPI GetSignature(
        /* [out] */ IAlgorithmIdentifier** ppSignature);

    CARAPI GetIssuer(
        /* [out] */ IName** ppIssuer);

    CARAPI GetValidity(
        /* [out] */ IValidity** ppValidity);

    CARAPI GetSubject(
        /* [out] */ IName** ppSubject);

    CARAPI GetSubjectPublicKeyInfo(
        /* [out] */ ISubjectPublicKeyInfo** ppInfo);

    CARAPI GetIssuerUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** ppUniqueId);

    CARAPI GetSubjectUniqueID(
        /* [out, callee] */ ArrayOf<Boolean>** ppUniqueId);

    CARAPI GetExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ IBigInteger* pSerialNumber,
        /* [in] */ IAlgorithmIdentifier* pSignature,
        /* [in] */ IName* pIssuer,
        /* [in] */ IValidity* pValidity,
        /* [in] */ IName* pSubject,
        /* [in] */ ISubjectPublicKeyInfo* pSubjectPublicKeyInfo,
        /* [in] */ ArrayOf<Boolean>* pIssuerUniqueID,
        /* [in] */ ArrayOf<Boolean>* pSubjectUniqueID,
        /* [in] */ IExtensions* pExtensions);

    CARAPI constructor(
        /* [in] */ Int32 ver,
        /* [in] */ IBigInteger* pSerialNumber,
        /* [in] */ IAlgorithmIdentifier* pSignature,
        /* [in] */ IName* pIssuer,
        /* [in] */ IValidity* pValidity,
        /* [in] */ IName* pSubject,
        /* [in] */ ISubjectPublicKeyInfo* pSubjectPublicKeyInfo,
        /* [in] */ ArrayOf<Boolean>* pIssuerUniqueID,
        /* [in] */ ArrayOf<Boolean>* pSubjectUniqueID,
        /* [in] */ IExtensions* pExtensions,
        /* [in] */ ArrayOf<Byte>* encoding);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of version field of the structure */
    Int32 mVersion;
    /** the value of serialNumber field of the structure */
    AutoPtr<IBigInteger> mSerialNumber;
    /** the value of signature field of the structure */
    AutoPtr<IAlgorithmIdentifier> mSignature;
    /** the value of issuer field of the structure */
    AutoPtr<IName> mIssuer;
    /** the value of validity field of the structure */
    AutoPtr<IValidity> mValidity;
    /** the value of subject field of the structure */
    AutoPtr<IName> mSubject;
    /** the value of subjectPublicKeyInfo field of the structure */
    AutoPtr<ISubjectPublicKeyInfo> mSubjectPublicKeyInfo;
    /** the value of issuerUniqueID field of the structure */
    AutoPtr<ArrayOf<Boolean> > mIssuerUniqueID;
    /** the value of subjectUniqueID field of the structure */
    AutoPtr<ArrayOf<Boolean> > mSubjectUniqueID;
    /** the value of extensions field of the structure */
    AutoPtr<IExtensions> mExtensions;
    /** the ASN.1 encoded form of TBSCertificate */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTIFICATE_H__
