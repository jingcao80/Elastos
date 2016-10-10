
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFO_H__

#include "_Org_Apache_Harmony_Security_Pkcs10_CCertificationRequestInfo.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ASN1Sequence.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::X501::IName;
using Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfo;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

CarClass(CCertificationRequestInfo)
    , public Object
    , public ICertificationRequestInfo
{
private:
    class ASN1SequenceDerived
        : public ASN1Sequence
    {
    public:
        ASN1SequenceDerived();

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCertificationRequestInfo();

    static CARAPI_(AutoPtr<IASN1Sequence>) InitStatic();

    CARAPI GetSubject(
        /* [out] */ IName** name);

    CARAPI GetVersion(
        /* [out] */ Int32* ver);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetAttributes(
        /* [out] */ IList** attri);

    CARAPI GetSubjectPublicKeyInfo(
        /* [out] */ ISubjectPublicKeyInfo** info);

    CARAPI constructor(
        /*[in] */ Int32 version,
        /*[in] */ IName* subject,
        /*[in] */ ISubjectPublicKeyInfo* subjectPublicKeyInfo,
        /*[in] */ IList* attributes,
        /*[in] */ ArrayOf<Byte>* encoding);

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    Int32 mVersion;

    /** the value of subject field of the structure */
    AutoPtr<IName> mSubject;

    /** the value of subjectPublicKeyInfo field of the structure */
    AutoPtr<ISubjectPublicKeyInfo> mSubjectPublicKeyInfo;

    /** the value of attributes field of the structure */
    AutoPtr<IList> mAttributes;

    /** the ASN.1 encoded form of CertificationRequestInfo */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFO_H__
