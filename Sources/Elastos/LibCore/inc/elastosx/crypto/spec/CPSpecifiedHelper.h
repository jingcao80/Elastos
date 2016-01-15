
#ifndef __ELASTOSX_CRYPTO_SPEC_CPSPECIFIEDHELPER_H__
#define __ELASTOSX_CRYPTO_SPEC_CPSPECIFIEDHELPER_H__

#include "_Elastosx_Crypto_Spec_CPSpecifiedHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CPSpecifiedHelper)
    , public Singleton
    , public IPSpecifiedHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * The instance of <code>PSpecified</code> with the default value
     * <code>byte[0]</code> for <code>P</code>
     */
    GetDEFAULT(
        /* [out] */ IPSpecified** result);

};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CPSPECIFIEDHELPER_H__
