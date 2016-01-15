
#ifndef __ELASTOS_DROID_ANIMATION_FLOATPROPERTYVALUESHOLDER_H__
#define __ELASTOS_DROID_ANIMATION_FLOATPROPERTYVALUESHOLDER_H__

#include "PropertyValuesHolder.h"

using Elastos::Droid::Utility::IFloatProperty;

namespace Elastos {
namespace Droid {
namespace Animation {

class FloatPropertyValuesHolder
    : public PropertyValuesHolder
    , public IFloatPropertyValuesHolder
{
public:
    CAR_INTERFACE_DECL();

    FloatPropertyValuesHolder(
        /* [in] */ const String& propertyName,
        /* [in] */ IFloatKeyframes* keyframeSet);

    FloatPropertyValuesHolder(
        /* [in] */ IProperty* property,
        /* [in] */ IFloatKeyframes* keyframeSet);

    FloatPropertyValuesHolder(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Float>* values);

    FloatPropertyValuesHolder(
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Float>* values);

    CARAPI SetFloatValues(
        /* [in] */ ArrayOf<Float>* values);

    CARAPI CalculateValue(
        /* [in] */ Float fraction);

    virtual CARAPI GetAnimatedValue(
        /* [out] */ IInterface** value);

    CARAPI Clone(
        /* [out] */ IInterface** holder);

    CARAPI SetAnimatedValue(
        /* [in] */ IInterface* target);

    CARAPI SetupSetter(
            /* [in] */ IInterface* targetClass);

protected:

    AutoPtr<IMethodInfo> mJniSetter;
    AutoPtr<IFloatKeyframes> mFloatKeyframes;
    Float mFloatAnimatedValue;
private:
    typedef HashMap<String, AutoPtr<IMethodInfo> > MethodMap;
    typedef typename MethodMap::Iterator MethodMapIterator;

    typedef HashMap<AutoPtr<IClassInfo>, AutoPtr<MethodMap> > ClassMethodMap;
    typedef typename ClassMethodMap::Iterator ClassMethodMapIterator;

    static ClassMethodMap sJNISetterPropertyMap;
    AutoPtr<IFloatProperty> mFloatProperty;

};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_FLOATPROPERTYVALUESHOLDER_H__
