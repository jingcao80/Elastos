
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNEDDATA_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNEDDATA_H__

#include "_Org_Apache_Harmony_Security_Pkcs7_CSignedData.h"

using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CarClass(CSignedData)
    , public Object
    , public ISignedData
{
private:
    friend class ASN1SequenceDerived;
    class ASN1SequenceDerived
        : public ASN1Sequence
        , public IASN1Sequence
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()
        ASN1TYPE_METHODS_DECL()
        CARAPI SetType(
        /* [in] */ ArrayOf<IASN1Type *>* type);

        CARAPI GetType(
            /* [out, callee] */ ArrayOf<IASN1Type *>** type);

        CARAPI SetOPTIONAL(
            /* [in] */ ArrayOf<Boolean>* optional);

        CARAPI GetOPTIONAL(
            /* [out, callee] */ ArrayOf<Boolean>** optional);

        CARAPI SetDEFAULT(
            /* [in] */ ArrayOf<IInterface *>* def);

        CARAPI GetDEFAULT(
            /* [out, callee] */ ArrayOf<IInterface *>** def);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);
        ASN1SequenceDerived(
            /* [in] */ ArrayOf<IASN1Type *>* type);
    };
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IASN1Sequence>) InitStatic();

    CSignedData();

    CSignedData(
        /* [in] */ Int32 version,
        /* [in] */ IList* digestAlgorithms,
        /* [in] */ IContentInfo* contentInfo,
        /* [in] */ IList* certificates,
        /* [in] */ IList* crls,
        /* [in] */ IList* signerInfos);

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

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS7_CSIGNEDDATA_H__
