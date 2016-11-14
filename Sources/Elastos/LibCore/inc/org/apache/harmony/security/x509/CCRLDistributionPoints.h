
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCRLDISTRIBUTIONPOINTS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCRLDISTRIBUTIONPOINTS_H__

#include "_Org_Apache_Harmony_Security_X509_CCRLDistributionPoints.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "org/apache/harmony/security/x509/ExtensionValue.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCRLDistributionPoints)
    , public ExtensionValue
    , public ICRLDistributionPoints
{
private:
    class MyASN1SequenceOf
        : public ASN1SequenceOf
    {
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [out] */ ICollection** coll);

        TO_STRING_IMPL("CCRLDistributionPoints::MyASN1SequenceOf")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* pEncoding,
        /* [out] */ ICRLDistributionPoints** ppCrlDistributionPoints);

    static CARAPI GetASN1(
        /* [out] */ IASN1Type** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Type* pAsn1);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* distributionPoints,
        /* [in] */ ArrayOf<Byte>* encoding);

private:
    static CARAPI_(AutoPtr<IASN1Type>) initASN1();

public:
    /**
     * Custom X.509 decoder.
     */
    static AutoPtr<IASN1Type> ASN1;

private:
    AutoPtr<IList> mDistributionPoints;

    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCRLDISTRIBUTIONPOINTS_H__
