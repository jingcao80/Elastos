//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_ANIMATION_OBJECTANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_OBJECTANIMATOR_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/ValueAnimator.h"

using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Graphics::IPath;

namespace Elastos {
namespace Droid {
namespace Animation {

class ObjectAnimator
    : public ValueAnimator
    , public IObjectAnimator
{
public:
    CAR_INTERFACE_DECL()

    ObjectAnimator();

    CARAPI constructor();

    /**
     * Sets the name of the property that will be animated. This name is used to derive
     * a setter function that will be called to set animated values.
     * For example, a property name of <code>foo</code> will result
     * in a call to the function <code>setFoo()</code> on the target object. If either
     * <code>valueFrom</code> or <code>valueTo</code> is NULL, then a getter function will
     * also be derived and called.
     *
     * <p>For best performance of the mechanism that calls the setter function determined by the
     * name of the property being animated, use <code>Float</code> or <code>Int32</code> typed values,
     * and make the setter function for those properties have a <code>void</code> return value. This
     * will cause the code to take an optimized path for these constrained circumstances. Other
     * property types and return types will work, but will have more overhead in processing
     * the requests due to normal reflection mechanisms.</p>
     *
     * <p>Note that the setter function derived from this property name
     * must take the same parameter type as the
     * <code>valueFrom</code> and <code>valueTo</code> properties, otherwise the call to
     * the setter function will fail.</p>
     *
     * <p>If this ObjectAnimator has been set up to animate several properties together,
     * using more than one PropertyValuesHolder objects, then setting the propertyName simply
     * sets the propertyName in the first of those PropertyValuesHolder objects.</p>
     *
     * @param propertyName The name of the property being animated. Should not be NULL.
     */
    virtual CARAPI SetPropertyName(
        /* [in] */ const String& propertyName);

    /**
     * Sets the property that will be animated. Property objects will take precedence over
     * properties specified by the {@link #setPropertyName(String)} method. Animations should
     * be set up to use one or the other, not both.
     *
     * @param property The property being animated. Should not be NULL.
     */
    virtual CARAPI SetProperty(
        /* [in] */ IProperty* property);

    /**
     * Gets the name of the property that will be animated. This name will be used to derive
     * a setter function that will be called to set animated values.
     * For example, a property name of <code>foo</code> will result
     * in a call to the function <code>setFoo()</code> on the target object. If either
     * <code>valueFrom</code> or <code>valueTo</code> is null, then a getter function will
     * also be derived and called.
     *
     * <p>If this animator was created with a {@link Property} object instead of the
     * string name of a property, then this method will return the {@link
     * Property#getName() name} of that Property object instead. If this animator was
     * created with one or more {@link PropertyValuesHolder} objects, then this method
     * will return the {@link PropertyValuesHolder#getPropertyName() name} of that
     * object (if there was just one) or a comma-separated list of all of the
     * names (if there are more than one).</p>
     */
    virtual CARAPI GetPropertyName(
        /* [out] */ String* name);

    // @Override
    CARAPI_(String) GetNameForTrace();

    /**
     * Constructs and returns an ObjectAnimator that animates between Int32 values. A single
     * value implies that that value is the one being animated to. Two values imply a starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated. This object should
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter.
     * @param propertyName The name of the property being animated.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates coordinates along a <code>Path</code>
     * using two properties. A <code>Path</code></> animation moves in two dimensions, animating
     * coordinates <code>(x, y)</code> together to follow the line. In this variation, the
     * coordinates are integers that are set to separate properties designated by
     * <code>xPropertyName</code> and <code>yPropertyName</code>.
     *
     * @param target The object whose properties are to be animated. This object should
     *               have public methods on it called <code>setNameX()</code> and
     *               <code>setNameY</code>, where <code>nameX</code> and <code>nameY</code>
     *               are the value of <code>xPropertyName</code> and <code>yPropertyName</code>
     *               parameters, respectively.
     * @param xPropertyName The name of the property for the x coordinate being animated.
     * @param yPropertyName The name of the property for the y coordinate being animated.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfInt32(
        /* [in] */ IObject* target,
        /* [in] */ const String& xPropertyName,
        /* [in] */ const String& yPropertyName,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates between int values. A single
     * value implies that that value is the one being animated to. Two values imply a starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates coordinates along a <code>Path</code>
     * using two properties.  A <code>Path</code></> animation moves in two dimensions, animating
     * coordinates <code>(x, y)</code> together to follow the line. In this variation, the
     * coordinates are integers that are set to separate properties, <code>xProperty</code> and
     * <code>yProperty</code>.
     *
     * @param target The object whose properties are to be animated.
     * @param xProperty The property for the x coordinate being animated.
     * @param yProperty The property for the y coordinate being animated.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* xProperty,
        /* [in] */ IProperty* yProperty,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates over int values for a multiple
     * parameters setter. Only public methods that take only int parameters are supported.
     * Each <code>int[]</code> contains a complete set of parameters to the setter method.
     * At least two <code>int[]</code> values must be provided, a start and end. More than two
     * values imply a starting value, values to animate through along the way, and an ending
     * value (these values will be distributed evenly across the duration of the animation).
     *
     * @param target The object whose property is to be animated. This object may
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter. <code>propertyName</code> may also
     * be the case-sensitive complete name of the public setter method.
     * @param propertyName The name of the property being animated or the name of the setter method.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfMultiInt32(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<ArrayOf<Int32>* >* values);

    /**
     * Constructs and returns an ObjectAnimator that animates the target using a multi-int setter
     * along the given <code>Path</code>. A <code>Path</code></> animation moves in two dimensions,
     * animating coordinates <code>(x, y)</code> together to follow the line. In this variation, the
     * coordinates are integer x and y coordinates used in the first and second parameter of the
     * setter, respectively.
     *
     * @param target The object whose property is to be animated. This object may
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter. <code>propertyName</code> may also
     * be the case-sensitive complete name of the public setter method.
     * @param propertyName The name of the property being animated or the name of the setter method.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfMultiInt32(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates over values for a multiple int
     * parameters setter. Only public methods that take only int parameters are supported.
     * <p>At least two values must be provided, a start and end. More than two
     * values imply a starting value, values to animate through along the way, and an ending
     * value (these values will be distributed evenly across the duration of the animation).</p>
     *
     * @param target The object whose property is to be animated. This object may
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter. <code>propertyName</code> may also
     * be the case-sensitive complete name of the public setter method.
     * @param propertyName The name of the property being animated or the name of the setter method.
     * @param converter Converts T objects into int parameters for the multi-value setter.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfMultiInt32(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates between color values. A single
     * value implies that that value is the one being animated to. Two values imply starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated. This object should
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter.
     * @param propertyName The name of the property being animated.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfArgb(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates between color values. A single
     * value implies that that value is the one being animated to. Two values imply starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfArgb(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates between Float values. A single
     * value implies that that value is the one being animated to. Two values imply a starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated. This object should
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter.
     * @param propertyName The name of the property being animated.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates coordinates along a <code>Path</code>
     * using two properties. A <code>Path</code></> animation moves in two dimensions, animating
     * coordinates <code>(x, y)</code> together to follow the line. In this variation, the
     * coordinates are floats that are set to separate properties designated by
     * <code>xPropertyName</code> and <code>yPropertyName</code>.
     *
     * @param target The object whose properties are to be animated. This object should
     *               have public methods on it called <code>setNameX()</code> and
     *               <code>setNameY</code>, where <code>nameX</code> and <code>nameY</code>
     *               are the value of the <code>xPropertyName</code> and <code>yPropertyName</code>
     *               parameters, respectively.
     * @param xPropertyName The name of the property for the x coordinate being animated.
     * @param yPropertyName The name of the property for the y coordinate being animated.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& xPropertyName,
        /* [in] */ const String& yPropertyName,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates between float values. A single
     * value implies that that value is the one being animated to. Two values imply a starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* propertyName,
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates coordinates along a <code>Path</code>
     * using two properties. A <code>Path</code></> animation moves in two dimensions, animating
     * coordinates <code>(x, y)</code> together to follow the line. In this variation, the
     * coordinates are floats that are set to separate properties, <code>xProperty</code> and
     * <code>yProperty</code>.
     *
     * @param target The object whose properties are to be animated.
     * @param xProperty The property for the x coordinate being animated.
     * @param yProperty The property for the y coordinate being animated.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* xProperty,
        /* [in] */ IProperty* yProperty,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates over float values for a multiple
     * parameters setter. Only public methods that take only float parameters are supported.
     * Each <code>float[]</code> contains a complete set of parameters to the setter method.
     * At least two <code>float[]</code> values must be provided, a start and end. More than two
     * values imply a starting value, values to animate through along the way, and an ending
     * value (these values will be distributed evenly across the duration of the animation).
     *
     * @param target The object whose property is to be animated. This object may
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter. <code>propertyName</code> may also
     * be the case-sensitive complete name of the public setter method.
     * @param propertyName The name of the property being animated or the name of the setter method.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfMultiFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<ArrayOf<Float>*>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates the target using a multi-float setter
     * along the given <code>Path</code>. A <code>Path</code></> animation moves in two dimensions,
     * animating coordinates <code>(x, y)</code> together to follow the line. In this variation, the
     * coordinates are float x and y coordinates used in the first and second parameter of the
     * setter, respectively.
     *
     * @param target The object whose property is to be animated. This object may
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter. <code>propertyName</code> may also
     * be the case-sensitive complete name of the public setter method.
     * @param propertyName The name of the property being animated or the name of the setter method.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfMultiFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates over values for a multiple float
     * parameters setter. Only public methods that take only float parameters are supported.
     * <p>At least two values must be provided, a start and end. More than two
     * values imply a starting value, values to animate through along the way, and an ending
     * value (these values will be distributed evenly across the duration of the animation).</p>
     *
     * @param target The object whose property is to be animated. This object may
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter. <code>propertyName</code> may also
     * be the case-sensitive complete name of the public setter method.
     * @param propertyName The name of the property being animated or the name of the setter method.
     * @param converter Converts T objects into float parameters for the multi-value setter.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfMultiFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates between Object values. A single
     * value implies that that value is the one being animated to. Two values imply a starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated. This object should
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter.
     * @param propertyName The name of the property being animated.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static AutoPtr<IObjectAnimator> OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates a property along a <code>Path</code>.
     * A <code>Path</code></> animation moves in two dimensions, animating coordinates
     * <code>(x, y)</code> together to follow the line. This variant animates the coordinates
     * in a <code>PointF</code> to follow the <code>Path</code>. If the <code>Property</code>
     * associated with <code>propertyName</code> uses a type other than <code>PointF</code>,
     * <code>converter</code> can be used to change from <code>PointF</code> to the type
     * associated with the <code>Property</code>.
     *
     * @param target The object whose property is to be animated. This object should
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter.
     * @param propertyName The name of the property being animated.
     * @param converter Converts a PointF to the type associated with the setter. May be
     *                  null if conversion is unnecessary.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    // @NonNull
    static CARAPI_(AutoPtr<IObjectAnimator>) OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates between Object values. A single
     * value implies that that value is the one being animated to. Two values imply a starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation).
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static AutoPtr<IObjectAnimator> OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates between Object values. A single
     * value implies that that value is the one being animated to. Two values imply starting
     * and ending values. More than two values imply a starting value, values to animate through
     * along the way, and an ending value (these values will be distributed evenly across
     * the duration of the animation). This variant supplies a <code>TypeConverter</code> to
     * convert from the animated values to the type of the property. If only one value is
     * supplied, the <code>TypeConverter</code> must be a
     * {@link android.animation.BidirectionalTypeConverter} to retrieve the current value.
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated.
     * @param converter Converts the animated object to the Property type.
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the necessary interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    // @NonNull
    static CARAPI_(AutoPtr<IObjectAnimator>) OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Constructs and returns an ObjectAnimator that animates a property along a <code>Path</code>.
     * A <code>Path</code></> animation moves in two dimensions, animating coordinates
     * <code>(x, y)</code> together to follow the line. This variant animates the coordinates
     * in a <code>PointF</code> to follow the <code>Path</code>. If <code>property</code>
     * uses a type other than <code>PointF</code>, <code>converter</code> can be used to change
     * from <code>PointF</code> to the type associated with the <code>Property</code>.
     *
     * <p>The PointF passed to <code>converter</code> or <code>property</code>, if
     * <code>converter</code> is <code>null</code>, is reused on each animation frame and should
     * not be stored by the setter or TypeConverter.</p>
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated. Should not be null.
     * @param converter Converts a PointF to the type associated with the setter. May be
     *                  null if conversion is unnecessary.
     * @param path The <code>Path</code> to animate values along.
     * @return An ObjectAnimator object that is set up to animate along <code>path</code>.
     */
    // @NonNull
    static CARAPI_(AutoPtr<IObjectAnimator>) OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ IPath* path);

