
#ifndef __HELLOCAR_DOG_H__
#define __HELLOCAR_DOG_H__

#include "Animal.h"                     // include 基类

namespace Elastos {
namespace HelloCar {

class Dog
    : public Animal
    , public IDog           // 实现　CAR 类　IDog
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    CARAPI Bark();
};

} // HelloCar
} // Elastos

#endif //__HELLOCAR_DOG_H__
