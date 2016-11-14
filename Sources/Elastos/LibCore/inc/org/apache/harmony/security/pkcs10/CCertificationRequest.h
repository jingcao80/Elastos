
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUEST_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUEST_H__

#include "_Org_Apache_Harmony_Security_Pkcs10_CCertificationRequest.h"
#include "ASN1Sequence.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

CarClass(CCertificationRequest)
    , public Object
    , public ICertificationRequest
{
private:
    class ASN1SequenceDerived
        : public ASN1Sequence
    {
    public:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type *>* type);

        ASN1SequenceDerived();

        TO_STRING_IMPL("CCertificationRequest::ASN1SequenceDerived")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCertificationRequest();

    CARAPI constructor(
        /* [in] */ ICertificationRequestInfo* info,
        /* [in] */ IAlgorithmIdentifier* algId,
        /* [in] */ ArrayOf<Byte>* signature,
        /* [in] */ ArrayOf<Byte>* encoding);

    CARAPI constructor(
        /* [in] */ ICertificationRequestInfo* info,
        /* [in] */ IAlgorithmIdentifier* algId,
        /* [in] */ ArrayOf<Byte>* signature);

    CARAPI GetInfo(
        /* [out] */ ICertificationRequestInfo** info);

    CARAPI GetSignature(
        /* [out, callee] */ ArrayOf<Byte>** signature);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encoded);

    CARAPI GetAlgorithmIdentifier(
        /* [out] */ IAlgorithmIdentifier** algId);

private:
    static AutoPtr<IASN1Sequence> InitStatic();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
        /** the value of certificationRequestInfo field of the structure */
    AutoPtr<ICertificationRequestInfo> mInfo;

    /** the value of signatureAlgorithm field of the structure */
    AutoPtr<IAlgorithmIdentifier> mAlgId;

    /** the value of signature field of the structure */
    AutoPtr<ArrayOf<Byte> > mSignature;

    /** the ASN.1 encoded form of CertificationRequest */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUEST_H__