    /**
     * Constructs and returns an ObjectAnimator that animates between the sets of values specified
     * in <code>PropertyValueHolder</code> objects. This variant should be used when animating
     * several properties at once with the same ObjectAnimator, since PropertyValuesHolder allows
     * you to associate a set of animation values with a property name.
     *
     * @param target The object whose property is to be animated. Depending on how the
     * PropertyValuesObjects were constructed, the target object should either have the {@link
     * android.util.Property} objects used to construct the PropertyValuesHolder objects or (if the
     * PropertyValuesHOlder objects were created with property names) the target object should have
     * public methods on it called <code>setName()</code>, where <code>name</code> is the name of
     * the property passed in as the <code>propertyName</code> parameter for each of the
     * PropertyValuesHolder objects.
     * @param values A set of PropertyValuesHolder objects whose values will be animated between
     * over time.
     * @return An ObjectAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IObjectAnimator>) OfPropertyValuesHolder(
        /* [in] */ IInterface* target,
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values);

    //@Override
    CARAPI SetInt32Values(
        /* [in] */ ArrayOf<Int32>* values);

    //@Override
    CARAPI SetFloatValues(
        /* [in] */ ArrayOf<Float>* values);

    //@Override
    CARAPI SetObjectValues(
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * autoCancel controls whether an ObjectAnimator will be canceled automatically
     * when any other ObjectAnimator with the same target and properties is started.
     * Setting this flag may make it easier to run different animators on the same target
     * object without having to keep track of whether there are conflicting animators that
     * need to be manually canceled. Canceling animators must have the same exact set of
     * target properties, in the same order.
     *
     * @param cancel Whether future ObjectAnimators with the same target and properties
     * as this ObjectAnimator will cause this ObjectAnimator to be canceled.
     */
    virtual CARAPI SetAutoCancel(
        /* [in] */ Boolean cancel);

    //@Override
    CARAPI Start();

    /**
     * This function is called immediately before processing the first animation
     * frame of an animation. If there is a nonzero <code>startDelay</code>, the
     * function is called after that delay ends.
     * It takes care of the final initialization steps for the
     * animation. This includes setting mEvaluator, if the user has not yet
     * set it up, and the setter/getter methods, if the user did not supply
     * them.
     *
     *  <p>Overriders of this method should call the superclass method to cause
     *  internal mechanisms to be set up correctly.</p>
     */
    //@Override
    CARAPI InitAnimation();

    /**
     * Sets the length of the animation. The default duration is 300 milliseconds.
     *
     * @param duration The length of the animation, in milliseconds.
     * @return ObjectAnimator The object called with setDuration(). This return
     * value makes it easier to compose statements together that construct and then set the
     * duration, as in
     * <code>ObjectAnimator.ofInt(target, propertyName, 0, 10).setDuration(500).start()</code>.
     */
    //@Override
    CARAPI SetDuration(
        /* [in] */ Int64 duration);

    /**
     * The target object whose property will be animated by this animation
     *
     * @return The object being animated
     */
    CARAPI GetTarget(
        /* [out] */ IInterface** object);

    /**
     * Sets the target object whose property will be animated by this animation
     *
     * @param target The object being animated
     */
    //@Override
    CARAPI SetTarget(
        /*[in] */ IInterface* target);

    //@Override
    CARAPI SetupStartValues();

    //@Override
    CARAPI SetupEndValues();

    //@Override
    CARAPI AnimateValue(
        /* [in] */ Float fraction);

    //@Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Private utility constructor that initializes the target object and name of the
     * property being animated.
     *
     * @param target The object whose property is to be animated. This object should
     * have a public method on it called <code>setName()</code>, where <code>name</code> is
     * the value of the <code>propertyName</code> parameter.
     * @param propertyName The name of the property being animated.
     */
    CARAPI constructor(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName);

    /**
     * Private utility constructor that initializes the target object and property being animated.
     *
     * @param target The object whose property is to be animated.
     * @param property The property being animated.
     */
    CARAPI constructor(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property);

    CARAPI_(Boolean) HasSameTargetAndProperties(
        /* [in] */ IAnimator* anim);

    CARAPI CloneImpl(
        /* [in] */ IObjectAnimator* anim);

private:
    static const String LOGTAG;

    static const Boolean DBG;

    /**
     * A weak reference to the target object on which the property exists, set
     * in the constructor. We'll cancel the animation if this goes away.
     */
    AutoPtr<IWeakReference> mTarget;

    String mPropertyName;

    AutoPtr<IProperty> mProperty;

    Boolean mAutoCancel;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_OBJECTANIMATOR_H__
