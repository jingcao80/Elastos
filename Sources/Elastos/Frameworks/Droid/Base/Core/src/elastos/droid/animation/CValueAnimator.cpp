
#include "elastos/droid/animation/CValueAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_OBJECT_IMPL(CValueAnimator);

AutoPtr<IValueAnimator> CValueAnimator::OfInt32(
    /* [in] */ ArrayOf<Int32>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetInt32Values(values);
    return anim;
}

AutoPtr<IValueAnimator> CValueAnimator::OfFloat(
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetFloatValues(values);
    return anim;
}

AutoPtr<IValueAnimator> CValueAnimator::OfPropertyValuesHolder(
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetValues(values);
    return anim;
}

AutoPtr<IValueAnimator> CValueAnimator::OfObject(
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetObjectValues(values);
    anim->SetEvaluator(evaluator);
    return anim;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
