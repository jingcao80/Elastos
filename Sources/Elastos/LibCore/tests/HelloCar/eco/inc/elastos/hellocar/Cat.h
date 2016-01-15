
#ifndef __HELLOCAR_CAT_H__
#define __HELLOCAR_CAT_H__

#include "Animal.h"                     // include 基类

namespace Elastos {
namespace HelloCar {

class Cat
    : public Animal
{
public:
    CARAPI CanFly(                      // 覆写基类方法
        /* [out] */ Boolean* canFly);
};

} // HelloCar
} // Elastos

#endif //__HELLOCAR_CAT_H__
