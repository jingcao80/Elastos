
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CNAMEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CNAMEHELPER_H__

#include "_Org_Apache_Harmony_Security_X501_CNameHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CNameHelper)
    , public Singleton
    , public INameHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1_RDN(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1SetOf ** ppAsn1_rdn);

    CARAPI SetASN1_RDN(
        /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1SetOf * pAsn1_rdn);

    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf ** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf * pAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CNAMEHELPER_H__
