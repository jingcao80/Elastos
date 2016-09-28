
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFOHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFOHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CPKIStatusInfoHelper.h"
#include <elastos/core/Singleton.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CPKIStatusInfoHelper)
    , public Singleton
    , public IPKIStatusInfoHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ IASN1Sequence* pAsn1);

private:
    // TODO: Add your private member variables here.
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CPKISTATUSINFOHELPER_H__
