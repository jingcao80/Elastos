#include "elastos/hellocar/CAnimalHelper.h"

namespace Elastos {
namespace HelloCar {

CAR_INTERFACE_IMPL(CAnimalHelper, Singleton, IAnimalHelper)

CAR_SINGLETON_IMPL(CAnimalHelper)

ECode CAnimalHelper::CanFly(
    /* [in] */ IAnimal* animal,
    /* [out] */ Boolean* canFly)
{
    VALIDATE_NOT_NULL(canFly);
    *canFly = FALSE;
    VALIDATE_NOT_NULL(animal)

    return animal->CanFly(canFly);
}

} // HelloCar
} // Elastos