
#ifndef __ELASTOS_CORE_ENUM_H__
#define __ELASTOS_CORE_ENUM_H__

#include "Object.h"

using Elastos::IO::ISerializable;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Core {

class Enum
    : public Object
    , public IEnum
    , public ISerializable
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the name of this enum constant. The name is the field as it
     * appears in the {@code enum} declaration.
     *
     * @return the name of this enum constant.
     * @see #toString()
     */
    CARAPI Name(
        /* [out] */ String* str);

    /**
     * Returns the position of the enum constant in the declaration. The first
     * constant has an ordinal value of zero.
     *
     * @return the ordinal value of this enum constant.
     */
    CARAPI Ordinal(
        /* [out] */ Int32* value);

    /**
     * Returns a string containing a concise, human-readable description of this
     * object. In this case, the enum constant's name is returned.
     *
     * @return a printable representation of this object.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Compares this object with the specified object and indicates if they are
     * equal. In order to be equal, {@code object} must be identical to this
     * enum constant.
     *
     * @param other
     *            the object to compare this enum constant with.
     * @return {@code true} if the specified object is equal to this
     *         {@code Enum}; {@code false} otherwise.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* value);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * Returns the enum constant's declaring class.
     *
     * @return the class object representing the constant's enum type.
     */
    // @SuppressWarnings("unchecked")
    CARAPI GetDeclaringClass(
        /* [out] */ InterfaceID* riid);

    /**
     * Compares this object to the specified enum object to determine their
     * relative order. This method compares the object's ordinal values, that
     * is, their position in the enum declaration.
     *
     * @param o
     *            the enum object to compare this object to.
     * @return a negative value if the ordinal value of this enum constant is
     *         less than the ordinal value of {@code o}; 0 if the ordinal
     *         values of this enum constant and {@code o} are equal; a positive
     *         value if the ordinal value of this enum constant is greater than
     *         the ordinal value of {@code o}.
     * @see java.lang.Comparable
     */
    CARAPI CompareTo(
        /* [in] */ IInterface* o,
        /* [out] */ Int32* value);

    /**
     * Returns the constant with the specified name of the specified enum type.
     *
     * @param enumType
     *            the class of the enumerated type to search for the constant
     *            value.
     * @param name
     *            the name of the constant value to find.
     * @return the enum constant.
     * @throws NullPointerException
     *             if either {@code enumType} or {@code name} are {@code null}.
     * @throws IllegalArgumentException
     *             if {@code enumType} is not an enumerated type or does not
     *             have a constant value called {@code name}.
     */
    static CARAPI ValueOf(
        /* [in] */ InterfaceID enumType,
        /* [in] */ const String& name,
        /* [out] */ IInterface** obj);

    /**
     * Returns a shared, mutable array containing the constants of this enum. It
     * is an error to modify the returned array.
     *
     * @hide
     */
    // @SuppressWarnings("unchecked") // the cache always returns the type matching enumType
    static CARAPI_(AutoPtr< ArrayOf<IInterface*> >) GetSharedConstants(
        /* [in] */ InterfaceID enumType);

protected:
    /**
     * Constructor for constants of enum subtypes.
     *
     * @param name
     *            the enum constant's declared name.
     * @param ordinal
     *            the enum constant's ordinal, which corresponds to its position
     *            in the enum declaration, starting at zero.
     */
    Enum(
        /* [in] */ const String& name,
        /* [in] */ Int32 ordinal);

    /**
     * {@code Enum} objects are singletons, they may not be cloned. This method
     * always throws a {@code CloneNotSupportedException}.
     *
     * @return does not return.
     * @throws CloneNotSupportedException
     *             is always thrown.
     */
    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** value);

    /**
     * Enum types may not have finalizers.
     *
     * @since 1.6
     */
    // @Override
    // @SuppressWarnings("FinalizeDoesntCallSuperFinalize")
    CARAPI Finalize();

    // private static final BasicLruCache<Class<? extends Enum>, Object[]> sharedConstantsCache
    //         = new BasicLruCache<Class<? extends Enum>, Object[]>(64) {
    //     @Override protected Object[] create(Class<? extends Enum> enumType) {
    //         if (!enumType.isEnum()) {
    //             return null;
    //         }
    //         Method method = (Method) Class.getDeclaredConstructorOrMethod(
    //                 enumType, "values", EmptyArray.CLASS);
    //         try {
    //             return (Object[]) method.invoke((Object[]) null);
    //         } catch (IllegalAccessException impossible) {
    //             throw new AssertionError();
    //         } catch (InvocationTargetException impossible) {
    //             throw new AssertionError();
    //         }
    //     }
    // };

private:
    const static Int64 sSerialVersionUID = -4300926546619394005L;

    String mName;

    Int32 mOrdinal;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_ENUM_H__
