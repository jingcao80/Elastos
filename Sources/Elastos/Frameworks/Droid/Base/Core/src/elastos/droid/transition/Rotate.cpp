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

#include "elastos/droid/transition/Rotate.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ObjectAnimator;

using Elastos::Core::CString;
using Elastos::Core::CFloat;
using Elastos::Core::IFloat;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// Rotate::
//===============================================================

const String Rotate::PROPNAME_ROTATION("android:rotate:rotation");

ECode Rotate::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ct = (CTransitionValues*)transitionValues;
    Float r = 0.0f;
    ct->mView->GetRotation(&r);
    AutoPtr<IFloat> rf;
    CFloat::New(r, (IFloat**)&rf);
    AutoPtr<ICharSequence> pro_rot;
    CString::New(PROPNAME_ROTATION, (ICharSequence**)&pro_rot);
    ct->mValues->Put(pro_rot, rf);
    return NOERROR;
}

ECode Rotate::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ct = (CTransitionValues*)transitionValues;
    AutoPtr<ICharSequence> pro_rot;
    CString::New(PROPNAME_ROTATION, (ICharSequence**)&pro_rot);
    Float r = 0.0f;
    ct->mView->GetRotation(&r);
    AutoPtr<IFloat> rf;
    CFloat::New(r, (IFloat**)&rf);
    ct->mValues->Put(pro_rot, rf);
    return NOERROR;
}

ECode Rotate::CreateAnimator(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)
    if (startValues == NULL || endValues == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cStartValue = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cEndValue = (CTransitionValues*)endValues;
    AutoPtr<IView> view = cEndValue->mView;
    AutoPtr<ICharSequence> pro_rot;
    CString::New(PROPNAME_ROTATION, (ICharSequence**)&pro_rot);
    AutoPtr<IInterface> sp, ep;
    cStartValue->mValues->Get(pro_rot, (IInterface**)&sp);
    cEndValue->mValues->Get(pro_rot, (IInterface**)&ep);
    AutoPtr<IFloat> sf = IFloat::Probe(sp);
    AutoPtr<IFloat> ef = IFloat::Probe(ep);
    Float startRotation, endRotation;
    sf->GetValue(&startRotation);
    ef->GetValue(&endRotation);
    if (startRotation != endRotation) {
        view->SetRotation(startRotation);
        AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(2);
        (*arr)[0] = startRotation;
        (*arr)[1] = endRotation;
        AutoPtr<IObjectAnimator> p = ObjectAnimator::OfFloat(view,
                                    Elastos::Droid::View::View::ROTATION, arr);
        *result = IAnimator::Probe(p);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
