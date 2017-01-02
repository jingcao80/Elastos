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

#include "elastos/droid/animation/CObjectAnimatorHelper.h"
#include "elastos/droid/animation/ObjectAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_SINGLETON_IMPL(CObjectAnimatorHelper)
CAR_INTERFACE_IMPL(CObjectAnimatorHelper, Singleton, IObjectAnimatorHelper)

ECode CObjectAnimatorHelper::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfInt32(target, propertyName, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

ECode CObjectAnimatorHelper::OfInt32(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfInt32(target, property, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

ECode CObjectAnimatorHelper::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfFloat(target, propertyName, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

ECode CObjectAnimatorHelper::OfFloat(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfFloat(target, property, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

ECode CObjectAnimatorHelper::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfObject(target, propertyName, evaluator, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

ECode CObjectAnimatorHelper::OfObject(
    /* [in] */ IInterface* target,
    /* [in] */ IProperty* property,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfObject(target, property, evaluator, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

ECode CObjectAnimatorHelper::OfPropertyValuesHolder(
    /* [in] */ IInterface* target,
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values,
    /* [out] */ IObjectAnimator** anim)
{
    AutoPtr<IObjectAnimator> animTemp = ObjectAnimator::OfPropertyValuesHolder(target, values);
    *anim = animTemp;
    REFCOUNT_ADD(*anim)
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
