
#ifndef __ELASTOSX_CRYPTO_CKEYGENERATORHELPER_H__
#define __ELASTOSX_CRYPTO_CKEYGENERATORHELPER_H__

#include "_Elastosx_Crypto_CKeyGeneratorHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Crypto {

CarClass(CKeyGeneratorHelper)
    , public Singleton
    , public IKeyGeneratorHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Creates a new {@code KeyGenerator} instance that provides the specified
     * key algorithm,
     *
     * @param algorithm
     *            the name of the requested key algorithm
     * @return the new {@code KeyGenerator} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available by any provider.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyGenerator ** ka);

    /**
     * Creates a new {@code KeyGenerator} instance that provides the specified
     * key algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested key algorithm.
     * @param provider
     *            the name of the provider that is providing the algorithm.
     * @return the new {@code KeyGenerator} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws IllegalArgumentException
     *             if the specified provider is name is {@code null} or empty.
     * @throws NullPointerException
     *             if the specified algorithm name is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyGenerator ** ka);

    /**
     * Creates a new {@code KeyGenerator} instance that provides the specified
     * key algorithm from the specified provider. The {@code provider}
     * supplied does not have to be registered.
     *
     * @param algorithm
     *            the name of the requested key algorithm.
     * @param provider
     *            the provider that is providing the algorithm
     * @return the new {@code KeyGenerator} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException
     *             if the specified algorithm name is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IKeyGenerator ** ka);
};

}
}

#endif // __ELASTOSX_CRYPTO_CKEYGENERATORHELPER_H__
