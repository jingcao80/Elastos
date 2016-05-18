
#ifndef __ELASTOS_DROID_ANIMATION_INT32PROPERTYVALUESHOLDER_H__
#define __ELASTOS_DROID_ANIMATION_INT32PROPERTYVALUESHOLDER_H__

#include "PropertyValuesHolder.h"

using Elastos::Droid::Utility::IInt32Property;
using Elastos::Droid::Utility::EIID_IInt32Property;

namespace Elastos {
namespace Droid {
namespace Animation {

class Int32PropertyValuesHolder
    : public PropertyValuesHolder
    , public IInt32PropertyValuesHolder
{
public:
    CAR_INTERFACE_DECL()

    Int32PropertyValuesHolder(
        /* [in] */ const String& propertyName,
        /* [in] */ IInt32Keyframes* keyframes);

    Int32PropertyValuesHolder(
        /* [in] */ IProperty* property,
        /* [in] */ IInt32Keyframes* keyframes);

    Int32PropertyValuesHolder(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Int32>* values);

    Int32PropertyValuesHolder(
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Int32>* values);

    CARAPI SetInt32Values(
        /* [in] */ ArrayOf<Int32>* values);

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
    AutoPtr<IInt32Keyframes> mInt32Keyframes;
    Int32 mInt32AnimatedValue;

private:
    typedef HashMap<String, AutoPtr<IMethodInfo> > MethodMap;
    typedef typename MethodMap::Iterator MethodMapIterator;

    typedef HashMap<AutoPtr<IClassInfo>, AutoPtr<MethodMap> > ClassMethodMap;
    typedef typename ClassMethodMap::Iterator ClassMethodMapIterator;

    static ClassMethodMap sJNISetterPropertyMap;
    AutoPtr<IInt32Property> mInt32Property;

};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_INT32PROPERTYVALUESHOLDER_H__
