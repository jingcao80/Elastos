#include "org/conscrypt/COpenSSLEngineHelper.h"
#include "org/conscrypt/OpenSSLEngine.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLEngineHelper)

CAR_INTERFACE_IMPL(COpenSSLEngineHelper, Singleton, IOpenSSLEngineHelper)

ECode COpenSSLEngineHelper::GetInstance(
    /* [in] */ const String& engine,
    /* [out] */ IOpenSSLEngine** result)
{
    return OpenSSLEngine::GetInstance(engine, result);
}

} // namespace Conscrypt
} // namespace Org
