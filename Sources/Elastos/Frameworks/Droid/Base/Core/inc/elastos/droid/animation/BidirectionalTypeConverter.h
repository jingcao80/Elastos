
#ifndef  __ELASTOS_DROID_ANIMATION_BIDIRECTIONALTYPECONVERTER_H__
#define  __ELASTOS_DROID_ANIMATION_BIDIRECTIONALTYPECONVERTER_H__

#include "elastos/droid/animation/TypeConverter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * Abstract base class used convert type T to another type V and back again. This
 * is necessary when the value types of in animation are different from the property
 * type. BidirectionalTypeConverter is needed when only the final value for the
 * animation is supplied to animators.
 * @see PropertyValuesHolder#setConverter(TypeConverter)
 */
class BidirectionalTypeConverter
    : public TypeConverter
    , public IBidirectionalTypeConverter
{
public:
    CAR_INTERFACE_DECL();

    BidirectionalTypeConverter();

    BidirectionalTypeConverter(
        /* [in] */ const InterfaceID& fromClass,
        /* [in] */ const InterfaceID& toClass);

    /**
     * Does a conversion from the target type back to the source type. The subclass
     * must implement this when a TypeConverter is used in animations and current
     * values will need to be read for an animation.
     * @param value The Object to convert.
     * @return A value of type T, converted from <code>value</code>.
     */
    virtual CARAPI ConvertBack(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** result) = 0;

    /**
     * Returns the inverse of this converter, where the from and to classes are reversed.
     * The inverted converter uses this convert to call {@link #convertBack(Object)} for
     * {@link #convert(Object)} calls and {@link #convert(Object)} for
     * {@link #convertBack(Object)} calls.
     * @return The inverse of this converter, where the from and to classes are reversed.
     */
    virtual CARAPI Invert(
        /* [out] */ IBidirectionalTypeConverter** result);

protected:
    CARAPI constructor(
        /* [in] */ const InterfaceID& fromClass,
        /* [in] */ const InterfaceID& toClass);

private:
    AutoPtr<IBidirectionalTypeConverter> mInvertedConverter;
};

class InvertedConverter
    : public BidirectionalTypeConverter
{
public:
    InvertedConverter(
        /* [in] */ BidirectionalTypeConverter/*<To, From>*/* converter);

    // @Override
    virtual CARAPI ConvertBack(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** result);

    // @Override
    virtual CARAPI Convert(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** result);

private:
    BidirectionalTypeConverter* mConverter;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_BIDIRECTIONALTYPECONVERTER_H__
