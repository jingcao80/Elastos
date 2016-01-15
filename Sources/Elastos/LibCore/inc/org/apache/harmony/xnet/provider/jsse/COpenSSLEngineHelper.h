
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLENGINEHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLENGINEHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLEngineHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLEngineHelper)
{
public:
    CARAPI GetInstance(
        /* [in] */ const String& engine,
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IOpenSSLEngine ** ppInstance);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLENGINEHELPER_H__
