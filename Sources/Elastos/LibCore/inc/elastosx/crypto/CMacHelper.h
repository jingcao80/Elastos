
#ifndef __ELASTOSX_CRYPTO_CMACHELPER_H__
#define __ELASTOSX_CRYPTO_CMACHELPER_H__

#include "_Elastosx_Crypto_CMacHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Crypto {

CarClass(CMacHelper)
    , public Singleton
    , public IMacHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Creates a new {@code Mac} instance that provides the specified MAC
     * algorithm.
     *
     * @param algorithm
     *            the name of the requested MAC algorithm.
     * @return the new {@code Mac} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available by any provider.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release).
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IMac ** mac);

    /**
     * Creates a new {@code Mac} instance that provides the specified MAC
     * algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested MAC algorithm.
     * @param provider
     *            the name of the provider that is providing the algorithm.
     * @return the new {@code Mac} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws IllegalArgumentException
     *             if the specified provider name is {@code null} or empty.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release).
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IMac ** mac);

    /**
     * Creates a new {@code Mac} instance that provides the specified MAC
     * algorithm from the specified provider. The {@code provider} supplied
     * does not have to be registered.
     *
     * @param algorithm
     *            the name of the requested MAC algorithm.
     * @param provider
     *            the provider that is providing the algorithm.
     * @return the new {@code Mac} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws IllegalArgumentException
     *             if {@code provider} is {@code null}.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release).
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IMac ** mac);
};

}
}

#endif // __ELASTOSX_CRYPTO_CMACHELPER_H__
