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

#include "elastos/droid/transition/CChangeClipBounds.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/animation/RectEvaluator.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::RectEvaluator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Transition::CTransitionValues;
using Elastos::Droid::Graphics::CRect;

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CChangeClipBounds::
//===============================================================
const String CChangeClipBounds::TAG("ChangeTransform");

const String CChangeClipBounds::PROPNAME_CLIP("android:clipBounds:clip");
const String CChangeClipBounds::PROPNAME_BOUNDS("android:clipBounds:bounds");

AutoPtr<ArrayOf<String> > CChangeClipBounds::sTransitionProperties = ArrayOf<String>::Alloc(1);

CAR_OBJECT_IMPL(CChangeClipBounds)

CChangeClipBounds::CChangeClipBounds()
{
    (*sTransitionProperties)[0] = PROPNAME_CLIP;
}

ECode CChangeClipBounds::constructor()
{
    return NOERROR;
}

ECode CChangeClipBounds::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Transition::constructor(context, attrs);
    return NOERROR;
}

ECode CChangeClipBounds::GetTransitionProperties(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = sTransitionProperties;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CChangeClipBounds::CaptureValues(
    /* [in] */ ITransitionValues* values)
{
    AutoPtr<CTransitionValues> cV = (CTransitionValues*)values;
    AutoPtr<IView> view = cV->mView;
    Int32 v = 0;
    view->GetVisibility(&v);
    if (v == IView::GONE) {
        return;
    }

    AutoPtr<IRect> clip;
    view->GetClipBounds((IRect**)&clip);
    AutoPtr<ICharSequence> pro_clip;
    CString::New(PROPNAME_CLIP, (ICharSequence**)&pro_clip);
    cV->mValues->Put(pro_clip, clip);
    if (clip == NULL) {
        Int32 w = 0, h = 0;
        view->GetWidth(&w);
        view->GetHeight(&h);
        AutoPtr<IRect> bounds;
        CRect::New(0, 0, w, h, (IRect**)&bounds);
        AutoPtr<ICharSequence> pro_bounds;
        CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
        cV->mValues->Put(pro_bounds, bounds);
    }
}

ECode CChangeClipBounds::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeClipBounds::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeClipBounds::CreateAnimator(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICharSequence> pro_clip;
    CString::New(PROPNAME_CLIP, (ICharSequence**)&pro_clip);
    AutoPtr<CTransitionValues> cS = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cE = (CTransitionValues*)endValues;
    Boolean bS = FALSE, bE = FALSE;
    if (cS == NULL || cE == NULL
            || !(cS->mValues->ContainsKey(pro_clip, &bS), bS)
            || !(cE->mValues->ContainsKey(pro_clip, &bE), bE)) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> s, e;
    cS->mValues->Get(pro_clip, (IInterface**)&s);
    cE->mValues->Get(pro_clip, (IInterface**)&e);
    AutoPtr<IRect> start = IRect::Probe(s);
    AutoPtr<IRect> end = IRect::Probe(e);
    if (start == NULL && end == NULL) {
        *result = NULL; // No animation required since there is no clip.
        return NOERROR;
    }

    if (start == NULL) {
        AutoPtr<ICharSequence> pro_bounds;
        CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
        cS->mValues->Get(pro_bounds, (IInterface**)&s);
        start = IRect::Probe(s);
    }
    else if (end == NULL) {
        AutoPtr<ICharSequence> pro_bounds;
        CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
        cE->mValues->Get(pro_bounds, (IInterface**)&e);
        end = IRect::Probe(e);
    }
    if (Object::Equals(start->Probe(EIID_IInterface), end->Probe(EIID_IInterface))) {
        *result = NULL;
        return NOERROR;
    }

    cE->mView->SetClipBounds(start);
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    AutoPtr<ITypeEvaluator> evaluator = new RectEvaluator(r);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(2);
    (*arr)[0] = start;
    (*arr)[1] = end;
    AutoPtr<IObjectAnimator> oA = ObjectAnimator::OfObject(cE->mView, String("clipBounds"), evaluator, arr);
    *result = IAnimator::Probe(oA);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CChangeClipBounds::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ITransition> changes;
    CChangeClipBounds::New((ITransition**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CChangeClipBounds::CloneImpl(
    /* [in] */ ITransition* boundsObj)
{
    VALIDATE_NOT_NULL(boundsObj);

    return Transition::CloneImpl(ITransition::Probe(boundsObj));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
