
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class TypeReference
    : public Object
    , public ITypeReference
{
public:
    CAR_INTERFACE_DECL()

    TypeReference();

    virtual ~TypeReference() {}

    /**
     * Create a new type reference for {@code T}.
     *
     * @throws IllegalArgumentException if {@code T}'s actual type contains a type variable
     *
     * @see TypeReference
     */
    CARAPI constructor();

private:
    const Int32 mHash;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__
