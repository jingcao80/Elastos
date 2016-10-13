#include "org/conscrypt/COpenSSLECPointContextHelper.h"
#include "org/conscrypt/OpenSSLECPointContext.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLECPointContextHelper)

CAR_INTERFACE_IMPL(COpenSSLECPointContextHelper, Singleton, IOpenSSLECPointContext)

ECode COpenSSLECPointContextHelper::GetInstance(
    /* [in] */ Int32 curveType,
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [in] */ IECPoint* javaPoint,
    /* [out] */ IOpenSSLECPointContext** result)
{
    return OpenSSLECPointContext::GetInstance(
            curveType, group, javaPoint, result);
}

} // namespace Conscrypt
} // namespace Org
