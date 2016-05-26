
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__

#include "_Org_Apache_Harmony_Security_X509_CPrivateKeyUsagePeriod.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CPrivateKeyUsagePeriod)
    , public Object
    , public IPrivateKeyUsagePeriod
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNotBefore(
        /* [out] */ Elastos::Utility::IDate ** ppNotBefore);

    CARAPI GetNotAfter(
        /* [out] */ Elastos::Utility::IDate ** ppNotAfter);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI constructor(
        /* [in] */ Elastos::Utility::IDate * pNotBeforeDate,
        /* [in] */ Elastos::Utility::IDate * pNotAfterDate);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__
