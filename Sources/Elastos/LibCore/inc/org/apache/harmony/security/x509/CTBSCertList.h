
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTLIST_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTLIST_H__

#include "_Org_Apache_Harmony_Security_X509_CTBSCertList.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ASN1Sequence.h"

using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X501::IName;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CTBSCertList)
    , public Object
    , public ITBSCertList
{
public:
    class RevokedCertificate
        : public Object
        , public IRevokedCertificate
    {
    private:
        class ASN1SequenceWrapper2: public ASN1Sequence
        {
        public:
            ASN1SequenceWrapper2();

            CARAPI constructor(
                /* [in] */ ArrayOf<IASN1Type*>* types);

        protected:
            CARAPI GetDecodedObject(
                /* [in] */ IBerInputStream* in,
                /* [out] */ IInterface** result);

            CARAPI GetValues(
                /* [in] */ IInterface* object,
                /* [in] */ ArrayOf<IInterface*>* values);
        };

    public:
        CAR_INTERFACE_DECL()

        RevokedCertificate(
            /* [in] */ IBigInteger* userCertificate,
            /* [in] */ IDate* revocationDate,
            /* [in] */ IExtensions* crlEntryExtensions);

        CARAPI_(AutoPtr<IExtensions>) GetCrlEntryExtensions();

        CARAPI_(AutoPtr<IBigInteger>) GetUserCertificate();

        CARAPI_(AutoPtr<IDate>) GetRevocationDate();

        /**
         * Returns the value of Certificate Issuer Extension, if it is
         * presented.
         */
        CARAPI_(AutoPtr<IX500Principal>) GetIssuer();

        CARAPI_(AutoPtr<ArrayOf<Byte> >) GetEncoded();

        CARAPI Equals(
            /* [in] */ IInterface* rc,
            /* [out] */ Boolean* result);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI_(void) DumpValue(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ const String& prefix);

    private:
        static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

    public:
        static AutoPtr<IASN1Sequence> ASN1;

    private:
        AutoPtr<IBigInteger> mUserCertificate;
        AutoPtr<IDate> mRevocationDate;
        AutoPtr<IExtensions> mCrlEntryExtensions;

        Boolean mIssuerRetrieved;
        AutoPtr<IX500Principal> mIssuer;
        AutoPtr<ArrayOf<Byte> > mEncoding;
    };

    /**
     * X.509 TBSCertList encoder/decoder.
     */
private:
    class ASN1SequenceWrapper: public ASN1Sequence
    {
    public:
        ASN1SequenceWrapper();

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type*>* types);

    protected:
        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CTBSCertList();

    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ IAlgorithmIdentifier* signature,
        /* [in] */ IName* issuer,
        /* [in] */ IDate* thisUpdate,
        /* [in] */ IDate* nextUpdate,
        /* [in] */ IList* revokedCertificates,
        /* [in] */ IExtensions* crlExtensions,
        /* [in] */ ArrayOf<Byte>* encoding);

    CARAPI GetVersion(
        /* [out] */ Int32* pVer);

    CARAPI GetSignature(
        /* [out] */ IAlgorithmIdentifier** ppSignature);

    CARAPI GetIssuer(
        /* [out] */ IName** ppIssuer);

    CARAPI GetThisUpdate(
        /* [out] */ IDate** ppDate);

    CARAPI GetNextUpdate(
        /* [out] */ IDate** ppDate);

    CARAPI GetRevokedCertificates(
        /* [out] */ IList** ppCertificates);

    CARAPI GetCrlExtensions(
        /* [out] */ IExtensions** ppExtensions);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI Equals(
        /* [in] */ IInterface* pRc,
        /* [out] */ Boolean* pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32* pHashCode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of version field of the structure */
    Int32 mVersion;
    /** the value of signature field of the structure */
    AutoPtr<IAlgorithmIdentifier> mSignature;
    /** the value of issuer field of the structure */
    AutoPtr<IName> mIssuer;
    /** the value of thisUpdate of the structure */
    AutoPtr<IDate> mThisUpdate;
    /** the value of nextUpdate of the structure */
    AutoPtr<IDate> mNextUpdate;
    /** the value of revokedCertificates of the structure */
    AutoPtr<IList> mRevokedCertificates;
    /** the value of crlExtensions field of the structure */
    AutoPtr<IExtensions> mCrlExtensions;
    /** the ASN.1 encoded form of TBSCertList */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CTBSCERTLIST_H__
