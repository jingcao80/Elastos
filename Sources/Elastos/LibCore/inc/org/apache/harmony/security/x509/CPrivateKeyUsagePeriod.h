
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__

#include "_Org_Apache_Harmony_Security_X509_CPrivateKeyUsagePeriod.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IDate;

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
        /* [out] */ IDate** ppNotBefore);

    CARAPI GetNotAfter(
        /* [out] */ IDate** ppNotAfter);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ IDate* pNotBeforeDate,
        /* [in] */ IDate* pNotAfterDate);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CPRIVATEKEYUSAGEPERIOD_H__
