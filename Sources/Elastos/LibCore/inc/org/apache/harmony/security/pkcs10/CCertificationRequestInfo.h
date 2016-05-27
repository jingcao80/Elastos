
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFO_H__

#include "_Org_Apache_Harmony_Security_Pkcs10_CCertificationRequestInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Org::Apache::Harmony::Security::X501::IName;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

CarClass(CCertificationRequestInfo)
    , public Object
    , public ICertificationRequestInfo
{
public:
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

        CARAPI SetOptional(
            /* [in] */ ArrayOf<Boolean>* optional);

        CARAPI GetOptional(
            /* [out, callee] */ ArrayOf<Boolean>** optional);

        CARAPI SetDefault(
            /* [in] */ ArrayOf<IInterface *>* def);

        CARAPI GetDefault(
            /* [out, callee] */ ArrayOf<IInterface *>** def);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        ASN1SequenceDerived(
            /* [in] */ ArrayOf<IASN1Type *>* type);
    };

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static AutoPtr<IASN1Sequence> InitStatic();

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

private:
    CCertificationRequestInfo(
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
