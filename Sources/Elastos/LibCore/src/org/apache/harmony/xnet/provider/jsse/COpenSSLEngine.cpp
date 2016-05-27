
#include "COpenSSLEngine.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLEngine)

CAR_INTERFACE_IMPL(COpenSSLEngine, Object, IOpenSSLEngine)

ECode COpenSSLEngine::GetPrivateKeyById(
    /* [in] */ const String& id,
    /* [out] */ Elastos::Security::IPrivateKey ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLEngine::Equals(
    /* [in] */ IInterface * pObj,
    /* [out] */ Boolean * pIsEqual)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLEngine::GetHashCode(
    /* [out] */ Int32 * pHashCode)
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

