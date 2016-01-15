
#ifndef __ELASTOS_DROID_ANIMATION_TYPECONVERTER_H__
#define __ELASTOS_DROID_ANIMATION_TYPECONVERTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITypeConverter;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * Abstract base class used convert type T to another type V. This
 * is necessary when the value types of in animation are different
 * from the property type.
 * @see PropertyValuesHolder#setConverter(TypeConverter)
 */
class TypeConverter/*<T, V>*/
    : public Object
    , public ITypeConverter
{
public:
    CAR_INTERFACE_DECL();

    TypeConverter();

    TypeConverter(
        /* [in] */ const InterfaceID& fromClass,
        /* [in] */ const InterfaceID& toClass);

    /**
     * Returns the target converted type. Used by the animation system to determine
     * the proper setter function to call.
     * @return The Class to convert the input to.
     */
    virtual CARAPI GetTargetType(
        /* [out] */ InterfaceID* type);

    /**
     * Returns the source conversion type.
     */
    virtual CARAPI GetSourceType(
        /* [out] */ InterfaceID* type);

    /**
     * Converts a value from one type to another.
     * @param value The Object to convert.
     * @return A value of type V, converted from <code>value</code>.
     */
    virtual CARAPI Convert(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** v) = 0;

protected:
    CARAPI constructor(
        /* [in] */ const InterfaceID& fromClass,
        /* [in] */ const InterfaceID& toClass);

private:
    /*private Class<T> */InterfaceID mFromClass;
    /*private Class<V> */InterfaceID mToClass;
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_TYPECONVERTER_H__
