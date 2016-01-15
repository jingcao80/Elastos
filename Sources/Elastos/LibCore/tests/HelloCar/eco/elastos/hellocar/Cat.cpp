
#include "elastos/hellocar/Cat.h"

namespace Elastos {
namespace HelloCar {

ECode Cat::CanFly(
    /* [out] */ Boolean* canFly)
{
    printf("Though cat has nine lives, but it can not fly!\n");
    return Animal::CanFly(canFly);
}

} // HelloCar
} // Elastos