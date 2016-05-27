
#include "CSSLParametersImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CSSLParametersImplHelper)

CAR_INTERFACE_IMPL(CSSLParametersImplHelper, Singleton, ISSLParametersImplHelper)

ECode CSSLParametersImplHelper::GetDefaultTrustManager(
    /* [out] */ Elastosx::Net::Ssl::IX509TrustManager ** ppManager)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

