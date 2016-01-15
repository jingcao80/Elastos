
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTMANAGERFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTMANAGERFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CTrustManagerFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CTrustManagerFactoryImpl)
{
public:
    CARAPI EngineInit(
        /* [in] */ Elastos::Security::IKeyStore * pKs);

    CARAPI EngineInitEx(
        /* [in] */ Elastosx::Net::Ssl::IManagerFactoryParameters * pSpec);

    CARAPI EngineGetTrustManagers(
        /* [out, callee] */ ArrayOf<Elastosx::Net::Ssl::ITrustManager *> ** ppManagers);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTMANAGERFACTORYIMPL_H__
