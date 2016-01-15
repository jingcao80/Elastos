
#ifndef  __ELASTOS_DROID_ANIMATION_PROPERTYVALUESHOLDER_H__
#define  __ELASTOS_DROID_ANIMATION_PROPERTYVALUESHOLDER_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/Int32KeyframeSet.h"
#include "elastos/droid/animation/TypeConverter.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Core::INumber;
using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(IClassInfo);

namespace Elastos {
namespace Droid {
namespace Animation {

class PropertyValuesHolder
    : public Object
    , public IPropertyValuesHolder
{
private:
    /**
     * Convert from PointF to float[] for multi-float setters along a Path.
     */
    class PointFToFloatArray
        : public TypeConverter/*<PointF, float[]>*/
    {
    public:
        CAR_INTERFACE_DECL();

        PointFToFloatArray();

        // @Override
        virtual CARAPI Convert(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** values);

    private:
        AutoPtr<ArrayOf<Float> > mCoordinates;
    };

    /**
     * Convert from PointF to int[] for multi-int setters along a Path.
     */
    class PointFToInt32Array
        : public TypeConverter/*<PointF, int[]>*/
    {
    public:
        CAR_INTERFACE_DECL();

        PointFToInt32Array();

        // @Override
        virtual CARAPI Convert(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** values);

    private:
        AutoPtr<ArrayOf<Int32> > mCoordinates;
    };

public:
    typedef HashMap< String, AutoPtr<IMethodInfo> > MethodMap;
    typedef HashMap< AutoPtr<IClassInfo>, AutoPtr<PropertyValuesHolder::MethodMap > > ClassMethodMap;

    CAR_INTERFACE_DECL();

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name and
     * set of Int32 values.
     * @param propertyName The name of the property being animated.
     * @param values The values that the named property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfInt32(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * set of Int32 values.
     * @param property The property being animated. Should not be NULL.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfInt32(
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name and
     * set of <code>int[]</code> values. At least two <code>int[]</code> values must be supplied,
     * a start and end value. If more values are supplied, the values will be animated from the
     * start, through all intermediate values to the end value. When used with ObjectAnimator,
     * the elements of the array represent the parameters of the setter function.
     *
     * @param propertyName The name of the property being animated. Can also be the
     *                     case-sensitive name of the entire setter method. Should not be null.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see IntArrayEvaluator#IntArrayEvaluator(int[])
     * @see ObjectAnimator#ofMultiInt(Object, String, TypeConverter, TypeEvaluator, Object[])
     */
    static CARAPI OfMultiInt32(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<ArrayOf<Int32>* >* values,
        /* [out] */ IPropertyValuesHolder** holder);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name to use
     * as a multi-int setter. The values are animated along the path, with the first
     * parameter of the setter set to the x coordinate and the second set to the y coordinate.
     *
     * @param propertyName The name of the property being animated. Can also be the
     *                     case-sensitive name of the entire setter method. Should not be null.
     *                     The setter must take exactly two <code>int</code> parameters.
     * @param path The Path along which the values should be animated.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see ObjectAnimator#ofPropertyValuesHolder(Object, PropertyValuesHolder...)
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfMultiInt32(
        /* [in] */ const String& propertyName,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * set of Object values for use with ObjectAnimator multi-value setters. The Object
     * values are converted to <code>int[]</code> using the converter.
     *
     * @param propertyName The property being animated or complete name of the setter.
     *                     Should not be null.
     * @param converter Used to convert the animated value to setter parameters.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see ObjectAnimator#ofMultiInt(Object, String, TypeConverter, TypeEvaluator, Object[])
     * @see ObjectAnimator#ofPropertyValuesHolder(Object, PropertyValuesHolder...)
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfMultiInt32(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder object with the specified property name or
     * setter name for use in a multi-int setter function using ObjectAnimator. The values can be
     * of any type, but the type should be consistent so that the supplied
     * {@link android.animation.TypeEvaluator} can be used to to evaluate the animated value. The
     * <code>converter</code> converts the values to parameters in the setter function.
     *
     * <p>At least two values must be supplied, a start and an end value.</p>
     *
     * @param propertyName The name of the property to associate with the set of values. This
     *                     may also be the complete name of a setter function.
     * @param converter    Converts <code>values</code> into int parameters for the setter.
     *                     Can be null if the Keyframes have int[] values.
     * @param evaluator    Used to interpolate between values.
     * @param values       The values at specific fractional times to evaluate between
     * @return A PropertyValuesHolder for a multi-int parameter setter.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfMultiInt32(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IKeyframe*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name and
     * set of <code>float[]</code> values. At least two <code>float[]</code> values must be supplied,
     * a start and end value. If more values are supplied, the values will be animated from the
     * start, through all intermediate values to the end value. When used with ObjectAnimator,
     * the elements of the array represent the parameters of the setter function.
     *
     * @param propertyName The name of the property being animated. Can also be the
     *                     case-sensitive name of the entire setter method. Should not be null.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see FloatArrayEvaluator#FloatArrayEvaluator(float[])
     * @see ObjectAnimator#ofMultiFloat(Object, String, TypeConverter, TypeEvaluator, Object[])
     */
    static CARAPI OfMultiFloat(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<ArrayOf<Float>* >* values,
        /* [out] */ IPropertyValuesHolder** holder);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name to use
     * as a multi-float setter. The values are animated along the path, with the first
     * parameter of the setter set to the x coordinate and the second set to the y coordinate.
     *
     * @param propertyName The name of the property being animated. Can also be the
     *                     case-sensitive name of the entire setter method. Should not be null.
     *                     The setter must take exactly two <code>float</code> parameters.
     * @param path The Path along which the values should be animated.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see ObjectAnimator#ofPropertyValuesHolder(Object, PropertyValuesHolder...)
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfMultiFloat(
        /* [in] */ const String& propertyName,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * set of Object values for use with ObjectAnimator multi-value setters. The Object
     * values are converted to <code>float[]</code> using the converter.
     *
     * @param propertyName The property being animated or complete name of the setter.
     *                     Should not be null.
     * @param converter Used to convert the animated value to setter parameters.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see ObjectAnimator#ofMultiFloat(Object, String, TypeConverter, TypeEvaluator, Object[])
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfMultiFloat(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder object with the specified property name or
     * setter name for use in a multi-float setter function using ObjectAnimator. The values can be
     * of any type, but the type should be consistent so that the supplied
     * {@link android.animation.TypeEvaluator} can be used to to evaluate the animated value. The
     * <code>converter</code> converts the values to parameters in the setter function.
     *
     * <p>At least two values must be supplied, a start and an end value.</p>
     *
     * @param propertyName The name of the property to associate with the set of values. This
     *                     may also be the complete name of a setter function.
     * @param converter    Converts <code>values</code> into float parameters for the setter.
     *                     Can be null if the Keyframes have float[] values.
     * @param evaluator    Used to interpolate between values.
     * @param values       The values at specific fractional times to evaluate between
     * @return A PropertyValuesHolder for a multi-float parameter setter.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfMultiFloat(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IKeyframe*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name and
     * a Path along which the values should be animated. This variant supports a
     * <code>TypeConverter</code> to convert from <code>PointF</code> to the target
     * type.
     *
     * <p>The PointF passed to <code>converter</code> or <code>property</code>, if
     * <code>converter</code> is <code>null</code>, is reused on each animation frame and should
     * not be stored by the setter or TypeConverter.</p>
     *
     * @param propertyName The name of the property being animated.
     * @param converter Converts a PointF to the type associated with the setter. May be
     *                  null if conversion is unnecessary.
     * @param path The Path along which the values should be animated.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfObject(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * set of Object values. This variant also takes a TypeEvaluator because the system
     * cannot automatically interpolate between objects of unknown type. This variant also
     * takes a <code>TypeConverter</code> to convert from animated values to the type
     * of the property. If only one value is supplied, the <code>TypeConverter</code>
     * must be a {@link android.animation.BidirectionalTypeConverter} to retrieve the current
     * value.
     *
     * @param property The property being animated. Should not be null.
     * @param converter Converts the animated object to the Property type.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     * @see #setConverter(TypeConverter)
     * @see TypeConverter
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfObject(
        /* [in] */ IProperty* property,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * a Path along which the values should be animated. This variant supports a
     * <code>TypeConverter</code> to convert from <code>PointF</code> to the target
     * type.
     *
     * <p>The PointF passed to <code>converter</code> or <code>property</code>, if
     * <code>converter</code> is <code>null</code>, is reused on each animation frame and should
     * not be stored by the setter or TypeConverter.</p>
     *
     * @param property The property being animated. Should not be null.
     * @param converter Converts a PointF to the type associated with the setter. May be
     *                  null if conversion is unnecessary.
     * @param path The Path along which the values should be animated.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfObject(
        /* [in] */ IProperty* property,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ IPath* path);

    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfKeyframes(
        /* [in] */ const String& propertyName,
        /* [in] */ IKeyframes* keyframes);

    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfKeyframes(
        /* [in] */ IProperty* property,
        /* [in] */ IKeyframes* keyframes);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name and
     * set of Float values.
     * @param propertyName The name of the property being animated.
     * @param values The values that the named property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfFloat(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * set of Float values.
     * @param property The property being animated. Should not be NULL.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfFloat(
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property name and
     * set of Object values. This variant also takes a TypeEvaluator because the system
     * cannot automatically interpolate between objects of unknown type.
     *
     * @param propertyName The name of the property being animated.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values The values that the named property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfObject(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder with a given property and
     * set of Object values. This variant also takes a TypeEvaluator because the system
     * cannot automatically interpolate between objects of unknown type.
     *
     * @param property The property being animated. Should not be NULL.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values The values that the property will animate between.
     * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfObject(
        /* [in] */ IProperty* property,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder object with the specified property name and set
     * of values. These values can be of any type, but the type should be consistent so that
     * an appropriate {@link android.animation.TypeEvaluator} can be found that matches
     * the common type.
     * <p>If there is only one value, it is assumed to be the end value of an animation,
     * and an initial value will be derived, if possible, by calling a getter function
     * on the object. Also, if any value is NULL, the value will be filled in when the animation
     * starts in the same way. This mechanism of automatically getting NULL values only works
     * if the PropertyValuesHolder object is used in conjunction
     * {@link ObjectAnimator}, and with a getter function
     * derived automatically from <code>propertyName</code>, since otherwise PropertyValuesHolder has
     * no way of determining what the value should be.
     * @param propertyName The name of the property associated with this set of values. This
     * can be the actual property name to be used when using a ObjectAnimator object, or
     * just a name used to get animated values, such as if this object is used with an
     * ValueAnimator object.
     * @param values The set of values to animate between.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfKeyframe(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<IKeyframe*>* values);

    /**
     * Constructs and returns a PropertyValuesHolder object with the specified property and set
     * of values. These values can be of any type, but the type should be consistent so that
     * an appropriate {@link android.animation.TypeEvaluator} can be found that matches
     * the common type.
     * <p>If there is only one value, it is assumed to be the end value of an animation,
     * and an initial value will be derived, if possible, by calling the property's
     * {@link android.util.Property#get(Object)} function.
     * Also, if any value is NULL, the value will be filled in when the animation
     * starts in the same way. This mechanism of automatically getting NULL values only works
     * if the PropertyValuesHolder object is used in conjunction with
     * {@link ObjectAnimator}, since otherwise PropertyValuesHolder has
     * no way of determining what the value should be.
     * @param property The property associated with this set of values. Should not be NULL.
     * @param values The set of values to animate between.
     */
    static CARAPI_(AutoPtr<IPropertyValuesHolder>) OfKeyframe(
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<IKeyframe*>* values);

    /**
     * Set the animated values for this object to this set of ints.
     * If there is only one value, it is assumed to be the end value of an animation,
     * and an initial value will be derived, if possible, by calling a getter function
     * on the object. Also, if any value is NULL, the value will be filled in when the animation
     * starts in the same way. This mechanism of automatically getting NULL values only works
     * if the PropertyValuesHolder object is used in conjunction
     * {@link ObjectAnimator}, and with a getter function
     * derived automatically from <code>propertyName</code>, since otherwise PropertyValuesHolder has
     * no way of determining what the value should be.
     *
     * @param values One or more values that the animation will animate between.
     */
    virtual CARAPI SetInt32Values(
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Set the animated values for this object to this set of floats.
     * If there is only one value, it is assumed to be the end value of an animation,
     * and an initial value will be derived, if possible, by calling a getter function
     * on the object. Also, if any value is NULL, the value will be filled in when the animation
     * starts in the same way. This mechanism of automatically getting NULL values only works
     * if the PropertyValuesHolder object is used in conjunction
     * {@link ObjectAnimator}, and with a getter function
     * derived automatically from <code>propertyName</code>, since otherwise PropertyValuesHolder has
     * no way of determining what the value should be.
     *
     * @param values One or more values that the animation will animate between.
     */
    virtual CARAPI SetFloatValues(
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Set the animated values for this object to this set of Keyframes.
     *
     * @param values One or more values that the animation will animate between.
     */
    virtual CARAPI SetKeyframes(
        /* [in] */ ArrayOf<IKeyframe*>* values);

    /**
     * Set the animated values for this object to this set of Objects.
     * If there is only one value, it is assumed to be the end value of an animation,
     * and an initial value will be derived, if possible, by calling a getter function
     * on the object. Also, if any value is NULL, the value will be filled in when the animation
     * starts in the same way. This mechanism of automatically getting NULL values only works
     * if the PropertyValuesHolder object is used in conjunction
     * {@link ObjectAnimator}, and with a getter function
     * derived automatically from <code>propertyName</code>, since otherwise PropertyValuesHolder has
     * no way of determining what the value should be.
     *
     * @param values One or more values that the animation will animate between.
     */
    virtual CARAPI SetObjectValues(
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Sets the converter to convert from the values type to the setter's parameter type.
     * If only one value is supplied, <var>converter</var> must be a
     * {@link android.animation.BidirectionalTypeConverter}.
     * @param converter The converter to use to convert values.
     */
    virtual CARAPI SetConverter(
        /* [in] */ ITypeConverter* converter);

    /**
     * Utility function to get the setter from targetClass
     * @param targetClass The Class on which the requested method should exist.
     */
    virtual CARAPI SetupSetter(
        /* [in] */ IInterface* targetClass);

    /**
     * Internal function (called from ObjectAnimator) to set up the setter and getter
     * prior to running the animation. If the setter has not been manually set for this
     * object, it will be derived automatically given the property name, target object, and
     * types of values supplied. If no getter has been set, it will be supplied iff any of the
     * supplied values was NULL. If there is a NULL value, then the getter (supplied or derived)
     * will be called to set those NULL values to the current value of the property
     * on the target object.
     * @param target The object on which the setter (and possibly getter) exist.
     */
    virtual CARAPI SetupSetterAndGetter(
        /* [in] */ IInterface* target);

    /**
     * This function is called by ObjectAnimator when setting the start values for an animation.
     * The start values are set according to the current values in the target object. The
     * property whose value is extracted is whatever is specified by the propertyName of this
     * PropertyValuesHolder object.
     *
     * @param target The object which holds the start values that should be set.
     */
    virtual CARAPI SetupStartValue(
        /* [in] */ IInterface* target);

    /**
     * This function is called by ObjectAnimator when setting the end values for an animation.
     * The end values are set according to the current values in the target object. The
     * property whose value is extracted is whatever is specified by the propertyName of this
     * PropertyValuesHolder object.
     *
     * @param target The object which holds the start values that should be set.
     */
    virtual CARAPI SetupEndValue(
        /* [in] */ IInterface* target);

    CARAPI CloneSuperData(
        /* [in] */ PropertyValuesHolder* holder);
    /**
     * Internal function to set the value on the target object, using the setter set up
     * earlier on this PropertyValuesHolder object. This function is called by ObjectAnimator
     * to handle turning the value calculated by ValueAnimator into a value set on the object
     * according to the name of the property.
     * @param target The target object on which the value is set
     */
    virtual CARAPI SetAnimatedValue(
        /* [in] */ IInterface* target);

    /**
     * Internal function, called by ValueAnimator, to set up the TypeEvaluator that will be used
     * to calculate animated values.
     */
    virtual CARAPI Init();

    /**
     * The TypeEvaluator will the automatically determined based on the type of values
     * supplied to PropertyValuesHolder. The evaluator can be manually set, however, if so
     * desired. This may be important in cases where either the type of the values supplied
     * do not match the way that they should be interpolated between, or if the values
     * are of a custom type or one not currently understood by the animation system. Currently,
     * only values of type Float and Int32 (and their Object equivalents: Float
     * and Integer) are  correctly interpolated; all other types require setting a TypeEvaluator.
     * @param evaluator
     */
    virtual CARAPI SetEvaluator(
        /* [in] */ ITypeEvaluator* evaluator);

    /**
     * Function used to calculate the value according to the evaluator set up for
     * this PropertyValuesHolder object. This function is called by ValueAnimator.animateValue().
     *
     * @param fraction The elapsed, interpolated fraction of the animation.
     */
    virtual CARAPI CalculateValue(
        /* [in] */ Float fraction);

    /**
     * Sets the name of the property that will be animated. This name is used to derive
     * a setter function that will be called to set animated values.
     * For example, a property name of <code>foo</code> will result
     * in a call to the function <code>setFoo()</code> on the target object. If either
     * <code>valueFrom</code> or <code>valueTo</code> is NULL, then a getter function will
     * also be derived and called.
     *
     * <p>Note that the setter function derived from this property name
     * must take the same parameter type as the
     * <code>valueFrom</code> and <code>valueTo</code> properties, otherwise the call to
     * the setter function will fail.</p>
     *
     * @param propertyName The name of the property being animated.
     */
    virtual CARAPI SetPropertyName(
        /* [in] */ const String& propertyName);

    /**
     * Sets the property that will be animated.
     *
     * <p>Note that if this PropertyValuesHolder object is used with ObjectAnimator, the property
     * must exist on the target object specified in that ObjectAnimator.</p>
     *
     * @param property The property being animated.
     */
    virtual CARAPI SetProperty(
        /* [in] */ IProperty* property);

    /**
     * Gets the name of the property that will be animated. This name will be used to derive
     * a setter function that will be called to set animated values.
     * For example, a property name of <code>foo</code> will result
     * in a call to the function <code>setFoo()</code> on the target object. If either
     * <code>valueFrom</code> or <code>valueTo</code> is NULL, then a getter function will
     * also be derived and called.
     */
    virtual CARAPI GetPropertyName(
        /* [out] */ String* name);

    /**
     * Internal function, called by ValueAnimator and ObjectAnimator, to retrieve the value
     * most recently calculated in calculateValue().
     * @return
     */
    virtual CARAPI GetAnimatedValue(
        /* [out] */ IInterface** value);

    CARAPI Clone(
        /* [out] */ IInterface** holder);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Utility method to derive a setter/getter method name from a property name, where the
     * prefix is typically "set" or "get" and the first letter of the property name is
     * capitalized.
     *
     * @param prefix The precursor to the method name, before the property name begins, typically
     * "set" or "get".
     * @param propertyName The name of the property that represents the bulk of the method name
     * after the prefix. The first letter of this word will be capitalized in the resulting
     * method name.
     * @return String the property name converted to a method name according to the conventions
     * specified above.
     */
    static CARAPI_(String) GetMethodName(
        /* [in] */ const String& prefix,
        /* [in] */ const String& propertyName);

    static CARAPI_(AutoPtr<IClassInfo>) TransformClassInfo(
        /* [in] */ IInterface* o);

protected:
    /**
     * Internal utility constructor, used by the factory methods to set the property name.
     * @param propertyName The name of the property for this holder.
     */
    PropertyValuesHolder(
        /* [in] */ const String& propertyName);

    /**
     * Internal utility constructor, used by the factory methods to set the property.
     * @param property The property for this holder.
     */
    PropertyValuesHolder(
        /* [in] */ IProperty* property);

    //The function will help you transform from a object to classInfo

    static CARAPI_(AutoPtr<IMethodInfo>) nGetInt32Method(
        /* [in] */ IInterface* targetClass,
        /* [in] */ const String& methodName);

    static CARAPI_(AutoPtr<IMethodInfo>) nGetFloatMethod(
        /* [in] */ IInterface* targetClass,
        /* [in] */ const String& methodName);

    static CARAPI_(AutoPtr<IMethodInfo>) nGetMultipleInt32Method(
        /* [in] */ IInterface* targetClass,
        /* [in] */ const String& methodName,
        /* [in] */ Int32 numParams);

    static CARAPI_(AutoPtr<IMethodInfo>) nGetMultipleFloatMethod(
        /* [in] */ IInterface* targetClass,
        /* [in] */ const String& methodName,
        /* [in] */ Int32 numParams);

    static CARAPI_(void) nCallInt32Method(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ Int32 arg);

    static CARAPI_(void) nCallFloatMethod(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ Float arg);

    static CARAPI_(void) nCallTwoInt32Method(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    static CARAPI_(void) nCallFourInt32Method(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Int32 arg3,
        /* [in] */ Int32 arg4);

    static CARAPI_(void) nCallMultipleInt32Method(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ ArrayOf<Int32>* args);

    static CARAPI_(void) nCallTwoFloatMethod(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ Float arg1,
        /* [in] */ Float arg2);

    static CARAPI_(void) nCallFourFloatMethod(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ Float arg1,
        /* [in] */ Float arg2,
        /* [in] */ Float arg3,
        /* [in] */ Float arg4);

    static CARAPI_(void) nCallMultipleFloatMethod(
        /* [in] */ IInterface* target,
        /* [in] */ IMethodInfo* methodID,
        /* [in] */ ArrayOf<Float>* args);

private:
    /**
     * Determine the setter or getter function using the JavaBeans convention of setFoo or
     * getFoo for a property named 'foo'. This function figures out what the name of the
     * function should be and uses reflection to find the Method with that name on the
     * target object.
     *
     * @param targetClass The class to search for the method
     * @param prefix "set" or "get", depending on whether we need a setter or getter.
     * @param valueType The type of the parameter (in the case of a setter). This type
     * is derived from the values set on this PropertyValuesHolder. This type is used as
     * a first guess at the parameter type, but we check for methods with several different
     * types to avoid problems with slight mis-matches between supplied values and actual
     * value types used on the setter.
     * @return Method the method associated with mPropertyName.
     */

    //TODO
    AutoPtr<IMethodInfo> GetPropertyFunction(
        /* [in] */ IClassInfo* targetClass,
        /* [in] */ const String& prefix,
        /* [in] */ InterfaceID* type);

    /**
     * Returns the setter or getter requested. This utility function checks whether the
     * requested method exists in the propertyMapMap cache. If not, it calls another
     * utility function to request the Method from the targetClass directly.
     * @param targetClass The Class on which the requested method should exist.
     * @param propertyMapMap The cache of setters/getters derived so far.
     * @param prefix "set" or "get", for the setter or getter.
     * @param valueType The type of parameter passed into the method (NULL for getter).
     * @return Method the method associated with mPropertyName.
     */

    AutoPtr<IMethodInfo> SetupSetterOrGetter(
        /* [in] */ IClassInfo* targetClass,
        /* [in] */ PropertyValuesHolder::ClassMethodMap * propertyMapMap,
        /* [in] */ const String& prefix,
        /* [in] */ InterfaceID* type);

    /**
     * Utility function to get the getter from targetClass
     */

    //TODO
    CARAPI SetupGetter(
        /* [in] */ IInterface* target);

    /**
     * Utility function to set the value stored in a particular Keyframe. The value used is
     * whatever the value is for the property name specified in the keyframe on the target object.
     *
     * @param target The target object from which the current value should be extracted.
     * @param kf The keyframe which holds the property name and value.
     */

    //TODO
    CARAPI SetupValue(
        /* [in] */ IInterface* target,
        /* [in] */ IKeyframe* kf);

    virtual CARAPI ConvertBack(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** outValue);

private:
    static CARAPI_(AutoPtr<ITypeEvaluator>) InitInt32Evaluator();

    static CARAPI_(AutoPtr<ITypeEvaluator>) InitFloatEvaluator();

protected:
    /**
     * The name of the property associated with the values. This need not be a real property,
     * unless this object is being used with ObjectAnimator. But this is the name by which
     * aniamted values are looked up with getAnimatedValue(String) in ValueAnimator.
     */
    String mPropertyName;

    /**
     * @hide
     */
    AutoPtr<IProperty> mProperty;

    /**
     * The setter function, if needed. ObjectAnimator hands off this functionality to
     * PropertyValuesHolder, since it holds all of the per-property information. This
     * property is automatically
     * derived when the animation starts in setupSetterAndGetter() if using ObjectAnimator.
     */
    AutoPtr<IMethodInfo> mSetter;

    /**
     * The type of values supplied. This information is used both in deriving the setter/getter
     * functions and in deriving the type of TypeEvaluator.
     */
    InterfaceID mValueType;

    /**
     * The set of keyframes (time/value pairs) that define this animation.
     */
    AutoPtr<IKeyframes> mKeyframes;

    // This lock is used to ensure that only one thread is accessing the property maps
    // at a time.
    //ReentrantReadWriteLock mPropertyMapLock = new ReentrantReadWriteLock();
    Object mPropertyMapLock;

    // Used to pass single value to varargs parameter in setter invocation
    //final Object[] mTmpValueArray = new Object[1];
    AutoPtr<ArrayOf<IInterface*> > mTmpValueArray;

private:
    /**
     * The getter function, if needed. ObjectAnimator hands off this functionality to
     * PropertyValuesHolder, since it holds all of the per-property information. This
     * property is automatically
     * derived when the animation starts in setupSetterAndGetter() if using ObjectAnimator.
     * The getter is only derived and used if one of the values is NULL.
     */
    AutoPtr<IMethodInfo> mGetter;

    // type evaluators for the primitive types handled by this implementation
    static AutoPtr<ITypeEvaluator> sInt32Evaluator;
    static AutoPtr<ITypeEvaluator> sFloatEvaluator;

    // We try several different types when searching for appropriate setter/getter functions.
    // The caller may have supplied values in a type that does not match the setter/getter
    // functions (such as the integers 0 and 1 to represent floating point values for alpha).
    // Also, the use of generics in constructors means that we end up with the Object versions
    // of primitive types (Float vs. Float). But most likely, the setter/getter functions
    // will take primitive types instead.
    // So we supply an ordered array of other types to try before giving up.
    static Int32 FLOAT_VARIANTS[6];
    static Int32 INTEGER_VARIANTS[6];
    static Int32 DOUBLE_VARIANTS[6];

    //TODO
    // These maps hold all property entries for a particular class. This map
    // is used to speed up property/setter/getter lookups for a given class/property
    // combination. No need to use reflection on the combination more than once.
    //TODO
    static AutoPtr< ClassMethodMap > sSetterPropertyMap;
    static AutoPtr< ClassMethodMap > sGetterPropertyMap;

    /**
     * The type evaluator used to calculate the animated values. This evaluator is determined
     * automatically based on the type of the start/end objects passed into the constructor,
     * but the system only knows about the primitive types Int32 and Float. Any other
     * type will need to set the evaluator to a custom evaluator for that type.
     */
    AutoPtr<ITypeEvaluator> mEvaluator;

    /**
     * The value most recently calculated by calculateValue(). This is set during
     * that function and might be retrieved later either by ValueAnimator.animatedValue() or
     * by the property-setting logic in ObjectAnimator.animatedValue().
     */
    AutoPtr<IInterface> mAnimatedValue;

    /**
     * Converts from the source Object type to the setter Object type.
     */
    AutoPtr<ITypeConverter> mConverter;

    friend class CLayoutTransition;
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_PROPERTYVALUESHOLDER_H__
