
#ifndef __HELLOCAR_CANIMALHELPER_H__
#define __HELLOCAR_CANIMALHELPER_H__

#include "_Elastos_HelloCar_CAnimalHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Singleton.h>             // include 单例基类


namespace Elastos {
namespace HelloCar {

// CAnimalHelper 是单例 CAR 类，单例 CAR 类需要继承 Singleton，并使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL
CarClass(CAnimalHelper)
    , public Singleton      // 单例 CAR 类需要继承 Singleton
    , public IAnimalHelper  // 实现　CAR 类IAnimalHelper
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

    CARAPI CanFly(
        /* [in] */ IAnimal* animal,
        /* [out] */ Boolean* canFly);
};

} // HelloCar
} // Elastos

#endif //__HELLOCAR_CANIMALHELPER_H__
