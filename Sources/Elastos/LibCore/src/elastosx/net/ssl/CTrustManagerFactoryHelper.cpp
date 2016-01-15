
#include "CTrustManagerFactoryHelper.h"
#include "TrustManagerFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CTrustManagerFactoryHelper, Singleton, ITrustManagerFactoryHelper)

CAR_SINGLETON_IMPL(CTrustManagerFactoryHelper)

ECode CTrustManagerFactoryHelper::GetDefaultAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    return TrustManagerFactory::GetDefaultAlgorithm(algorithm);
}

ECode CTrustManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return TrustManagerFactory::GetInstance(algorithm, factory);
}

ECode CTrustManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return TrustManagerFactory::GetInstance(algorithm, provider, factory);
}

ECode CTrustManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return TrustManagerFactory::GetInstance(algorithm, provider, factory);
}

} //namespace Ssl
} // namespace Net
} // namespace Elastosx
