
#ifndef __ORG_APACHE_HARMONY_SECURITY_PKCS8_CPRIVATEKEYINFOHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PKCS8_CPRIVATEKEYINFOHELPER_H__

#include "_Org_Apache_Harmony_Security_Pkcs8_CPrivateKeyInfoHelper.h"
#include <elastos/core/Singleton.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs8 {

CarClass(CPrivateKeyInfoHelper)
    , public Singleton
    , public IPrivateKeyInfoHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** asn1);
};

} // namespace Pkcs8
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PKCS8_CPRIVATEKEYINFOHELPER_H__
