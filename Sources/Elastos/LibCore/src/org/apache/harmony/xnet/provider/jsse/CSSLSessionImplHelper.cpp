
#include "CSSLSessionImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CSSLSessionImplHelper)

CAR_INTERFACE_IMPL(CSSLSessionImplHelper, Singleton, ISSLSessionImplHelper)

ECode CSSLSessionImplHelper::GetNULL_SESSION(
    /* [out] */ Elastosx::Net::Ssl::ISSLSession ** ppNullSession)
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

