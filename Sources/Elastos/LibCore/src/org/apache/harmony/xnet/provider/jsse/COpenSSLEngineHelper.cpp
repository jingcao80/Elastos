
#include "COpenSSLEngineHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(COpenSSLEngineHelper)

CAR_INTERFACE_IMPL(COpenSSLEngineHelper, Singleton, IOpenSSLEngineHelper)

ECode COpenSSLEngineHelper::GetInstance(
    /* [in] */ const String& engine,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IOpenSSLEngine ** ppInstance)
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

