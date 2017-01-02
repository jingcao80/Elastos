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

#ifndef __ELASTOSX_CRYPTO_SPEC_COAEPPARAMETERSPECHELPER_H__
#define __ELASTOSX_CRYPTO_SPEC_COAEPPARAMETERSPECHELPER_H__

#include "_Elastosx_Crypto_Spec_COAEPParameterSpecHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(COAEPParameterSpecHelper)
    , public Singleton
    , public IOAEPParameterSpecHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * The algorithm parameter instance with default values.
     * <p>
     * It uses the following parameters:
     * <ul><li>message digest : <code>"SHA-1"</code></li>
     * <li>mask generation function (<i>mgf</i>) : <code>"MGF1"</code></li>
     * <li>parameters for the <i>mgf</i> : "SHA-1" {@link MGF1ParameterSpec#SHA1}</li>
     * <li>the source of the label <code>L</code>: {@link PSource.PSpecified#DEFAULT}</li>
     * </ul>
     */
    CARAPI GetDEFAULT(
        /* [out] */ IOAEPParameterSpec ** spec);

};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_COAEPPARAMETERSPECHELPER_H__
