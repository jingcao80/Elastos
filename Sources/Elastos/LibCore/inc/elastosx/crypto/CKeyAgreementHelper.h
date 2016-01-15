
#ifndef __ELASTOSX_CRYPTO_CKEYAGREEMENTHELPER_H__
#define __ELASTOSX_CRYPTO_CKEYAGREEMENTHELPER_H__

#include "_Elastosx_Crypto_CKeyAgreementHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Crypto {

CarClass(CKeyAgreementHelper)
    , public Singleton
    , public IKeyAgreementHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Creates a new {@code KeyAgreement} for the specified algorithm.
     *
     * @param algorithm
     *            the name of the key agreement algorithm to create.
     * @return a key agreement for the specified algorithm.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested algorithm.
     * @throws NullPointerException
     *             if the specified algorithm is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyAgreement ** KeyAgreement);

    /**
     * Creates a new {@code KeyAgreement} for the specified algorithm from the
     * specified provider.
     *
     * @param algorithm
     *            the name of the key agreement algorithm to create.
     * @param provider
     *            the name of the provider that provides the requested
     *            algorithm.
     * @return a key agreement for the specified algorithm from the specified
     *         provider.
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
        /* [out] */ IKeyAgreement ** KeyAgreement);

    /**
     * Create a new {@code KeyAgreement} for the specified algorithm from the
     * specified provider. The {@code provider} supplied does not have to be
     * registered.
     *
     * @param algorithm
     *            the name of the key agreement algorithm to create.
     * @param provider
     *            the provider that provides the requested algorithm.
     * @return a key agreement for the specified algorithm from the specified
     *         provider.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             algorithm.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException
     *             if the specified algorithm name is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IKeyAgreement ** KeyAgreement);
};

}
}

#endif // __ELASTOSX_CRYPTO_CKEYAGREEMENTHELPER_H__
