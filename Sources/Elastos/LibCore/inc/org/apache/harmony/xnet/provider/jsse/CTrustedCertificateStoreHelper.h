
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATESTOREHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATESTOREHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CTrustedCertificateStoreHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CTrustedCertificateStoreHelper)
{
public:
    CARAPI IsSystem(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pIsSystem);

    CARAPI IsUser(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pIsUser);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATESTOREHELPER_H__
