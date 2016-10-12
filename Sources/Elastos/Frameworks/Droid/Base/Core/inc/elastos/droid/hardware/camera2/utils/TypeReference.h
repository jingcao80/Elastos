
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
    CARAPI constructor(
        /* [in] */ const ClassID& clsID);

    /**
     * Return the dynamic {@link Type} corresponding to the captured type {@code T}.
     */
    CARAPI GetType(
        /* [out] */ ClassID* clsID);

    /**
     * Returns the raw type of T.
     *
     * <p><ul>
     * <li>If T is a Class itself, T itself is returned.
     * <li>If T is a ParameterizedType, the raw type of the parameterized type is returned.
     * <li>If T is a GenericArrayType, the returned type is the corresponding array class.
     * For example: {@code List<Integer>[]} => {@code List[]}.
     * <li>If T is a type variable or a wildcard type, the raw type of the first upper bound is
     * returned. For example: {@code <X extends Foo>} => {@code Foo}.
     * </ul>
     *
     * @return the raw type of {@code T}
     */
    //@SuppressWarnings("unchecked")
    CARAPI GetRawType(
        /* [out] */ IClassInfo** classInfo);

    /**
     * Get the component type, e.g. {@code T} from {@code T[]}.
     *
     * @return component type, or {@code null} if {@code T} is not an array
     */
    CARAPI GetComponentType(
        /* [out] */ ITypeReference** reference);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

    static AutoPtr<ITypeReference> CreateSpecializedTypeReference(
        /* [in] */ const ClassID& clsID);

private:
    Int32 mHash;
    ClassID mType;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TYPEREFERENCE_H__
