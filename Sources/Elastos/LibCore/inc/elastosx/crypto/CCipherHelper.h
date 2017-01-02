//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOSX_CRYPTO_CCIPHERHELPER_H__
#define __ELASTOSX_CRYPTO_CCIPHERHELPER_H__

#include "_Elastosx_Crypto_CCipherHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Security::IProvider;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {

CarClass(CCipherHelper)
    , public Singleton
    , public ICipherHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Creates a new Cipher for the specified transformation. The installed
     * providers are searched in order for an implementation of the specified
     * transformation. The first found provider providing the transformation is
     * used to create the cipher. If no provider is found an exception is
     * thrown.
     *
     * @param transformation
     *            the name of the transformation to create a cipher for.
     * @return a cipher for the requested transformation.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the
     *             <i>transformation</i>, or it is {@code null}, empty or in an
     *             invalid format.
     * @throws NoSuchPaddingException
     *             if no installed provider can provide the padding scheme in
     *             the <i>transformation</i>.
     */
    CARAPI GetInstance(
        /* [in] */ const String& transformation,
        /* [out] */ ICipher ** cipher);

    /**
     * Creates a new cipher for the specified transformation provided by the
     * specified provider.
     *
     * @param transformation
     *            the name of the transformation to create a cipher for.
     * @param provider
     *            the name of the provider to ask for the transformation.
     * @return a cipher for the requested transformation.
     * @throws NoSuchAlgorithmException
     *             if the specified provider can not provide the
     *             <i>transformation</i>, or it is {@code null}, empty or in an
     *             invalid format.
     * @throws NoSuchProviderException
     *             if no provider with the specified name can be found.
     * @throws NoSuchPaddingException
     *             if the requested padding scheme in the <i>transformation</i>
     *             is not available.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& transformation,
        /* [in] */ const String& provider,
        /* [out] */ ICipher ** cipher);

    /**
     * Creates a new cipher for the specified transformation. The
     * {@code provider} supplied does not have to be registered.
     *
     * @param transformation
     *            the name of the transformation to create a cipher for.
     * @param provider
     *            the provider to ask for the transformation.
     * @return a cipher for the requested transformation.
     * @throws NoSuchAlgorithmException
     *             if the specified provider can not provide the
     *             <i>transformation</i>, or it is {@code null}, empty or in an
     *             invalid format.
     * @throws NoSuchPaddingException
     *             if the requested padding scheme in the <i>transformation</i>
     *             is not available.
     * @throws IllegalArgumentException
     *             if the provider is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& transformation,
        /* [in] */ IProvider * provider,
        /* [out] */ ICipher ** cipher);

    /**
     * Returns the maximum key length for the specified transformation.
     *
     * @param transformation
     *            the transformation name.
     * @return the maximum key length, currently {@code Integer.MAX_VALUE}.
     * @throws NoSuchAlgorithmException
     *             if no provider for the specified {@code transformation} can
     *             be found.
     * @throws NullPointerException
     *             if {@code transformation} is {@code null}.
     */
    CARAPI GetMaxAllowedKeyLength(
        /* [in] */ const String& transformation,
        /* [out] */ Int32 * value);

    /**
     * Returns the maximum cipher parameter value for the specified
     * transformation. If there is no maximum limit, {@code null} is returned.
     *
     * @param transformation
     *            the transformation name.
     * @return a parameter spec holding the maximum value or {@code null}.
     *         Currently {@code null}.
     * @throws NoSuchAlgorithmException
     *             if no provider for the specified {@code transformation} can
     *             be found.
     * @throws NullPointerException
     *             if {@code transformation} is {@code null}.
    */
    CARAPI GetMaxAllowedParameterSpec(
        /* [in] */ const String& transformation,
        /* [out] */ IAlgorithmParameterSpec ** value);
};

}
}

#endif // __ELASTOSX_CRYPTO_CCIPHERHELPER_H__
