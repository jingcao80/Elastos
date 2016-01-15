
#ifndef __ELASTOSX_CRYPTO_CSECRETKEYFACTORYHELPER_H__
#define __ELASTOSX_CRYPTO_CSECRETKEYFACTORYHELPER_H__

#include "_Elastosx_Crypto_CSecretKeyFactoryHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Crypto {

CarClass(CSecretKeyFactoryHelper)
    , public Singleton
    , public ISecretKeyFactoryHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Creates a new {@code SecretKeyFactory} instance for the specified key
     * algorithm.
     *
     * @param algorithm
     *            the name of the key algorithm.
     * @return a secret key factory for the specified key algorithm.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested algorithm.
     * @throws NullPointerException
     *             if the specified algorithm is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKeyFactory ** secretKeyFactory);

    /**
     * Creates a new {@code SecretKeyFactory} instance for the specified key
     * algorithm from the specified {@code provider}.
     *
     * @param algorithm
     *            the name of the key algorithm.
     * @param provider
     *            the name of the provider that provides the requested
     *            algorithm.
     * @return a secret key factory for the specified key algorithm from the
     *         specified provider.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             algorithm.
     * @throws NoSuchProviderException
     *             if the specified provider does not exist.
     * @throws IllegalArgumentException
     *             if the specified provider name is {@code null} or empty.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ISecretKeyFactory ** secretKeyFactory);

    /**
     * Creates a new {@code SecretKeyFactory} instance for the specified key
     * algorithm from the specified provider. The {@code provider} supplied
     * does not have to be registered.
     *
     * @param algorithm
     *            the name of the key algorithm.
     * @param provider
     *            the provider that provides the requested algorithm.
     * @return a secret key factory for the specified key algorithm from the
     *         specified provider.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provider the requested
     *             algorithm.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException
     *             is the specified algorithm name is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ ISecretKeyFactory ** secretKeyFactory);
};

}
}

#endif // __ELASTOSX_CRYPTO_CSECRETKEYFACTORYHELPER_H__
