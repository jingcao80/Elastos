
#ifndef __ELASTOS_DROID_ANIMATION_COBJECTANIMATORHELPER_H__
#define __ELASTOS_DROID_ANIMATION_COBJECTANIMATORHELPER_H__

#include "_Elastos_Droid_Animation_CObjectAnimatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Droid::Utility::IProperty;

namespace Elastos {
namespace Droid {
namespace Animation {

CarClass(CObjectAnimatorHelper)
    , public Singleton
    , public IObjectAnimatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfInt32(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Float>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfFloat(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ArrayOf<Float>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfObject(
        /* [in] */ IInterface* target,
        /* [in] */ IProperty* property,
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ IObjectAnimator** anim);

    CARAPI OfPropertyValuesHolder(
        /* [in] */ IInterface* target,
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values,
        /* [out] */ IObjectAnimator** anim);
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_COBJECTANIMATORHELPER_H__
