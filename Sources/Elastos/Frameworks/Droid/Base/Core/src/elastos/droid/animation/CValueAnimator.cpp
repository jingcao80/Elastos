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
