
#include "CTrustManagerFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode CTrustManagerFactoryImpl::EngineInit(
    /* [in] */ Elastos::Security::IKeyStore * pKs)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerFactoryImpl::EngineInitEx(
    /* [in] */ Elastosx::Net::Ssl::IManagerFactoryParameters * pSpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustManagerFactoryImpl::EngineGetTrustManagers(
    /* [out, callee] */ ArrayOf<Elastosx::Net::Ssl::ITrustManager *> ** ppManagers)
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

