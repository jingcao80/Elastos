
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALHELPERS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALHELPERS_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

class MarshalHelpers
{
public:
    /**
     * Get the size in bytes for the native camera metadata type.
     *
     * <p>This used to determine how many bytes it would take to encode/decode a single value
     * of that {@link nativeType}.</p>
     *
     * @param nativeType the native type, e.g.
     *        {@link android.hardware.camera2.impl.CameraMetadataNative#TYPE_BYTE TYPE_BYTE}.
     * @return size in bytes >= 1
     *
     * @throws UnsupportedOperationException if nativeType was not one of the built-in types
     */
    static CARAPI GetPrimitiveTypeSize(
        /* [in] */ Int32 nativeType,
        /* [out] */ Int32* value);

    /**
     * Ensure that the {@code klass} is one of the metadata-primitive classes.
     *
     * @param klass a non-{@code null} reference
     * @return {@code klass} instance
     *
     * @throws UnsupportedOperationException if klass was not one of the built-in classes
     * @throws NullPointerException if klass was null
     *
     * @see #isPrimitiveClass
     */
    static CARAPI CheckPrimitiveClass(
        /* [in] */ const ClassID& klass,
        /* [out] */ ClassID* outcla);

    /**
     * Checks whether or not {@code klass} is one of the metadata-primitive classes.
     *
     * <p>The following types (whether boxed or unboxed) are considered primitive:
     * <ul>
     * <li>byte
     * <li>int
     * <li>float
     * <li>double
     * <li>Rational
     * </ul>
     * </p>
     *
     * <p>This doesn't strictly follow the java understanding of primitive since
     * boxed objects are included, Rational is included, and other types such as char and
     * short are not included.</p>
     *
     * @param klass a {@link Class} instance; using {@code null} will return {@code false}
     * @return {@code true} if primitive, {@code false} otherwise
     */
    static CARAPI IsPrimitiveClass(
        /* [in] */ const ClassID& klass,
        /* [out] */ Boolean* value);

    /**
     * Wrap {@code klass} with its wrapper variant if it was a {@code Class} corresponding
     * to a Java primitive.
     *
     * <p>Non-primitive classes are passed through as-is.</p>
     *
     * <p>For example, for a primitive {@code int.class => Integer.class},
     * but for a non-primitive {@code Rational.class => Rational.class}.</p>
     *
     * @param klass a {@code Class} reference
     *
     * @return wrapped class object, or same class object if non-primitive
     */
    // @SuppressWarnings("unchecked")
    static CARAPI WrapClassIfPrimitive(
        /* [in] */ const ClassID& klass,
        /* [out] */ ClassID* outcl);

    /**
     * Return a human-readable representation of the {@code nativeType}, e.g. "TYPE_INT32"
     *
     * <p>Out-of-range values return a string with "UNKNOWN" as the prefix.</p>
     *
     * @param nativeType the native type
     *
     * @return human readable type name
     */
    static CARAPI ToStringNativeType(
        /* [in] */ Int32 nativeType,
        /* [out] */ String* str);

    /**
     * Ensure that the {@code nativeType} is one of the native types supported
     * by {@link CameraMetadataNative}.
     *
     * @param nativeType the native type
     *
     * @return the native type
     *
     * @throws UnsupportedOperationException if the native type was invalid
     */
    static CARAPI CheckNativeType(
        /* [in] */ Int32 nativeType,
        /* [out] */ Int32* value);

    /**
     * Ensure that the expected and actual native types are equal.
     *
     * @param expectedNativeType the expected native type
     * @param actualNativeType the actual native type
     * @return the actual native type
     *
     * @throws UnsupportedOperationException if the types are not equal
     */
    static CARAPI CheckNativeTypeEquals(
        /* [in] */ Int32 expectedNativeType,
        /* [in] */ Int32 actualNativeType,
        /* [out] */ Int32* value);

private:
    MarshalHelpers() {}
};

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALHELPERS_H__
