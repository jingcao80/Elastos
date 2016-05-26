
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFOHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFOHELPER_H__

#include "_Org_Apache_Harmony_Security_Pkcs10_CCertificationRequestInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs10 {

CarClass(CCertificationRequestInfoHelper)
    , public Singleton
    , public ICertificationRequestInfoHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** asn1);

private:
    // TODO: Add your private member variables here.
};

} // namespace Pkcs10
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS10_CCERTIFICATIONREQUESTINFOHELPER_H__
