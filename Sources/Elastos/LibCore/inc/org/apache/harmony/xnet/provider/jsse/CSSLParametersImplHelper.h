
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLPARAMETERSIMPLHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLPARAMETERSIMPLHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLParametersImplHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLParametersImplHelper)
    , public Singleton
    , public ISSLParametersImplHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetDefaultTrustManager(
        /* [out] */ Elastosx::Net::Ssl::IX509TrustManager ** ppManager);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLPARAMETERSIMPLHELPER_H__
