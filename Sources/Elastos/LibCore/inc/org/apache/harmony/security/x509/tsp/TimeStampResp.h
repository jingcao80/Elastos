
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TIMESTAMPRESP_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TIMESTAMPRESP_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Pkcs7::IContentInfo;
using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

class TimeStampResp
    : public Object
    , public ITimeStampResp
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

        TO_STRING_IMPL("TimeStampResp::MyASN1Sequence")
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetStatus(
        /* [out] */ IPKIStatusInfo** ppStatus);

    CARAPI GetTimeStampToken(
        /* [out] */ IContentInfo** ppTimeStampToken);

    CARAPI constructor(
        /* [in] */ IPKIStatusInfo* pStatus,
        /* [in] */ IContentInfo* pTimeStampToken);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    AutoPtr<IPKIStatusInfo> mStatus;

    AutoPtr<IContentInfo> mTimeStampToken;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_TIMESTAMPRESP_H__
