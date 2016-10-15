
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTPUBLICKEYINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTPUBLICKEYINFO_H__

#include "_Org_Apache_Harmony_Security_X509_CSubjectPublicKeyInfo.h"
#include "Elastos.CoreLibrary.Security.h"
#include "ASN1Sequence.h"

using Elastos::Core::Object;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::IKeySpec;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CSubjectPublicKeyInfo)
    , public Object
    , public ISubjectPublicKeyInfo
{
private:
    class ASN1SequenceWrapper: public ASN1Sequence
    {
    public:
        ASN1SequenceWrapper();

        // @Override protected
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result);

        // @Override protected
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSubjectPublicKeyInfo();

    CARAPI GetAlgorithmIdentifier(
        /* [out] */ IAlgorithmIdentifier** ppAlgorithmIdentifier);

    CARAPI GetSubjectPublicKey(
        /* [out, callee] */ ArrayOf<Byte>** ppPublicKey);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** ppPublicKey);

    CARAPI constructor(
        /* [in] */ IAlgorithmIdentifier* pAlgID,
        /* [in] */ ArrayOf<Byte>* pSubjectPublicKey);

    CARAPI constructor(
        /* [in] */ IAlgorithmIdentifier* pAlgID,
        /* [in] */ ArrayOf<Byte>* pSubjectPublicKey,
        /* [in] */ Int32 unused);

    CARAPI constructor(
        /* [in] */ IAlgorithmIdentifier* pAlgID,
        /* [in] */ ArrayOf<Byte>* pSubjectPublicKey,
        /* [in] */ Int32 unused,
        /* [in] */ ArrayOf<Byte>* encoding);

private:
    /**
     * Try to generate a PublicKey for a given {@code keySpec} and
     * {@code algorithm} identifier. If there a problem generating the key like
     * a missing {@code KeyFactory} or invalid {@code KeySpec}, it will return
     * {@code null}.
     */
    static CARAPI_(AutoPtr<IPublicKey>) GenerateKeyForAlgorithm(
        /* [in] */ IKeySpec* keySpec,
        /* [in] */ const String& algorithm);

    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of algorithmID field of the structure */
    AutoPtr<IAlgorithmIdentifier> mAlgorithmID;
    /** the value of subjectPublicKey field of the structure */
    AutoPtr<ArrayOf<Byte> > mSubjectPublicKey;
    /** the public key corresponding to this SubjectPublicKeyInfo */
    AutoPtr<IPublicKey> mPublicKey;
    /** the value of unusedBits field of the structure */
    Int32 mUnusedBits;
    /** the ASN.1 encoded form of SubjectPublicKeyInfo */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTPUBLICKEYINFO_H__
