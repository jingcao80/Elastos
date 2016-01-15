
#ifndef __ELASTOS_CORE_COBJECT_H__
#define __ELASTOS_CORE_COBJECT_H__

#include "_Elastos_Core_CObject.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Core {

/**
 * The root class of the Java class hierarchy. All non-primitive types
 * (including arrays) inherit either directly or indirectly from this class.
 *
 * <a name="writing_equals"><h4>Writing a correct {@code equals} method</h4></a>
 * <p>Follow this style to write a canonical {@code equals} method:
 * <pre>
 *   // Use @Override to avoid accidental overloading.
 *   &#x0040;Override public boolean equals(Object o) {
 *     // Return true if the objects are identical.
 *     // (This is just an optimization, not required for correctness.)
 *     if (this == o) {
 *       return true;
 *     }
 *
 *     // Return false if the other object has the wrong type.
 *     // This type may be an interface depending on the interface's specification.
 *     if (!(o instanceof MyType)) {
 *       return false;
 *     }
 *
 *     // Cast to the appropriate type.
 *     // This will succeed because of the instanceof, and lets us access private fields.
 *     MyType lhs = (MyType) o;
 *
 *     // Check each field. Primitive fields, reference fields, and nullable reference
 *     // fields are all treated differently.
 *     return primitiveField == lhs.primitiveField &amp;&amp;
 *             referenceField.equals(lhs.referenceField) &amp;&amp;
 *             (nullableField == null ? lhs.nullableField == null
 *                                    : nullableField.equals(lhs.nullableField));
 *   }
 * </pre>
 * <p>If you override {@code equals}, you should also override {@code hashCode}: equal
 * instances must have equal hash codes.
 *
 * <p>See <i>Effective Java</i> item 8 for much more detail and clarification.
 *
 * <a name="writing_hashCode"><h4>Writing a correct {@code hashCode} method</h4></a>
 * <p>Follow this style to write a canonical {@code hashCode} method:
 * <pre>
 *   &#x0040;Override public int hashCode() {
 *     // Start with a non-zero constant.
 *     int result = 17;
 *
 *     // Include a hash for each field.
 *     result = 31 * result + (booleanField ? 1 : 0);
 *
 *     result = 31 * result + byteField;
 *     result = 31 * result + charField;
 *     result = 31 * result + shortField;
 *     result = 31 * result + intField;
 *
 *     result = 31 * result + (int) (longField ^ (longField >>> 32));
 *
 *     result = 31 * result + Float.floatToIntBits(floatField);
 *
 *     long doubleFieldBits = Double.doubleToLongBits(doubleField);
 *     result = 31 * result + (int) (doubleFieldBits ^ (doubleFieldBits >>> 32));
 *
 *     result = 31 * result + Arrays.hashCode(arrayField);
 *
 *     result = 31 * result + referenceField.hashCode();
 *     result = 31 * result +
 *         (nullableReferenceField == null ? 0
 *                                         : nullableReferenceField.hashCode());
 *
 *     return result;
 *   }
 * </pre>
 *
 * <p>If you don't intend your type to be used as a hash key, don't simply rely on the default
 * {@code hashCode} implementation, because that silently and non-obviously breaks any future
 * code that does use your type as a hash key. You should throw instead:
 * <pre>
 *   &#x0040;Override public int hashCode() {
 *     throw new UnsupportedOperationException();
 *   }
 * </pre>
 *
 * <p>See <i>Effective Java</i> item 9 for much more detail and clarification.
 *
 * <a name="writing_toString"><h4>Writing a useful {@code toString} method</h4></a>
 * <p>For debugging convenience, it's common to override {@code toString} in this style:
 * <pre>
 *   &#x0040;Override public String toString() {
 *     return getClass().getName() + "[" +
 *         "primitiveField=" + primitiveField + ", " +
 *         "referenceField=" + referenceField + ", " +
 *         "arrayField=" + Arrays.toString(arrayField) + "]";
 *   }
 * </pre>
 * <p>The set of fields to include is generally the same as those that would be tested
 * in your {@code equals} implementation.
 * <p>See <i>Effective Java</i> item 10 for much more detail and clarification.
 */
CarClass(CObject)
    , public Object
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Core
} // namespace Elastos

#endif // __ELASTOS_CORE_COBJECT_H__
