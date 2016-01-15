
#include "elastos/hellocar/CMyThread.h"

namespace Elastos {
namespace HelloCar {

CAR_OBJECT_IMPL(CMyThread)

ECode CMyThread::constructor()
{
    return Thread::constructor();   // 实现 IThread 的 CAR 类必须调用基类 Thread 的某个 constructor 以便进行必要的初始化
}

ECode CMyThread::Run()
{
    printf("CMyThread::Run\n");
    return Thread::Run();
}

} // HelloCar
} // Elastos