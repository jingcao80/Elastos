
#ifndef __ELASTOSX_CRYPTO_SPEC_CDESEDEKEYSPECHELPER_H__
#define __ELASTOSX_CRYPTO_SPEC_CDESEDEKEYSPECHELPER_H__

#include "_Elastosx_Crypto_Spec_CDESedeKeySpecHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CDESedeKeySpecHelper)
    , public Singleton
    , public IDESedeKeySpecHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

public:
    /**
     * Returns whether the specified key data starting at <code>offset</code> is
     * <i>parity-adjusted</i>.
     *
     * @param key
     *            the key data.
     * @param offset
     *            the offset to start checking at.
     * @return {@code true} if the specified key data is parity-adjusted,
     *            {@code false} otherwise.
     * @throws InvalidKeyException
     *             if the length of the key data starting at offset is less than
     *             8, or the key is null.
     */
    CARAPI IsParityAdjusted(
        /* [in] */ ArrayOf<Byte> * key,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean * result);

};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CDESEDEKEYSPECHELPER_H__
