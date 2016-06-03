
#ifndef __ELASTOS_DROID_ANIMATION_FLOATPROPERTYVALUESHOLDER_H__
#define __ELASTOS_DROID_ANIMATION_FLOATPROPERTYVALUESHOLDER_H__

#include "Elastos.Droid.Utility.h"
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
    CAR_INTERFACE_DECL()

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

    CARAPI GetAnimatedValue(
        /* [out] */ IInterface** value);

    CARAPI Clone(
        /* [out] */ IInterface** holder);

    CARAPI SetAnimatedValue(
        /* [in] */ IInterface* target);

    CARAPI SetupSetter(
            /* [in] */ IClassInfo* targetClass);

    CARAPI SetupGetter(
        /* [in] */ IClassInfo* target);

private:
    CARAPI CallGetter(
        /* [in] */ IInterface* target,
        /* [out] */ IInterface** value);

protected:
    AutoPtr<IMethodInfo> mNativeSetter;
    AutoPtr<IFloatKeyframes> mFloatKeyframes;
    Float mFloatAnimatedValue;

private:
    typedef HashMap<String, AutoPtr<IMethodInfo> > MethodMap;
    typedef typename MethodMap::Iterator MethodMapIterator;

    typedef HashMap<AutoPtr<IClassInfo>, AutoPtr<MethodMap> > ClassMethodMap;
    typedef typename ClassMethodMap::Iterator ClassMethodMapIterator;

    static ClassMethodMap sNativeGetterSetterPropertyMap;
    AutoPtr<IFloatProperty> mFloatProperty;
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_FLOATPROPERTYVALUESHOLDER_H__
