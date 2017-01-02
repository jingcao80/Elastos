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

#include "elastos/droid/animation/AnimatorListenerAdapter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL_2(AnimatorListenerAdapter, Object, IAnimatorListener, IAnimatorPauseListener);

AnimatorListenerAdapter::AnimatorListenerAdapter()
{}

AnimatorListenerAdapter::~AnimatorListenerAdapter()
{}

ECode AnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationPause(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorListenerAdapter::OnAnimationResume(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
