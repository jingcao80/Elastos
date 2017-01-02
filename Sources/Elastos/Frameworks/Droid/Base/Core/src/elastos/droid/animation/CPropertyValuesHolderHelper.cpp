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

#include "elastos/droid/animation/CPropertyValuesHolderHelper.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(CPropertyValuesHolderHelper, Singleton, IPropertyValuesHolderHelper)
CAR_SINGLETON_IMPL(CPropertyValuesHolderHelper)

/**
 * Constructs and returns a PropertyValuesHolder with a given property name and
 * set of Int32 values.
 * @param propertyName The name of the property being animated.
 * @param values The values that the named property will animate between.
 * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
 */
ECode CPropertyValuesHolderHelper::OfInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfInt32(propertyName, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

/**
 * Constructs and returns a PropertyValuesHolder with a given property and
 * set of Int32 values.
 * @param property The property being animated. Should not be NULL.
 * @param values The values that the property will animate between.
 * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
 */
ECode CPropertyValuesHolderHelper::OfInt32(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfInt32(property, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

/**
 * Constructs and returns a PropertyValuesHolder with a given property name and
 * set of Float values.
 * @param propertyName The name of the property being animated.
 * @param values The values that the named property will animate between.
 * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
 */
ECode CPropertyValuesHolderHelper::OfFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfFloat(propertyName, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

/**
 * Constructs and returns a PropertyValuesHolder with a given property and
 * set of Float values.
 * @param property The property being animated. Should not be NULL.
 * @param values The values that the property will animate between.
 * @return PropertyValuesHolder The constructed PropertyValuesHolder object.
 */
ECode CPropertyValuesHolderHelper::OfFloat(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfFloat(property, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

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
ECode CPropertyValuesHolderHelper::OfObject(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfObject(propertyName, evaluator, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

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
ECode CPropertyValuesHolderHelper::OfObject(
    /* [in] */ IProperty* property,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfObject(property, evaluator, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

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
ECode CPropertyValuesHolderHelper::OfKeyframe(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<IKeyframe*>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfKeyframe(propertyName, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

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
ECode CPropertyValuesHolderHelper::OfKeyframe(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<IKeyframe*>* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    AutoPtr<IPropertyValuesHolder> temp = PropertyValuesHolder::OfKeyframe(property, values);
    *holder = temp;
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
