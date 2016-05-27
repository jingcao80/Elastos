
#include "CKeyManagerFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CKeyManagerFactoryImpl)

ECode CKeyManagerFactoryImpl::EngineInit(
    /* [in] */ Elastos::Security::IKeyStore * pKs,
    /* [in] */ ArrayOf<Char32> * pPassword)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerFactoryImpl::EngineInitEx(
    /* [in] */ Elastosx::Net::Ssl::IManagerFactoryParameters * pSpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerFactoryImpl::EngineGetKeyManagers(
    /* [out, callee] */ ArrayOf<Elastosx::Net::Ssl::IKeyManager *> ** ppManagers)
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

