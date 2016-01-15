
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSESSIONIMPLHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSESSIONIMPLHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLSessionImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLSessionImplHelper)
{
public:
    CARAPI GetNULL_SESSION(
        /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppNullSession);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSESSIONIMPLHELPER_H__
