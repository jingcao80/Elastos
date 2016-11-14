
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_SIGNEDDATA_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_SIGNEDDATA_H__

#include "asn1/ASN1Sequence.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IList;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

class SignedData
    : public Object
    , public ISignedData
{
private:
    class ASN1SequenceDerived
        : public ASN1Sequence
    {
    public:
        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type);

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("SignedData::ASN1SequenceDerived")
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IASN1Sequence>) InitStatic();

    CARAPI GetCertificates(
        /* [out] */ IList** certificate);

    CARAPI GetCRLs(
        /* [out] */ IList** crls);

    CARAPI GetSignerInfos(
        /* [out] */ IList** signerInfo);

    CARAPI GetVersion(
        /* [out] */ Int32* ver);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    SignedData(
        /* [in] */ Int32 version,
        /* [in] */ IList* digestAlgorithms,
        /* [in] */ IContentInfo* contentInfo,
        /* [in] */ IList* certificates,
        /* [in] */ IList* crls,
        /* [in] */ IList* signerInfos);

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;
    AutoPtr<IList> mDigestAlgorithms;
    AutoPtr<IContentInfo> mContentInfo;
    AutoPtr<IList> mCertificates;
    AutoPtr<IList> mCrls;
    AutoPtr<IList> mSignerInfos;
};

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_SIGNEDDATA_H__
