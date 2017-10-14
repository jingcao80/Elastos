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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/StateListAnimator.h"
#include "elastos/droid/utility/StateSet.h"

using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::View::EIID_IView;

namespace Elastos {
namespace Droid {
namespace Animation {

StateListAnimator::Tuple::Tuple(
    /* [in] */ ArrayOf<Int32>* specs,
    /* [in] */ IAnimator* animator)
    : mSpecs(specs)
    , mAnimator(animator)
{
}

AutoPtr<ArrayOf<Int32> > StateListAnimator::Tuple::GetSpecs()
{
    return mSpecs;
}

AutoPtr<IAnimator> StateListAnimator::Tuple::GetAnimator()
{
    return mAnimator;
}

StateListAnimator::AnimatorListener::AnimatorListener(
    /* [in] */ StateListAnimator* host)
    : mHost(host)
{}

ECode StateListAnimator::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    animation->SetTarget(NULL);
    if (mHost->mRunningAnimator.Get() == animation) {
        mHost->mRunningAnimator = NULL;
    }
    return NOERROR;
}


CAR_INTERFACE_IMPL(StateListAnimator, Object, IStateListAnimator);
StateListAnimator::StateListAnimator()
{
    mAnimatorListener = new AnimatorListener(this);
}

ECode StateListAnimator::AddState(
    /* [in] */ ArrayOf<Int32>* specs,
    /* [in] */ IAnimator* animator)
{
    AutoPtr<Tuple> tuple = new Tuple(specs, animator);
    tuple->mAnimator->AddListener(mAnimatorListener);
    mTuples.PushBack(tuple);
    return NOERROR;
}

ECode StateListAnimator::GetRunningAnimator(
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = mRunningAnimator;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode StateListAnimator::GetTarget(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    if (mViewRef != NULL) {
        mViewRef->Resolve(EIID_IView, (IInterface**)view);
    }
    return NOERROR;
}

ECode StateListAnimator::SetTarget(
    /* [in] */ IView* view)
{
    AutoPtr<IView> current;
    GetTarget((IView**)&current);
    if (current.Get() == view) {
        return NOERROR;
    }
    if (current != NULL) {
        ClearTarget();
    }
    if (view != NULL) {
        mViewRef = NULL;
        IWeakReferenceSource::Probe(view)->GetWeakReference((IWeakReference**)&mViewRef);
    }

    return NOERROR;
}

void StateListAnimator::ClearTarget()
{
    List<AutoPtr<Tuple> >::Iterator ator = mTuples.Begin();
    for (; ator != mTuples.End(); ++ator) {
        (*ator)->mAnimator->SetTarget(NULL);
    }

    mViewRef = NULL;
    mLastMatch = NULL;
    mRunningAnimator = NULL;
}

ECode StateListAnimator::SetState(
    /* [in] */ ArrayOf<Int32>* state)
{
    AutoPtr<Tuple> match;
    List<AutoPtr<Tuple> >::Iterator ator = mTuples.Begin();
    for (; ator != mTuples.End(); ++ator) {
        AutoPtr<Tuple> tuple = *ator;
        if (StateSet::StateSetMatches(tuple->mSpecs, state)) {
            match = tuple;
            break;
        }
    }

    if (match == mLastMatch) {
        return NOERROR;
    }
    if (mLastMatch != NULL) {
        Cancel();
    }
    mLastMatch = match;
    if (match != NULL) {
        Start(match);
    }
    return NOERROR;
}

void StateListAnimator::Start(
    /* [in] */ Tuple* match)
{
    AutoPtr<IView> view;
    GetTarget((IView**)&view);
    match->mAnimator->SetTarget(view);
    mRunningAnimator = match->mAnimator;
    mRunningAnimator->Start();
}

void StateListAnimator::Cancel()
{
    if (mRunningAnimator != NULL) {
        mRunningAnimator->Cancel();
        mRunningAnimator = NULL;
    }
}

ECode StateListAnimator::GetTuples(
    /* [out] */ IArrayList** list)
{
    assert(list != NULL && *list != NULL);
    List<AutoPtr<Tuple> >::Iterator ator = mTuples.Begin();
    for (; ator != mTuples.End(); ++ator) {
        (*list)->Add((*ator)->Probe(EIID_IInterface));
    }
    return NOERROR;
}

ECode StateListAnimator::JumpToCurrentState()
{
    if (mRunningAnimator != NULL) {
        mRunningAnimator->End();
    }
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

