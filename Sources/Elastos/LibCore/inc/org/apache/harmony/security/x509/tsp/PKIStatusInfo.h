
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKISTATUSINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKISTATUSINFO_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

class PKIStatusInfo
    : public Object
    , public IPKIStatusInfo
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
    CAR_INTERFACE_DECL()

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetFailInfo(
        /* [out] */ IPKIFailureInfo** pFailInfo);

    CARAPI GetStatus(
        /* [out] */ IPKIStatus** pStatus);

    CARAPI GetStatusString(
        /* [out] */ IList** ppList);

    CARAPI constructor(
        /* [in] */ IPKIStatus* pkiStatus,
        /* [in] */ IList* pStatusString,
        /* [in] */ IPKIFailureInfo* failInfo);

    static CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Sequence>) initASN1();

public:
    static AutoPtr<IASN1Sequence> ASN1;

private:
    AutoPtr<IPKIStatus> mStatus;

    AutoPtr<IList> mStatusString;

    AutoPtr<IPKIFailureInfo> mFailInfo;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKISTATUSINFO_H__
