
#include "CKeyManagerFactoryHelper.h"
#include "KeyManagerFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CKeyManagerFactoryHelper, Singleton, IKeyManagerFactoryHelper)

CAR_SINGLETON_IMPL(CKeyManagerFactoryHelper)

ECode CKeyManagerFactoryHelper::GetDefaultAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    return KeyManagerFactory::GetDefaultAlgorithm(algorithm);
}

ECode CKeyManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return KeyManagerFactory::GetInstance(algorithm, factory);
}

ECode CKeyManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return KeyManagerFactory::GetInstance(algorithm, provider, factory);
}

ECode CKeyManagerFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return KeyManagerFactory::GetInstance(algorithm, provider, factory);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
