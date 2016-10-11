
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATELIST_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATELIST_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificateList.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificateList)
    , public Object
    , public ICertificateList
{
private:
    class MyASN1Sequence
        : public ASN1Sequence
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [in] */ ArrayOf<IInterface*>* values);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetTbsCertList(
        /* [out] */ ITBSCertList** ppTbsCertList);

    CARAPI GetSignatureValue(
        /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI constructor(
        /* [in] */ ITBSCertList* pTbsCertList,
        /* [in] */ IAlgorithmIdentifier* pSignatureAlgorithm,
        /* [in] */ ArrayOf<Byte>* pSignatureValue);

    CARAPI constructor(
        /* [in] */ ITBSCertList* pTbsCertList,
        /* [in] */ IAlgorithmIdentifier* pSignatureAlgorithm,
        /* [in] */ ArrayOf<Byte>* pSignatureValue,
        /* [in] */ ArrayOf<Byte>* encoding);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    /**
     * X.509 CertList encoder/decoder.
     */
    static AutoPtr<IASN1Sequence> ASN1;

private:
    /** the value of tbsCertList field of the structure */
    AutoPtr<ITBSCertList> mTbsCertList;

    /** the value of signatureAlgorithm field of the structure */
    AutoPtr<IAlgorithmIdentifier> mSignatureAlgorithm;

    /** the value of signatureValue field of the structure */
    AutoPtr<ArrayOf<Byte> > mSignatureValue;

    /** the ASN.1 encoded form of CertList */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATELIST_H__
