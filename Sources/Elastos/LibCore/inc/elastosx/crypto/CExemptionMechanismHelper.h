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

#ifndef __ELASTOSX_CRYPTO_CEXEMPTIONMECHANISMHELPER_H__
#define __ELASTOSX_CRYPTO_CEXEMPTIONMECHANISMHELPER_H__

#include "_Elastosx_Crypto_CExemptionMechanismHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Security::IProvider;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {

CarClass(CExemptionMechanismHelper)
    , public Singleton
    , public IExemptionMechanismHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Returns a new {@code ExemptionMechanism} instance that provides the
     * specified exemption mechanism algorithm.
     *
     * @param algorithm
     *            the name of the requested exemption mechanism.
     * @return the new {@code ExemptionMechanism} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available by any provider.
     * @throws NullPointerException
     *             if the algorithm parameter is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IExemptionMechanism ** exemptionMechanism);

    /**
     * Returns a new {@code ExemptionMechansm} instance that provides the
     * specified exemption mechanism algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested exemption mechanism.
     * @param provider
     *            the name of the provider that is providing the algorithm.
     * @return the new {@code ExemptionMechanism} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws NullPointerException
     *             if the algorithm parameter is {@code null}.
     * @throws IllegalArgumentException
     *             if the provider parameter is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IExemptionMechanism ** exemptionMechanism);

    /**
     * Returns a new {@code ExemptionMechanism} instance that provides the
     * specified exemption mechanism algorithm from the specified provider.
     * The {@code provider} supplied does not have to be registered.
     *
     * @param algorithm
     *            the name of the requested exemption mechanism.
     * @param provider
     *            the provider that is providing the algorithm.
     * @return the new {@code ExemptionMechanism} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NullPointerException
     *             if the algorithm parameter is {@code null}.
     * @throws IllegalArgumentException
     *             if the provider parameter is {@code null}.
     */
    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IExemptionMechanism ** exemptionMechanism);
};

}
}

#endif // __ELASTOSX_CRYPTO_CEXEMPTIONMECHANISMHELPER_H__
