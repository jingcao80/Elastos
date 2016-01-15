
#ifndef __HELLOCAR_CCAT_H__
#define __HELLOCAR_CCAT_H__

#include "_Elastos_HelloCar_CCat.h"     // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include "Cat.h"                        // include 基类

namespace Elastos {
namespace HelloCar {

CarClass(CCat)
    , public Cat
{
public:
    CAR_OBJECT_DECL()   // 非单例 CAR 类需要使用宏 CAR_OBJECT_DECL/CAR_OBJECT_IMPL
};

} // HelloCar
} // Elastos

#endif //__HELLOCAR_CCAT_H__
