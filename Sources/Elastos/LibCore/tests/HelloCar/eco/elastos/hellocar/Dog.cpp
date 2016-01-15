
#include "elastos/hellocar/Dog.h"

namespace Elastos {
namespace HelloCar {

CAR_INTERFACE_IMPL(Dog, Animal, IDog)

ECode Dog::Bark()
{
    printf("%s : wang! wang!\n", mName.string());
    return NOERROR;
}

} // HelloCar
} // Elastos