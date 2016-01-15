
#ifndef __ELASTOS_DROID_ANIMATION_MULTIFLOATVALUESHOLDER_H__
#define __ELASTOS_DROID_ANIMATION_MULTIFLOATVALUESHOLDER_H__

#include "PropertyValuesHolder.h"

namespace Elastos {
namespace Droid {
namespace Animation {

class MultiFloatValuesHolder
    : public PropertyValuesHolder
    , public IMultiFloatValuesHolder
{
public:
    CAR_INTERFACE_DECL();

    MultiFloatValuesHolder(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);

    MultiFloatValuesHolder(
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeConverter* converter,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ IKeyframes* keyframes);

    /**
     * Internal function to set the value on the target object, using the setter set up
     * earlier on this PropertyValuesHolder object. This function is called by ObjectAnimator
     * to handle turning the value calculated by ValueAnimator into a value set on the object
     * according to the name of the property.
     *
     * @param target The target object on which the value is set
     */
    // @Override
    CARAPI SetAnimatedValue(
        /* [in] */ IInterface* target);

    /**
     * Internal function (called from ObjectAnimator) to set up the setter and getter
     * prior to running the animation. No getter can be used for multiple parameters.
     *
     * @param target The object on which the setter exists.
     */
    // @Override
    CARAPI SetupSetterAndGetter(
        /* [in] */ IInterface* target);

    // @Override
    CARAPI SetupSetter(
        /* [in] */ IInterface* targetClass);

private:
    AutoPtr<IMethodInfo> mJniSetter;

    typedef HashMap<String, AutoPtr<IMethodInfo> > MethodMap;
    typedef typename MethodMap::Iterator MethodMapIterator;

    typedef HashMap<AutoPtr<IClassInfo>, AutoPtr<MethodMap> > ClassMethodMap;
    typedef typename ClassMethodMap::Iterator ClassMethodMapIterator;

    static ClassMethodMap sJNISetterPropertyMap;
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_MULTIFLOATVALUESHOLDER_H__
