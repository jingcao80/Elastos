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

#ifndef __ELASTOS_DROID_ANIMATION_ANIMATORLISTENERADAPTER_H__
#define __ELASTOS_DROID_ANIMATION_ANIMATORLISTENERADAPTER_H__

#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Animation {

class ECO_PUBLIC AnimatorListenerAdapter
    : public Object
    , public IAnimatorListener
    , public IAnimatorPauseListener
{
public:
    CAR_INTERFACE_DECL()

    AnimatorListenerAdapter();

    virtual ~AnimatorListenerAdapter();

    virtual CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationCancel(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationPause(
        /* [in] */ IAnimator* animation);

    virtual CARAPI OnAnimationResume(
        /* [in] */ IAnimator* animation);
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_ANIMATORLISTENERADAPTER_H__
