
#ifndef __HELLOCAR_CMYTHREAD_H__
#define __HELLOCAR_CMYTHREAD_H__

#include "_Elastos_HelloCar_CMyThread.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
#include <elastos/core/Thread.h>            // include 基类

using Elastos::Core::Thread;

namespace Elastos {
namespace HelloCar {

CarClass(CMyThread)
    , public Thread
{
public:
    CAR_OBJECT_DECL()   // 非单例 CAR 类需要使用宏 CAR_OBJECT_DECL/CAR_OBJECT_IMPL

    CARAPI constructor();

    CARAPI Run();
};

} // HelloCar
} // Elastos

#endif //__HELLOCAR_CTHREAD_H__
