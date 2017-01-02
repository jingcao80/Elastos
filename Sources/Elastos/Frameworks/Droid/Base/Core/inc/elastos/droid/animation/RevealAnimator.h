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

#ifndef __ELASTOS_DROID_ANIMATION_REVEALANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_REVEALANIMATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/RenderNodeAnimator.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::RenderNodeAnimator;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * Reveals a View with an animated clipping circle.
 * The clipping is implemented efficiently by talking to a private reveal API on View.
 * This hidden class currently only accessed by the {@link android.view.View}.
 *
 * @hide
 */
class RevealAnimator
    : public RenderNodeAnimator
{
public:
    RevealAnimator(
        /* [in] */ IView* clipView,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Float startRadius,
        /* [in] */ Float endRadius);

protected:
    // @Override
    virtual CARAPI_(void) OnFinished();

private:
    AutoPtr<IView> mClipView;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_REVEALANIMATOR_H__
