
#ifndef __ELASTOS_DROID_ANIMATION_KEYFRAME_H__
#define __ELASTOS_DROID_ANIMATION_KEYFRAME_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Animation {

class PropertyValuesHolder;

/**
 * This class holds a time/value pair for an animation. The Keyframe class is used
 * by {@link ValueAnimator} to define the values that the animation target will have over the course
 * of the animation. As the time proceeds from one keyframe to the other, the value of the
 * target object will animate between the value at the previous keyframe and the value at the
 * next keyframe. Each keyframe also holds an optional {@link TimeInterpolator}
 * object, which defines the time interpolation over the intervalue preceding the keyframe.
 *
 * <p>The Keyframe class itself is abstract. The type-specific factory methods will return
 * a subclass of Keyframe specific to the type of value being stored. This is done to improve
 * performance when dealing with the most common cases (e.g., <code>float</code> and
 * <code>int</code> values). Other types will fall into a more general Keyframe class that
 * treats its values as Objects. Unless your animation requires dealing with a custom type
 * or a data structure that needs to be animated directly (and evaluated using an implementation
 * of {@link TypeEvaluator}), you should stick to using float and int as animations using those
 * types have lower runtime overhead than other types.</p>
 */
class Keyframe
    : public Object
    , public IKeyframe
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    Keyframe();

    /**
     * Constructs a Keyframe object with the given time and value. The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     * @param value The value that the object will animate to as the animation time approaches
     * the time in this keyframe, and the the value animated from as the time passes the time in
     * this keyframe.
     */
    static CARAPI OfInt32(
        /* [in] */ Float fraction,
        /* [in] */ Int32 value,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time. The value at this time will be derived
     * from the target object when the animation first starts (note that this implies that keyframes
     * with no initial value must be used as part of an ObjectAnimator).
     * The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @sa ObjectAnimator
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     */
    static CARAPI OfInt32(
        /* [in] */ Float fraction,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time and value. The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     * @param value The value that the object will animate to as the animation time approaches
     * the time in this keyframe, and the the value animated from as the time passes the time in
     * this keyframe.
     */
    static CARAPI OfFloat(
        /* [in] */ Float fraction,
        /* [in] */ Float value,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time. The value at this time will be derived
     * from the target object when the animation first starts (note that this implies that keyframes
     * with no initial value must be used as part of an ObjectAnimator).
     * The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @sa ObjectAnimator
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     */
    static CARAPI OfFloat(
        /* [in] */ Float fraction,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time and value. The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     * @param value The value that the object will animate to as the animation time approaches
     * the time in this keyframe, and the the value animated from as the time passes the time in
     * this keyframe.
     */
    static CARAPI OfObject(
        /* [in] */ Float fraction,
        /* [in] */ IInterface* value,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time. The value at this time will be derived
     * from the target object when the animation first starts (note that this implies that keyframes
     * with no initial value must be used as part of an ObjectAnimator).
     * The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @sa ObjectAnimator
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     */
    static CARAPI OfObject(
        /* [in] */ Float fraction,
        /* [out] */ IKeyframe** obj);

    /**
     * Indicates whether this keyframe has a valid value. This method is called internally when
     * an {@link ObjectAnimator} first starts; keyframes without values are assigned values at
     * that time by deriving the value for the property from the target object.
     *
     * @return Boolean Whether this object has a value assigned.
     */
    virtual CARAPI HasValue(
        /* [out] */ Boolean* value);

    /**
     * Gets the value for this Keyframe.
     *
     * @return The value for this Keyframe.
     */
    virtual CARAPI GetValue(
        /* [out] */ IInterface** value) = 0;

    /**
     * Sets the value for this Keyframe.
     *
     * @param value value for this Keyframe.
     */
    virtual CARAPI SetValue(
        /* [in] */ IInterface* value) = 0;

    /**
     * Gets the time for this keyframe, as a fraction of the overall animation duration.
     *
     * @return The time associated with this keyframe, as a fraction of the overall animation
     * duration. This should be a value between 0 and 1.
     */
    virtual CARAPI GetFraction(
        /* [out] */ Float* fraction);

    /**
     * Sets the time for this keyframe, as a fraction of the overall animation duration.
     *
     * @param fraction time associated with this keyframe, as a fraction of the overall animation
     * duration. This should be a value between 0 and 1.
     */
    virtual CARAPI SetFraction(
        /* [in] */ Float fraction);

    /**
     * Gets the optional interpolator for this Keyframe. A value of <code>NULL</code> indicates
     * that there is no interpolation, which is the same as linear interpolation.
     *
     * @return The optional interpolator for this Keyframe.
     */
    virtual CARAPI GetInterpolator(
        /* [out] */ ITimeInterpolator** interpolator);

    /**
     * Sets the optional interpolator for this Keyframe. A value of <code>NULL</code> indicates
     * that there is no interpolation, which is the same as linear interpolation.
     *
     * @return The optional interpolator for this Keyframe.
     */
    virtual CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* interpolator);

    /**
     * Gets the type of keyframe. This information is used by ValueAnimator to determine the type of
     * {@link TypeEvaluator} to use when calculating values between keyframes. The type is based
     * on the type of Keyframe created.
     *
     * @return The type of the value stored in the Keyframe.
     */
    virtual CARAPI GetType(
        /* [out] */ InterfaceID* id);

    virtual CARAPI Clone(
        /* [out] */ IInterface** object) = 0;

protected:
    /**
     * If the Keyframe's value was acquired from the target object, this flag should be set so that,
     * if target changes, value will be reset.
     *
     * @return boolean Whether this Keyframe's value was retieved from the target object or not.
     */
    virtual CARAPI_(Boolean) ValueWasSetOnStart();

    virtual CARAPI_(void) SetValueWasSetOnStart(
        /* [in] */ Boolean valueWasSetOnStart);

protected:
    /**
     * Flag to indicate whether this keyframe has a valid value. This flag is used when an
     * animation first starts, to populate placeholder keyframes with real values derived
     * from the target object.
     */
    Boolean mHasValue;

    /**
     * Flag to indicate whether the value in the keyframe was read from the target object or not.
     * If so, its value will be recalculated if target changes.
     */
    Boolean mValueWasSetOnStart;

    /**
     * The time at which mValue will hold TRUE.
     */
    Float mFraction;

    /**
     * The type of the value in this Keyframe. This type is determined at construction time,
     * based on the type of the <code>value</code> object passed into the constructor.
     */
    InterfaceID mValueType;

private:
    /**
     * The optional time interpolator for the interval preceding this keyframe. A NULL interpolator
     * (the default) results in linear interpolation over the interval.
     */
    AutoPtr<ITimeInterpolator> mInterpolator;

    friend class PropertyValuesHolder;
};

/**
 * This internal subclass is used for all types which are not int or float.
 */
class ObjectKeyframe
    : public Keyframe
    , public IObjectKeyframe
{
public:
    CAR_INTERFACE_DECL()

    ObjectKeyframe(
        /* [in] */ Float fraction,
        /* [in] */ IInterface* value);

    virtual CARAPI GetValue(
        /* [out] */ IInterface** value);

    virtual CARAPI SetValue(
        /* [in] */ IInterface* value);

    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

private:
    /**
     * The value of the animation at the time mFraction.
     */
    AutoPtr<IInterface> mValue;
};

/**
 * Internal subclass used when the keyframe value is of type int.
 */
class Int32Keyframe
    : public Keyframe
    , public IInt32Keyframe
{
public:
    CAR_INTERFACE_DECL()

    Int32Keyframe(
        /* [in] */ Float fraction);

    Int32Keyframe(
        /* [in] */ Float fraction,
        /* [in] */ Int32 value);

    virtual CARAPI GetValue(
        /* [out] */ IInterface** value);

    virtual CARAPI SetValue(
        /* [in] */ IInterface* value);

    virtual CARAPI GetInt32Value(
        /* [in] */ Int32* value);

    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

private:
    /**
     * The value of the animation at the time mFraction.
     */
    AutoPtr<IInteger32> mValue;
};

/**
 * Internal subclass used when the keyframe value is of type float.
 */
class FloatKeyframe
    : public Keyframe
    , public IFloatKeyframe
{
public:
    CAR_INTERFACE_DECL()

    FloatKeyframe(
        /* [in] */ Float fraction);

    FloatKeyframe(
        /* [in] */ Float fraction,
        /* [in] */ Float value);

    virtual CARAPI GetValue(
        /* [out] */ IInterface** value);

    virtual CARAPI SetValue(
        /* [in] */ IInterface* value);

    virtual CARAPI GetFloatValue(
        /* [out] */ Float* value);

    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

private:
    /**
     * The value of the animation at the time mFraction.
     */
    AutoPtr<IFloat> mValue;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_KEYFRAME_H__
